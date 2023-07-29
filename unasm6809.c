/*	M6800 unassembler
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

#include "unasm6800.h"
#include "utils.h"

struct fact *swi_facts;
int targets[65536];
struct fact *facts[65536];

struct fact *mkfact(unsigned short addr, int type, int len, char *label, char *comment)
{
	struct fact *f = (struct fact *)malloc(sizeof(struct fact));
        f->next = 0;
	f->addr = addr;
	f->type = type;
	f->len = len;
	f->label = strdup(label);
	f->comment = strdup(comment);
	/* printf("Create fact addr=%x type=%d len=%d label=%s comment=%s\n",
               addr, type, len, label, comment); */
	return f;
}

/* Parse known facts */

void parse_facts(FILE *f)
{
	char buf[100];
	char *p;
	int line = 0;
	while (fgets(buf, sizeof(buf)-1, f)) {
		if (buf[0] && buf[strlen(buf)-1]=='\n')
			buf[strlen(buf)-1] = 0;
		if (buf[0] && buf[strlen(buf)-1]=='\r')
			buf[strlen(buf)-1] = 0;
		p = buf;
		int iaddr;
		unsigned short addr;
		char keyword[100];
		int len;
		char label[100];
		char *comment = 0;
		addr = 0;
		keyword[0] = 0;
		len = 0;
		label[0] = 0;
		++line;
		if (parse_hex(&p, &iaddr) && skipws(&p)) {
			addr = (unsigned short)iaddr;
			if (parse_word(&p, keyword) && skipws(&p)) {
				if (!strcmp(keyword, "code")) {
					if (parse_word(&p, label) && skipws(&p)) {
						comment = p;
						facts[addr] = mkfact(addr, 0, 0, label, comment);
					}
                                } else if (!strcmp(keyword, "subr")) {
					if (parse_word(&p, label) && skipws(&p)) {
						comment = p;
						facts[addr] = mkfact(addr, 5, 0, label, comment);
					}
				} else {
					if (parse_dec(&p, &len) && skipws(&p) &&
					    parse_word(&p, label) && skipws(&p)) {
					    	comment = p;
					    	if (!strcmp(keyword, "swi")) {
					    	        struct fact *f = mkfact(addr, 6, len, label, comment);
					    	        f->next = swi_facts;
					    	        swi_facts = f;
					    	} else if (!strcmp(keyword, "fcb")) {
							facts[addr] = mkfact(addr, 1, len, label, comment);
					    	} else if (!strcmp(keyword, "fdb")) {
							facts[addr] = mkfact(addr, 2, len, label, comment);
					    	} else if (!strcmp(keyword, "fcc")) {
							facts[addr] = mkfact(addr, 3, len, label, comment);
					    	} else if (!strcmp(keyword, "rmb")) {
							facts[addr] = mkfact(addr, 4, len, label, comment);
					    	} else {
					    		printf("%d: Unknown keyword '%s'\n", line, keyword);
					    	}
					} else {
						printf("%d: Syntax error\n", line);
					}
				}
			}
		}
	}
}

int fcb_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int flag)
{
	char buf[256];
	char buf1[256];
	unsigned short pc = *at_pc;
	struct fact *fact;
	int flg = targets[pc];
	sprintf(buf, "%4.4X: %2.2X ", pc, mem[pc]);
	fact = facts[pc];
	pc++;
	if (fact) {
		sprintf(buf1, "%-10s FCB $%2.2X", fact->label, mem[pc - 1]);
	} else if (flg) {
		sprintf(buf1, "L%4.4d      FCB $%2.2X", flg, mem[pc - 1]);
	} else {
		sprintf(buf1, "           FCB $%2.2X", mem[pc - 1]);
	}
	sprintf(outbuf, "%-15s%-9s",buf, buf1);
        if (fact)
                sprintf(outbuf + strlen(outbuf),"			* %s", fact->comment);
	*at_pc = pc;
	return flg;
}

