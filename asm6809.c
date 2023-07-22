/*	6800 Assembler
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
#include "asm6800.h"
#include "utils.h"

/* Fixup types */

enum {
        FIXUP_EXT,	/* An extended address */
        FIXUP_DIR,	/* An 8-bit address */
        FIXUP_REL,	/* A branch offset */
        FIXUP_LREL,	/* A branch offset */
};

/* A pending fixup */

struct fixup {
        struct fixup *next;
        unsigned short fixup;	/* Address of data which needs to be fixed up */
        int type;		/* Type of fixup */
        int ofst;
};

/* Symbol table */

struct symbol {
        struct symbol *next;
        char *name;		/* Name of symbol */
        int valid;		/* Set if symbol's value is known */
        unsigned short val;	/* Value of symbol */
        struct fixup *fixups;	/* Fixups waiting for this symbol */
} *symbols;

struct symbol *find_symbol(char *name)
{
        struct symbol *sy;
        for (sy = symbols; sy; sy = sy->next)
                if (!strcmp(sy->name, name))
                        return sy;
        sy = (struct symbol *)malloc(sizeof(struct symbol));
        sy->next = symbols;
        symbols = sy;
        sy->name = strdup(name);
        sy->valid = 0;
        sy->val = 0;
        sy->fixups = 0;
        return sy;
}

/* Get symbol name by address */

char *find_label(unsigned short val)
{
        struct symbol *sy;
        for (sy = symbols; sy; sy = sy->next)
                if (sy->val == val)
                        return sy->name;
        return 0;
}

/* Add a fixup */

void add_fixup(struct symbol *sy, unsigned short addr, int type, int ofst)
{
        struct fixup *fix = (struct fixup *)malloc(sizeof(struct fixup));
        fix->type = type;
        fix->fixup = addr;
        fix->next = sy->fixups;
        fix->ofst = ofst;
        sy->fixups = fix;
}

/* Show symbol table */

void show_syms(FILE *f)
{
        struct symbol *sy;
        for (sy = symbols; sy; sy = sy->next) {
                struct fixup *fix;
                if (sy->valid) {
                        fprintf(f,"%4.4X %s\n", sy->val, sy->name);
                } else {
                        fprintf(f,"???? %s\n", sy->name);
                }
                for (fix = sy->fixups; fix; fix = fix->next) {
                        if (fix->type == FIXUP_EXT)
                                fprintf(f,"    16-bit fixup at %4.4X\n", fix->fixup);
                        else if (fix->type == FIXUP_DIR)
                                fprintf(f,"    8-bit fixup at  %4.4X\n", fix->fixup);
                        else if (fix->type == FIXUP_REL)
                                fprintf(f,"    8-bit rel fixup %4.4X\n", fix->fixup);
                        else if (fix->type == FIXUP_LREL)
                                fprintf(f,"    16-bit rel fixup %4.4X\n", fix->fixup);
                }
        }
}

/* Clear symbol table */

void clr_syms(void)
{
        struct symbol *sy;
        while ((sy = symbols)) {
                struct fixup *fix;
                symbols = sy->next;
                while ((fix = sy->fixups)) {
                        sy->fixups = fix->next;
                        free(fix);
                }
                free(sy->name);
                free(sy);
        }
}

/* Set symbol's value, process pending fixups */

void set_symbol(unsigned char *mem, struct symbol *sy, unsigned short val)
{
        struct fixup *fix;
        if (!sy)
                return;
        if (sy->valid) {
                printf("Symbol '%s' already defined to %4.4x\n", sy->name, sy->val);
                return;
        }
        sy->valid = 1;
        sy->val = val;
        while ((fix = sy->fixups)) {
                sy->fixups = fix->next;
                if (fix->type == FIXUP_EXT) {
                        mem[fix->fixup] = ((val + fix->ofst) >> 8);
                        mem[fix->fixup + 1]  = (val + fix->ofst);
                        printf("Address at %4.4X set to %4.4X\n", fix->fixup, val + fix->ofst);
                } else if (fix->type == FIXUP_DIR) {
                        mem[fix->fixup]  = (val + fix->ofst);
                        printf("Byte at %4.4X set to %2.2X\n", fix->fixup, ((val + fix->ofst) & 255));
                } else if (fix->type == FIXUP_REL) {
                        mem[fix->fixup] = val + fix->ofst - (fix->fixup + 1);
                        printf("Offset at %4.4X set to %2.2X\n", fix->fixup, val + fix->ofst - (fix->fixup + 1));
                } else if (fix->type == FIXUP_LREL) {
                        unsigned short v = val + fix->ofst - (fix->fixup + 1);
                        mem[fix->fixup] = (v >> 8);
                        mem[fix->fixup + 1] = v;
                        printf("Offset at %4.4X set to %4.4X\n", fix->fixup, v);
                }
                free(fix);
        }
}

