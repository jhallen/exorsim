#ifndef _Iexor
#define _Iexor

#include <stdio.h>

extern int trace_disk;

extern FILE *mon_out;
extern FILE *mon_in;

extern unsigned char mem[65536];

void simulated(unsigned short addr);
void parse_facts(FILE *f);

void add_reader(unsigned short base, unsigned short offset, unsigned char (*func)(unsigned short));
void add_writer(unsigned short base, unsigned short offset, void (*func)(unsigned short addr, unsigned char data));
void add_jumper(unsigned short addr, int (*func)(unsigned short addr));
void add_rom(unsigned short low, unsigned short high);

#endif