int rmb_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int flag, int len)
{
	char buf[256];
	char buf1[256];
	unsigned short pc = *at_pc;
	struct fact *fact;
	int flg = targets[pc];
	sprintf(buf, "%4.4X: %2.2X ", pc, mem[pc]);
	fact = facts[pc];
	pc += len;
	if (fact) {
		sprintf(buf1, "%-10s RMB %d", fact->label, len);
	} else if (flg) {
		sprintf(buf1, "L%4.4d      RMB %d", flg, len);
	} else {
		sprintf(buf1, "           RMB %d", len);
	}
	sprintf(outbuf, "%-15s%-9s",buf, buf1);
        if (fact)
                sprintf(outbuf + strlen(outbuf),"			* %s", fact->comment);
	*at_pc = pc;
	return flg;
}

int fcc_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int flag, int len)
{
	char buf[256];
	char buf1[256];
	unsigned short pc = *at_pc;
	struct fact *fact;
	int flg = targets[pc];
	sprintf(buf, "%4.4X: %2.2X ", pc, mem[pc]);
	fact = facts[pc];
	pc += len;
	if (fact) {
		sprintf(buf1, "%-10s FCC %d", fact->label, len);
	} else if (flg) {
		sprintf(buf1, "L%4.4d      FCC %d", flg, len);
	} else {
		sprintf(buf1, "           FCC %d", len);
	}
	sprintf(outbuf, "%-15s%-9s",buf, buf1);
        if (fact)
                sprintf(outbuf + strlen(outbuf),"			* %s", fact->comment);
	*at_pc = pc;
	return flg;
}

int fdb_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int flag)
{
	char buf[256];
	char buf1[256];
	int pc = *at_pc;
	struct fact *fact;
	int flg = targets[pc];
	sprintf(buf, "%4.4X: %2.2X %2.X", pc, mem[pc], mem[pc+1]);
	fact = facts[pc];
	pc += 2;
	if (fact) {
		sprintf(buf1, "%-10s FDB $%4.4X", fact->label, (((int)mem[pc - 2] << 8) + mem[pc -1]));
	} else if (flg) {
		sprintf(buf1, "L%4.4d      FDB $%4.4X", flg, (((int)mem[pc - 2] << 8) + mem[pc -1]));
	} else {
		sprintf(buf1, "           FDB $%4.4X", (((int)mem[pc - 2] << 8) + mem[pc -1]));
	}
	sprintf(outbuf, "%-15s%-9s",buf, buf1);
        if (fact)
                sprintf(outbuf + strlen(outbuf),"			* %s", fact->comment);
	*at_pc = pc;
	return flg;
}

extern char *regname[];
int tfr_name(char **name, int r);
void show_list(int us, char *right, unsigned char cb);

