/*	EXORcister simulator
 *	Copyright
 *		(C) 2011 Joseph H. Allen
 *
 * This is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 1, or (at your option) any later version.  
 *
 * It is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.  
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this software; see the file COPYING.  If not, write to the Free Software Foundation, 
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Exorciser / swtpc emulator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#include "sim6800.h"
#include "exor.h"
#include "exorterm.h"
#include "utils.h"

char *lpt_name; /* Name of line printer file */
FILE *lpt_file; /* Line printer file */

int echo_flag_addr;
int exbug_detected = 0;

/* Options */

int swtpc = 0;
int trace_disk = 0; /* Enable disk trace */
int lower = 0; /* Allow lower case */

int protect_roms = 1; /* Protect "ROM"s from writing if set */

/* Diskettes */

struct drive_info {
        const char *name;
        FILE *f;
        int bytes; /* Bytes per sector */
        int tracks; /* Tracks per disk */
        int sects; /* Sectors per track */
} drive[4] =
{
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 }
};

/* Memory */
unsigned char mem[65536];

int pending_read_ahead = 1;
unsigned char read_ahead_c;

int count = 10;

int polling = 1; /* Allow ACIA polling */

static int saved;

const char *local_prefix;

/* Copy a file by name */

int copyfile(const char *src, const char *dest)
{
        FILE *f, *g;
        printf("Copying %s to %s...\n", src, dest);
        f = fopen(src, "r");
        if (f)
        {
                g = fopen(dest, "w");
                if (g)
                {
                        char buf[1024];
                        size_t len;
                        while ((len = fread(buf, 1, sizeof(buf), f)))
                                fwrite(buf, 1, len, g);
                        fclose(g);
                        fclose(f);
                        return 0;
                }
                else
                {
                        fclose(f);
                        return -1;
                }
        }
        else
        {
                return -1;
        }
}

/* Find configuration or state file */
/* If 'copy' set, make a writable copy of the file in the user's home directory */

const char *choose_config_file(const char *name, int copy)
{
        FILE *f;
        /* Create path $HOME/.exorsim */
        if (!local_prefix)
        {
                char *home = getenv("HOME");
                char *tmp = malloc(strlen(home) + strlen("/.exorsim") + 1);
                sprintf(tmp, "%s/.exorsim", home);
                local_prefix = tmp;
                mkdir(local_prefix, 0700); /* Create directory in case it doesn't exist */
        }
        /* First try current directory */
        f = fopen(name, "r");
        if (f)
        {
                fclose(f);
                return name;
        }
        else
        {
                /* Next, try ~/.exotsim */
                char *local = malloc(strlen(local_prefix) + 1 + strlen(name) + 1);
                sprintf(local, "%s/%s", local_prefix, name);
                f = fopen(local, "r");
                if (f)
                {
                        fclose(f);
                        return local;
                }
                else
                {
                        /* Try /usr/local/share/exorsim */
                        char *sys = malloc(strlen(DATADIR) + strlen(name) + 1);
                        sprintf(sys, "%s%s", DATADIR, name);
                        if (copy)
                        {
                                /* Make local copy */
                                copyfile(sys, local);
                                return local;
                        }
                        else
                        {
                                /* Otherwise it had better be there */
                                return sys;
                        }
                }
        }
}

void getsect(int n, int addr, int sect, int len)
{
        if (trace_disk) printf("Read sector %d into %x, size=%d\n", sect, addr, len);
        if (drive[n].f) {
                fseek(drive[n].f, sect * drive[n].bytes, SEEK_SET);
                fread(mem + addr, len, 1, drive[n].f);
        } else {
                printf("Tried to read from non-existent disk %d\n", n);
                stop = 1;
        }
}

void getsect1(int n, unsigned char *addr, int ofst, int len)
{
        if (trace_disk) printf("Read sector %d\n", ofst / len);
        if (drive[n].f) {
                fseek(drive[n].f, ofst, SEEK_SET);
                fread(addr, len, 1, drive[n].f);
        } else {
                printf("Tried to read from non-existent disk %d\n", n);
                stop = 1;
        }
}

void putsect(int n, int addr, int sect, int len)
{
        if (trace_disk) printf("Write sector %d into %x, size=%d\n", sect, addr, len);
        if (drive[n].f) {
                fseek(drive[n].f, sect * drive[n].bytes, SEEK_SET);
                fwrite(mem + addr, len, 1, drive[n].f);
                fflush(drive[n].f);
        } else {
                printf("Tried to write to non-existent disk %d\n", n);
                stop = 1;
        }
}

void putsect1(int n, unsigned char *addr, int ofst, int len)
{
        if (trace_disk) printf("Write sector %d\n", ofst / len);
        if (drive[n].f) {
                fseek(drive[n].f, ofst, SEEK_SET);
                fwrite(addr, len, 1, drive[n].f);
                fflush(drive[n].f);
        } else {
                printf("Tried to write to non-existent disk %d\n", n);
                stop = 1;
        }
}

/* FD1771 emulation.. */

int cur_drive = 0;
int cur_sect = 1;
int cur_track = 0;
unsigned char cur_status = 0;
unsigned char cur_buf[256];
int cur_count;
unsigned char cur_data;
int cur_state; /* 0 = IDLE, 1 = read single, 2 = read multiple, 3 = write single */
int cur_dir = 1;

int count1;

/* All memory reads go through mread */

struct reader
{
        unsigned short base; // Base address of this device
        unsigned char (*func)(unsigned short offset); // Offset is read address - base
} **reader_root[64];

void add_reader(unsigned short base, unsigned short offset, unsigned char (*func)(unsigned short))
{
        unsigned short addr = base + offset;
        struct reader **second;
        struct reader *third = 0;

        second = reader_root[addr >> 10];
        if (!second)
        {
                second = (struct reader **)calloc(32, sizeof(struct reader *));
                reader_root[addr >> 10] = second;
        }

        third = second[31 & (addr >> 5)];
        if (!third)
        {
                third = (struct reader *)calloc(32, sizeof(struct reader));
                second[31 & (addr >> 5)] = third;
        }

        third[31 & addr].base = base;
        third[31 & addr].func = func;
}

