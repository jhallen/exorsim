/*	EXORcister simulator
 *	Copyright
 *		(C) 2011 Joseph H. Allen
 *
 * This is free software; you can redistribute it and/or modify it under the 
 * terms of the GNU General Public License as published by the Free Software 
 * Foundation; either version 1, or (at your option) any later version.  
 *
 * It is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more 
 * details.  
 * 
 * You should have received a copy of the GNU General Public License along with 
 * this software; see the file COPYING.  If not, write to the Free Software Foundation, 
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Line printer driver */

#include <stdio.h>
#include "exor.h"
#include "sim.h"
#include "exorterm.h"
#include "lpt.h"

static FILE *lpt_file; /* Line printer file */

/* Send character to printer */

static void lpt_out(unsigned char c)
{
        if (c)
        {
                if (lpt_file)
                {
                        fputc(c, lpt_file);
                        if (c == '\n')
                        {
                                fflush(lpt_file);
                        }
                }
                else
                {
                        term_out(c);
                }
        }
}

// Line printer driver in exordisk_ii PROM

static int lpt_lpinit(unsigned short addr)
{
        if (trace_disk) printf("LPINIT\n");
        c_flag = 0;
        return 1;
}

static int lpt_list(unsigned short addr)
{
        if (trace_disk) printf("LIST\n");
        lpt_out(acca);
        c_flag = 0;
        return 1;
}

static int lpt_ldata(unsigned short addr)
{
        if (trace_disk) printf("LDATA\n");
        while (mem[ix] != 4) {
                lpt_out(mem[ix]);
                ++ix;
        }
        lpt_out('\r');
        lpt_out('\n');
        c_flag = 0;
        return 1;
}

static int lpt_ldata1(unsigned short addr)
{
        if (trace_disk) printf("LDATA1\n");
        while (mem[ix] != 4) {
                lpt_out(mem[ix]);
                ++ix;
        }
        c_flag = 0;
        return 1;
}

void add_exordisk_ii_lpt(unsigned short addr)
{
        add_jumper(0xebc0, lpt_lpinit);
        add_jumper(0xebcc, lpt_list);
        add_jumper(0xebe4, lpt_ldata);
        add_jumper(0xebf2, lpt_ldata1);
}

void add_exordisk_i_lpt_char(unsigned short addr)
{
        add_jumper(addr, lpt_list);
}

void add_exordisk_i_lpt_line(unsigned short addr)
{
        add_jumper(addr, lpt_ldata1);
}

void add_exordisk_i_lpt_crlf_line(unsigned short addr)
{
        add_jumper(addr, lpt_ldata);
}

void close_lpt()
{
        if (lpt_file)
        {
                fclose(lpt_file);
                lpt_file = 0;
        }
}

void open_lpt(char *name)
{
        close_lpt();
        lpt_file = fopen(name, "w");
        if (lpt_file)
                fprintf(stderr, "Line printer file %s opened\n", name);
        else
                fprintf(stderr, "Couldn't open lpt file %s\n", name);
}

void append_lpt(char *name)
{
        close_lpt();
        lpt_file = fopen(name, "a");
        if (lpt_file)
                fprintf(stderr, "Line printer file %s opened\n", name);
        else
                fprintf(stderr, "Couldn't open lpt file %s\n", name);
}
