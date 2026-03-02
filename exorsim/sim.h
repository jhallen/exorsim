#ifndef _Isim6800
#define _Isim6800

/* Trace buffer size */
#define TRACESIZE 65536

/* Max no. of breakpoints */
#define NBREAKS 5

/* CPU type */

extern int cputype;
#define M6800 0
#define M6809 1

/* Controls */

extern int skip;
extern int trace;
extern int stop;
extern int reset;
extern int abrt;
extern int sp_stop;
extern int sp_over;

/* Return true if instruction at address is jump to subroutine or SWI */

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
extern unsigned short iy; /* 6809 */
extern unsigned short up; /* 6809 */
extern unsigned char dp; /* 6809 */
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
