/*	EDOS diskette access
 *	Copyright
 *		(C) 2023 Joseph H. Allen
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* EDOS disk access */

#define SECTOR_SIZE 128
#define TRACK_SIZE 26

#define SECTOR_DIR 3 /* Starting directory sector */
#define SECTOR_DIR_SIZE 5 /* Number of directory sectors */

#define ENTRY_SIZE 11 /* Size of directory entry */
#define ENTRIES_PER_SECTOR 11 /* Number of entries per sector */

/* Sector interleave table */

int interleave_table[]=
{
    0, 9,  18,
    1, 10, 19,
    2, 11, 20,
    3, 12, 21,
    4, 13, 22,
    5, 14, 23,
    6, 15, 24,
    7, 16, 25,
    8, 17
};

/* Directory entry */

struct dirent {
    unsigned char name[5]; /* 5 '*'s if deleted */
    unsigned char mark; /* 0xFF means end of directory, 0x80 means file was deleted */
    unsigned char track; /* Starting track number of file */
    unsigned char sect; /* Starting sector number of file (1 based) */
    unsigned char size_hi; /* Size high byte? */
    unsigned char size_lo; /* Size low byte: size is one larger than actual size I think */
    unsigned char unknown; /* This is observed to have 0xFF, 0x02 or 0x00 */
};

FILE *disk;

/* Get sector: account for interleave */

void getsect(unsigned char *buf, int lsn)
{
        int track = lsn / TRACK_SIZE;
        int sect = lsn - track * TRACK_SIZE;
        sect = interleave_table[sect];
        fseek(disk, (track * TRACK_SIZE + sect) * SECTOR_SIZE, SEEK_SET);
        fread((char *)buf, SECTOR_SIZE, 1, disk);
}

/* Put sector: account for interleave */

void putsect(unsigned char *buf, int lsn)
{
        int track = lsn / TRACK_SIZE;
        int sect = lsn - track * TRACK_SIZE;
        sect = interleave_table[sect];
        fseek(disk, (track * TRACK_SIZE + sect) * SECTOR_SIZE, SEEK_SET);
        fwrite((char *)buf, SECTOR_SIZE, 1, disk);
}

int lower(int c)
{
        if (c >= 'A' && c <= 'Z')
                return c - 'A' + 'a';
        else
                return c;
}

int upper(int c)
{
        if (c >= 'a' && c <= 'z')
                return c - 'a' + 'A';
        else
                return c;
}


struct name
{
        char *name;

        int size;
        int lsn;
        int mark;
        int unknown;
};

struct name *names[SECTOR_DIR_SIZE * ENTRIES_PER_SECTOR];
int name_n;

int comp(struct name **l, struct name **r)
{
        return strcmp((*l)->name, (*r)->name);
}

int force_convert;

void read_file(int lsn, int size, FILE *f, char raw, char crlf)
{
        unsigned char buf[SECTOR_SIZE];
        int x;
        int guess_type = 0;
        for (x = lsn; x != lsn + size; ++x)
        {
                getsect(buf, x);

                if (raw)
                {
                        /* No conversions */
                        fwrite(buf, SECTOR_SIZE, 1, f);
                }
                else
                {
                        int ends;
                        int n;

                        /* Try to guess object files */
                        if (x == lsn && buf[0] == 'D')
                                guess_type = 1; /* Object file- don't delete NULs */

                        /* On last sector, delete trailing NULs */
                        ends = SECTOR_SIZE;
                        if (x + 1 == lsn + size)
                        {
                                /* Last sector, delete trailing NULs */
                                while (ends && buf[ends-1] == 0) --ends;
                        }
                        if (guess_type)
                        {
                                /* Object files: we only trim trailing NULs in last sector */
                                fwrite(buf, ends, 1, f);
                        }
                        else
                        {
                                /* ASCII files: trim all NULs, fix line ending */
                                for (n = 0; n != ends; ++n) {
                                        int c = buf[n];
                                        if (c == 10) {
                                                /* Convert to MS-DOS */
                                                if (crlf) {
                                                        fputc('\r', f);
                                                }
                                                fputc('\n', f);
                                        } else if (c == 13) {
                                                /* Delete CRs */
                                        } else if (c == 0) {
                                                /* Delete NULs */
                                        } else {
                                                fputc(c, f);
                                        }
                                }
                        }
                }
        }
}