/* Instruction type codes */

enum {
        LREL=1,	/* Long branch, 16-bit offset */
        REL=2,	/* Relative branch, 8-bit offset */
        IMM8=3,
        IMM16=4,
        TFR=5,
        IND=6,	/* Indexed */
        SLIST=7,
        ULIST=8,
        DIR=9,	/* Direct */
        EXT=10,	/* Extended */
        FCB=11,	/* FCB pseudo-op */
        FDB=12,	/* FDB pseudo-op */
        EQU=13,	/* EQU pseudo-op */
        RMB=14,	/* RMB pseudo-op */
        ORG=15,	/* ORG pseudo-op */
        IGN=16,	/* Ignore these pseudo-ops */
        NONE=17	/* No operand */
};

struct variant
{
        unsigned short opcode;
        short type;
};

struct insn
{
        char *insn;
        struct variant variant[4];
} table[] =
{
        { "nop", { { 0x12, NONE } } },
        { "sync", { { 0x13, NONE } } },
        { "lbra", { { 0x16, LREL } } },
        { "lbsr", { { 0x17, LREL } } },
        { "daa", { { 0x19, NONE } } },
        { "orcc", { { 0x1A, IMM8 } } },
        { "andcc", { { 0x1C, IMM8 } } },
        { "sex", { { 0x1D, NONE } } },
        { "exg", { { 0x1E, TFR } } },
        { "tfr", { { 0x1F, TFR } } },

        { "bra", { { 0x20, REL } } },
        { "brn", { { 0x21, REL } } },
        { "bhi", { { 0x22, REL } } },
        { "bls", { { 0x23, REL } } },
        { "bcc", { { 0x24, REL } } },
        { "bhs", { { 0x24, REL } } },
        { "bcs", { { 0x25, REL } } },
        { "blo", { { 0x25, REL } } },
        { "bne", { { 0x26, REL } } },
        { "beq", { { 0x27, REL } } },
        { "bvc", { { 0x28, REL } } },
        { "bvs", { { 0x29, REL } } },
        { "bpl", { { 0x2a, REL } } },
        { "bmi", { { 0x2b, REL } } },
        { "bge", { { 0x2c, REL } } },
        { "blt", { { 0x2d, REL } } },
        { "bgt", { { 0x2e, REL } } },
        { "ble", { { 0x2f, REL } } },

        /* { "lbra", { { 0x1020, LREL } }, Use shorter version, opcode=0x16 */
        { "lbrn", { { 0x1021, LREL } } },
        { "lbhi", { { 0x1022, LREL } } },
        { "lbls", { { 0x1023, LREL } } },
        { "lbcc", { { 0x1024, LREL } } },
        { "lbhs", { { 0x1024, LREL } } },
        { "lbcs", { { 0x1025, LREL } } },
        { "lblo", { { 0x1025, LREL } } },
        { "lbne", { { 0x1026, LREL } } },
        { "lbeq", { { 0x1027, LREL } } },
        { "lbvc", { { 0x1028, LREL } } },
        { "lbvs", { { 0x1029, LREL } } },
        { "lbpl", { { 0x102a, LREL } } },
        { "lbmi", { { 0x102b, LREL } } },
        { "lbge", { { 0x102c, LREL } } },
        { "lblt", { { 0x102d, LREL } } },
        { "lbgt", { { 0x102e, LREL } } },
        { "lble", { { 0x102f, LREL } } },

        { "leax", { { 0x30, IND } } },
        { "leay", { { 0x31, IND } } },
        { "leas", { { 0x32, IND } } },
        { "leau", { { 0x33, IND } } },
        { "pshs", { { 0x34, SLIST } } },
        { "puls", { { 0x35, SLIST } } },
        { "pshu", { { 0x36, ULIST } } },
        { "pulu", { { 0x37, ULIST } } },
        { "rts", { { 0x39, NONE } } },
        { "abx", { { 0x3A, NONE } } },
        { "rti", { { 0x3B, NONE } } },
        { "cwai", { { 0x3C, IMM8 } } },
        { "mul", { { 0x3D, NONE } } },
        { "swi", { { 0x3F, NONE } } },
        { "swi2", { { 0x103F, NONE } } },
        { "swi3", { { 0x113F, NONE } } },

        { "nega", { { 0x40, NONE } } },
        { "negb", { { 0x50, NONE } } },
        { "neg", { { 0x60, IND }, { 0x00, DIR }, { 0x70, EXT } } },
        { "coma", { { 0x43, NONE } } },
        { "comb", { { 0x53, NONE } } },
        { "com", { { 0x63, IND }, { 0x03, DIR }, { 0x73, EXT } } },
        { "lsra", { { 0x44, NONE } } },
        { "lsrb", { { 0x54, NONE } } },
        { "lsr", { { 0x64, IND }, { 0x04, DIR }, { 0x74, EXT } } },
        { "rora", { { 0x46, NONE } } },
        { "rorb", { { 0x56, NONE } } },
        { "ror", { { 0x66, IND }, { 0x06, DIR }, { 0x76, EXT } } },
        { "asra", { { 0x47, NONE } } },
        { "asrb", { { 0x57, NONE } } },
        { "asr", { { 0x67, IND }, { 0x07, DIR }, { 0x77, EXT } } },
        { "asla", { { 0x48, NONE } } },
        { "aslb", { { 0x58, NONE } } },
        { "asl", { { 0x68, IND }, { 0x08, DIR }, { 0x78, EXT } } },
        { "lsla", { { 0x48, NONE } } },
        { "lslb", { { 0x58, NONE } } },
        { "lsl", { { 0x68, IND }, { 0x08, DIR }, { 0x78, EXT } } },
        { "rola", { { 0x49, NONE } } },
        { "rolb", { { 0x59, NONE } } },
        { "rol", { { 0x69, IND }, { 0x09, DIR }, { 0x79, EXT } } },
        { "deca", { { 0x4A, NONE } } },
        { "decb", { { 0x5A, NONE } } },
        { "dec", { { 0x6A, IND }, { 0x0A, DIR }, { 0x7A, EXT } } },
        { "inca", { { 0x4C, NONE } } },
        { "incb", { { 0x5C, NONE } } },
        { "inc", { { 0x6C, IND }, { 0x0C, DIR }, { 0x7C, EXT } } },
        { "tsta", { { 0x4D, NONE } } },
        { "tstb", { { 0x5D, NONE } } },
        { "tst", { { 0x6D, IND }, { 0x0D, DIR }, { 0x7D, EXT } } },
        { "jmp", { { 0x6E, IND }, { 0x0E, DIR }, { 0x7E, EXT } } },
        { "clra", { { 0x4F, NONE } } },
        { "clrb", { { 0x5F, NONE } } },
        { "clr", { { 0x6F, IND }, { 0x0F, DIR }, { 0x7F, EXT } } },

        { "suba", { { 0x80, IMM8 }, { 0xA0, IND }, { 0x90, DIR }, { 0xB0, EXT } } },
        { "subb", { { 0xC0, IMM8 }, { 0xE0, IND }, { 0xD0, DIR }, { 0xF0, EXT } } },
        { "cmpa", { { 0x81, IMM8 }, { 0xA1, IND }, { 0x91, DIR }, { 0xB1, EXT } } },
        { "cmpb", { { 0xC1, IMM8 }, { 0xE1, IND }, { 0xD1, DIR }, { 0xF1, EXT } } },
        { "sbca", { { 0x82, IMM8 }, { 0xA2, IND }, { 0x92, DIR }, { 0xB2, EXT } } },
        { "sbcb", { { 0xC2, IMM8 }, { 0xE2, IND }, { 0xD2, DIR }, { 0xF2, EXT } } },
        { "subd", { { 0x83, IMM16 }, { 0xA3, IND }, { 0x93, DIR }, { 0xB3, EXT } } },
        { "cmpd", { { 0x1083, IMM16 }, { 0x10A3, IND }, { 0x1093, DIR }, { 0x10B3, EXT } } },
        { "cmpu", { { 0x1183, IMM16 }, { 0x11A3, IND }, { 0x1193, DIR }, { 0x11B3, EXT } } },
        { "addd", { { 0xC3, IMM16 }, { 0xE3, IND }, { 0xD3, DIR }, { 0xF3, EXT } } },
        { "anda", { { 0x84, IMM8 }, { 0xA4, IND }, { 0x94, DIR }, { 0xB4, EXT } } },
        { "andb", { { 0xC4, IMM8 }, { 0xE4, IND }, { 0xD4, DIR }, { 0xF4, EXT } } },
        { "bita", { { 0x85, IMM8 }, { 0xA5, IND }, { 0x95, DIR }, { 0xB5, EXT } } },
        { "bitb", { { 0xC5, IMM8 }, { 0xE5, IND }, { 0xD5, DIR }, { 0xF5, EXT } } },
        { "lda", { { 0x86, IMM8 }, { 0xA6, IND }, { 0x96, DIR }, { 0xB6, EXT } } },
        { "ldb", { { 0xC6, IMM8 }, { 0xE6, IND }, { 0xD6, DIR }, { 0xF6, EXT } } },
        { "sta", { { 0xA7, IND }, { 0x97, DIR }, { 0xB7, EXT } } },
        { "stb", { { 0xE7, IND }, { 0xD7, DIR }, { 0xF7, EXT } } },
        { "eora", { { 0x88, IMM8 }, { 0xA8, IND }, { 0x98, DIR }, { 0xB8, EXT } } },
        { "eorb", { { 0xC8, IMM8 }, { 0xE8, IND }, { 0xD8, DIR }, { 0xF8, EXT } } },
        { "adca", { { 0x89, IMM8 }, { 0xA9, IND }, { 0x99, DIR }, { 0xB9, EXT } } },
        { "adcb", { { 0xC9, IMM8 }, { 0xE9, IND }, { 0xD9, DIR }, { 0xF9, EXT } } },
        { "ora", { { 0x8A, IMM8 }, { 0xAA, IND }, { 0x9A, DIR }, { 0xBA, EXT } } },
        { "orb", { { 0xCA, IMM8 }, { 0xEA, IND }, { 0xDA, DIR }, { 0xFA, EXT } } },
        { "adda", { { 0x8B, IMM8 }, { 0xAB, IND }, { 0x9B, DIR }, { 0xBB, EXT } } },
        { "addb", { { 0xCB, IMM8 }, { 0xEB, IND }, { 0xDB, DIR }, { 0xFB, EXT } } },
        { "cmpx", { { 0x8C, IMM16 }, { 0xAC, IND }, { 0x9C, DIR }, { 0xBC, EXT } } },
        { "cmpy", { { 0x108C, IMM16 }, { 0x10AC, IND }, { 0x109C, DIR }, { 0x10BC, EXT } } },
        { "cmps", { { 0x118C, IMM16 }, { 0x11AC, IND }, { 0x119C, DIR }, { 0x11BC, EXT } } },
        { "ldd", { { 0xCC, IMM16 }, { 0xEC, IND }, { 0xDC, DIR }, { 0xFC, EXT } } },

        { "bsr", { { 0x8D, REL } } },
        { "jsr", { { 0x9D, IND }, { 0x9D, DIR }, { 0x9D, EXT } } },

        { "std", { { 0xED, IND }, { 0xDD, DIR }, { 0xFD, EXT } } },

        { "ldx", { { 0x8E, IMM16 }, { 0xAE, IND }, { 0x9E, DIR }, { 0xBE, EXT } } },
        { "ldy", { { 0x108E, IMM16 }, { 0x10AE, IND }, { 0x109E, DIR }, { 0x10BE, EXT } } },
        { "ldu", { { 0xCE, IMM16 }, { 0xEE, IND }, { 0xDE, DIR }, { 0xFE, EXT } } },
        { "lds", { { 0x10CE, IMM16 }, { 0x10EE, IND }, { 0x10DE, DIR }, { 0x10FE, EXT } } },

        { "stx", { { 0xAF, IND }, { 0x9F, DIR }, { 0xBF, EXT } } },
        { "sty", { { 0x10AF, IND }, { 0x109F, DIR }, { 0x10BF, EXT } } },
        { "stu", { { 0xEF, IND }, { 0xDF, DIR }, { 0xFF, EXT } } },
        { "sts", { { 0x10EF, IND }, { 0x10DF, DIR }, { 0x10FF, EXT } } },

        { "fcb", { { 0, FCB } } },
        { "fcc", { { 0, FCB } } },
        { "fdb", { { 0, FDB } } },
        { "equ", { { 0, EQU } } },
        { "rmb", { { 0, RMB } } },
        { "org", { { 0, ORG } } },

        { "end", { { 0, IGN } } },
        { "mon", { { 0, IGN } } },
        { "opt", { { 0, IGN } } },
        { "nam", { { 0, IGN } } },
        { "ttl", { { 0, IGN } } },
        { "spc", { { 0, IGN } } },
        { "page", { { 0, IGN } } },

        { 0, { { 0, 0 } } }
};