unsigned char mread(unsigned short addr)
{
        struct reader **second;
        struct reader *third = 0;

        second = reader_root[addr >> 10];
        if (second)
        {
                third = second[31 & (addr >> 5)];
                if (third)
                {
                        if (third[31 & addr].func)
                                return third[31 & addr].func(addr - third[31 & addr].base);
                }
        }

        return mem[addr];
}

/* All writes got through mwrite */

struct writer
{
        unsigned short base;
        void (*func)(unsigned short offset, unsigned char data);
} **writer_root[64];

void add_writer(unsigned short base, unsigned short offset, void (*func)(unsigned short addr, unsigned char data))
{
        unsigned short addr = base + offset;
        struct writer **second;
        struct writer *third = 0;

        second = writer_root[addr >> 10];
        if (!second)
        {
                second = (struct writer **)calloc(32, sizeof(struct writer *));
                writer_root[addr >> 10] = second;
        }

        third = second[31 & (addr >> 5)];
        if (!third)
        {
                third = (struct writer *)calloc(32, sizeof(struct writer));
                second[31 & (addr >> 5)] = third;
        }

        third[31 & addr].base = base;
        third[31 & addr].func = func;
}

#define MAX_ROMS 10

struct
{
        unsigned short low;
        unsigned short high;
} roms[MAX_ROMS];
int rom_count;

void add_rom(unsigned short low, unsigned short high)
{
        if (rom_count != MAX_ROMS)
        {
                roms[rom_count].low = low;
                roms[rom_count].high = high;
                ++rom_count;
        }
        else
        {
                printf("Too many ROMS\n");
        }
}

void mwrite(unsigned short addr, unsigned char data)
{
        int x;
        struct writer **second;
        struct writer *third = 0;

        second = writer_root[addr >> 10];
        if (second)
        {
                third = second[31 & (addr >> 5)];
                if (third)
                {
                        if (third[31 & addr].func)
                        {
                                third[31 & addr].func(addr - third[31 & addr].base, data);
                        }
                }
        }

        // Skip write to memory if it's marked as a ROM

        for (x = 0; x != rom_count; ++x)
                if (addr >= roms[x].low && addr < roms[x].high)
                        goto skip;

        mem[addr] = data;

        skip:;
}

/* All jumps go through jump */

struct jumper
{
        int (*func)(unsigned short addr); // Return true for automatic rts
} **jumper_root[64];

void add_jumper(unsigned short addr, int (*func)(unsigned short addr))
{
        struct jumper **second;
        struct jumper *third = 0;

        second = jumper_root[addr >> 10];
        if (!second)
        {
                second = (struct jumper **)calloc(32, sizeof(struct jumper *));
                jumper_root[addr >> 10] = second;
        }

        third = second[31 & (addr >> 5)];
        if (!third)
        {
                third = (struct jumper *)calloc(32, sizeof(struct jumper));
                second[31 & (addr >> 5)] = third;
        }

        third[31 & addr].func = func;
}

unsigned short pull2();

void jump(unsigned short addr)
{
        struct jumper **second;
        struct jumper *third = 0;

        second = jumper_root[addr >> 10];
        if (second)
        {
                third = second[31 & (addr >> 5)];
                if (third)
                {
                        if (third[31 & addr].func)
                        {
                                int rtn = third[31 & addr].func(addr);
                                /* Notice that call was intercepted */
                                simulated(addr);
                                if (rtn)
                                {
                                        /* Return from subroutine now */
                                        addr = pull2();
                                        jump(addr);
                                }
                                return;
                        }
                }
        }

        pc = addr;
}

/* Simple ACIA for SWTPC */

unsigned char acia_simple_read_status(unsigned short addr)
{
        if (polling) {

                int flags;
                int rtn;

                if (pending_read_ahead)
                        return 0x03;

                flags = fcntl(fileno(stdin), F_GETFL);
                if (flags == -1) {
                        printf("fcntl error\n");
                        exit(-1);
                }
                fcntl(fileno(stdin), F_SETFL, flags | O_NONBLOCK);

                rtn = read(fileno(stdin), &read_ahead_c, 1);

                fcntl(fileno(stdin), F_SETFL, flags);

                if (rtn == 1) {
                        count = 0;
                        pending_read_ahead = 1;
                        return 0x03;
                } else {
                        if (count == 1000)
                                poll(NULL, 0, 1); /* Don't hog CPU time */
                        else
                                ++count;
                        return 0x02;
                }
        } else {
                /* No polling: return false then true */
                if (count--)
                        return 0x00;
                else {
                        count = 10;
                        return 0x03;
                }
        }
}

unsigned char acia_simple_read_data(unsigned short addr)
{
        unsigned char c;
        if (polling) {
                c = read_ahead_c;
                pending_read_ahead = 0;
        } else {
                int rtn = 0;
                int flags = fcntl(fileno(stdin), F_GETFL);
                c = '?';
                if (flags == -1) {
                        printf("fcntl error\n");
                        exit(-1);
                }
                while (rtn < 1 && !stop) {
                        fcntl(fileno(stdin), F_SETFL, flags | O_NONBLOCK);
                        rtn = read(fileno(stdin), &c, 1);
                        fcntl(fileno(stdin), F_SETFL, flags);
                        if (rtn < 1 && !stop) {
                                poll(NULL, 0, 8); /* Don't hog CPU time */
                        }
                }
        }
        if (!lower && c >= 'a' && c <= 'z')
                c += 'A' - 'a';
        if (swtpc) {
                if (c == 127)
                        c = 8;
        } else {
                if (c == 8)
                        c = 127;
        }
        saved = c;
        return c;
}

void acia_simple_write_data(unsigned short addr, unsigned char data)
{
        putchar(data); fflush(stdout);
}

/* Add simple ACIA driver: SWTPC address is 0x8004 */

