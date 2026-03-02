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

/* EXORdisk emulator */

#include <stdio.h>
#include <stdlib.h>
#include "sim.h"
#include "exor.h"
#include "drive.h"

int swtpc = 0;

/* Diskettes */

struct drive_info drive[4] =
{
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 0 }
};

void getsect(int n, int addr, int sect, int len)
{
        if (trace_disk)
        {
                
                printf("Read sector %d into %x, size=%d\n", sect, addr, len);
        }
        if (drive[n].f) {
                fseek(drive[n].f, sect * drive[n].bytes, SEEK_SET);
                fread(mem + addr, len, 1, drive[n].f);
        } else {
                printf("Tried to read from non-existent disk %d\n", n);
                stop = 1;
        }
}

int physical_to_logical[]=
{
        0,  3,  6,  9, 12, 15, 18, 21, 24, 1,
        4,  7, 10, 13, 16, 19, 22, 25,  2, 5,
        8, 11, 14, 17, 20, 23
};

void geom(int ofst, int len, int *o_track, int *o_sect)
{
        int sect = ofst / len;
        int track = sect / 26;
        sect -= (track * 26);
        sect = physical_to_logical[sect];
        *o_track = track;
        *o_sect = sect + 1;
}

void getsect1(int n, unsigned char *addr, int ofst, int len)
{
        if (trace_disk)
        {
                int track;
                int sector;
                geom(ofst, len, &track, &sector);
                printf("Read sector drive=%d, track=%d, sector=%d\n", n, track, sector);
        }
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
        if (trace_disk)
        {
                int track;
                int sector;
                geom(ofst, len, &track, &sector);
                printf("Write sector drive=%d, track=%d, sector=%d\n", n, track, sector);
        }
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

// EXORdisk-I

unsigned char dinbuf[128];
unsigned char doutbuf[128];
int inbuf_idx;
int outbuf_idx;

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
        //if (trace_disk)
        //        printf("Disk code %x\n", data);
        switch (data)
        {
                case 0x00: { // Do nothing...
                        break;
                } case 0x02: { // Read sector
                        getsect1(cur_drive,
                                 dinbuf,
                                 (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes,
                                 drive[cur_drive].bytes);
                        inbuf_idx = 0;
                        break;
                } case 0x04: { // Write sector
                        if (trace_disk)
                                printf("writing sector, idx is %d\n", outbuf_idx);
                        putsect1(cur_drive,
                                 doutbuf,
                                 (cur_track * drive[cur_drive].sects + (cur_sect - 1)) * drive[cur_drive].bytes,
                                 drive[cur_drive].bytes);
                        outbuf_idx = 0; // In case we didn't write 128...
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
                        if (trace_disk) printf("Set track 0x%x\n", mem[0xEC06]);
                        cur_track = mem[0xEC06];
                        break;
                } case 0x20: { // Give unit/sector
                        // Set unit/sector in dkdod
                        if (trace_disk) printf("Set sector/unit 0x%x\n", mem[0xEC06]);
                        cur_drive = (mem[0xEC06] >> 6);
                        cur_sect = (mem[0xEC06] & 0x3F);
                        break;
                } case 0x30: { // Write data byte to buffer
                        // Write byte in dkdod to buffer
                        doutbuf[outbuf_idx++] = mem[0xEC06];
                        if (outbuf_idx == 128)
                                outbuf_idx = 0;
                        break;
                } case 0x40: { // Read data byte
                        // On rising edge of bit 6 read data byte so that it appears on dkdid
                        if (!(mem[0xEC02] & 0x40))
                        {
                                // Rising edge
                                dkdid_data = dinbuf[inbuf_idx++];
                                if (inbuf_idx == 128)
                                        inbuf_idx = 0;
                        }
                        break;
                } case 0x80: { // Reset
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