/* Find a file, return its starting sector number and size */

int find_file(char *filename, int *size)
{
        unsigned char buf[SECTOR_SIZE];
        int x;
        for (x = SECTOR_DIR; x != SECTOR_DIR + SECTOR_DIR_SIZE; ++x) {
                int y;
                getsect(buf, x);
                for (y = 0; y <= SECTOR_SIZE - ENTRY_SIZE; y += ENTRY_SIZE) {
                        struct dirent *d = (struct dirent *)(buf + y);
                        /* if (d->mark != 0xff && d->name[0] != '*') { */ /* Not deleted */
                        if (d->name[0] >= 'A' && d->name[0] <= 'Z') {
                                char s[50];
                                int p = 0;
                                int i;
                                for (i = 0; i != sizeof(d->name); i++) {
                                        s[p++] = lower(d->name[i]);
                                }
                                while (p && s[p - 1] == ' ') --p;
                                s[p] = 0;
                                *size = d->size_hi * 256 + d->size_lo - 1;
                                if (!strcmp(s, filename)) {
                                        return (d->track * TRACK_SIZE) + d->sect - 1;
                                }
                        }
                }
        }
        return -1;
}

/* Print raw directory */

void edos_raw_dir()
{
        int free_lsn = 0;
        unsigned char buf[SECTOR_SIZE];
        int x;
        printf("NAME   FLAG TRACK SECT SIZE\n");
        for (x = SECTOR_DIR; x != SECTOR_DIR + SECTOR_DIR_SIZE; ++x) {
                int y;
                getsect(buf, x);
                for (y = 0; y <= SECTOR_SIZE - ENTRY_SIZE; y += ENTRY_SIZE) {
                        struct dirent *d = (struct dirent *)(buf + y);

                        int i;
                        for (i = 0; i != 5; ++i)
                                printf("%c", d->name[i]);
                        printf("  %4x %5x %4x %4x\n", d->mark, d->track, d->sect, (d->size_hi * 256) + d->size_lo - 1);

                        if (d->mark == 0xFF)
                        {
                                goto done;
                        }
                        else
                        {
                                int lsn;
                                int size;
                                lsn = (d->track * TRACK_SIZE) + d->sect - 1;
                                size = d->size_hi * 256 + d->size_lo - 1;
                                free_lsn = lsn + size;
                        }
                }
        }
        done:;
        printf("\n");
        printf("Used = 0x%x sectors\n", free_lsn);
        printf("Free = 0x%x sectors\n", 77 * TRACK_SIZE - free_lsn);
}


/* Load directory */

void edos_load_dir()
{
        unsigned char buf[SECTOR_SIZE];
        int x;
        for (x = SECTOR_DIR; x != SECTOR_DIR + SECTOR_DIR_SIZE; ++x) {
                int y;
                getsect(buf, x);
                for (y = 0; y <= SECTOR_SIZE - ENTRY_SIZE; y += ENTRY_SIZE) {
                        struct dirent *d = (struct dirent *)(buf + y);
                        if (d->mark == 0xFF)
                                goto done;
                        /* if (d->mark != 0xff && d->name[0] != '*') { */
                        /* if (d->name[0] >= 'A' && d->name[0] <= 'Z') { */
                        if (d->mark != 0x80) {
                                struct name *nam;
                                char s[50];
                                int p = 0;
                                int i;
                                for (i = 0; i != sizeof(d->name); i++) {
                                        s[p++] = lower(d->name[i]);
                                }
                                while (p && s[p - 1] == ' ') --p;
                                s[p] = 0;
                                nam = (struct name *)malloc(sizeof(struct name));
                                nam->name = strdup(s);
                                nam->lsn = (d->track * TRACK_SIZE) + d->sect - 1;
                                nam->size = d->size_hi * 256 + d->size_lo - 1;
                                nam->mark = d->mark;
                                nam->unknown = d->unknown;
                                names[name_n++] = nam;
                        }
                }
        }
        done:
        qsort(names, name_n, sizeof(struct name *), (int (*)(const void *, const void *))comp);
}

/* True if file already exists */

