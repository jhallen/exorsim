unsigned short assemble(unsigned char *mem, unsigned short addr, char *buf);

void show_syms(FILE *f);
void clr_syms(void);
char *find_label(unsigned short val);