/* Parse a value (this should really be an expression parser) */
/* We return the value, or a symbol for an as-yet unknown value */

int parse_val(char **buf, int *operand, struct symbol **sy, unsigned short addr)
{
        char str[80];
        *sy = 0;
        if (!parse_dec(buf, operand)) {
                if (parse_word(buf, str)) {
                        char *p;
                        if (!strcmp(str, "*")) {
                                *operand = addr;
                        } else {
                                *sy = find_symbol(str);
                                *operand = (*sy)->val;
                                if ((*sy)->valid)
                                        *sy = 0;
                        }
                        p = *buf;
                        if (*p == '+' || *p == '-') {
                                char c = *p++;
                                int ofst;
                                *buf = p;
                                if (parse_dec(buf, &ofst)) {
                                        if (c == '+')
                                                *operand += ofst;
                                        else
                                                *operand -= ofst;
                                } else {
                                        printf("Missing value after + or -\n");
                                }
                        }
                } else {
                        return 0;
                }
        }
        return 1;
}

int parse_imm(char **buf, int *operand, struct symbol **sy, unsigned short addr)
{
        char *p = *buf;
        if (*p == '#')
        {
                ++p;
                if (parse_val(&p, operand, sy, addr))
                {
                        *buf = p;
                        return 1;
                }
        }
        return 0;
}