void add_acia_simple(unsigned short addr)
{
        add_reader(addr, 0, acia_simple_read_status);
        add_reader(addr, 1, acia_simple_read_data);
        add_writer(addr, 1, acia_simple_write_data);
}

/* On SWTPC, this is an alias of the ACIA: it's used to test if this is really an ACIA
   Just return previous read data */

unsigned char acia_simple_read_alias(unsigned short addr)
{
        return saved;
}

/* Normal address is 0x8006 */

void add_acia_alias(unsigned short addr)
{
        add_reader(addr, 1, acia_simple_read_alias);
}

/* Exorciser console ACIA */

unsigned char acia_read_status(unsigned short addr)
{
        // Check serial port status
        if (quick_term_poll())
                return 0x03;
        else
                return 0x02;
}

unsigned char acia_read_data(unsigned short addr)
{
        // Read from serial port
        if (quick_term_poll())
                return term_in();
        else
                return 0;
}

void acia_write_data(unsigned short addr, unsigned char data)
{
        /* Write to serial port */
        term_out(data);
}

int exbug_inch(unsigned short addr)
{
        /* Intercept INCH function */
        /* Note that we don't intercept output functions, we just emulate the ACIA hardware, see mwrite() */
        acca = term_in();
        if (!mem[echo_flag_addr]) { /* Echo flag */
                term_out(acca);
                /* putchar(c);
                fflush(stdout); */
        } else {
                mem[echo_flag_addr] = 0;
        }
        c_flag = 0; /* No error */
        return 1;
}

/* Add exbug console ACIA: address is 0xFCF4 */
/* Look for EXBUG-1.1 or EXBUG-1.2 in memory: if they are there, intercept INCH function */

void add_acia(unsigned short addr)
{
        add_reader(addr, 0, acia_read_status);
        add_reader(addr, 1, acia_read_data);
        add_writer(addr, 1, acia_write_data);
        if (!memcmp(&mem[0xFA8B], "\xb6\xfc\xf4\x47", 4))
        {
                printf("  EXBUG-1.1 detected\n");
                add_jumper(0xFA8B, exbug_inch);
                echo_flag_addr = 0xFF53;
                exbug_detected = 1;
        }
        else if (!memcmp(&mem[0xFA6B], "\xb6\xfc\xf4\x47", 4))
        {
                printf("  EXBUG-1.2 detected\n");
                add_jumper(0xFA6B, exbug_inch);
                echo_flag_addr = 0xFF53;
                exbug_detected = 1;
        }
#ifdef M6809
        else if (!memcmp(&mem[0xF0D2], "\xb6\xfc\xf4\x47", 4))
        {
                printf("  EXBUG09-2.1 detected\n");
                add_jumper(0xF0D2, exbug_inch);
                echo_flag_addr = 0xFF58;
                exbug_detected = 1;
        }
#endif
}


/* Exbug PIA: just return something to make exbug happy */
/* Normal address: 0xFCF8 */

unsigned char exbug_pia_read_data(unsigned short addr)
{
        return 0x0F;
}

unsigned char exbug_pia_read_ctrl(unsigned short addr)
{
        return 0x80;
}

void add_exbug_pia(unsigned short addr)
{
        add_reader(addr, 0, exbug_pia_read_data);
        add_reader(addr, 1, exbug_pia_read_ctrl);
}

/* SWTPC 1771 */

