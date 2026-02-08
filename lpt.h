#ifndef _Ilpt
#define _Ilpt

void add_exordisk_ii_lpt(unsigned short addr);
void add_exordisk_i_lpt(unsigned short addr);
void close_lpt();
void open_lpt(char *name);
void append_lpt(char *name);

#endif