int parse_tfr_nibble(char **buf, unsigned char *at_cb)
{
        unsigned char cb = 0;
        char *p = *buf;
        char c = to_upper(p[0]);
        char d = to_upper(p[1]);
        if (c == 'D' && d == 'P') { cb = 0x0B; p += 2; }
        else if (c == 'C' && d == 'C') { cb = 0x0A; p += 2; }
        else if (c == 'D') { cb = 0x00; p += 1; }
        else if (c == 'X') { cb = 0x01; p += 1; }
        else if (c == 'Y') { cb = 0x02; p += 1; }
        else if (c == 'U') { cb = 0x03; p += 1; }
        else if (c == 'S') { cb = 0x04; p += 1; }
        else if (c == 'P' && d == 'C') { cb = 0x05; p += 2; }
        else if (c == 'A') { cb = 0x08; p += 1; }
        else if (c == 'B') { cb = 0x09; p += 1; }
        else return 0;
        *buf = p;
        *at_cb = cb;
        return 1;
}

int parse_tfr(char **buf, unsigned char *at_cb)
{
        unsigned char upper;
        unsigned char lower;
        char *p = *buf;
        if (parse_tfr_nibble(&p, &upper))
        {
                if (*p == ',')
                {
                        ++p;
                        if (parse_tfr_nibble(&p, &lower))
                        {
                                if ((upper < 0x8 && lower < 0x8) || (upper >= 0x8 && lower >= 0x8))
                                {
                                        *buf = p;
                                        *at_cb = (upper << 4) + lower;
                                        return 1;
                                }
                        }
                }
        }
        return 0;
}

