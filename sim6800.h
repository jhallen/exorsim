#ifndef _Isim6800
#define _Isim6800

/* Trace buffer size */
#define TRACESIZE 65536

/* Max no. of breakpoints */
#define NBREAKS 5

/* Controls */

extern int skip;
extern int trace;
extern int stop;
extern int reset;
extern int abrt;
extern int sp_stop;

/* Return size of instruction at specified address */

int insn_size(unsigned short addr);

extern unsigned short step_over;
extern int step_over_enable;
void set_break(unsigned short addr);
void list_breaks(void);

/* CPU registers */

extern unsigned char acca;
extern unsigned char accb;
#define accd ((acca << 8) + accb)
extern unsigned short pc;
extern unsigned short ix;
#ifdef M6809
extern unsigned short iy; /* 6809 */
extern unsigned short up; /* 6809 */
extern unsigned char dp; /* 6809 */
#endif
extern unsigned short sp;
extern unsigned char c_flag;
extern unsigned char v_flag;
extern unsigned char z_flag;
extern unsigned char n_flag;
extern unsigned char i_flag; /* 1=masked, 0=enabled */
extern unsigned char h_flag;
extern unsigned char f_flag; /* 6809 */
extern unsigned char e_flag; /* 6809 */

unsigned char read_flags();
void write_flags(unsigned char f);

/* Simulate */

void sim(void);

/* Dump trace buffer */
void show_traces(int n);

/* Provided externally */

void jump(unsigned short addr);
unsigned char mread(unsigned short addr);
void mwrite(unsigned short addr, unsigned char data);

void monitor(void);
int load_setup(const char *name);

#endif