int edos_exist(char *name)
{
        unsigned char buf[SECTOR_SIZE];
        int x;
        for (x = SECTOR_DIR; x != SECTOR_DIR + SECTOR_DIR_SIZE; ++x) {
                int y;
                getsect(buf, x);
                for (y = 0; y <= SECTOR_SIZE - ENTRY_SIZE; y += ENTRY_SIZE) {
                        int i;
                        struct dirent *d = (struct dirent *)(buf + y);
                        if (d->mark == 0xFF)
                        {
                                goto done;
                        }
                        for (i = 0; i != 5 && name[i]; ++i)
                                if (upper(name[i]) != d->name[i])
                                        break;
                        if (i == 5 && !name[i])
                                goto match;
                        if (i != 5 && !name[i])
                        {
                                /* Check for spaces... */
                                for (; i != 5; ++i)
                                        if (d->name[i] != ' ')
                                                break;
                                if (i == 5)
                                        goto match;
                        }
                        goto nope;
                        match:
                        if (!(d->mark & 0x80))
                                return 1;
                        nope:;
                }
        }
        done:
        return 0;
}

/* Find file and delete it */

int edos_rm(char *name)
{
        unsigned char buf[SECTOR_SIZE];
        int x;
        for (x = SECTOR_DIR; x != SECTOR_DIR + SECTOR_DIR_SIZE; ++x) {
                int y;
                getsect(buf, x);
                for (y = 0; y <= SECTOR_SIZE - ENTRY_SIZE; y += ENTRY_SIZE) {
                        int i;
                        struct dirent *d = (struct dirent *)(buf + y);
                        if (d->mark == 0xFF)
                        {
                                goto done;
                        }
                        if (d->mark & 0x80)
                        {
                                goto nope;
                        }
                        for (i = 0; i != 5 && name[i]; ++i)
                                if (upper(name[i]) != d->name[i])
                                        break;
                        if (i == 5 && !name[i])
                                goto match;
                        if (i != 5 && !name[i])
                        {
                                /* Check for spaces... */
                                for (; i != 5; ++i)
                                        if (d->name[i] != ' ')
                                                break;
                                if (i == 5)
                                        goto match;
                        }
                        goto nope;
                        match:
                        d->mark = 0x80;
                        putsect(buf, x);
                        printf("File %s marked for purge\n", name);
                        return 0;
                        nope:;
                }
        }
        done:
        printf("File %s not found\n", name);
        return -1;
}


/* Find start of free space on disk */

int edos_alloc()
{
        int free_lsn = 0;
        unsigned char buf[SECTOR_SIZE];
        int x;
        for (x = SECTOR_DIR; x != SECTOR_DIR + SECTOR_DIR_SIZE; ++x) {
                int y;
                getsect(buf, x);
                for (y = 0; y <= SECTOR_SIZE - ENTRY_SIZE; y += ENTRY_SIZE) {
                        struct dirent *d = (struct dirent *)(buf + y);
                        int lsn;
                        int size;
                        if (d->mark == 0xFF)
                                goto done;
                        lsn = (d->track * TRACK_SIZE) + d->sect - 1;
                        size = d->size_hi * 256 + d->size_lo - 1;
                        free_lsn = lsn + size;
                }
        }
        printf("All directory entries used.\n");
        done:
        return free_lsn;
}

/* Append directory entry, set end mark on following one */

void edos_append(struct dirent *new_d)
{
        unsigned char buf[SECTOR_SIZE];
        int x;
        int dirty = 0;
        int next = 0;
        int done = 0;
        int lsn = new_d->track * TRACK_SIZE + new_d->sect - 1;
        int size = new_d->size_hi * 256 + new_d->size_lo - 1;
        int end_lsn = lsn + size;
        int end_track = end_lsn / TRACK_SIZE;
        int end_sect = end_lsn % TRACK_SIZE;
        for (x = SECTOR_DIR; x != SECTOR_DIR + SECTOR_DIR_SIZE; ++x) {
                int y;
                getsect(buf, x);
                for (y = 0; y <= SECTOR_SIZE - ENTRY_SIZE; y += ENTRY_SIZE) {
                        struct dirent *d = (struct dirent *)(buf + y);
                        if (next)
                        {
                                int i;
                                for (i = 0; i != 5; ++i)
                                        d->name[i] = '*';
                                d->mark = 0xFF;
                                d->track = end_track;
                                d->sect = end_sect + 1;
                                d->size_lo = 1;
                                d->size_hi = 0;
                                
                                dirty = 1;
                                done = 1;
                                break;
                        }
                        else if (d->mark == 0xFF)
                        {
                                memcpy(d, new_d, ENTRY_SIZE);
                                dirty = 1;
                                next = 1;
                        }
                }
                if (dirty)
                {
                        putsect(buf, x);
                        dirty = 0;
                }
                if (done)
                        break;
        }
}