int parse_list(char **buf, unsigned char *at_cb, int up)
{
        char *p = *buf;
        unsigned char cb = 0;
        int more;
        do {
                char c = to_upper(p[0]);
                char d = to_upper(p[1]);
                more = 0;
                if (c == 'C' && d == 'C')
                {
                        cb |= 0x01;
                        p += 2;
                }
                else if (c == 'A')
                {
                        cb |= 0x02;
                        p += 1;
                }
                else if (c == 'B')
                {
                        cb |= 0x04;
                        p += 1;
                }
                else if (c == 'D' && d == 'P')
                {
                        cb |= 0x08;
                        p += 2;
                }
                else if (c == 'D')
                {
                        cb |= 0x06;
                        p += 1;
                }
                else if (c == 'X')
                {
                        cb |= 0x10;
                        p += 1;
                }
                else if (c == 'Y')
                {
                        cb |= 0x20;
                        p += 1;
                }
                else if ((!up && c == 'U') || (up && c == 'S'))
                {
                        cb |= 0x40;
                        p += 1;
                }
                else if (c == 'P' && d == 'C')
                {
                        cb |= 0x80;
                        p += 2;
                }
                else
                {
                        break;
                }
                if (*p == ',')
                {
                        p += 1;
                        more = 1;
                }
        } while (more);
        if (cb)
        {
                *at_cb = cb;
                *buf = p;
                return 1;
        }
        return 0;
}

int parse_ireg(char **buf, unsigned char *cb)
{
        char *p = *buf;
        char c = to_upper(*p);
        if (c == 'X')
        {
                ++p;
        }
        else if (c == 'Y')
        {
                *cb |= 0x20;
                ++p;
        }
        else if (c == 'U')
        {
                *cb |= 0x40;
                ++p;
        }
        else if (c == 'S')
        {
                *cb |= 0x60;
                ++p;
        }
        else
        {
                return 0;
        }
        *buf = p;
        return 1;
}

