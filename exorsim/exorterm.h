#ifndef _Iexorterm
#define _Iexorterm

/* Initialize the EXORterm */
void izexorterm();

/* Handle a character sent to the terminal */
void term_out(int c);

/* Read from terminal */
int term_in();

/* Poll for user input, return true if there is any */
int quick_term_poll(); /* Actual OS poll only occasionally */
int term_poll();
void update();
extern int exorterm; /* Set to enable EXORTerm emulation */
extern int exbug_detected;
extern int lower; /* Allow lower case */
extern int polling;

void add_acia_simple(unsigned short addr);
void add_acia_alias(unsigned short addr);
void add_acia(unsigned short addr);
void add_exbug_pia(unsigned short addr);

#endif
