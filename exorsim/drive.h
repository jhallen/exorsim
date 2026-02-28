#ifndef _Idrive
#define _Idrive

extern int swtpc;

struct drive_info {
        const char *name;
        FILE *f;
        int bytes; /* Bytes per sector */
        int tracks; /* Tracks per disk */
        int sects; /* Sectors per track */
};

extern struct drive_info drive[4];

void add_swtpc_1771(unsigned short addr);
void add_swtpc_1771_ds(unsigned short addr);
void add_exordisk_ii(unsigned short addr);
void add_exordisk_i(unsigned short addr);

void close_drive(int n);
void show_drive(int n);
void set_drive(int n, const char *name);
int load_drive(int n);

#endif