int parse_ind(char **buf, unsigned char *at_cb, int *operand, struct symbol **sy, unsigned short addr, int oplen, int *size)
{
        char *p = *buf;
        int indirect = 0;
        unsigned char cb = 0;
        *operand = 0;
        if (*p == '[')
        {
                ++p;
                indirect = 1;
        }
        *size = 0;
        if (to_upper(p[0]) == 'D' && p[1] == ',')
        {
                cb = 0x8B;
                p += 2;
                if (!parse_ireg(&p, &cb))
                        return 0;
                        
        }
        else if (to_upper(p[0]) == 'A' && p[1] == ',')
        {
                cb = 0x86;
                p += 2;
                if (!parse_ireg(&p, &cb))
                        return 0;
        }
        else if (to_upper(p[0]) == 'B' && p[1] == ',')
        {
                cb = 0x85;
                p += 2;
                if (!parse_ireg(&p, &cb))
                        return 0;
        }
        else if (p[0] == ',')
        {
                ++p;
                cb = 0x84;
                if (p[0] == '-')
                {
                        cb = 0x82;
                        ++p;
                        if (p[0] == '-')
                        {
                                cb = 0x83;
                                ++p;
                        }
                        else if (indirect)
                        {
                                return 0; /* Not allowed */
                        }
                }
                if (!parse_ireg(&p, &cb))
                        return 0;
                if ((cb & 0x8F) == 0x84)
                {
                        if (p[0] == '+')
                        {
                                cb = (cb & 0xF0) + 0x00;
                                ++p;
                                if (p[0] == '+')
                                {
                                        cb = (cb & 0xF0) + 0x01;
                                        ++p;
                                }
                                else if (indirect)
                                {
                                        return 0; /* Not allowed */
                                }
                        }
                }
        }
        else if (parse_val(&p, operand, sy, addr))
        {
                if (*p == ',')
                {
                        ++p;
                        if (to_upper(p[0]) == 'P' && to_upper(p[1]) == 'C' && to_upper(p[2]) == 'R')
                        {
                                int target;
                                p += 3;
                                /* Try one byte offset */
                                target = *operand - (addr + oplen + 2);
                                if (!*sy && target >= -128 && target < 128)
                                {
                                        /* It fits in a single byte */
                                        cb = 0x8C;
                                        *size = 1;
                                        *operand = target;
                                }
                                else
                                {
                                        target = *operand - (addr + oplen + 3); /* Account for larger offset */
                                        *operand = target;
                                        cb = 0x8D;
                                        *size = 2;
                                }
                        }
                        else if (parse_ireg(&p, &cb))
                        {
                                /* n,X modes */
                                if (*sy || *operand > 127 || *operand < -128)
                                {
                                        cb = 0x89;
                                        *size = 2;
                                }
                                else if (*operand > 15 || *operand < -16 || indirect)
                                {
                                        cb = 0x88;
                                        *size = 1;
                                }
                                else
                                {
                                        cb = (0x1F & *operand);
                                        *size = 0;
                                }
                        }
                        else
                        {
                                return 0;
                        }
                }
                else if (indirect)
                {
                        /* Extended */
                        cb = 0x8F;
                        *size = 2;
                }
                else
                {
                        /* Not allowed- prefer DIR/EXT */
                        return 0;
                }
        }
        else
        {
                return 0;
        }
        if (indirect)
        {
                if (*p == ']')
                {
                        ++p;
                        *buf = p;
                        cb |= 0x10;
                        *at_cb = cb;
                        return 1;
                }
                else
                {
                        return 0;
                }
        }
        else
        {
                *buf = p;
                *at_cb = cb;
                return 1;
        }
}

void emit_opcode(unsigned char *mem, unsigned short *addr, unsigned short opcode)
{
        if (opcode >= 0x100)
        {
                mem[*addr] = (opcode >> 8);
                *addr = *addr + 1;
        }
        mem[*addr] = opcode;
        *addr = *addr + 1;
}

