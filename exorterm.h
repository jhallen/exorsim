/* Initialize the EXORterm */
void izexorterm();

/* Handle a character sent to the terminal */
void term_out(int c);

/* Read from terminal */
int term_in();

/* Poll for user input, return true if there is any */
int quick_term_poll(); /* Actual OS poll only occasionally */
int term_poll();