/* Write file starting at lsn.  Returns -1 for errors, or number of sectors written */

int put_file_lsn(int lsn, char *local_name, int free_sects, char sys, char rawmode)
{
        FILE *f;
        int size;
        int up;
        int up_sects;
        unsigned char *buf;
        int x;
        int c;
        int i;

        f = fopen(local_name, "r");

        if (!f) {
                printf("Couldn't open '%s'\n", local_name);
                return -1;
        }

        if (fseek(f, 0, SEEK_END)) {
                printf("Couldn't get file size of '%s'\n", local_name);
                fclose(f);
                return -1;
        }
        size = ftell(f);
        if (size < 0)  {
                printf("Couldn't get file size of '%s'\n", local_name);
                fclose(f);
                return -1;
        }
        rewind(f);
        up = ((size + SECTOR_SIZE - 1) & ~(long)(SECTOR_SIZE - 1));
        buf = (unsigned char *)malloc(up*2);

        /* Read file, convert UNIX or MS-DOS ASCII to EDOS ASCII (unless rawmode is set) */
        x = 0;
        while ((c = fgetc(f)) != -1)
        {
                if (rawmode) {
                        buf[x++] = c;
                } else if (c == '\n') {
                        /* EDOS EDIT uses \n\r\0 as terminator */
                        /* But S19 files usually have \r\n */
                        buf[x++] = '\r';
                        buf[x++] = '\n';
/*
                        buf[x++] = '\n';
                        buf[x++] = '\r';
                        buf[x++] = 0;
*/
                } else if (c == '\r') {
                        /* Ignore carriage returns */
                } else {
                        buf[x++] = c;
                }
        }
        if (sys && !rawmode)
        {
                /* Append EDOS PROM terminator */
                /* Not doing this since we don't strip the S ESC during reads
                buf[x++] = 'S';
                buf[x++] = 0x1b;
                */
        }
        size = x;
        fclose(f);

        /* Round up to sector size again */
        up = ((size + SECTOR_SIZE - 1) & ~(long)(SECTOR_SIZE - 1));

        /* Fill with NULs to end of sector */
        while (x != up)
                buf[x++] = 0;

        if (up > free_sects * SECTOR_SIZE)
        {
                printf("Not enough room for %s\n", local_name);
                return -1;
        }

        /* Write sectors */
        up_sects = up / SECTOR_SIZE;

        for (i = 0; i != up_sects; ++i)
        {
                // printf("Write lsn %d (track=%x, sector=%x)\n", alloc+i, (alloc+i)/TRACK_SIZE, 1+((alloc+i)%TRACK_SIZE));
                putsect(buf + i * SECTOR_SIZE, lsn + i);
        }

        /* Pad out to entire given size */
        if (sys)
        {
                memset(buf, 0xe5, SECTOR_SIZE);

                for (; i < free_sects; ++i)
                {
                        putsect(buf + i * SECTOR_SIZE, lsn + i);
                }
        }

        printf("Wrote %d sectors\n", i);

        return up_sects;
}

int put_file(char *local_name, char *mdos_name, char rawmode)
{
        int i;
        int alloc = edos_alloc();
        int free_sects = TRACK_SIZE * 77 - alloc;
        int start_track = alloc / TRACK_SIZE;
        int start_sect = alloc % TRACK_SIZE;
        int up_sects;
        struct dirent d[1];

        if (strlen(mdos_name) > 5)
        {
                printf("File name too long for EDOS\n");
                return -1;
        }

        if (edos_exist(mdos_name))
        {
                printf("File %s already exists\n", mdos_name);
                return -1;
        }

        d->track = start_track;
        d->sect = start_sect + 1;
        d->mark = 0;
        d->unknown = 0;
        d->size_lo = 0;
        d->size_hi = 0;

        for (i = 0; i != 5 && mdos_name[i]; ++i)
                d->name[i] = upper(mdos_name[i]);
        while (i != 5)
                d->name[i++] = ' ';

        printf("Starting free sector = %d\n", alloc);
        printf("Free sectors = %d\n", free_sects);
        printf("Starting track = 0x%x, sector = 0x%x\n", alloc / TRACK_SIZE, (alloc % TRACK_SIZE) + 1);

        up_sects = put_file_lsn(alloc, local_name, free_sects, 0, rawmode);
        if (up_sects < 0)
        {
                return -1;
        }

        /* Add directory entry */
        up_sects = up_sects + 1; // Why?
        d->size_lo = up_sects % 256;
        d->size_hi = up_sects / 256;
        edos_append(d);

        return 0;
}