unsigned short assemble(unsigned char *mem, unsigned short addr, char *buf)
{
        char str[80];
        unsigned short label_addr = addr;
        struct symbol *label_sy;
        struct symbol *sy;
        struct insn *insn;
        int x;

        label_sy = 0;
        sy = 0;

        if (buf[0] == '*' || !buf[0]) {
                /* Comment line, ignore */
                return addr;
        }

        if (!(buf[0] == ' ' || buf[0] == '\t') && parse_word(&buf, str)) {
                /* A label */
                label_sy = find_symbol(str);

                skipws(&buf);

                if (!parse_word(&buf, str)) {
                        goto done;
                }
        } else {
                skipws(&buf);
                if (!parse_word(&buf, str)) {
                        printf("Huh?\n");
                        return addr;
                }
        }

        /* Lookup instruction */
        for (x = 0; table[x].insn; ++x)
                if (!jstricmp(table[x].insn, str)) {
                        insn = &table[x];
                        break;
                }

        if (!insn) {
                printf("Huh?\n");
                return addr;
        }

        skipws(&buf);

        /* Determine instruction variant based on operand
           First one that works wins */
        for (x = 0; x != 4 && insn->variant[x].type; ++x)
                switch (insn->variant[x].type)
                {
                        case IGN:
                        {
                                goto done;
                        }
                        case NONE:
                        {
                                emit_opcode(mem, &addr, insn->variant[x].opcode);
                                goto done;
                        }
                        case REL:
                        {
                                int operand;
                                if (parse_val(&buf, &operand, &sy, addr))
                                {
                                        emit_opcode(mem, &addr, insn->variant[x].opcode);
                                        if (sy)
                                        {
                                                mem[addr] = 0;
                                                add_fixup(sy, addr, FIXUP_REL, operand);
                                        }
                                        else
                                        {
                                                mem[addr] = operand - (addr + 1);
                                        }
                                        addr++;
                                        goto done;
                                }
                                break;
                        }
                        case LREL:
                        {
                                int operand;
                                if (parse_val(&buf, &operand, &sy, addr))
                                {
                                        emit_opcode(mem, &addr, insn->variant[x].opcode);
                                        if (sy)
                                        {
                                                mem[addr] = 0;
                                                mem[addr+1] = 0;
                                                add_fixup(sy, addr, FIXUP_LREL, operand);
                                        }
                                        else
                                        {
                                                unsigned short v = operand - (addr + 2);
                                                mem[addr] = (v >> 8);
                                                mem[addr + 1] = v;
                                        }
                                        addr += 2;
                                        goto done;
                                }
                                break;
                        }
                        case IMM8:
                        {
                                int operand;
                                if (parse_imm(&buf, &operand, &sy, addr))
                                {
                                        emit_opcode(mem, &addr, insn->variant[x].opcode);
                                        if (sy)
                                        {
                                                mem[addr] = 0;
                                                add_fixup(sy, addr, FIXUP_DIR, operand);
                                        }
                                        else
                                        {
                                                mem[addr] = operand;
                                        }
                                        addr += 1;
                                        goto done;
                                }
                                break;
                        }
                        case IMM16:
                        {
                                int operand;
                                if (parse_imm(&buf, &operand, &sy, addr))
                                {
                                        emit_opcode(mem, &addr, insn->variant[x].opcode);
                                        if (sy)
                                        {
                                                mem[addr] = 0;
                                                mem[addr + 1] = 0;
                                                add_fixup(sy, addr, FIXUP_EXT, operand);
                                        }
                                        else
                                        {
                                                mem[addr] = operand >> 8;
                                                mem[addr+1] = operand;
                                        }
                                        addr += 2;
                                        goto done;
                                }
                                break;
                        }
                        case TFR:
                        {
                                unsigned char cb;
                                emit_opcode(mem, &addr, insn->variant[x].opcode);
                                if (parse_tfr(&buf, &cb))
                                {
                                        mem[addr] = cb;
                                }
                                else
                                {
                                        mem[addr] = 0;
                                        printf("Syntax error\n");
                                }
                                addr += 1;
                                goto done;
                        }
                        case SLIST:
                        {
                                unsigned char cb;
                                emit_opcode(mem, &addr, insn->variant[x].opcode);
                                if (parse_list(&buf, &cb, 0))
                                {
                                        mem[addr] = cb;
                                }
                                else
                                {
                                        mem[addr] = 0;
                                        printf("Syntax error\n");
                                }
                                addr += 1;
                                goto done;
                        }
                        case ULIST:
                        {
                                unsigned char cb;
                                emit_opcode(mem, &addr, insn->variant[x].opcode);
                                if (parse_list(&buf, &cb, 1))
                                {
                                        mem[addr] = cb;
                                }
                                else
                                {
                                        mem[addr] = 0;
                                        printf("Syntax error\n");
                                }
                                addr += 1;
                                goto done;
                        }
                        case IND:
                        {
                                unsigned char cb;
                                int size;
                                int operand;
                                if (parse_ind(&buf, &cb, &operand, &sy, addr, (insn->variant[x].opcode >= 0x100 ? 2 : 1), &size))
                                {
                                        emit_opcode(mem, &addr, insn->variant[x].opcode);
                                        mem[addr++] = cb;
                                        if (size == 2)
                                        {
                                                if (sy)
                                                        add_fixup(sy, addr, FIXUP_EXT, operand);
                                                mem[addr++] = (operand >> 8);
                                                mem[addr++] = operand;
                                        }
                                        else if (size == 1)
                                        {
                                                if (sy)
                                                        add_fixup(sy, addr, FIXUP_DIR, operand);
                                                mem[addr++] = operand;
                                        }
                                        goto done;
                                }
                                break;
                        }
                        case DIR:
                        {
                                int operand;
                                char *org = buf;
                                if (parse_val(&buf, &operand, &sy, addr))
                                {
                                        if (!sy && operand >= 0 && operand < 0x100)
                                        {
                                                emit_opcode(mem, &addr, insn->variant[x].opcode);
                                                mem[addr] = operand;
                                                addr += 1;
                                                goto done;
                                        }
                                        else
                                        {
                                        /* If we don't know the operand yet or if value is
                                           out of range, fail here so that EXT mode is used */
                                                buf = org;
                                        }
                                }
                                break;
                        }
                        case EXT:
                        {
                                int operand;
                                if (parse_val(&buf, &operand, &sy, addr))
                                {
                                        emit_opcode(mem, &addr, insn->variant[x].opcode);
                                        if (sy)
                                        {
                                                mem[addr] = 0;
                                                mem[addr + 1] = 0;
                                                add_fixup(sy, addr, FIXUP_EXT, operand);
                                        }
                                        else
                                        {
                                                mem[addr] = operand >> 8;
                                                mem[addr+1] = operand;
                                        }
                                        addr += 2;
                                        goto done;
                                }
                                break;
                        }
                        case FCB:
                        {
                                char c;
                                if (!*buf)
                                {
                                        printf("Missing argument\n");
                                        goto done;
                                }
                                for (;;)
                                {
                                        /* Special case for FCB/FCC */
                                        int operand;
                                        if (*buf == '"' || *buf == '/')
                                        {
                                                c = *buf++;
                                                while (*buf && *buf != c)
                                                {
                                                        mem[addr++] = *buf++;
                                                }
                                                if (*buf)
                                                        ++buf;
                                        }
                                        else if (parse_val(&buf, &operand, &sy, addr))
                                        {
                                                if (sy)
                                                {
                                                        add_fixup(sy, addr, FIXUP_DIR, operand);
                                                        mem[addr++] = 0;
                                                }
                                                else
                                                {
                                                        if ((operand >= 0 && (operand & ~0xFF)) ||
                                                            (operand < 0 && ((operand & ~0xFF) + 0x100)))
                                                                printf("Argument range error\n");
                                                        mem[addr++] = operand;
                                                }
                                        }
                                        else
                                        {
                                                mem[addr++] = 0;
                                        }
                                        skipws(&buf);
                                        if (buf[0] == ',')
                                        {
                                                ++buf;
                                                skipws(&buf);
                                        }
                                        else
                                        {
                                                break;
                                        }
                                }
                                goto done;
                        }
                        case FDB:
                        {
                                if (!*buf)
                                {
                                        printf("Missing argument\n");
                                        goto done;
                                }
                                for (;;)
                                {
                                        int operand;
                                        if (parse_val(&buf, &operand, &sy, addr))
                                        {
                                                if (sy)
                                                {
                                                        add_fixup(sy, addr, FIXUP_EXT, operand);
                                                        mem[addr++] = 0;
                                                        mem[addr++] = 0;
                                                }
                                                else
                                                {
                                                        mem[addr++] = (operand >> 8);
                                                        mem[addr++] = operand;
                                                }
                                        }
                                        else
                                        {
                                                mem[addr++] = 0;
                                                mem[addr++] = 0;
                                        }
                                        skipws(&buf);
                                        if (buf[0] == ',')
                                        {
                                                ++buf;
                                                skipws(&buf);
                                        }
                                        else
                                        {
                                                break;
                                        }
                                }
                                goto done;
                        }
                        case RMB:
                        {
                                int operand;
                                if (parse_val(&buf, &operand, &sy, addr))
                                {
                                        if (sy)
                                        {
                                                printf("Resolved symbol required for rmb\n");
                                        }
                                        else
                                        {
                                                addr += operand;
                                        }
                                }
                                else
                                {
                                        printf("Missing argument\n");
                                }
                                goto done;
                        }
                        case EQU:
                        {
                                int operand;
                                if (parse_val(&buf, &operand, &sy, addr))
                                {
                                        if (sy) {
                                                printf("Resolved symbol required for equ\n");
                                        } else if (!label_sy) {
                                                printf("Label required for equ\n");
                                        } else {
                                                label_addr = operand;
                                        }
                                }
                                else
                                {
                                        printf("Missing argument\n");
                                }
                                goto done;
                        }
                        case ORG:
                        {
                                int operand;
                                if (parse_val(&buf, &operand, &sy, addr))
                                {
                                        if (sy)
                                        {
                                                printf("Resolved symbol required for org\n");
                                        }
                                        else
                                        {
                                                label_addr = addr = operand;
                                        }
                                }
                                else
                                {
                                        printf("Missing argument\n");
                                }
                                goto done;
                        }
                }
        printf("Syntax error\n");

        done:
        set_symbol(mem, label_sy, label_addr);
        return addr;
}