unsigned char swtpc_1771_read(unsigned short addr)
{
        unsigned char c;
        switch (addr) {
                case 0x0000: { /* status */
                        if (cur_state == 1) {
                                if (count1) {
                                        if (!--count1) {
                                                /* Not readying?  Give lost data. */
                                                /* The code sometimes reads without taking data to verify a previous
                                                   write (by checking for CRC error). */
                                                cur_state = 0;
                                                cur_status = 0x04;
                                        }
                                }
                        }
                        return cur_status;
                } case 0x0001: { /* track */
                        return cur_track;
                } case 0x0002: { /* sector */
                        return cur_sect;
                } case 0x0003: { /* data */
                        /* printf("Read %d\n", cur_state); */
                        if (cur_state == 1) {
                                count1 = 100;
                                c = cur_buf[cur_count++];
                                if (cur_count == drive[cur_drive].bytes) {
                                        /* printf("Sector %d done\n", cur_sect); */
                                        cur_state = 0;
                                        cur_status = 0;
                                }
                                return c;
                        } else if (cur_state == 2) {
                                count1 = 100;
                                c = cur_buf[cur_count++];
                                if (cur_count == drive[cur_drive].bytes) {
                                        if (cur_sect == drive[cur_drive].sects) {
                                                /* printf("Sector %d done, track done.\n", cur_sect); */
                                                cur_status = 0; /* All done! */
                                                cur_state = 0;
                                        } else {
                                                /* printf("Sector %d done\n", cur_sect); */
                                                ++cur_sect;
                                                cur_count = 0;
                                                getsect1(cur_drive, cur_buf, (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes, drive[cur_drive].bytes);
                                        }
                                }
                                return c;
                        } else {
                                return 0;
                        }
                }
        }
        return 0;
}

void swtpc_1771_write(unsigned short addr, unsigned char data)
{
        switch (addr) {
                case 0x0000: { /* Command */
                        switch (data & 0xF0) {
                                case 0x00: { /* Restore */
                                        /* printf("FD1771 restore!\n"); */
                                        cur_track = 0;
                                        cur_sect = 1;
                                        cur_dir = 1;
                                        if (drive[cur_drive].f)
                                                cur_status = 0x00;
                                        else
                                                cur_status = 0xD0;
                                        break;
                                } case 0x10: { /* Seek */
                                        /* printf("FD1771 seek to track %d\n", cur_data); */
                                        cur_track = cur_data;
                                        if (drive[cur_drive].f)
                                                cur_status = 0x00;
                                        else
                                                cur_status = 0xD0;
                                        break;
                                } case 0x30: { /* Step */
                                        printf("FD1771 tried to step\n");
                                        if ((int)cur_track + cur_dir != -1)
                                                cur_track += cur_dir;
                                        cur_state = 0x00;
                                        if (drive[cur_drive].f)
                                                cur_status = 0x00;
                                        else
                                                cur_status = 0xD0;
                                        break;
                                } case 0x50: { /* Step in */
                                        printf("FD1771 tried to step in\n");
                                        ++cur_track;
                                        cur_dir = 1;
                                        if (drive[cur_drive].f)
                                                cur_status = 0x00;
                                        else
                                                cur_status = 0xD0;
                                        break;
                                } case 0x70: { /* Step out */
                                        printf("FD1771 tried to step out\n");
                                        if (cur_track)
                                                --cur_track;
                                        cur_dir = -1;
                                        if (drive[cur_drive].f)
                                                cur_status = 0x00;
                                        else
                                                cur_status = 0xD0;
                                        break;
                                } case 0x80: { /* Read single */
                                        /* printf("FD1771 read single\n"); */
                                        if (drive[cur_drive].f) {
                                                getsect1(cur_drive, cur_buf, (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes, drive[cur_drive].bytes);
                                                cur_state = 1;
                                                cur_count = 0;
                                                cur_status = 0x03; /* DRQ + BUSY */
                                                count1 = 100;
                                        } else {
                                                cur_status = 0x90;
                                        }
                                        break;
                                } case 0x90: { /* Read multiple */
                                        /* printf("FD1771 read multiple\n"); */
                                        if (drive[cur_drive].f) {
                                                getsect1(cur_drive, cur_buf, (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes, drive[cur_drive].bytes);
                                                cur_state = 2;
                                                cur_count = 0;
                                                cur_status = 0x03; /* DRQ + BUSY */
                                        } else {
                                                cur_status = 0x90;
                                        }
                                        break;
                                } case 0xA0: { /* Write single */
                                        if (drive[cur_drive].f) {
                                                cur_state = 3;
                                                cur_count = 0;
                                                cur_status = 0x03; /* DQA + BUSY */
                                        } else {
                                                cur_status = 0x90;
                                        }
                                        /* printf("FD1771 write single\n"); */
                                        break;
                                } case 0xB0: { /* Write multiple */
                                        printf("FD1771 tried to write multiple\n");
                                        break;
                                } case 0xC0: { /* Read track */
                                        printf("FD1771 tried to read track\n");
                                        break;
                                } case 0xF0: { /* Write track */
                                        printf("FD1771 tried to write track\n");
                                        break;
                                } case 0xD0: { /* Force interrupt */
                                        break;
                                } default: {
                                        printf("Unknown FD1771 command %x\n", data);
                                        exit(-1);
                                }
                        }
                        break;
                } case 0x0001: { /* Track */
                        /* printf("Set track = %d\n", data); */
                        cur_track = data;
                        break;
                } case 0x0002: { /* Sector */
                        /* printf("Set sector = %d\n", data); */
                        if (!data)
                                data = 1;
                        cur_sect = data;
                        break;
                } case 0x0003: { /* Data */
                        cur_data = data;
                        if (cur_state == 3) {
                                /* printf("Write data %d\n", cur_count); */
                                cur_buf[cur_count++] = data;
                                if (cur_count == drive[cur_drive].bytes) {
                                        printf("Write done.\n");
                                        cur_state = 0;
                                        cur_status = 0;
                                        putsect1(cur_drive, cur_buf, (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes, drive[cur_drive].bytes);
                                }
                        }
                        break;
                }
        }
}

void swtpc_1771_write_drive(unsigned short addr, unsigned char data)
{
        cur_drive = (data & 3);
}

// Normally 0x8018

void add_swtpc_1771(unsigned short addr)
{
        add_reader(addr, 0, swtpc_1771_read); // 0x8018
        add_reader(addr, 1, swtpc_1771_read); // 0x8019
        add_reader(addr, 2, swtpc_1771_read); // 0x801a
        add_reader(addr, 3, swtpc_1771_read); // 0x801b
        add_writer(addr, 0, swtpc_1771_write); // 0x8018
        add_writer(addr, 1, swtpc_1771_write); // 0x8019
        add_writer(addr, 2, swtpc_1771_write); // 0x801a
        add_writer(addr, 3, swtpc_1771_write); // 0x801b
}

// Normally 0x8014

void add_swtpc_1771_ds(unsigned short addr)
{
        add_writer(addr, 0, swtpc_1771_write_drive);
}

/* Addresses of floppy parameters */

#define CURDRV 0	/* Current drive: 0 -3 */
#define STRSCT 1	/* Starting sector (2 bytes) */
#define NUMSCT 3	/* Number of sectors (2 bytes) */
#define LSCTLN 5	/* Length of last sector (1 byte) */
#define CURADR 6	/* Transfer address (2 bytes) */
#define FDSTAT 8	/* Error status: 0x30 means no error */

#define SCTCNT 0x0B	/* Sector count (2 byts): (STRSCT + NUMSCT - SCTCNT - 1) is bad sector number */
#define SIDES 0x0D	/* bit 7 = 1 means single-sided, bit 7 = 0 means double-sided */

/* MDOS disk error codes for FDSTAT */

#define ER_NON '0'	/* No error */
#define ER_CRC '1'	/* Data CRC error */
#define ER_WRT '2'	/* Write protected disk */
#define ER_RDY '3'	/* Disk not ready */
#define ER_MRK '4'	/* Deleted data mark encountered */
#define ER_TIM '5'	/* Timeout */
#define ER_DAD '6'	/* Invalid disk address */
#define ER_SEK '7'	/* Seek error */
#define ER_DMA '8'	/* Data address mark error */
#define ER_ACR '9'	/* Address mark CRC error */

/* Check drive number */

int check_drive(int n)
{
        if (n >= 4) {
                // printf("\r\nFloppy error: attempt to access drive number %d >= 4\n", n);
                mem[FDSTAT] = ER_RDY;
                c_flag = 1;
                return -1;
        }
        if (!drive[n].f) {
                // printf("\r\nFloppy error: attempt to access non-existent disk %d\n", n);
                mem[FDSTAT] = ER_RDY;
                c_flag = 1;
                return -1;
        }
        return 0;
}

/* Check sector number */

int check_sect(int n, int sect)
{
        if (sect >= drive[n].sects * drive[n].tracks) {
                printf("\r\nFloppy error: attempt to access past end of disk %d, sector %d\n", n, sect);
                mem[FDSTAT] = ER_DAD;
                c_flag = 1;
                return -1;
        }
        return 0;
}

/* Send character to printer */

void lpt_out(unsigned char c)
{
        if (c)
        {
                if (lpt_file)
                {
                        fputc(c, lpt_file);
                        if (c == '\n')
                        {
                                fflush(lpt_file);
                        }
                }
                else
                {
                        term_out(c);
                }
        }
}

/* All jumps go through this function */


// EXORdisk-II driver
// Intercepted PROM functions

int exordisk_ii_osload(unsigned short addr) // 0xE800
{
        printf("\nOSLOAD...\n");
        getsect(0, 0x0020, 23, 128);
        getsect(0, 0x0020 + 0x0080, 24, 128);
        pc = 0x0020;
        sp = 0x00FF;
        return 0;
}

int exordisk_ii_fdinit(unsigned short addr) // 0xE822
{
        c_flag = 0;
        return 1;
}

int exordisk_ii_readps(unsigned short call_addr) // 0xE86D
{
        int x;
        int n = mem[CURDRV];
        int first = (mem[STRSCT] << 8) + mem[STRSCT + 1];
        int num = (mem[NUMSCT] << 8) + mem[NUMSCT + 1];
        int addr = (mem[CURADR] << 8) + mem[CURADR + 1];
        int last = mem[LSCTLN];
        if (trace_disk) printf("Read sectors: drive=%d, first=%d, number=%d, addr=%x, size of last=%d\n", n, first, num,
               addr, last);
        if (check_drive(n))
                goto oops;
        for (x = 0; x != num; ++x) {
                if (check_sect(n, first + x))
                        goto oops;
                getsect(n, addr + 128 * x, first + x, ((x + 1 == num) ? mem[LSCTLN] : 128));
        }
        mem[FDSTAT] = ER_NON;
        if (drive[n].tracks == 77)
                mem[SIDES] = 0x80;
        else
                mem[SIDES] = 0;
        c_flag = 0;
        oops:;
        return 1;
}

int exordisk_ii_readsc(unsigned short addr) // 0xE869
{
        mem[LSCTLN] = 128;
        return exordisk_ii_readps(addr);
}

int exordisk_ii_rdcrc(unsigned short call_addr) // 0xE86F
{
        if (trace_disk) printf("RDCRC\n");
        int x;
        int n = mem[CURDRV];
        int first = (mem[STRSCT] << 8) + mem[STRSCT + 1];
        int num = (mem[NUMSCT] << 8) + mem[NUMSCT + 1];
        int addr = (mem[CURADR] << 8) + mem[CURADR + 1];
        int last = mem[LSCTLN];
        if (trace_disk) printf("RDCRC: drive=%d, first=%d, number=%d, addr=%x, size of last=%d\n", n, first, num,
               addr, last);
        if (check_drive(n))
                goto oops;
        for (x = 0; x != num; ++x) {
                if (check_sect(n, first + x))
                        goto oops;
        }
        mem[FDSTAT] = ER_NON;
        if (drive[n].tracks == 77)
                mem[SIDES] = 0x80;
        else
                mem[SIDES] = 0;
        c_flag = 0;
        oops:;
        return 1;
}

int exordisk_ii_restor(unsigned short addr) // 0xE875
{
        int n = mem[CURDRV];
        if (trace_disk) printf("RESTOR\n");
        if (check_drive(n))
                goto oops;
        mem[FDSTAT] = ER_NON;
        if (drive[n].tracks == 77)
                mem[SIDES] = 0x80;
        else
                mem[SIDES] = 0;
        c_flag = 0;
        oops:;
        return 1;
}

int exordisk_ii_seek(unsigned short addr) // 0xE878
{
        int n = mem[CURDRV];
        int first = (mem[STRSCT] << 8) + mem[STRSCT + 1];
        if (trace_disk) printf("SEEK\n");
        if (check_drive(n))
                goto oops;
        if (check_sect(n, first))
                goto oops;
        if (drive[n].tracks == 77)
                mem[SIDES] = 0x80;
        else
                mem[SIDES] = 0;
        c_flag = 0;
        oops:;
        return 1;
}

int exordisk_ii_wrtest(unsigned short call_addr) // 0xE87B
{
        unsigned char buf[128];
        int x;
        int n = mem[CURDRV];
        int first = (mem[STRSCT] << 8) + mem[STRSCT + 1];
        int num = (mem[NUMSCT] << 8) + mem[NUMSCT + 1];
        int addr = (mem[CURADR] << 8) + mem[CURADR + 1];
        if (trace_disk) printf("WRTEST\n");
        if (check_drive(n))
                goto oops;
        for (x = 0; x != 128; x += 2) {
                buf[x] = mem[addr];
                buf[x + 1] = mem[addr + 1];
        }
        for(x=0; x != num; ++x) {
                if (check_sect(n, first + x))
                        goto oops;
                if (trace_disk) printf("Wrtest sector %d drive %d\n", first + x, n);
                fseek(drive[n].f, (first + x) * 128, SEEK_SET);
                fwrite(buf, 128, 1, drive[n].f);
                fflush(drive[n].f);
        }
        c_flag = 0;
        if (drive[n].tracks == 77)
                mem[SIDES] = 0x80;
        else
                mem[SIDES] = 0;
        mem[FDSTAT] = ER_NON;
        oops:;
        return 1;
}

int exordisk_ii_rwtest(unsigned short addr) // 0xE872
{
        if (trace_disk) printf("RWTEST\n");
        exordisk_ii_wrtest(addr);
        return 1;
}

int exordisk_ii_wrddam(unsigned short addr) // 0xE87E
{
        int n = mem[CURDRV];
        printf("\r\nFloppy error: we do not support WRDDAM\n");
        c_flag = 1;
        if (drive[n].tracks == 77)
                mem[SIDES] = 0x80;
        else
                mem[SIDES] = 0;
        mem[FDSTAT] = ER_WRT;
        return 1;
}

int exordisk_ii_wrverf(unsigned short call_addr) // 0xE881
{
        int x;
        int n = mem[CURDRV];
        int first = (mem[STRSCT] << 8) + mem[STRSCT + 1];
        int num = (mem[NUMSCT] << 8) + mem[NUMSCT + 1];
        int addr = (mem[CURADR] << 8) + mem[CURADR + 1];
        int last = mem[LSCTLN];
        if (trace_disk) printf("WRVERF: drive=%d, first=%d, number=%d, addr=%x, size of last=%d\n", n, first, num,
               addr, last);
        if (check_drive(n))
                goto oops;
        for(x=0; x != num; ++x) {
                if (check_sect(n, first + x))
                        goto oops;
                putsect(n, addr + 128 * x, first + x, 128);
        }
        if (drive[n].tracks == 77)
                mem[SIDES] = 0x80;
        else
                mem[SIDES] = 0;
        mem[FDSTAT] = ER_NON;
        c_flag = 0;
        oops:;
        return 1;
}

int exordisk_ii_writsc(unsigned short addr) // 0xE884
{
        if (trace_disk) printf("WRITSC\n");
        exordisk_ii_wrverf(addr);
        return 1;
}


int exordisk_ii_clock(unsigned short addr) // 0xE887
{
        printf("Floppy: Someone called CLOCK?\n");
        c_flag = 0;
        return 1;
}

void add_exordisk_ii(unsigned short addr)
{
        add_jumper(0xe800, exordisk_ii_osload);
        add_jumper(0xe822, exordisk_ii_fdinit);
        add_jumper(0xe869, exordisk_ii_readsc);
        add_jumper(0xe86d, exordisk_ii_readps);
        add_jumper(0xe86f, exordisk_ii_rdcrc);
        add_jumper(0xe875, exordisk_ii_restor);
        add_jumper(0xe878, exordisk_ii_seek);
        add_jumper(0xe872, exordisk_ii_rwtest);
        add_jumper(0xe87b, exordisk_ii_wrtest);
        add_jumper(0xe87e, exordisk_ii_wrddam);
        add_jumper(0xe884, exordisk_ii_writsc);
        add_jumper(0xe881, exordisk_ii_wrverf);
        add_jumper(0xe887, exordisk_ii_clock);
}

// Line printer driver in exordisk_ii PROM

int lpt_lpinit(unsigned short addr)
{
        if (trace_disk) printf("LPINIT\n");
        c_flag = 0;
        return 1;
}

int lpt_list(unsigned short addr)
{
        if (trace_disk) printf("LIST\n");
        lpt_out(acca);
        c_flag = 0;
        return 1;
}

int lpt_ldata(unsigned short addr)
{
        if (trace_disk)printf("LDATA\n");
        while (mem[ix] != 4) {
                lpt_out(mem[ix]);
                ++ix;
        }
        lpt_out('\r');
        lpt_out('\n');
        c_flag = 0;
        return 1;
}

int lpt_ldata1(unsigned short addr)
{
        if (trace_disk) printf("LDATA1\n");
        while (mem[ix] != 4) {
                lpt_out(mem[ix]);
                ++ix;
        }
        c_flag = 0;
        return 1;
}

void add_exordisk_ii_lpt(unsigned short addr)
{
        add_jumper(0xebc0, lpt_lpinit);
        add_jumper(0xebcc, lpt_list);
        add_jumper(0xebe4, lpt_ldata);
        add_jumper(0xebf2, lpt_ldata1);
}

// EXORdisk-I

int secbuf_idx; // Index into cur_buf
unsigned char dkdid_data;

unsigned char exordisk_i_read_dkdid(unsigned short addr)
{
        if ((mem[0xec03] & 0x38) == 0x38)
                return dkdid_data;
        else {
                if (drive[cur_drive].f)
                        return (cur_drive << 1); // Report no errors
                else
                        return 0x20 | (cur_drive << 1); // Report missing disk
        }
}

unsigned char exordisk_i_read_dkdic(unsigned short addr)
{
        return mem[0xEC01] | 0x80; // Report not busy
}

void exordisk_i_write_dkcod(unsigned short addr, unsigned char data)
{
        switch (data)
        {
                case 0x00: { // Do nothing...
                        break;
                } case 0x02: { // Read sector
                        getsect1(cur_drive,
                                 cur_buf,
                                 (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes,
                                 drive[cur_drive].bytes);
                        break;
                } case 0x04: { // Write sector
                        putsect1(cur_drive,
                                 cur_buf,
                                 (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes,
                                 drive[cur_drive].bytes);
                        break;
                } case 0x06: { // Read for CRC (Verify)
                        break;
                } case 0x08: { // Seek
                        break;
                } case 0x0A: { // Clear error flag
                        break;
                } case 0x0C: { // Restore to track 0
                        cur_track = 0;
                        cur_sect = 1;
                        break;
                } case 0x0E: { // Write as DD
                        break;
                } case 0x10: { // Give track
                        // Set track in dkdod
                        cur_track = mem[0xEC06];
                        break;
                } case 0x20: { // Give unit/sector
                        // Set unit/sector in dkdod
                        cur_drive = (mem[0xEC06] >> 6);
                        cur_sect = (mem[0xEC06] & 0x3F);
                        break;
                } case 0x30: { // Write data byte to buffer
                        // Write byte in dkdod to buffer
                        cur_buf[secbuf_idx++] = mem[0xEC06];
                        if (secbuf_idx == 128)
                                secbuf_idx = 0;
                        break;
                } case 0x40: { // Read data byte
                        // On rising edge of bit 6 read data byte so that it appears on dkdid
                        if (!(mem[0xEC02] & 0x40))
                        {
                                // Rising edge
                                dkdid_data = cur_buf[secbuf_idx++];
                                if (secbuf_idx == 128)
                                        secbuf_idx = 0;
                        }
                        break;
                } case 0x80: { // Reset
                        secbuf_idx = 0;
                        break;
                } case 0xFF: { // ?? Reset?
                        break;
                } default: {
                        printf("Unknown EXORdisk-I command code! %x, %x\n", addr, data);
                        break;
                }
        }
}

void add_exordisk_i(unsigned short addr)
{
        add_reader(0xEC00, 0, exordisk_i_read_dkdid); // PIA data A
        add_reader(0xEC00, 1, exordisk_i_read_dkdic); // PIA control A
        add_writer(0xEC00, 2, exordisk_i_write_dkcod); // PIA data B
}

void close_drive(int n)
{
        if (drive[n].f) {
                fclose(drive[n].f);
        }
        drive[n].f = 0;
}

void show_drive(int n)
{
        if (drive[n].f) {
                printf("%s mounted as drive %d\n", drive[n].name, n);
        } else {
                printf("No disk in drive %d\n", n);
        }
}

void set_drive(int n, const char *name)
{
        drive[n].name = name;
}

int load_drive(int n)
{
        FILE *f;
        long t;
        f = fopen(drive[n].name, "r+");
        if (!f) {
                fprintf(stderr, "Couldn't open '%s'\n", drive[n].name);
                return -1;
        }
        fseek(f, 0, SEEK_END);
        t = ftell(f);
        if (swtpc) {
                if (t == 256 * 10 * 35) {
                        drive[n].bytes = 256;
                        drive[n].sects = 10;
                        drive[n].tracks = 35;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[cur_drive].tracks, drive[cur_drive].sects);
                } else if (t == 256 * 10 * 40) {
                        drive[n].bytes = 256;
                        drive[n].sects = 10;
                        drive[n].tracks = 40;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 20 * 35) {
                        drive[n].bytes = 256;
                        drive[n].sects = 20;
                        drive[n].tracks = 35;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 20 * 40) {
                        drive[n].bytes = 256;
                        drive[n].sects = 20;
                        drive[n].tracks = 40;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 18 * 80) {
                        drive[n].bytes = 256;
                        drive[n].sects = 18;
                        drive[n].tracks = 80;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 20 * 80) {
                        drive[n].bytes = 256;
                        drive[n].sects = 20;
                        drive[n].tracks = 80;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 36 * 80) {
                        drive[n].bytes = 256;
                        drive[n].sects = 36;
                        drive[n].tracks = 80;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else if (t == 256 * 72 * 80) {
                        drive[n].bytes = 256;
                        drive[n].sects = 72;
                        drive[n].tracks = 80;
                        drive[n].f = f;
                        printf("'%s' opened for drive %d (tracks=%d sectors=%d)\n", drive[n].name, n, drive[n].tracks, drive[n].sects);
                } else {
                        fclose(f);
                        printf("'%s' is not a valid disk\n", drive[n].name);
                        return -1;
                }
        } else {
                if (t == 128 * 26 * 77) {
                        printf("'%s' opened for drive %d (single sided)\n", drive[n].name, n);
                        drive[n].f = f;
                        drive[n].bytes = 128;
                        drive[n].tracks = 77;
                        drive[n].sects = 26;
                } else if (t == 128 * 26 * 77 * 2) {
                        printf("'%s' opened for drive %d (double sided)\n", drive[n].name, n);
                        drive[n].f = f;
                        drive[n].bytes = 128;
                        drive[n].tracks = 77 * 2;
                        drive[n].sects = 26;
                } else if (t == 128 * 40 * 16) {
                        printf("'%s' opened for drive %d (single sided minifloppy)\n", drive[n].name, n);
                        drive[n].f = f;
                        drive[n].bytes = 128;
                        drive[n].tracks = 40;
                        drive[n].sects = 16;
                } else {
                        fclose(f);
                        printf("'%s' is not a valid disk: it's size must be %d or %d\n", drive[n].name, 128*26*77*2, 128*26*77);
                        return -1;
                }
        }
        return 0;
}

void ctrl_c()
{
        printf("Interrupt!\n");
        stop = 1;
}

int main(int argc, char *argv[])
{
        int x;
        int diskn = 0;
        int gotox = 0;
        mon_out = stdout;
        mon_in = stdin;
        const char *facts_name = 0;
        const char *setup_name = "exor.setup";
        int lpt_append = 1;

        for (x = 1; x != argc; ++x) {
                if (argv[x][0] == '-') {
                        if (!strcmp(argv[x], "--setup") && x + 1 != argc) {
                                ++x;
                                setup_name = argv[x];
                        } else if (!strcmp(argv[x], "--facts") && x + 1 != argc) {
                                ++x;
                                facts_name = argv[x];
                        } else if (!strcmp(argv[x], "--trace")) {
                                trace = 1;
                        } else if (!strcmp(argv[x], "--swtpc")) {
                                swtpc = 1;
                        } else if (!strcmp(argv[x], "--dtrace")) {
                                trace_disk = 1;
                        } else if (!strcmp(argv[x], "--mon")) {
                                stop = 1;
                        } else if (!strcmp(argv[x], "--no_exorterm")) {
                                exorterm = 0;
                        } else if (!strcmp(argv[x], "--skip") && x + 1 != argc) {
                                ++x;
                                skip = atoi(argv[x]);
                        } else if (!strcmp(argv[x], "-x")) {
                                gotox = 1;
                        } else if (!strcmp(argv[x], "-0") && x + 1 != argc) {
                                drive[0].name = argv[++x];
                        } else if (!strcmp(argv[x], "-1") && x + 1 != argc) {
                                drive[1].name = argv[++x];
                        } else if (!strcmp(argv[x], "-2") && x + 1 != argc) {
                                drive[2].name = argv[++x];
                        } else if (!strcmp(argv[x], "-3") && x + 1 != argc) {
                                drive[3].name = argv[++x];
                        } else if (!strcmp(argv[x], "--lpt") && x + 1 != argc) {
                                lpt_append = 0;
                                lpt_name = argv[++x];
                        } else if (!strcmp(argv[x], "--append") && x + 1 != argc) {
                                lpt_append = 1;
                                lpt_name = argv[++x];
                        } else if (!strcmp(argv[x], "--lower")) {
                                lower = 1;
                        } else if (!strcmp(argv[x], "--no_protect")) {
                                protect_roms = 0;
                        } else {
                                printf("EXORciser emulator\n");
                                printf("\n");
                                printf("exor [options] [-0 disk0] [-1 disk1] [-2 disk2] [-3 disk3]\n");
                                printf("\n");
                                printf("  --setup file  Load setup file (exor.setup is default)\n");
                                printf("  --trace	Produce instruction trace on stderr\n");
                                printf("  --dtrace	Produce disk access trace on stderr\n");
                                printf("  --skip nnn    Skip first nnn insns in trace\n");
                                printf("  --swtpc       SWTPC mode instead of EXORciser\n");
                                printf("  -x            Go into EXBUG/SWTBUG instead of MDOS/FLEX\n");
                                printf("  --facts file  Process facts files for commented disassembly\n");
                                printf("  --lower       Allow lowercase\n");
                                printf("  --mon         Start at monitor prompt\n");
                                printf("  --lpt file    Save line printer output to a file\n");
                                printf("  --append file Append line printer output to a file\n");
                                printf("  --no_protect  Allow writing to ROMs\n");
                                printf("  --no_exorterm Disable exorterm emulation\n");
                                printf("\n");
#ifdef M6809
                                printf("Default disk0 is mdos09.dsk/flex09.dsk\n");
#endif
#ifdef M6800
                                printf("Default disk0 is mdos.dsk/flex.dsk\n");
#endif
                                printf("\n");
                                printf("Hints:\n");
                                printf("  To load MDOS from EXBUG, type MAID followed by E800;G\n");
                                printf("\n");
                                printf("  To load FLEX2 from SWTBUG, type D\n");
                                printf("\n");
                                exit(-1);
                        }
                } else {
                        if (diskn == 4) {
                                printf("Only up to four disks allowed\n");
                                return -1;
                        } else {
                                drive[diskn++].name = argv[x];
                        }
                }
        }

        /* Load setup file */

        if (load_setup(setup_name))
                return -1;

        /* Line printer */

        if (!lpt_name)
#ifdef M6809
                lpt_name = getenv("EXOR_LPT_NAME");
#else
                lpt_name = getenv("EXOR09_LPT_NAME");
#endif

        if (!lpt_name)
                lpt_name = "listing.lp";

        /* Open line printer */
        if (lpt_name)
        {
                if (lpt_append)
                        lpt_file = fopen(lpt_name, "a");
                else
                        lpt_file = fopen(lpt_name, "w");
                if (!lpt_file)
                {
                        fprintf(stderr, "Couldn't open line printer file %s\n", lpt_name);
                        return -1;
                }
        }

        /* Default disk image names */
#ifdef M6809
        if (!drive[0].name) drive[0].name = getenv("EXOR09_DRIVE0");
        if (!drive[1].name) drive[1].name = getenv("EXOR09_DRIVE1");
        if (!drive[2].name) drive[2].name = getenv("EXOR09_DRIVE2");
        if (!drive[3].name) drive[3].name = getenv("EXOR09_DRIVE3");
#else
        if (!drive[0].name) drive[0].name = getenv("EXOR_DRIVE0");
        if (!drive[1].name) drive[1].name = getenv("EXOR_DRIVE1");
        if (!drive[2].name) drive[2].name = getenv("EXOR_DRIVE2");
        if (!drive[3].name) drive[3].name = getenv("EXOR_DRIVE3");
#endif

        if (!drive[0].name)
        {
#ifdef M6809
                if (swtpc)
                        drive[0].name = choose_config_file("flex09.dsk", 1);
                else
                        drive[0].name = choose_config_file("mdos09.dsk", 1);
#else
                if (swtpc)
                        drive[0].name = choose_config_file("flex.dsk", 1);
                else
                        drive[0].name = choose_config_file("mdos.dsk", 1);
#endif
        }

        if (!facts_name) {
#ifdef M6809
                facts_name = getenv("EXOR09_FACTS");
#else
                facts_name = getenv("EXOR_FACTS");
#endif
        }

        if (!facts_name) {
#ifdef M6809
                facts_name = choose_config_file("facts09", 0);
#else
                facts_name = choose_config_file("facts", 0);
#endif
        }

        /* Load facts file */
        if (facts_name) {
                FILE *f;
                printf("Load facts file '%s'\n", facts_name);
                f = fopen(facts_name, "r");
                if (f) {
                        parse_facts(f);
                        fclose(f);
                } else {
                        printf("Couldn't load '%s'\n", facts_name);
                }
        }

        /* Mount drives */
        for (x = 0; x != 4; ++x) {
                if (drive[x].name)
                        load_drive(x);
        }

        if (!drive[0].f) {
                /* Do not boot DOS if there is no disk */
                gotox = 1;
        }

        /* Read starting address from reset vector */
        pc = ((mem[0xFFFE] << 8) + mem[0xFFFF]);

        /* ...but jump right to OS load unless gotox is set */
        if (!gotox) {
                if (swtpc) {
                        /* Jump right into flex */
                        sp = 0xA042;
                        pc = 0xE28F;
                } else {
                        /* Jump right into MDOS */
                        if (exbug_detected)
                        {
                                /* But only if EXBUG was recogized */
                                sp = 0xFF8A;
                                // pc = 0xE800;
                                jump(0xE800);
                        }
                }
        }

        /* system("stty cbreak -echo -icrnl"); */
        save_termios();
        sim_termios();

        signal(SIGINT, ctrl_c);

        printf("\nHit Ctrl-C for simulator command line.  Starting simulation...\n");
        izexorterm();

        sim();
        // echo test of terminal emulator
        // while (!stop) term_out(term_in());

        /* system("stty cooked echo icrnl"); */
        restore_termios();
        if (lpt_file)
                fclose(lpt_file);
        return 0;
}
