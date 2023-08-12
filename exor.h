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
