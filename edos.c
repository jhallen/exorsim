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
    unsigned char mark; /* 0xFF if deleted, 0x00 otherwise. Some files have 0x80? */
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

int lower(int c)
{
        if (c >= 'A' && c <= 'Z')
                return c - 'A' + 'a';
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

void read_file(int lsn, int size, FILE *f)
{
        unsigned char buf[SECTOR_SIZE];
        int x;
        int guess_type = 0;
        for (x = lsn; x != lsn + size; ++x)
        {
                getsect(buf, x);

                if (x == lsn && buf[0] == 'D')
                        guess_type = 1; /* Object file- don't delete NULs */

                if (guess_type)
                {
                        /* Not ASCII! */
                        if (x + 1 == lsn + size)
                        {
                                int y;
                                /* Last sector of file- delete trailing NULs */
                                for (y = SECTOR_SIZE - 1; y && !buf[y]; --y);
                                if (y)
                                        fwrite(buf, y + 1, 1, f);
                        }
                        else
                        {
                                fwrite(buf, SECTOR_SIZE, 1, f);
                        }
                }
                else
                {
                        /* ASCII */
                        int ends = SECTOR_SIZE;
                        int n;
                        if (x + 1 == lsn + size)
                        {
                                /* Last sector, delete trailing NULs */
                                while (ends && buf[ends-1] == 0) --ends;
                        }
                        for (n = 0; n != ends; ++n) {
                                int c = buf[n];
                                if (c == 13) {
                                        /* Convert to UNIX */
                                        fputc('\n', f);
                                } else if (c == 10) {
                                        /* Delete Line Feeds */
                                } else if (c == 0) {
                                        /* Delete NULs- not sure this is a good idea */
                                } else {
                                        fputc(c, f);
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
                        /* if (d->mark != 0xff && d->name[0] != '*') { */
                        if (d->name[0] >= 'A' && d->name[0] <= 'Z') {
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
        qsort(names, name_n, sizeof(struct name *), (int (*)(const void *, const void *))comp);
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

void cat(char *name)
{
        int size;
        int lsn = find_file(name, &size);
        if (lsn == -1) {
                printf("File '%s' not found\n", name);
                exit(-1);
        } else {
                read_file(lsn, size, stdout);
        }
}

/* get a file from the disk */

int get_file_lsn(int lsn, int size, char *local_name)
{
        FILE *f = fopen(local_name, "w");
        if (!f) {
                printf("Couldn't open local file '%s'\n", local_name);
                return -1;
        }
        read_file(lsn, size, f);
        if (fclose(f)) {
                printf("Couldn't close local file '%s'\n", local_name);
                return -1;
        }
        return 0;
}

int get_file(char *mdos_name, char *local_name)
{
        int size;
        int lsn = find_file(mdos_name, &size);
        if (lsn == -1) {
                fprintf(stderr, "File '%s' not found\n", mdos_name);
                return -1;
        } else {
                return get_file_lsn(lsn, size, local_name);
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
                printf("      ls [-la1A]                    Directory listing\n");
                printf("                  -l for long\n");
                printf("                  -1 to show a single name per line\n");
                printf("      cat mdos-name                 Type file to console\n");
                printf("      get mdos-name [local-name]    Copy file from diskette to local-name\n");
                printf("      x                             Extract all files into current directory\n");
                printf("\n");
                return -1;
	}
	disk_name = argv[x++];
	disk = fopen(disk_name, "r+");
	if (!disk) {
	        printf("Couldn't open '%s'\n", disk_name);
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
                                        default: printf("Unknown option '%c'\n", opt); return -1;
                                }
                        }
                        ++x;
                }
	        edos_dir(full, single);
                return 0;
	} else if (!strcmp(argv[x], "cat")) {
	        ++x;
	        if (x == argc) {
	                printf("Missing file name to cat\n");
	                return -1;
	        } else {
	                cat(argv[x++]);
	                return 0;
	        }
	} else if (!strcmp(argv[x], "get")) {
                char *local_name;
                char *mdos_name;
                ++x;
                if (x == argc) {
                        printf("Missing file name to get\n");
                        return -1;
                }
                mdos_name = argv[x];
                local_name = mdos_name;
                if (x + 1 != argc)
                        local_name = argv[++x];
                return get_file(mdos_name, local_name);
        } else if (!strcmp(argv[x], "x")) {
                char local_name[80];
                edos_load_dir();
                int x;
                int sta = 0;
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
                        if (get_file_lsn(names[x]->lsn, names[x]->size, local_name))
                        {
                                sta = -1;
                                printf("  failed reading file.\n");
                        }
                }
                return sta;
	} else {
	        printf("Unknown command '%s'\n", argv[x]);
	        return -1;
	}
	return 0;
}
