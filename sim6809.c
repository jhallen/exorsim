/*	M6800 Simulator
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exor.h"
#include "unasm6800.h"
#include "asm6800.h"
#include "sim6800.h"

int skip = 0; /* Skip first nn instructions in trace */
int trace = 0; /* Enable instruction trace */
int stop; /* Stop flag */
int reset; /* User hit reset */
int abrt; /* User hit abort */
int sp_stop;

/* Breakpoint */
int mybrk;
unsigned short brk_addr;

/* CPU registers */
unsigned char acca;
unsigned char accb;
unsigned short ireg[4]; /* Index registers */
unsigned short pc;
unsigned char dp;
unsigned char c_flag;
unsigned char v_flag;
unsigned char z_flag;
unsigned char n_flag;
unsigned char i_flag; /* 1=masked, 0=enabled */
unsigned char h_flag;
unsigned char f_flag; /* FIRQ mask */
unsigned char e_flag; /* All registers saved if set */

unsigned char read_flags()
{
	return (c_flag + (v_flag << 1) + (z_flag << 2) + (n_flag << 3) + (i_flag << 4) + (h_flag << 5) + (f_flag << 6) + (e_flag << 7));
}

void write_flags(unsigned char f)
{
	c_flag = (f & 1);
	v_flag = ((f >> 1) & 1);
	z_flag = ((f >> 2) & 1);
	n_flag = ((f >> 3) & 1);
	i_flag = ((f >> 4) & 1);
	h_flag = ((f >> 5) & 1);
	f_flag = ((f >> 6) & 1);
	e_flag = ((f >> 7) & 1);
}

/* Trace buffer */

int trace_idx;

#define TRACESIZE 128

/* Machine state before this instruction */

struct trace_entry
{
	unsigned char acca;
	unsigned char accb;
	unsigned short ireg[4];
	unsigned short pc; /* Address of instruction */
	unsigned char dp;
	unsigned char cc;
	unsigned short ea; /* Effective address */
	unsigned short data; /* Data read or written to effective address */
	unsigned char insn[5]; /* Copy of instruction */
	unsigned short status; /* Bit 0 set: instruction completed normally.  Bit 1 set: subroutine handled by simulator */
} trace_buf[TRACESIZE];

void record_trace(struct trace_entry *t)
{
	t->acca = acca;
	t->accb = accb;
	t->ireg[0] = ireg[0];
	t->ireg[1] = ireg[1];
	t->ireg[2] = ireg[2];
	t->ireg[3] = ireg[3];
	t->pc = pc;
	t->dp = dp;
	t->cc = read_flags();
	t->insn[0] = mread(pc);
	t->insn[1] = mread(pc + 1);
	t->insn[2] = mread(pc + 2);
	t->insn[3] = mread(pc + 3);
	t->insn[4] = mread(pc + 4);
	t->ea = 0;
	t->data = 0;
	t->status = 0;
}

unsigned short mread2(unsigned short addr)
{
	return (mread(addr) << 8) + mread(addr + 1);
}

/* Fetch instruction byte, record copy in trace entry */

unsigned char fetch()
{
        unsigned char c = mread(pc++);
	return c;
}

unsigned short fetch2()
{
        unsigned short v;
        v = fetch();
        v = (v << 8) + fetch();
        return v;
}

void mwrite2(unsigned short addr, unsigned short data)
{
	mwrite(addr, (data >> 8));
	mwrite(addr + 1, (data & 0xFF));
}

void pushs(unsigned char data)
{
	mwrite(--sp, data);
}

void pushu(unsigned char data)
{
	mwrite(--up, data);
}

unsigned char pull()
{
	return mread(sp++);
}

unsigned char pullu()
{
	return mread(up++);
}

void pushs2(unsigned short data)
{
	pushs(data & 0xFF);
	pushs(data >> 8);
}

unsigned short pull2()
{
	unsigned short w;
	w = pull();
	w = (w << 8) + pull();
	return w;
}

void pushu2(unsigned short data)
{
	pushu(data & 0xFF);
	pushu(data >> 8);
}

unsigned short pullu2()
{
	unsigned short w;
	w = pullu();
	w = (w << 8) + pullu();
	return w;
}

/* TFR get/put */

int tfr_get(unsigned short *rtn, int r)
{
	switch (r) {
		case 0x0: *rtn = accd; return 0;
		case 0x1: *rtn = ix; return 0;
		case 0x2: *rtn = iy; return 0;
		case 0x3: *rtn = up; return 0;
		case 0x4: *rtn = sp; return 0;
		case 0x5: *rtn = pc; return 0;
		case 0x8: *rtn = acca; return 0;
		case 0x9: *rtn = accb; return 0;
		case 0xA: *rtn = read_flags(); return 0;
		case 0xB: *rtn = dp; return 0;
	}
	return -1;
}

int tfr_put(int r, unsigned short val)
{
	switch (r) {
		case 0x0: acca = (val >> 8); accb = val; return 0;
		case 0x1: ix = val; return 0;
		case 0x2: iy = val; return 0;
		case 0x3: up = val; return 0;
		case 0x4: sp = val; return 0;
		case 0x5: jump(val); return 0;
		case 0x8: acca = val; return 0;
		case 0x9: accb = val; return 0;
		case 0xA: write_flags(val); return 0;
		case 0xB: dp = val; return 0;
	}
	return -1;
}

int tfr_name(char **name, int r)
{
	switch (r) {
		case 0x0: *name = "D"; return 0;
		case 0x1: *name = "X"; return 0;
		case 0x2: *name = "Y"; return 0;
		case 0x3: *name = "U"; return 0;
		case 0x4: *name = "S"; return 0;
		case 0x5: *name = "PC"; return 0;
		case 0x8: *name = "A"; return 0;
		case 0x9: *name = "B"; return 0;
		case 0xA: *name = "CC"; return 0;
		case 0xB: *name = "DP"; return 0;
	}
	return -1;
}

/* Push all registers */

void push_all()
{
	e_flag = 1; /* Set e_flag so that RTI knows to restore all regs */
	pushs2(pc);
	pushs2(up);
	pushs2(iy);
	pushs2(ix);
	pushs(dp);
	pushs2(accd);
	pushs(read_flags());
}

/* Process indexed mode post byte, return effective address */