/* Directory listing */

void edos_dir(int full, int single)
{
        int x, y;
        int rows;
        int cols = (80 / 13);

        edos_load_dir();

        if (full) {
                int totals = 0;
                printf("\n");
                for (x = 0; x != name_n; ++x) {
                        printf("%6d %-13s lsn = %d mark = %x unknown = %x\n",
                               names[x]->size * SECTOR_SIZE, names[x]->name, names[x]->lsn, names[x]->mark, names[x]->unknown);
                        totals += names[x]->size;
                }
                printf("\n%d entries\n", name_n);
                printf("\n%d sectors, %d bytes\n", totals, totals * SECTOR_SIZE);
                printf("\n");
        } else if (single) {
                int x;
                for (x = 0; x != name_n; ++x) {
                        printf("%s\n", names[x]->name);
                }
        } else {
                /* Rows of 12 names each ordered like ls */

                rows = (name_n + cols - 1) / cols;

                for (y = 0; y != rows; ++y) {
                        for (x = 0; x != cols; ++x) {
                                int n = y + x * rows;
                                /* printf("%11d  ", n); */
                                if (n < name_n)
                                        printf("%-11s  ", names[n]->name);
                                else
                                        printf("             ");
                        }
                        printf("\n");
                }
        }
}

/* cat a file */

void cat(char *name, char rawmode, char crlfmode)
{
        int size;
        int lsn = find_file(name, &size);
        if (lsn == -1) {
                printf("File '%s' not found\n", name);
                exit(-1);
        } else {
                read_file(lsn, size, stdout, rawmode, crlfmode);
        }
}

/* get a file from the disk */

int get_file_lsn(int lsn, int size, char *local_name, char rawmode, char crlfmode)
{
        FILE *f = fopen(local_name, "w");
        if (!f) {
                printf("Couldn't open local file '%s'\n", local_name);
                return -1;
        }
        read_file(lsn, size, f, rawmode, crlfmode);
        if (fclose(f)) {
                printf("Couldn't close local file '%s'\n", local_name);
                return -1;
        }
        return 0;
}

int get_file(char *mdos_name, char *local_name, char rawmode, char crlfmode)
{
        int size;
        int lsn = find_file(mdos_name, &size);
        if (lsn == -1) {
                fprintf(stderr, "File '%s' not found\n", mdos_name);
                return -1;
        } else {
                return get_file_lsn(lsn, size, local_name, rawmode, crlfmode);
        }
}

