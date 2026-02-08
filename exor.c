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
#include "lpt.h"
#include "drive.h"
#include "exor.h"
#include "exorterm.h"
#include "utils.h"

int echo_flag_addr;
int exbug_detected = 0;

/* Options */

int trace_disk = 0; /* Enable disk trace */
int lower = 0; /* Allow lower case */

int protect_roms = 1; /* Protect "ROM"s from writing if set */

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
        const char *setup_name = "exor.setup";
        const char *facts_name = 0;
        int lpt_append = 1;
        char *disk_name[4] = { 0, 0, 0, 0 };
        char *lpt_name = 0;

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
                                disk_name[0] = argv[++x];
                        } else if (!strcmp(argv[x], "-1") && x + 1 != argc) {
                                disk_name[1] = argv[++x];
                        } else if (!strcmp(argv[x], "-2") && x + 1 != argc) {
                                disk_name[2] = argv[++x];
                        } else if (!strcmp(argv[x], "-3") && x + 1 != argc) {
                                disk_name[3] = argv[++x];
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

        /* Open line printer */

        if (lpt_name)
        {
                if (lpt_append)
                        append_lpt(lpt_name);
                else
                        open_lpt(lpt_name);
        }

        /* Mount drives */

        for (x = 0; x != 4; ++x) {
                if (disk_name[x])
                {
                        close_drive(x);
                        set_drive(x, disk_name[x]);
                        load_drive(x);
                }
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
        close_lpt();
        return 0;
}
