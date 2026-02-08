#ifndef _Iexor
#define _Iexor

#include <stdio.h>

extern int trace_disk;

extern FILE *mon_out;
extern FILE *mon_in;

extern unsigned char mem[65536];
extern int lower;
extern int polling;

void simulated(unsigned short addr);
void parse_facts(FILE *f);

int load_drive(int n);
void set_drive(int n, const char *name);
void close_drive(int n);
void show_drive(int n);

void add_reader(unsigned short base, unsigned short offset, unsigned char (*func)(unsigned short));
void add_writer(unsigned short base, unsigned short offset, void (*func)(unsigned short addr, unsigned char data));
void add_jumper(unsigned short addr, int (*func)(unsigned short addr));


void add_acia_simple(unsigned short addr);
void add_acia_alias(unsigned short addr);
void add_acia(unsigned short addr);
void add_exbug_pia(unsigned short addr);
void add_swtpc_1771(unsigned short addr);
void add_swtpc_1771_ds(unsigned short addr);
void add_exordisk_ii(unsigned short addr);
void add_exordisk_i(unsigned short addr);

void add_rom(unsigned short low, unsigned short high);

#endif