int main(int argc, char *argv[])
{
        int full = 0;
        int single = 0;
	int x;
	char *disk_name;
	x = 1;
	if (x == argc || !strcmp(argv[x], "--help") || !strcmp(argv[x], "-h"))
	{
                help:
                printf("\nEXORciser EDOS-II diskette access\n");
                printf("\n");
                printf("Syntax: edos path-to-diskette command args\n");
                printf("\n");
                printf("  Commands:\n");
                printf("      ls [-la1A]                     Directory listing, sorted\n");
                printf("                  -l for long\n");
                printf("                  -1 to show a single name per line\n");
                printf("      dir                            Directory in raw EDOS format\n");
                printf("      cat edos-name [--raw] [--crlf] Type file to console\n");
                printf("      get edos-name [local-name] [--raw] [--crlf]\n");
                printf("                                     Copy file from diskette to local-name\n");
                printf("      getexec local-name [--raw] [--crlf]\n");
                printf("                                     Copy EXEC from diskette to local-name\n");
                printf("      put local-name [edos-name] [--raw]\n");
                printf("                                     Copy file to diskette to edos-name\n");
                printf("      putexec local-name [--raw]     Copy local-name to EXEC area of diskette\n");
                printf("      rm edos-name                   Delete a file\n");
                printf("      x [--raw] [--crlf]             Extract all files into current directory\n");
                printf("\n");
                printf("      --crlf  use CR-LF (MSDOS) line endings instead of UNIX\n");
                printf("      --raw   no ASCII conversion (otherwise fixes line endings and deletes NULs)\n");
                printf("\n");
                return 0;
	}
	disk_name = argv[x++];
	disk = fopen(disk_name, "r+");
	if (!disk) {
	        fprintf(stderr, "Couldn't open '%s'\n", disk_name);
	        return -1;
	}

	if (x == argc || argv[x][0] == '-') {
	        /* Just print a directory listing */
	        goto dir;
        } else if (!strcmp(argv[x], "help")) {
                goto help;
        } else if (!strcmp(argv[x], "ls")) {
                ++x;
                dir:
                /* Directory options */
                while (x != argc && argv[x][0] == '-') {
                        int y;
                        for (y = 1;argv[x][y];++y) {
                                int opt = argv[x][y];
                                switch (opt) {
                                        case 'l': full = 1; break;
                                        case '1': single = 1; break;
                                        default: fprintf(stderr, "Unknown option '%c'\n", opt); return -1;
                                }
                        }
                        ++x;
                }
	        edos_dir(full, single);
                return 0;
        } else if (!strcmp(argv[x], "dir")) {
                edos_raw_dir();
                return 0;
	} else if (!strcmp(argv[x], "cat")) {
	        char *name = 0;
	        char rawmode = 0;
	        char crlfmode = 0;

                /* File read options */
                for (++x; argv[x]; ++x)
                {
                        if (!strcmp(argv[x], "--raw"))
                        {
                                rawmode = 1;
                        }
                        else if (!strcmp(argv[x], "--crlf"))
                        {
                                crlfmode = 1;
                        }
                        else if (argv[x][0] == '-')
                        {
                                fprintf(stderr, "Unknown option %s\n", argv[x]);
                                return -1;
                        }
                        else if (name)
                        {
                                fprintf(stderr, "Syntax error\n");
                                return -1;
                        }
                        else
                        {
                                name = argv[x];
                        }
                }

                if (!name) {
	                fprintf(stderr, "Missing file name to cat\n");
	                return -1;
	        } else {
	                cat(name, rawmode, crlfmode);
	                return 0;
	        }
        } else if (!strcmp(argv[x], "rm")) {
                char *name;
                ++x;
                if (x == argc) {
                        fprintf(stderr, "Missing name to delete\n");
                        return -1;
                } else {
                        name = argv[x];
                }
                return edos_rm(name);
        } else if (!strcmp(argv[x], "put")) {
                char *local_name = 0;
                char *mdos_name = 0;
	        char rawmode = 0;

                /* File write options */
                for (++x; argv[x]; ++x)
                {
                        if (!strcmp(argv[x], "--raw"))
                        {
                                rawmode = 1;
                        }
                        else if (argv[x][0] == '-')
                        {
                                fprintf(stderr, "Unknown option %s\n", argv[x]);
                                return -1;
                        }
                        else if (!local_name)
                        {
                                local_name = argv[x];
                        }
                        else if (!mdos_name)
                        {
                                mdos_name = argv[x];
                        }
                        else
                        {
                                fprintf(stderr, "Syntax error\n");
                                return -1;
                        }
                }
                if (!local_name) {
                        fprintf(stderr, "Missing file name to put\n");
                        return -1;
                }
                if (!mdos_name)
                        mdos_name = local_name;
                return put_file(local_name, mdos_name, rawmode);
        } else if (!strcmp(argv[x], "putexec")) {
                char *local_name = 0;
	        char rawmode = 0;

                /* File write options */
                for (++x; argv[x]; ++x)
                {
                        if (!strcmp(argv[x], "--raw"))
                        {
                                rawmode = 1;
                        }
                        else if (argv[x][0] == '-')
                        {
                                fprintf(stderr, "Unknown option %s\n", argv[x]);
                                return -1;
                        }
                        else if (!local_name)
                        {
                                local_name = argv[x];
                        }
                        else
                        {
                                fprintf(stderr, "Syntax error\n");
                                return -1;
                        }
                }
                if (!local_name) {
                        fprintf(stderr, "Missing file name to put\n");
                        return -1;
                }
                return put_file_lsn(26, local_name, 4 * TRACK_SIZE, 1, rawmode); /* Write up to 4 tracks */
	} else if (!strcmp(argv[x], "get")) {
	        char *mdos_name = 0;
	        char *local_name = 0;
	        char rawmode = 0;
	        char crlfmode = 0;

                /* File read options */
                for (++x; argv[x]; ++x)
                {
                        if (!strcmp(argv[x], "--raw"))
                        {
                                rawmode = 1;
                        }
                        else if (!strcmp(argv[x], "--crlf"))
                        {
                                crlfmode = 1;
                        }
                        else if (argv[x][0] == '-')
                        {
                                fprintf(stderr, "Unknown option %s\n", argv[x]);
                                return -1;
                        }
                        else if (!mdos_name)
                        {
                                mdos_name = argv[x];
                        }
                        else if (!local_name)
                        {
                                local_name = argv[x];
                        }
                        else
                        {
                                fprintf(stderr, "Syntax error\n");
                                return -1;
                        }
                }

                if (!mdos_name) {
                        fprintf(stderr, "Missing file name to get\n");
                        return -1;
                }
                if (!local_name)
                        local_name = mdos_name;
                return get_file(mdos_name, local_name, rawmode, crlfmode);
	} else if (!strcmp(argv[x], "getexec")) {
	        char *name = 0;
	        char rawmode = 0;
	        char crlfmode = 0;

                /* File read options */
                for (++x; argv[x]; ++x)
                {
                        if (!strcmp(argv[x], "--raw"))
                        {
                                rawmode = 1;
                        }
                        else if (!strcmp(argv[x], "--crlf"))
                        {
                                crlfmode = 1;
                        }
                        else if (argv[x][0] == '-')
                        {
                                fprintf(stderr, "Unknown option %s\n", argv[x]);
                                return -1;
                        }
                        else if (!name)
                        {
                                name = argv[x];
                        }
                        else
                        {
                                fprintf(stderr, "Syntax error\n");
                                return -1;
                        }
                }
                if (!name) {
                        fprintf(stderr, "Missing file name to get\n");
                        return -1;
                }
                return get_file_lsn(26, 104, name, rawmode, crlfmode);
        } else if (!strcmp(argv[x], "x")) {
                char local_name[80];
                char rawmode = 0;
                char crlfmode = 0;
                int sta = 0;

                /* File read options */
                for (++x; argv[x]; ++x)
                {
                        if (!strcmp(argv[x], "--raw"))
                        {
                                rawmode = 1;
                        }
                        else if (!strcmp(argv[x], "--crlf"))
                        {
                                crlfmode = 1;
                        }
                        else if (argv[x][0] == '-')
                        {
                                fprintf(stderr, "Unknown option %s\n", argv[x]);
                                return -1;
                        }
                        else
                        {
                                fprintf(stderr, "Syntax error\n");
                                return -1;
                        }
                }

                edos_load_dir();

                for (x = 0; x != name_n; ++x)
                {
                        int n;
                        printf("Extracting %s\n", names[x]->name);
                        sprintf(local_name, "%s", names[x]->name);
                        for (n = 1; n != 10; ++n)
                        {
                                FILE *f = fopen(local_name, "r");
                                if (f)
                                {
                                        fclose(f);
                                        sprintf(local_name, "%s.%d", names[x]->name, n);
                                }
                                else
                                        break;
                        }
                        if (n == 10)
                        {
                                printf("Couldn't extract '%s' at LSN=%d, too many files with same name\n", names[x]->name, names[x]->lsn);
                        }
                        else if (n != 1)
                        {
                                printf("File already exists, renamed to %s\n", local_name);
                        }
                        if (get_file_lsn(names[x]->lsn, names[x]->size, local_name, rawmode, crlfmode))
                        {
                                sta = -1;
                                printf("  failed reading file.\n");
                        }
                }
                return sta;
	} else {
	        fprintf(stderr, "Unknown command '%s'\n", argv[x]);
	        return -1;
	}
	return 0;
}