int ind(unsigned short *rtn)
{
	unsigned char i = fetch(); /* Fetch index byte */
	unsigned char ind = (i & 0x10); /* Indirection bit */
	unsigned char o = (i & 0x0f); /* Index type / low bits of 5-bit offset */
	int r = (3 & (i >> 5)); /* Index register number */
	unsigned short ea;

	if (!(i & 0x80))
	{ /* 5-bit signed offset */
		ea = ireg[r];
		if (ind) {
			*rtn = ea + o + 0xFFF0;
			return 0;
		} else {
			*rtn = ea + o;
			return 0;
		}
	}
	else
	{
		switch (o)
		{
			case 0x00: /* ,R+ */
			{
				ea = ireg[r];
				++ireg[r];
				if (ind) /* Not valid!? */
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x01: /* ,R++ */
			{
				ea = ireg[r];
				ireg[r] += 2;
				if (ind)
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x02: /* ,-R */
			{
				--ireg[r];
				ea = ireg[r];
				if (ind) /* Not valid!? */
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x03: /* ,--R */
			{
				ireg[r] -= 2;
				ea = ireg[r];
				if (ind)
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x04: /* ,R */
			{
				ea = ireg[r];
				if (ind)
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x05: /* B,R */
			{
				ea = ireg[r];
				ea += (signed char)accb;
				if (ind)
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x06: /* A,R */
			{
				ea = ireg[r];
				ea += (signed char)acca;
				if (ind)
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x08: /* Offset8,R */
			{
				ea = ireg[r];
				ea += (signed char)fetch();
				if (ind)
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x09: /* Offset16,R */
			{
				ea = ireg[r];
				ea += fetch2();
				if (ind)
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x0B: /* D,R */
			{
				ea = ireg[r] + accd;
				if (ind)
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x0C: /* Offset8,PC */
			{
				unsigned char o = fetch();
				ea = pc + (signed char)o;
				if (ind)
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x0D: /* Offset16,PC */
			{
				unsigned short o = fetch2();
				ea = pc + o;
				if (ind)
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
			case 0x0F: /* Offset16 */
			{
				unsigned short ea = fetch2();
				if (ind) /* Must be true here? */
					ea = mread2(ea);
				*rtn = ea;
				return 0;
			}
		}
	}
	/* Unknown if we get here... */
	return -1;
}

/* Show indexed */

char *regname[]=
{
	"X", "Y", "U", "S"
};

int show_ind(struct trace_entry *t, int *x, char *buf, char *right)
{
	unsigned char i = t->insn[*x]; /* Fetch index byte */
	unsigned char ind = (i & 0x10); /* Indirection bit */
	unsigned char o = (i & 0x0f); /* Index type / low bits of 5-bit offset */
	int r = (3 & (i >> 5)); /* Index register number */

	sprintf(buf + strlen(buf), " %2.2X", i);
	*x = *x + 1;

	if (!(i & 0x80))
	{ /* 5-bit signed offset */
		if (ind) {
			int v = -16 + o;
			sprintf(right, "%d,%s", v, regname[r]);
			return 0;
		} else {
			sprintf(right, "%d,%s", o, regname[r]);
			return 0;
		}
	}
	else
	{
		switch (o)
		{
			case 0x00: /* ,R+ */
			{
				if (ind)
					sprintf(right, "[,%s+]", regname[r]);
				else
					sprintf(right, ",%s+", regname[r]);
				return 0;
			}
			case 0x01: /* ,R++ */
			{
				if (ind)
					sprintf(right, "[,%s+]+", regname[r]);
				else
					sprintf(right, ",%s++", regname[r]);
				return 0;
			}
			case 0x02: /* ,-R */
			{
				if (ind)
					sprintf(right, "[,-%s]", regname[r]);
				else
					sprintf(right, ",-%s", regname[r]);
				return 0;
			}
			case 0x03: /* ,--R */
			{
				if (ind)
					sprintf(right, "[,--%s]", regname[r]);
				else
					sprintf(right, ",--%s", regname[r]);
				return 0;
			}
			case 0x04: /* ,R */
			{
				if (ind)
					sprintf(right, "[,%s]", regname[r]);
				else
					sprintf(right, ",%s", regname[r]);
				return 0;
			}
			case 0x05: /* B,R */
			{
				if (ind)
					sprintf(right, "[B,%s]", regname[r]);
				else
					sprintf(right, "B,%s", regname[r]);
				return 0;
			}
			case 0x06: /* A,R */
			{
				if (ind)
					sprintf(right, "[A,%s]", regname[r]);
				else
					sprintf(right, "A,%s", regname[r]);
				return 0;
			}
			case 0x08: /* Offset8,R */
			{
				unsigned short v = (signed char)t->insn[*x];
				sprintf(buf + strlen(buf), " %2.2X", t->insn[*x]);
				*x = *x + 1;
				if (ind)
					sprintf(right, "[%d,%s]", (signed short)v, regname[r]);
				else
					sprintf(right, "%d,%s", (signed short)v, regname[r]);
				return 0;
			}
			case 0x09: /* Offset16,R */
			{
				unsigned short v = (t->insn[*x] << 8);
				sprintf(buf + strlen(buf), " %2.2X", t->insn[*x]);
				*x = *x + 1;
				v += t->insn[*x];
				sprintf(buf + strlen(buf), "%2.2X", t->insn[*x]);
				*x = *x + 1;
				if (ind)
					sprintf(right, "[%d,%s]", (signed short)v, regname[r]);
				else
					sprintf(right, "%d,%s", (signed short)v, regname[r]);
				return 0;
			}
			case 0x0B: /* D,R */
			{
				if (ind)
					sprintf(right, "[D,%s]", regname[r]);
				else
					sprintf(right, "D,%s", regname[r]);
				return 0;
			}
			case 0x0C: /* Offset8,PC */
			{
				unsigned short v = (signed char)t->insn[*x];
				sprintf(buf + strlen(buf), " %2.2X", t->insn[*x]);
				*x = *x + 1;
				if (ind)
	 				sprintf(right, "[$%x,PCR]", (unsigned short)(t->pc + *x + v));
				else
	 				sprintf(right, "$%x,PCR", (unsigned short)(t->pc + *x + v));
				return 0;
			}
			case 0x0D: /* Offset16,PC */
			{
				unsigned short v = (t->insn[*x] << 8);
				sprintf(buf + strlen(buf), " %2.2X", t->insn[*x]);
				*x = *x + 1;
				v += t->insn[*x];
				sprintf(buf + strlen(buf), "%2.2X", t->insn[*x]);
				*x = *x + 1;
				if (ind)
	 				sprintf(right, "[$%x,PCR]", (unsigned short)(t->pc + *x + v));
				else
	 				sprintf(right, "$%x,PCR", (unsigned short)(t->pc + *x + v));
				return 0;
			}
			case 0x0F: /* Offset16 */
			{
				unsigned short v = (t->insn[*x] << 8);
				sprintf(buf + strlen(buf), " %2.2X", t->insn[*x]);
				*x = *x + 1;
				v += t->insn[*x];
				sprintf(buf + strlen(buf), "%2.2X", t->insn[*x]);
				*x = *x + 1;
				if (ind)
					sprintf(right, "[$%4.4x]", v);
				else
					sprintf(right, "$%4.4x", v);
				return 0;
			}
		}
	}
	/* Unknown if we get here... */
	return -1;
}

/* Show register list */

void show_list(int us, char *right, unsigned char cb)
{
	int first = 0;
	if (cb & 0x80) { strcat(right, first ? ",PC" : "PC"); first = 1; }
	if (cb & 0x40) {
		if (us)
			{ strcat(right, first ? ",S" : "S"); first = 1; }
		else
			{ strcat(right, first ? ",U" : "U"); first = 1; }
	}
	if (cb & 0x20) { strcat(right, first ? ",Y" : "Y"); first = 1; }
	if (cb & 0x10) { strcat(right, first ? ",X" : "X"); first = 1; }
	if (cb & 0x08) { strcat(right, first ? ",DP" : "DP"); first = 1; }
	if (cb & 0x04) { strcat(right, first ? ",B" : "B"); first = 1; }
	if (cb & 0x02) { strcat(right, first ? ",A" : "A"); first = 1; }
	if (cb & 0x01) { strcat(right, first ? ",CC" : "CC"); first = 1; }
}

#define IMM() (fetch(), (pc - 1))

#define IMM2() (fetch2(), (pc - 2))

#define DIR() ((dp << 8) + fetch())

#define EXT() (fetch2())

/* Macros which update flags following an arithmetic or logical operation */

#define Z_16(n) ((~((n) | -(n)) >> 15) & 1)
#define Z(n) (( ~((n) | -(n)) >> 7) & 1)

#define N_16(n) ((n >> 15) & 1)
#define N(n) ((n >> 7) & 1)

#define V_16(a,b,f) (((((f) ^ (a)) & ((f) ^ (b))) >> 15) & 1)
#define V(a,b,f) (((((f) ^ (a)) & ((f) ^ (b))) >> 7) & 1)

#define V_16_SUB(a,b,f) (((((a) ^ (b)) & ((f) ^ (a))) >> 15) & 1)
#define V_SUB(a,b,f) (((((a) ^ (b)) & ((f) ^ (a))) >> 7) & 1)

#define C_16(a,b,f) (((((a) & (b)) | (((a) | (b)) & ~(f))) >> 15) & 1)
#define C(a,b,f) (((((a) & (b)) | (((a) | (b)) & ~(f))) >> 7) & 1)

#define C_16_SUB(a,b,f) ((((~(a) & (b)) | ((~(a) | (b)) & (f))) >> 15) & 1)
#define C_SUB(a,b,f) ((((~(a) & (b)) | ((~(a) | (b)) & (f))) >> 7) & 1)

#define H(a,b,f) ((((f) ^ (a) ^ (b)) >> 4) & 1)

/* Print one trace line */

void show_trace(int insn_no, struct trace_entry *t)
{
	int x;
        char *fact_label;
        char *fact_comment;
        char *ea_label;

        char buf_ea[80];
	char buf1[80];
	char buf[80]; /* Address and fetched data */
	char right[20]; /* Address mode / operand */
	char buf3[120]; /* Effective address and data */
	char *insn = "Huh?";
	char *left = ""; /* Left operand, like the 'D' in ADDD */
	int subr = 0;
	right[0] = 0;
	buf3[0] = 0;
	buf[0] = 0;

	/* Any facts about this address? */
	fact_label = find_label(t->pc);
	fact_comment = "";
	if (!fact_label) {
                if (facts[t->pc]) {
                        fact_label = facts[t->pc]->label;
                        fact_comment = facts[t->pc]->comment;
                } else {
                        fact_label = "";
                }
        }

        /* About the effective address? */

        ea_label = find_label(t->ea);
        if (!ea_label && facts[t->ea])
                ea_label = facts[t->ea]->label;
	if (ea_label)
	        sprintf(buf_ea, "(%s)", ea_label);
        else
                buf_ea[0] = 0;

	if (t->status == 2) {
                if (insn_no >= skip) {
                        fprintf(mon_out, " %10d ---- Subroutine at %4.4X processed by simulator ---- RTS executed ---\n\n",
                               insn_no, t->pc);
                        return;
                }
	}

	/* PC */
	x = 0;
	sprintf(buf,"%4.4X: %2.2X", t->pc, t->insn[x]);

	/* Decode instruction: */
	/* Set 'insn' to instruction mnemonic */
        /* Set operand to its operand */

	/* Fetch op-code, deal with page2/page3 */
	unsigned char opcode = t->insn[x++];
	int page2 = 0;
	int page3 = 0;
	if (opcode == 0x10) {
		page2 = 1;
		sprintf(buf + strlen(buf), "%2.2X", t->insn[x]);
		opcode = t->insn[x++];
	} else if (opcode == 0x11) {
		page3 = 1;
		sprintf(buf + strlen(buf), "%2.2X", t->insn[x]);
		opcode = t->insn[x++];
	}

	if (opcode & 0x80) { /* R,M operations */
		unsigned char lowop = (opcode & 0x0F);
		int sixteen = 0; /* Assume 8-bit */

		/* Get operand A */

		if (opcode & 0x40) {
			if (lowop == 0x3 || lowop == 0xC || lowop == 0xD) {
				left = "D";
				sixteen = 1;
			} else if (lowop == 0xE || lowop == 0xF) {
				sixteen = 1;
				if (page2) {
					left = "S";
				} else {
					left = "U";
				}
			} else {
				left = "B";
			}
		} else {
			if (lowop == 0x3) {
				sixteen = 1;
				if (page3) {
					left = "U";
				} else {
					left = "D";
				}
			} else if (lowop == 0xC) {
				sixteen = 1;
				if (page2) {
					left = "Y";
				} else if (page3) {
					left = "S";
				} else {
					left = "X";
				}
			} else if (lowop == 0x0E || lowop == 0x0F) {
				sixteen = 1;
				if (page2) {
					left = "Y";
				} else {
					left = "X";
				}
			} else {
				/* will get here for lowop == 0x0D, but it's OK */
				left = "A";
			}
		}

		/* Get operand B's effective address */

		switch (opcode & 0x30) {
			case 0x00: {
				if (sixteen) {
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "#$%4.4x", (t->insn[x] << 8) + t->insn[x + 1]);
					x += 2;
				} else {
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "#$%2.2x", t->insn[x]);
					x += 1;
				}
				break;
			} case 0x10: {
				sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
				sprintf(right, "$%2.2x", t->insn[x++]);
				break;
			} case 0x20: {
				if (show_ind(t, &x, buf, right))
					goto invalid;
				break;
			} case 0x30: {
				sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
				sprintf(right, "$%4.4x", (t->insn[x] << 8) + t->insn[x + 1]);
				x += 2;
				break;
			}
		}

		/* Operate F = A op B */
		/* If A doesn't change, then we must F = A */

		switch (opcode & 0x0F) {
			case 0x00: /* SUB N,Z,V,C,H) */ {
				insn = "SUB";
				break;
			} case 0x01: /* CMP N,Z,V,C,H */ {
				insn = "CMP";
				break;
			} case 0x02: /* SBC N,Z,V,C,H */ {
				insn = "SBC";
				break;
			} case 0x03: /* SUBD/CMPD/CMPU or ADDD */ {
				if (opcode & 0x40) {
					insn = "ADD";
				} else {
					if (page3 || page2)
						insn = "CMP";
					else
						insn = "SUB";
				}
				break;
			} case 0x04: /* AND N,Z,V=0 */ {
				insn = "AND";
				break;
			} case 0x05: /* BIT N,Z,V=0 */ {
				insn = "BIT";
				break;
			} case 0x06: /* LDA N,Z,V=0 */ {
				insn = "LD";
				break;
			} case 0x07: /* STA N,Z,V=0 */ {
				insn = "ST";
				break;
			} case 0x08: /* EOR N,Z,V=0*/ {
				insn = "EOR";
				break;
			} case 0x09: /* ADC H,N,Z,V,C */ {
				insn = "ADC";
				break;
			} case 0x0A: /* ORA N,Z,V=0 */ {
				insn = "ORA";
				break;
			} case 0x0B: /* ADD H,N,Z,V,C */ {
				insn = "ADD";
				break;
			} case 0x0C: {
				if (opcode & 0x40) { /* LDD */
					insn = "LD";
				} else { /* CMPX/CMPY/CMPS */
					insn = "CMP";
				}
				break;
			} case 0x0D: {
				if (opcode & 0x40) { /* STD */
					insn = "ST";
				} else { /* BSR/JSR */
					if (!(opcode & 0x30)) { /* BSR */
						insn = "BSR";
						/* Replace immediate operand with branch target */
						sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x-1]));
						left = "";
					} else {
						insn = "JSR";
						left = "";
					}
				}
				break;
			} case 0x0E: { /* LDX/LDY/LDU/LDS */
				insn = "LD";
				break;
			} case 0x0F: { /* STX/STY/STU/STS */
				insn = "ST";
				break;
			}
		}

	} else if ((opcode & 0x40) || !(opcode & 0xF0)) { /* R.M.W. operations */

		/* Fetch load operand */

		if (!(opcode & 0xF0)) {
			sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
			sprintf(right, "$%2.2x", t->insn[x++]);
		} else {
			switch (opcode & 0x30) {
				case 0x00: {
					left = "A";
					break;
				} case 0x10: {
					left = "B";
					break;
				} case 0x20: {
					if (show_ind(t, &x, buf, right))
						goto invalid;
					break;
				} case 0x30: {
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x+1]);
					sprintf(right, "$%4.4x", (t->insn[x] << 8) + t->insn[x+1]);
					break;
				}
			}
		}

		/* Operate */

		switch (opcode & 0x0F) {
			case 0x00: /* NEG N, Z, V= ((f==0x80)?1:0), C= ((f==0)?1:0) */ {
				insn = "NEG";
				break;
			} case 0x01: /* ??? */ {
				goto invalid;
				break;
			} case 0x02: /* ??? */ {
				goto invalid;
				break;
			} case 0x03: /* COM N, Z, V=0, C=1 */ {
				insn = "COM";
				break;
			} case 0x04: /* LSR N=0,Z,C,V=N^C */ {
				insn = "LSR";
				break;
			} case 0x05: /* ??? */ {
				goto invalid;
				break;
			} case 0x06: /* ROR N,Z,C,V=N^C */ {
				insn = "ROR";
				break;
			} case 0x07: /* ASR N,Z,C,V=N^C */ {
				insn = "ASR";
				break;
			} case 0x08: /* ASL N,Z,C,V=N^C */ {
				insn = "ASL";
				break;
			} case 0x09: /* ROL N,Z,C,V=N^C */ {
				insn = "ROL";
				break;
			} case 0x0A: /* DEC N,Z,V = (a=0x80?1:0) */ {
				insn = "DEC";
				break;
			} case 0x0B: /* ??? */ {
				goto invalid;
				break;
			} case 0x0C: /* INC N,Z,V = (a=0x7F?1:0) */ {
				insn = "INC";
				break;
			} case 0x0D: /* TST N,Z,V=0,C=0 */ {
				insn = "TST";
				break;
			} case 0x0E: /* JMP */ {
				insn = "JMP";
				break;
			} case 0x0F: /* CLR N=0, Z=1, V=0, C=0 */ {
				insn = "CLR";
				break;
			}
		}
	} else {
		switch(opcode) {
			case 0x12: { /* NOP */
				insn = "NOP";
				break;
			} case 0x13: { /* SYNC */
				insn = "SYNC";
				break;
			} case 0x16: { /* LBRA */
				insn = "LBRA";
				sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
				sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x + 1]));
				x += 2;
				break;
			} case 0x17: { /* LBSR */
				insn = "LBSR";
				sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
				sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x + 1]));
				x += 2;
				break;
			} case 0x19: { /* DAA N,Z,V,C */
				insn = "DAA";
				break;
			} case 0x1A: { /* ORCC */
				insn = "ORCC";
				sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
				sprintf(right, "#$%2.2x", t->insn[x++]);
				break;
			} case 0x1C: { /* ANDCC */
				insn = "ANDCC";
				sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
				sprintf(right, "#$%2.2x", t->insn[x++]);
				break;
			} case 0x1D: { /* SEX */
				insn = "SEX";
				break;
			} case 0x1E: { /* EXG */
				char *n;
				insn = "EXG";
				sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
				if (tfr_name(&n, t->insn[x] >> 4))
					goto invalid;
				strcat(right, n);
				if (tfr_name(&n, t->insn[x] & 0xF))
					goto invalid;
				x++;
				strcat(right, ",");
				strcat(right, n);
				break;
			} case 0x1F: { /* TFR */
				char *n;
				insn = "TFR";
				sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
				if (tfr_name(&n, t->insn[x] >> 4))
					goto invalid;
				strcat(right, n);
				if (tfr_name(&n, t->insn[x] & 0xF))
					goto invalid;
				x++;
				strcat(right, ",");
				strcat(right, n);
				break;
			} case 0x30: { /* LEAX */
				insn = "LEAX";
				if (show_ind(t, &x, buf, right))
					goto invalid;
				break;
			} case 0x31: { /* LEAY */
				insn = "LEAY";
				if (show_ind(t, &x, buf, right))
					goto invalid;
				break;
			} case 0x32: { /* LEAS */
				insn = "LEAS";
				if (show_ind(t, &x, buf, right))
					goto invalid;
				break;
			} case 0x33: { /* LEAU */
				insn = "LEAU";
				if (show_ind(t, &x, buf, right))
					goto invalid;
				break;
			} case 0x34: { /* PSHS */
				insn = "PSHS";
				show_list(0, right, t->insn[x++]);
				break;
			} case 0x35: { /* PULS */
				insn = "PULS";
				show_list(0, right, t->insn[x++]);
				break;
			} case 0x36: { /* PSHU */
				insn = "PSHU";
				show_list(1, right, t->insn[x++]);
				break;
			} case 0x37: { /* PULU */
				insn = "PULU";
				show_list(1, right, t->insn[x++]);
				break;
			} case 0x39: { /* RTS */
				insn = "RTS";
				break;
			} case 0x3A: { /* ABX */
				insn = "ABX";
				break;
			} case 0x3B: { /* RTI */
				insn = "RTI";
				break;
			} case 0x3C: { /* CWAI */
				insn = "CWAI";
				break;
			} case 0x3D: { /* MUL */
				insn = "MUL";
				break;
			} case 0x3F: { /* SWI */
				if (page2)
					insn = "SWI2";
				else if (page3)
					insn = "SWI3";
				else {
					insn = "SWI";
				}
				break;
			} case 0x20: /* BRA */ {
				if (page2) {
					insn = "LBRA";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BRA";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x21: /* BRN */ {
				if (page2) {
					insn = "LBRN";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BRN";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x22: /* BHI */ {
				if (page2) {
					insn = "LBHI";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BHI";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x23: /* BLS */ {
				if (page2) {
					insn = "LBLS";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BLS";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x24: /* BCC */ {
				if (page2) {
					insn = "LBCC";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BCC";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x25: /* BCS */ {
				if (page2) {
					insn = "LBCS";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BCS";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x26: /* BNE */ {
				if (page2) {
					insn = "LBNE";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BNE";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x27: /* BEQ */ {
				if (page2) {
					insn = "LBEQ";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BEQ";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x28: /* BVC */ {
				if (page2) {
					insn = "LBVC";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BVC";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, " $%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x29: /* BVS */ {
				if (page2) {
					insn = "LBVS";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BVS";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x2A: /* BPL */ {
				if (page2) {
					insn = "LBPL";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BPL";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x2B: /* BMI */ {
				if (page2) {
					insn = "LBMI";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BMI";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x2C: /* BGE */ {
				if (page2) {
					insn = "LBGE";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BGE";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x2D: /* BLT */ {
				if (page2) {
					insn = "LBLT";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BLT";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x2E: /* BGT */ {
				if (page2) {
					insn = "LBGT";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BGT";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} case 0x2F: /* BLE */ {
				if (page2) {
					insn = "LBLE";
					sprintf(buf + strlen(buf), " %2.2X%2.2X", t->insn[x], t->insn[x + 1]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 3 + (t->insn[x] << 8) + t->insn[x+1]));
					x+=2;
				} else {
					insn = "BLE";
					sprintf(buf + strlen(buf), " %2.2X", t->insn[x]);
					sprintf(right, "$%4.4x", 0xFFFF & (t->pc + 2 + (signed char)t->insn[x]));
					x++;
				}
				if (t->status & 1)
                                        sprintf(buf3, "EA=%4.4X%s", t->ea, buf_ea);
				break;
			} default: /* ??? */ {
				goto invalid;
				break;
			}
		}
	}

	invalid:
	strcpy(buf1, insn);
	strcat(buf1, left);
	strcat(buf1, " ");
	strcat(buf1, right);
	if (insn_no >= skip) {
	        if (pc == t->pc)
	                fprintf(mon_out, ">");
                else
                        fprintf(mon_out, " ");
		fprintf(mon_out, "%10d A=%2.2X B=%2.2X X=%4.4X Y=%4.4X U=%4.4X S=%4.4X P=%2.2X %c%c%c%c%c%c%c%c %-10s %-18s %-11s %-14s %s\n",
		       insn_no, t->acca, t->accb, t->ireg[0], t->ireg[1], t->ireg[2], t->ireg[3], t->dp,
		       ((t->cc & 128) ? 'E' : '-'),
		       ((t->cc & 64) ? 'F' : '-'),
		       ((t->cc & 32) ? 'H' : '-'),
		       ((t->cc & 16) ? 'I' : '-'),
		       ((t->cc & 8) ? 'N' :'-'),
		       ((t->cc & 4) ? 'Z' : '-'),
		       ((t->cc & 2) ? 'V' : '-'),
		       ((t->cc & 1) ? 'C' : '-'), fact_label, buf, buf1, buf3, fact_comment);
		if (subr)
		        fprintf(mon_out, "\n");
        }
}

/* Note that subroutine was processed by simulator */

void simulated(unsigned short addr)
{
        struct trace_entry *t = trace_buf + (trace_idx++ & (TRACESIZE - 1));
        t->status = 2;
        t->pc = addr;
}

/* Show trace buffer */

void show_traces(int n)
{
	int x;
	for (x = 0; x != n; ++x) {
		show_trace(trace_idx + x - n, trace_buf + ((trace_idx + x - n) & (TRACESIZE - 1)));
	}
}

/* This is the simulator */

void sim(void)
{
        unsigned char opcode;
        int org_trace_idx;
        struct trace_entry *t;
	for (;;) {
		int page2 = 0;
		int page3 = 0;
		unsigned short ea; /* Effective address */
		unsigned short a; /* Operand A */
		unsigned short b; /* Operand B */
		unsigned short f; /* Result */
		int wb; /* Enable write-back to memory */

		wb = 1;

		/* About to fetch first byte of an instruction */
		org_trace_idx = trace_idx;
		t = (trace_buf + (trace_idx++ & (TRACESIZE - 1)));

		/* Record state at start of instruction */
		record_trace(t);

		if ((mybrk && (brk_addr == pc)) || stop) {
			if (mybrk && brk_addr == pc)
				printf("\r\nBreakpoint!\n");
			monitor();
			record_trace(t);
		}

		if (reset) {
			reset = 0;
			abrt = 0;
			pc = 0xF000;
			f_flag = 1;
			i_flag = 1;
			if (trace)
				printf("           RESET!\n");
			record_trace(t);
		}

		if (abrt) {
			abrt = 0;
			push_all();
			jump(mread2(0xFFFC));
			printf("           NMI! to PC=%4.4X\n", pc);
			record_trace(t);
		}

		/* Fetch op-code, deal with page2/page3 */
		opcode = fetch();
		if (opcode == 0x10) {
			if (page2 || page3) {
				goto invalid;
			} else {
				page2 = 1;
				opcode = fetch();
			}
		} else if (opcode == 0x11) {
			if (page2 || page3) {
				goto invalid;
			} else {
				page3 = 1;
				opcode = fetch();
			}
		}

		if (opcode & 0x80) { /* R,M operations */
			unsigned char lowop = (opcode & 0x0F);
			int sixteen = 0; /* Assume 8-bit */

			/* Get operand A */

			if (opcode & 0x40) {
				if (lowop == 0x3 || lowop == 0xC || lowop == 0xD) {
					a = accd;
					sixteen = 1;
				} else if (lowop == 0xE || lowop == 0xF) {
					sixteen = 1;
					if (page2) {
						a = sp;
					} else {
						a = up;
					}
				} else {
					a = accb;
				}
			} else {
				if (lowop == 0x3) {
					sixteen = 1;
					if (page3) {
						a = up;
					} else {
						a = accd;
					}
				} else if (lowop == 0xC) {
					sixteen = 1;
					if (page2) {
						a = iy;
					} else if (page3) {
						a = sp;
					} else {
						a = ix;
					}
				} else if (lowop == 0x0E || lowop == 0x0F) {
					sixteen = 1;
					if (page2) {
						a = iy;
					} else {
						a = ix;
					}
				} else {
					/* will get here for lowop == 0x0D, but it's OK */
					a = acca;
				}
			}

			/* Get operand B's effective address */

			switch (opcode & 0x30) {
				case 0x00: {
					if (sixteen)
						ea = IMM2();
					else
						ea = IMM();
					break;
				} case 0x10: {
					ea = DIR();
					break;
				} case 0x20: {
					if (ind(&ea))
						goto invalid;
					break;
				} case 0x30: {
					ea = EXT();
					break;
				}
			}

			/* Load operand B */

			if (lowop != 0x07 && lowop != 0x0d && lowop != 0x0f) { /* Not store or jump */
				if (sixteen)
					b = mread2(ea);
				else
					b = mread(ea);
				t->ea = ea;
				t->data = b;
			} else { /* Store or jump */
				b = 0;
				t->ea = ea;
				t->data = a;
			}

			/* Operate F = A op B */
			/* If A doesn't change, then we must F = A */

			switch (opcode & 0x0F) {
				case 0x00: /* SUB N,Z,V,C,H) */ {
					f = a - b;
					c_flag = C_SUB(a,b,f);
					v_flag = V_SUB(a,b,f);
					n_flag = N(f);
					z_flag = Z(f);
					/* h_flag is undefined */
					break;
				} case 0x01: /* CMP N,Z,V,C,H */ {
					f = a - b;
					c_flag = C_SUB(a,b,f);
					v_flag = V_SUB(a,b,f);
					n_flag = N(f);
					z_flag = Z(f);
					/* h_flag is undefined */
					f = a;
					break;
				} case 0x02: /* SBC N,Z,V,C,H */ {
					f = a - b - c_flag;
					c_flag = C_SUB(a,b,f);
					v_flag = V_SUB(a,b,f);
					n_flag = N(f);
					z_flag = Z(f);
					/* h_flag is undefined */
					break;
				} case 0x03: /* SUBD/CMPD/CMPU or ADDD */ {
					if (opcode & 0x40) {
						f = a + b;
						v_flag = V_16(a,b,f);
						c_flag = C_16(a,b,f);
					} else {
						f = a - b;
						c_flag = C_16_SUB(a,b,f);
						v_flag = V_16_SUB(a,b,f);
					}
					n_flag = N_16(f);
					z_flag = Z_16(f);
					break;
				} case 0x04: /* AND N,Z,V=0 */ {
					f = (a & b);
					n_flag = N(f);
					z_flag = Z(f);
					v_flag = 0;
					break;
				} case 0x05: /* BIT N,Z,V=0 */ {
					f = (a & b);
					n_flag = N(f);
					z_flag = Z(f);
					v_flag = 0;
					f = a;
					break;
				} case 0x06: /* LDA N,Z,V=0 */ {
					f = b;
					n_flag = N(f);
					z_flag = Z(f);
					v_flag = 0;
					break;
				} case 0x07: /* STA N,Z,V=0 */ {
					f = a;
					n_flag = N(f);
					z_flag = Z(f);
					v_flag = 0;
					mwrite(ea, f);
					break;
				} case 0x08: /* EOR N,Z,V=0*/ {
					f = a ^ b;
					n_flag = N(f);
					z_flag = Z(f);
					v_flag = 0;
					break;
				} case 0x09: /* ADC H,N,Z,V,C */ {
					f = a + b + c_flag;
					v_flag = V(a,b,f);
					c_flag = C(a,b,f);
					n_flag = N(f);
					z_flag = Z(f);
					h_flag = H(a,b,f);
					break;
				} case 0x0A: /* ORA N,Z,V=0 */ {
					f = a | b;
					n_flag = N(f);
					z_flag = Z(f);
					v_flag = 0;
					break;
				} case 0x0B: /* ADD H,N,Z,V,C */ {
					f = a + b;
					v_flag = V(a,b,f);
					c_flag = C(a,b,f);
					n_flag = N(f);
					z_flag = Z(f);
					h_flag = H(a,b,f);
					break;
				} case 0x0C: {
					if (opcode & 0x40) { /* LDD */
						f = b;
						n_flag = N_16(f);
						z_flag = Z_16(f);
						v_flag = 0;
					} else { /* CMPX/CMPY/CMPS */
						f = a - b;
						c_flag = C_16_SUB(a,b,f);
						v_flag = V_16_SUB(a,b,f);
						n_flag = N_16(f);
						z_flag = Z_16(f);
						f = a;
					}
					break;
				} case 0x0D: {
					if (opcode & 0x40) { /* STD */
						f = a;
						n_flag = N_16(f);
						z_flag = Z_16(f);
						v_flag = 0;
						mwrite2(ea, f);
					} else { /* BSR/JSR */
						f = a;
						pushs2(pc);
						if (!(opcode & 0x30)) { /* BSR */
 							ea = pc + (signed char)mread(ea);
						}
						t->ea = ea;
						jump(ea);
					}
					break;
				} case 0x0E: { /* LDX/LDY/LDU/LDS */
					f = b;
					n_flag = N_16(f);
					z_flag = Z_16(f);
					v_flag = 0;
					break;
				} case 0x0F: { /* STX/STY/STU/STS */
					f = a;
					n_flag = N_16(f);
					z_flag = Z_16(f);
					v_flag = 0;
					mwrite2(ea, f);
					break;
				}
			}

			/* Write back operand A */

			if (opcode & 0x40) {
				if (lowop == 0x3 || lowop == 0xC || lowop == 0xD) {
					acca = (f >> 8);
					accb = f;
				} else if (lowop == 0xE || lowop == 0xF) {
					if (page2) {
						sp = f;
					} else {
						up = f;
					}
				} else {
					accb = f;
				}
			} else {
				if (lowop == 0x3) {
					if (page3 || page2) {
						/* CMPD or CMPU */
					} else {
						/* SUBD */
						acca = (f >> 8);
						accb = f;
					}
				} else if (lowop == 0xC || lowop == 0x0E || lowop == 0x0F) {
					if (page2) {
						iy = f;
					} else if (page3) {
						sp = f;
					} else {
						ix = f;
					}
				} else {
					acca = f;
				}
			}
                } else if ((opcode & 0x40) || !(opcode & 0xF0)) { /* R.M.W. operations */

                	/* Fetch load operand */

                	if (!(opcode & 0xF0)) {
                		ea = DIR();
                		b = mread(ea);
                		t->ea = ea; t->data = b;
                	} else {
				switch (opcode & 0x30) {
					case 0x00: {
						b = acca;
						t->data = b;
						break;
					} case 0x10: {
						b = accb;
						t->data = b;
						break;
					} case 0x20: {
						if (ind(&ea))
							goto invalid;
						b = mread(ea);
						t->ea = ea; t->data = b;
						break;
					} case 0x30: {
						ea = EXT();
						b = mread(ea);
						t->ea = ea; t->data = b;
						break;
					}
				}
			}

			/* Operate */

			switch (opcode & 0x0F) {
				case 0x00: /* NEG N, Z, V= ((f==0x80)?1:0), C= ((f==0)?1:0) */ {
					f = -b;
					v_flag = ((b & f) >> 7);
					n_flag = N(f);
					z_flag = Z(f);
					c_flag = !z_flag; /* CPUTEST (take to 6800) */
					break;
				} case 0x01: /* ??? */ {
				        goto invalid;
					break;
				} case 0x02: /* ??? */ {
				        goto invalid;
					break;
				} case 0x03: /* COM N, Z, V=0, C=1 */ {
					f = ~b;
					c_flag = 1;
					n_flag = N(f);
					z_flag = Z(f);
					v_flag = 0;
					break;
				} case 0x04: /* LSR N=0,Z,C,V=N^C */ {
					f = (b >> 1);
					c_flag = (b & 1);
					n_flag = 0;
					z_flag = Z(f);
					// v_flag = c_flag; /* CPUTEST (this changed from 6800) */
					break;
				} case 0x05: /* ??? */ {
				        goto invalid;
					break;
				} case 0x06: /* ROR N,Z,C,V=N^C */ {
					f = (b >> 1) + (c_flag << 7);
					c_flag = (b & 1);
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (n_flag ^ c_flag);
					break;
				} case 0x07: /* ASR N,Z,C,V=N^C */ {
					f = (b >> 1) + (b & 0x80);
					c_flag = (b & 1);
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (n_flag ^ c_flag);
					break;
				} case 0x08: /* ASL N,Z,C,V=N^C */ {
					f = (b << 1);
					c_flag = (b >> 7);
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (n_flag ^ c_flag);
					break;
				} case 0x09: /* ROL N,Z,C,V=N^C */ {
					f = (b << 1) + c_flag;
					c_flag = (b >> 7);
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (n_flag ^ c_flag);
					break;
				} case 0x0A: /* DEC N,Z,V = (a=0x80?1:0) */ {
					f = b - 1;
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (((~f & b) >> 7) & 1);
					break;
				} case 0x0B: /* ??? */ {
				        goto invalid;
					break;
				} case 0x0C: /* INC N,Z,V = (a=0x7F?1:0) */ {
					f = b + 1;
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = (((f & ~b) >> 7) & 1);
					break;
				} case 0x0D: /* TST N,Z,V=0,C=0 */ {
				        f = b;
					z_flag = Z(f);
					n_flag = N(f);
					v_flag = 0;
					c_flag = 0;
					wb = 0;
					break;
				} case 0x0E: /* JMP */ {
				        wb = 0;
				        jump(ea);
					break;
				} case 0x0F: /* CLR N=0, Z=1, V=0, C=0 */ {
					f = 0;
					n_flag = 0;
					z_flag = 1;
					v_flag = 0;
					c_flag = 0;
					break;
				}
			}

			/* Write back */

			if (wb) {
				if (!(opcode & 0xF0)) {
					mwrite(t->ea, f);
				} else {
					switch (opcode & 0x30) {
						case 0x00: {
							acca = f;
							break;
						} case 0x10: {
							accb = f;
							break;
						} case 0x20: {
							mwrite(ea, f);
							break;
						} case 0x30: {
							mwrite(ea, f);
							break;
						}
					}
				}
			}
                } else {
                	switch(opcode) {
                		case 0x12: { /* NOP */
                			break;
                		} case 0x13: { /* SYNC */
				        printf("SYNC encountered...\n");
				        return;
                			break;;
                		} case 0x16: { /* LBRA */
                			t->ea = fetch2();
                			t->ea += pc;
                			jump(t->ea);
                			break;
                		} case 0x17: { /* LBSR */
                			t->ea = fetch2();
                			pushs2(pc);
                			t->ea += pc;
                			jump(t->ea);
                			break;
                		} case 0x19: { /* DAA N,Z,V,C */
				        /* Only set C, don't clear it */
				        /* Do not change H */
				        unsigned char orga = acca;
				        if (h_flag || (acca & 0x0F) >= 0x0A) {
				                acca += 0x06;
				        }
				        if (c_flag || (orga & 0xF0) >= 0xA0) { /* CPUTEST (take to 6800) */
				                acca += 0x60;
				                c_flag = 1;
				        }
				        n_flag = N(acca);
				        z_flag = Z(acca);
				        /* V is undefined */
				        break;
                		} case 0x1A: { /* ORCC */
                			write_flags(read_flags() | fetch());
                			break;
                		} case 0x1C: { /* ANDCC */
                			write_flags(read_flags() & fetch());
                			break;
                		} case 0x1D: { /* SEX */
                			if (accb & 0x80) {
                				acca = 0xFF;
                				n_flag = 1;
                			} else {
                				acca = 0x00;
                				n_flag = 0;
                				z_flag = Z(accb);
                			}
                			break;
                		} case 0x1E: { /* EXG */
                			unsigned char cb = fetch();
                			unsigned short tmp;
                			unsigned short tmp1;
                			if (tfr_get(&tmp, cb & 0x0F))
                				goto invalid;
                			if (tfr_get(&tmp1, cb >> 4))
                				goto invalid;
                			tfr_put((cb & 0x0F), tmp1);
                			tfr_put(cb >> 4, tmp);
                			break;
                		} case 0x1F: { /* TFR */
                			unsigned char cb = fetch();
                			unsigned short tmp1;
                			if (tfr_get(&tmp1, cb >> 4))
                				goto invalid;
					if (tfr_put((cb & 0x0F), tmp1))
						goto invalid;
                			break;
                		} case 0x30: { /* LEAX */
                			if (ind(&ea))
                				goto invalid;
                			ix = ea;
                			z_flag = Z_16(ea);
                			t->ea = ea;
                			break;
                		} case 0x31: { /* LEAY */
                			if (ind(&ea))
                				goto invalid;
                			iy = ea;
                			z_flag = Z_16(ea);
                			t->ea = ea;
                			break;
                		} case 0x32: { /* LEAS */
                			if (ind(&ea))
                				goto invalid;
                			sp = ea;
                			t->ea = ea;
                			break;
                		} case 0x33: { /* LEAU */
                			if (ind(&ea))
                				goto invalid;
                			up = ea;
                			t->ea = ea;
                			break;
                		} case 0x34: { /* PSHS */
                			unsigned char cb = fetch();
                			if (cb & 0x80) pushs2(pc);
                			if (cb & 0x40) pushs2(up);
                			if (cb & 0x20) pushs2(iy);
                			if (cb & 0x10) pushs2(ix);
                			if (cb & 0x08) pushs(dp);
                			if (cb & 0x04) pushs(accb);
                			if (cb & 0x02) pushs(acca);
                			if (cb & 0x01) pushs(read_flags());
                			break;
                		} case 0x35: { /* PULS */
                			unsigned char cb = fetch();
                			if (cb & 0x01) write_flags(pull());
                			if (cb & 0x02) acca = pull();
                			if (cb & 0x04) accb = pull();
                			if (cb & 0x08) dp = pull();
                			if (cb & 0x10) ix = pull2();
                			if (cb & 0x20) iy = pull2();
                			if (cb & 0x40) up = pull2();
                			if (cb & 0x80) jump(pull2());
                			break;
                		} case 0x36: { /* PSHU */
                			unsigned char cb = fetch();
                			if (cb & 0x80) pushu2(pc);
                			if (cb & 0x40) pushu2(sp);
                			if (cb & 0x20) pushu2(iy);
                			if (cb & 0x10) pushu2(ix);
                			if (cb & 0x08) pushu(dp);
                			if (cb & 0x04) pushu(accb);
                			if (cb & 0x02) pushu(acca);
                			if (cb & 0x01) pushu(read_flags());
                			break;
                		} case 0x37: { /* PULU */
                			unsigned char cb = fetch();
                			if (cb & 0x01) write_flags(pullu());
                			if (cb & 0x02) acca = pullu();
                			if (cb & 0x04) accb = pullu();
                			if (cb & 0x08) dp = pullu();
                			if (cb & 0x10) ix = pullu2();
                			if (cb & 0x20) iy = pullu2();
                			if (cb & 0x40) up = pullu2();
                			if (cb & 0x80) jump(pullu2());
                			break;
                		} case 0x39: { /* RTS */
				        if (sp == sp_stop) {
				                stop = 1;
				                sp_stop = -1;
				        } else
        					jump(pull2());
					break;
                		} case 0x3A: { /* ABX */
                			ix += accb;
                			break;
                		} case 0x3B: { /* RTI */
					write_flags(pull());
					if (e_flag) {
						acca = pull();
						accb = pull();
						dp = pull();
						ix = pull2();
						iy = pull2();
						up = pull2();
					}
					jump(pull2());
					break;
                		} case 0x3C: { /* CWAI */
                			write_flags(read_flags() & fetch());
                			push_all();
				        printf("CWAI encountered...\n");
				        return;
					break;
                		} case 0x3D: { /* MUL */
                			unsigned short tmp = (unsigned short)acca * (unsigned short)accb;
                			acca = (tmp >> 8);
                			accb = tmp;
                			z_flag = Z_16(tmp);
                			c_flag = (1 & (tmp >> 7));
                			break;
                		} case 0x3F: { /* SWI */
                			push_all();
					if (page2)
						jump(mread2(0xFFF4));
					else if (page3)
						jump(mread2(0xFFF2));
					else {
						i_flag = 1;
						f_flag = 1;
						jump(mread2(0xFFFA));
					}
					break;
				} case 0x20: /* BRA */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					jump(t->ea);
					break;
				} case 0x21: /* BRN */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					break;
				} case 0x22: /* BHI */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (!(c_flag | z_flag))
					        jump(t->ea);
					break;
				} case 0x23: /* BLS */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (c_flag | z_flag)
					        jump(t->ea);
					break;
				} case 0x24: /* BCC */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (!c_flag)
					        jump(t->ea);
					break;
				} case 0x25: /* BCS */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (c_flag)
					        jump(t->ea);
					break;
				} case 0x26: /* BNE */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (!z_flag)
					        jump(t->ea);
					break;
				} case 0x27: /* BEQ */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (z_flag)
					        jump(t->ea);
					break;
				} case 0x28: /* BVC */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (!v_flag)
					        jump(t->ea);
					break;
				} case 0x29: /* BVS */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (v_flag)
					        jump(t->ea);
					break;
				} case 0x2A: /* BPL */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (!n_flag)
					        jump(t->ea);
					break;
				} case 0x2B: /* BMI */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (n_flag)
					        jump(t->ea);
					break;
				} case 0x2C: /* BGE */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (!(n_flag ^ v_flag))
					        jump(t->ea);
					break;
				} case 0x2D: /* BLT */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (n_flag ^ v_flag)
					        jump(t->ea);
					break;
				} case 0x2E: /* BGT */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (!(z_flag | (n_flag ^ v_flag)))
					        jump(t->ea);
					break;
				} case 0x2F: /* BLE */ {
					if (page2)
						t->ea = fetch2();
					else
						t->ea = (signed char)fetch();
					t->ea += pc;
					if (z_flag | (n_flag ^ v_flag))
					        jump(t->ea);
					break;
				} default: /* ??? */ {
				        goto invalid;
					break;
				}
                	}
                }
                t->status = 1; /* Instruction completed normally */
                goto normal;

                invalid:
                printf("\nInvalid opcode=$%2.2X (%s) at $%4.4X\n", opcode, (page2 ? "page2" : (page3 ? "page3" : "page0")), t->pc);
                stop = 1;

                normal:
                if (trace) {
                        while (org_trace_idx != trace_idx) {
                                show_trace(org_trace_idx, trace_buf + (org_trace_idx & (TRACESIZE - 1)));
                                org_trace_idx++;
                        }
                }
	}
}