int unasm_ind(unsigned char *mem, unsigned short *pc, char *buf, char *right)
{
	unsigned char i = mem[*pc]; /* Fetch index byte */
	unsigned char ind = (i & 0x10); /* Indirection bit */
	unsigned char o = (i & 0x0f); /* Index type / low bits of 5-bit offset */
	int r = (3 & (i >> 5)); /* Index register number */

	sprintf(buf + strlen(buf), " %2.2X", i);
	*pc = *pc + 1;

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
				unsigned short v = (signed char)mem[*pc];
				sprintf(buf + strlen(buf), " %2.2X", mem[*pc]);
				*pc = *pc + 1;
				if (ind)
					sprintf(right, "[%d,%s]", (signed short)v, regname[r]);
				else
					sprintf(right, "%d,%s", (signed short)v, regname[r]);
				return 0;
			}
			case 0x09: /* Offset16,R */
			{
				unsigned short v = (mem[*pc] << 8);
				sprintf(buf + strlen(buf), " %2.2X", mem[*pc]);
				*pc = *pc + 1;
				v += mem[*pc];
				sprintf(buf + strlen(buf), "%2.2X", mem[*pc]);
				*pc = *pc + 1;
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
				unsigned short v = (signed char)mem[*pc];
				sprintf(buf + strlen(buf), " %2.2X", mem[*pc]);
				*pc = *pc + 1;
				if (ind)
	 				sprintf(right, "[$%x,PCR]", (unsigned short)(*pc + v));
				else
	 				sprintf(right, "$%x,PCR", (unsigned short)(*pc + v));
				return 0;
			}
			case 0x0D: /* Offset16,PC */
			{
				unsigned short v = (mem[*pc] << 8);
				sprintf(buf + strlen(buf), " %2.2X", mem[*pc]);
				*pc = *pc + 1;
				v += mem[*pc];
				sprintf(buf + strlen(buf), "%2.2X", mem[*pc]);
				*pc = *pc + 1;
				if (ind)
	 				sprintf(right, "[$%x,PCR]", (unsigned short)(*pc + v));
				else
	 				sprintf(right, "$%x,PCR", (unsigned short)(*pc + v));
				return 0;
			}
			case 0x0F: /* Offset16 */
			{
				unsigned short v = (mem[*pc] << 8);
				sprintf(buf + strlen(buf), " %2.2X", mem[*pc]);
				*pc = *pc + 1;
				v += mem[*pc];
				sprintf(buf + strlen(buf), "%2.2X", mem[*pc]);
				*pc = *pc + 1;
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

int unasm_lbra(unsigned char *mem, unsigned short *pc, char *buf, char *right)
{
	int branch_target = 0xFFFF & (*pc + 2 + (mem[*pc] << 8) + mem[*pc + 1]);
	sprintf(buf + strlen(buf), " %2.2X%2.2X", mem[*pc], mem[*pc + 1]);
	sprintf(right, "$%4.4x", branch_target);
	*pc = *pc + 2;
	return branch_target;
}

int unasm_bra(unsigned char *mem, unsigned short *pc, char *buf, char *right)
{
	int branch_target = (0xFFFF & (*pc + 1 + (signed char)mem[*pc]));
	sprintf(buf + strlen(buf), " %2.2X", mem[*pc]);
	sprintf(right, "$%4.4x", branch_target);
	*pc = *pc + 1;
	return branch_target;
}

void unasm_line(unsigned char *mem, unsigned short *at_pc, char *outbuf, int *at_target, int flag)
{
	unsigned short pc;
	struct fact *fact;
	char buf[150];
	char buf1[150];
	char *insn = "???";
	char *left = ""; /* Left operand, like the 'D' in ADDD */
	char right[20]; /* Address mode / operand */
        unsigned char opcode;
        int page2 = 0;
        int page3 = 0;
        int ea_fact = -1;
        int is_jsr = 0;
        int is_swi = -1;

        int branch_target = -1;

	int flg;

        buf[0] = 0;
	right[0] = 0;

	/* Fetch */

	pc = *at_pc;
	flg = targets[pc];
	fact = facts[pc];

	sprintf(buf,"%4.4X: ", pc);

	if (fact) switch(fact->type) {
	        case 1: /* FCB */ {
	                int x;
	                for (x = 0; x != fact->len; ++x) {
	                        fcb_line(mem, at_pc, outbuf, flag);
	                }
	                return;
	        } case 2: /* FDB */ {
	                int x;
	                for (x = 0; x != fact->len; ++x) {
	                        fdb_line(mem, at_pc, outbuf, flag);
	                }
	                return;
	        } case 3: /* FCC */ {
	                fcc_line(mem, at_pc, outbuf, flag, fact->len);
	                return;
	        } case 4: /* RMB */ {
	                rmb_line(mem, at_pc, outbuf, flag, fact->len);
	                return;
	        }
	}


	/* Decode instruction: */
	/* Set 'insn' to instruction mnemonic */
        /* Set operand to its operand */

	/* Fetch op-code, deal with page2/page3 */
	opcode = mem[pc++];
	sprintf(buf + strlen(buf), "%2.2X", opcode);
	if (opcode == 0x10) {
		page2 = 1;
		sprintf(buf + strlen(buf), "%2.2X", mem[pc]);
		opcode = mem[pc++];
	} else if (opcode == 0x11) {
		page3 = 1;
		sprintf(buf + strlen(buf), "%2.2X", mem[pc]);
		opcode = mem[pc++];
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
					sprintf(buf + strlen(buf), " %2.2X%2.2X", mem[pc], mem[pc + 1]);
					sprintf(right, "#$%4.4x", (mem[pc] << 8) + mem[pc + 1]);
					pc += 2;
				} else {
					sprintf(buf + strlen(buf), " %2.2X", mem[pc]);
					sprintf(right, "#$%2.2x", mem[pc]);
					pc += 1;
				}
				break;
			} case 0x10: {
				sprintf(buf + strlen(buf), " %2.2X", mem[pc]);
				sprintf(right, "$%2.2x", mem[pc++]);
				break;
			} case 0x20: {
				if (unasm_ind(mem, &pc, buf, right))
					goto invalid;
				break;
			} case 0x30: {
				sprintf(buf + strlen(buf), " %2.2X%2.2X", mem[pc], mem[pc + 1]);
				sprintf(right, "$%4.4x", (mem[pc] << 8) + mem[pc + 1]);
				pc += 2;
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
						branch_target = (0xFFFF & (pc + (signed char)mem[0xFFFF & (pc - 1)]));
						sprintf(right, "$%4.4x", branch_target);
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
			/* Direct */
			sprintf(buf + strlen(buf), " %2.2X", mem[pc]);
			sprintf(right, "$%2.2x", mem[pc++]);
		} else {
			switch (opcode & 0x30) {
				case 0x00: {
					/* ACCA */
					left = "A";
					break;
				} case 0x10: {
					/* ACCB */
					left = "B";
					break;
				} case 0x20: {
					/* Indexed */
					if (unasm_ind(mem, &pc, buf, right))
						goto invalid;
					break;
				} case 0x30: {
					/* Extended */
					sprintf(buf + strlen(buf), " %2.2X%2.2X", mem[pc], mem[pc+1]);
					sprintf(right, "$%4.4x", (mem[pc] << 8) + mem[pc+1]);
					pc += 2;
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
				branch_target = unasm_lbra(mem, &pc, buf, right);
				break;
			} case 0x17: { /* LBSR */
				insn = "LBSR";
				branch_target = unasm_lbra(mem, &pc, buf, right);
				break;
			} case 0x19: { /* DAA N,Z,V,C */
				insn = "DAA";
				break;
			} case 0x1A: { /* ORCC */
				insn = "ORCC";
				sprintf(buf + strlen(buf), " %2.2X", mem[pc]);
				sprintf(right, "#$%2.2x", mem[pc++]);
				break;
			} case 0x1C: { /* ANDCC */
				insn = "ANDCC";
				sprintf(buf + strlen(buf), " %2.2X", mem[pc]);
				sprintf(right, "#$%2.2x", mem[pc++]);
				break;
			} case 0x1D: { /* SEX */
				insn = "SEX";
				break;
			} case 0x1E: { /* EXG */
				char *n;
				insn = "EXG";
				sprintf(buf + strlen(buf), " %2.2X", mem[pc]);
				if (tfr_name(&n, mem[pc] >> 4))
					goto invalid;
				strcat(right, n);
				if (tfr_name(&n, mem[pc] & 0xF))
					goto invalid;
				pc++;
				strcat(right, ",");
				strcat(right, n);
				break;
			} case 0x1F: { /* TFR */
				char *n;
				insn = "TFR";
				sprintf(buf + strlen(buf), " %2.2X", mem[pc]);
				if (tfr_name(&n, mem[pc] >> 4))
					goto invalid;
				strcat(right, n);
				if (tfr_name(&n, mem[pc] & 0xF))
					goto invalid;
				pc++;
				strcat(right, ",");
				strcat(right, n);
				break;
			} case 0x30: { /* LEAX */
				insn = "LEAX";
				if (unasm_ind(mem, &pc, buf, right))
					goto invalid;
				break;
			} case 0x31: { /* LEAY */
				insn = "LEAY";
				if (unasm_ind(mem, &pc, buf, right))
					goto invalid;
				break;
			} case 0x32: { /* LEAS */
				insn = "LEAS";
				if (unasm_ind(mem, &pc, buf, right))
					goto invalid;
				break;
			} case 0x33: { /* LEAU */
				insn = "LEAU";
				if (unasm_ind(mem, &pc, buf, right))
					goto invalid;
				break;
			} case 0x34: { /* PSHS */
				insn = "PSHS";
				show_list(0, right, mem[pc++]);
				break;
			} case 0x35: { /* PULS */
				insn = "PULS";
				show_list(0, right, mem[pc++]);
				break;
			} case 0x36: { /* PSHU */
				insn = "PSHU";
				show_list(1, right, mem[pc++]);
				break;
			} case 0x37: { /* PULU */
				insn = "PULU";
				show_list(1, right, mem[pc++]);
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
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BRA";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x21: /* BRN */ {
				if (page2) {
					insn = "LBRN";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BRN";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x22: /* BHI */ {
				if (page2) {
					insn = "LBHI";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BHI";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x23: /* BLS */ {
				if (page2) {
					insn = "LBLS";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BLS";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x24: /* BCC */ {
				if (page2) {
					insn = "LBCC";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BCC";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x25: /* BCS */ {
				if (page2) {
					insn = "LBCS";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BCS";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x26: /* BNE */ {
				if (page2) {
					insn = "LBNE";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BNE";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x27: /* BEQ */ {
				if (page2) {
					insn = "LBEQ";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BEQ";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x28: /* BVC */ {
				if (page2) {
					insn = "LBVC";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BVC";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x29: /* BVS */ {
				if (page2) {
					insn = "LBVS";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BVS";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x2A: /* BPL */ {
				if (page2) {
					insn = "LBPL";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BPL";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x2B: /* BMI */ {
				if (page2) {
					insn = "LBMI";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BMI";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x2C: /* BGE */ {
				if (page2) {
					insn = "LBGE";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BGE";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x2D: /* BLT */ {
				if (page2) {
					insn = "LBLT";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BLT";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x2E: /* BGT */ {
				if (page2) {
					insn = "LBGT";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BGT";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} case 0x2F: /* BLE */ {
				if (page2) {
					insn = "LBLE";
					branch_target = unasm_lbra(mem, &pc, buf, right);
				} else {
					insn = "BLE";
					branch_target = unasm_bra(mem, &pc, buf, right);
				}
				break;
			} default: /* ??? */ {
				goto invalid;
				break;
			}
		}
	}
	invalid:

	if (fact)
	        sprintf(buf1, "%-10s ", fact->label);
	else if (flg)
		sprintf(buf1, "L%4.4d      ", flg);
	else
		sprintf(buf1, "           ");
	strcat(buf1, insn);
	strcat(buf1, left);
	strcat(buf1, " ");
	strcat(buf1, right);
	if (ea_fact == -1 && (branch_target >= 0 && branch_target <= 0xFFFF))
	        ea_fact = branch_target;
	if (ea_fact != -1) {
	        struct fact *f = facts[ea_fact];
	        if (f) {
	                sprintf(buf1 + strlen(buf1)," [%s %s]", f->label,f->comment);
	        }
	}
	sprintf(outbuf, "%-15s%-9s",buf, buf1);
	if (is_swi != -1) {
	        struct fact *f;
	        for (f = swi_facts;f;f = f->next)
	                if (f->len == is_swi)
	                        break;
                if (f) {
                        sprintf(outbuf + strlen(outbuf), " (%s    %s)",f->label,f->comment);
                }
	} else if (branch_target > 0 && branch_target < 0x10000 && facts[branch_target]) {
	        /* sprintf(outbuf + strlen(outbuf), " (to %s [%s])", facts[branch_target]->label, facts[branch_target]->comment); */
	} else if (branch_target > 0 && branch_target < 0x10000 && targets[branch_target])
		sprintf(outbuf + strlen(outbuf), " (to L%4.4d)", targets[branch_target]);

        if (fact && flag && fact->type == 5) {
                printf("\n");
                printf("* %s\n", fact->comment);
                printf("\n");
        } else if (fact)
                sprintf(outbuf + strlen(outbuf),"			* %s", fact->comment);
	*at_pc = pc;
	if (is_jsr)
		*at_target = branch_target;
}
