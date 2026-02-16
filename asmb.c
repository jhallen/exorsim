/*	EXORcister simulator
 *	Copyright
 *		(C) 2026 Joseph H. Allen
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

/* Stand alone MC6800 absolute cross assembler
   This one understands the original Motorola MC6800 syntax
   Generates S19 records and listing. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Options - disable them all to be compatible with original MC6800 assembler */
#define ALLOW_TABS
#define ALLOW_SEMI_COMMENTS
#define ALLOW_TRAILING_QUOTE /* Allow 'A' instead of 'A */
#define ALLOW_BLANK_LINES
#define ALLOW_LEADING_WS /* Allow leading whitespace before * or ; comment */
#define ALLOW_EXTRA_IDENT_CHARS /* Allow ., $ and _ in identifiers */

FILE *infile;
FILE *outfile;
FILE *listing;

char option_listing;
char option_memory;
char option_symbols;
char option_generate;
char option_object;

char pass; /* Set for 2nd pass */

int location; /* Current location */

char inbuf[128];
char *inptr; /* Line input pointer */
char known; /* True if all symbols in the expression are known */
char knownfirst; /* True if all symbols in the expression were known in first pass */
int val; /* Value of expression */
char strval[128]; /* Value of string */

int error_count;

/* Information about current line */
char line_number_mode; /* Set if each input line has a line number */
int line_number;
int user_number; /* User supplied line number */
char line_label[32];
struct insn *line_insn;

/* True for whitespace */

int iswhite(char c)
{
#ifdef ALLOW_TABS
    return c == ' ' || c == '\t';
#else
    return c == ' ';
#endif
}

/* Print an error */

void error(char *s)
{
    if (pass && listing)
        fprintf(listing, "*** %d: %s\n", user_number, s);
    fprintf(stderr, "*** %d: %s\n", user_number, s);
    ++error_count;
}

/* Fold ASCII character to upper case */

char fold(char c)
{
    if (c >= 'a' && c <= 'z')
        return c + 'A' - 'a';
    else
        return c;
}

/* Compare strings ignoring case */

int istrcmp(char *l, char *r)
{
    while (*l && *r && fold(*l) == fold(*r))
    {
        ++l;
        ++r;
    }
    if (*l > *r)
        return 1;
    else if (*l < *r)
        return -1;
    else
        return 0;
}

/* Listing */

/* Field positions */
#define LIST_LINE_NO 0
#define LIST_ADDR 6
#define LIST_DATA 11
//#define LIST_INSN 11
//#define LIST_DATA 14
#define LIST_LABEL 23
#define LIST_MNEMONIC 30
#define LIST_OPERAND 37
#define LIST_COMMENT 54 /* Initial position: this is bumped if LIST_OPERAND is long */

char listbuf[256]; /* Listing line buffer */
unsigned char listextra[256]; /* Buffer for FCC data that spills current line */
int extraidx;
unsigned short extraaddr;

char listaddr; /* Clear if we've not yet printed the line's address */
char listdata; /* Where next data gets printed */
int comment_start;

/* Like strcpy, but no NUL */

void fieldcpy(char *d, char *s)
{
    while (*s)
    {
        *d++ = *s++;
    }
}

/* Like fieldcpy, but skip periods in the source string and stop at space */

void fieldcpy1(char *d, char *s)
{
    while (*s != ' ')
    {
        if (*s != '.')
            *d++ = *s++;
        else
            ++s;
    }
}

/* Clear listing line */

void list_clear()
{
    listaddr = 0;
    memset(listbuf, ' ', sizeof(listbuf));
    listdata = LIST_DATA;
    comment_start = LIST_COMMENT;
}

/* Begin a new listing line */

void list_start()
{
    char buf[20];
    list_clear();
    extraidx = 0;
    sprintf(buf, "%5d", user_number);
    fieldcpy(listbuf + 0, buf); // Line number
}

/* Remove trailing spaces and zero-terminate */
/* Assumes right-most column in never written */

void trim_listbuf()
{
    int i;
    for (i = sizeof(listbuf)-1; i && listbuf[i-1] == ' '; --i);
    listbuf[i] = 0;
}

/* Insert address into listing line */

void list_addr(unsigned short addr)
{
    char buf[20];
    if (!listaddr)
    {
        sprintf(buf, "%4.4X", addr);
        fieldcpy(listbuf + LIST_ADDR, buf);
        listaddr = 1;
    }
}

/* Insert label into listing line */

void list_label(char *s)
{
    fieldcpy(listbuf + LIST_LABEL, s);
}

/* Insert instruction into listing line */

void list_insn(char *s)
{
    fieldcpy1(listbuf + LIST_MNEMONIC, s);
}

/* Insert operand into listing line: bump comment position if needed */

void list_operand(char *s, int len)
{
    memcpy(listbuf + LIST_OPERAND, s, len);
    if (len > (LIST_COMMENT - LIST_OPERAND - 1))
        // Move comment.. operand is large
        comment_start += len - (LIST_COMMENT - LIST_OPERAND - 1);
}

/* Insert comment into listing line */

void list_comment(char *s)
{
    fieldcpy(listbuf + comment_start, s);
}

/* Insert output byte into listing line */

void list_byte(unsigned char val)
{
    if (listdata + 3 <= LIST_LABEL)
    {
        char buf[4];
        sprintf(buf, "%2.2X", val);
        fieldcpy(listbuf + listdata, buf);
        listdata += 3;
    }
    else
    {
        /* Doesn't fit, append it to spill buffer */
        if (!extraidx)
            extraaddr = location; /* Record starting location of spill */
        listextra[extraidx++] = val;
    }
}

/* Insert output word into listing line */

void list_word(unsigned short val)
{
    if (listdata + 5 <= LIST_LABEL)
    {
        char buf[6];
        sprintf(buf, "%4.4X", val);
        fieldcpy(listbuf + listdata, buf);
        listdata += 5;
    }
}

/* Print listbuf */

void list_out()
{
    trim_listbuf();
    if (listing)
    {
        fputs(listbuf, listing);
        fputc('\n', listing);
    }
}

/* Listing line is done, print it, then print any spillover FCC data */

void list_done()
{
    if (pass)
    {
        int i = 0;
        list_out();

        while (i < extraidx)
        {
            list_clear();
            listbuf[4] = '+';
            list_addr(extraaddr);
            while (i < extraidx && listdata + 3 <= LIST_LABEL)
            {
                list_byte(listextra[i++]);
                extraaddr++;
            }
            list_out();
        }
    }
}

/* Emit S records */

#define OUTBUF_SIZE 32
unsigned char outbuf[OUTBUF_SIZE];
int outaddr = 0;
int outidx = 0;

void emit_hex2(unsigned char c)
{
    fputc("0123456789ABCDEF"[0xF & (c >> 4)], outfile);
    fputc("0123456789ABCDEF"[0xF & c], outfile);
}

void emit_hex4(unsigned short val)
{
    emit_hex2(val >> 8);
    emit_hex2(val);
}

void emit_flush()
{
    if (pass && outfile && outidx)
    {
        int i;
        unsigned char cksum = outidx + 2 + 1;
        fputs("S1", outfile);
        emit_hex2(cksum); // Count
        emit_hex4(outaddr);
        cksum += (255 & (outaddr >> 8));
        cksum += (255 & outaddr);
        for (i = 0; i != outidx; ++i)
        {
            emit_hex2(outbuf[i]);
            cksum += outbuf[i];
        }
        emit_hex2(~cksum);
        fputs("\n", outfile);
        outaddr += outidx;
        outidx = 0;
    }
}

void emitb(unsigned char val)
{
    unsigned short addr = location;
    if (pass && outfile)
    {
        if (addr != outaddr + outidx)
        {
            emit_flush();
            /* Start a new record */
            outaddr = addr;
        }

        outbuf[outidx++] = val;

        /* Flush if we have too much */
        if (outidx == OUTBUF_SIZE)
        {
            emit_flush();
            outaddr = addr;
        }
    }
    ++location;
}

void emit(unsigned char val)
{
    if (pass && listing)
    {
        list_addr(location); /* Print address if we don't already have it */
        list_byte(val);
    }
    emitb(val);
}

void emitw(unsigned short val)
{
    if (pass && listing)
    {
        list_addr(location);
        list_word(val);
    }
    emitb(val >> 8);
    emitb(val);
}

void emits0(char *name)
{
    if (pass && outfile)
    {
        int l = strlen(name);
        unsigned char cksum = l + 3;
        fputs("S0", outfile);
        emit_hex2(cksum);
        fputs("0000", outfile);
        while (*name)
        {
            cksum += *name;
            emit_hex2(*name++);
        }
        emit_hex2(~cksum);
        fputs("\n", outfile);
    }
}

void emit_final()
{
    if (pass && outfile)
    {
        emit_flush();
        fputs("S9030000FC\n", outfile);
    }
}

/* Symbol table */

#define SYMTAB_SIZE 1024
#define SET_FIRST 1
#define SET_SECOND 2
#define SET (SET_FIRST | SET_SECOND)

struct symbol
{
    char name[7];
    char flag;
    int val;
} symtab[SYMTAB_SIZE];

int symtab_idx;

struct symbol *lookup_symbol(char *s)
{
    int i;
    for (i = 0; i != symtab_idx; ++i)
        if (!strcmp(symtab[i].name, s))
            return &symtab[i];
    if (i == SYMTAB_SIZE)
    {
        error("Too many symbols");
        exit(-1);
    }
    ++symtab_idx;
    strcpy(symtab[i].name, s);
    return &symtab[i];
}

/* Table of instructions */
/* Within an instruction mnemonic: . means allow whitespace.  ' ' (space)
   terminates the string.  This is to match what the assembler is going to
   do */

#define OPBRA 0
#define OPBYTE 1
#define OPEND 2
#define OPEQU 3
#define OPFCB 4
#define OPFCC 5
#define OPFDB 6
#define OPJMP 7
#define OPNAM 8
#define OPNONE 9
#define OPOPT 10
#define OPORG 11
#define OPPAGE 12
#define OPRMB 13
#define OPRMW 14
#define OPSAVE 15
#define OPSPC 16
#define OPWORD 17

struct insn
{
    char name[6];
    unsigned char opcode;
    unsigned char type;
} insns[] = {
    { "ABA ", 0x1B, OPNONE },
    { "ADC.A ", 0x89, OPBYTE },
    { "ADC.B ", 0xC9, OPBYTE },
    { "ADD.A ", 0x8B, OPBYTE },
    { "ADD.B ", 0xCB, OPBYTE },
    { "AND.A ", 0x84, OPBYTE },
    { "AND.B ", 0xC4, OPBYTE },
    { "ASL.A ", 0x48, OPNONE },
    { "ASL.B ", 0x58, OPNONE },
    { "ASL ", 0x48, OPRMW },
    { "ASR.A ", 0x47, OPNONE },
    { "ASR.B ", 0x57, OPNONE },
    { "ASR ", 0x47, OPRMW },
    { "BCC ", 0x24, OPBRA },
    { "BCS ", 0x25, OPBRA },
    { "BEQ ", 0x27, OPBRA },
    { "BGE ", 0x2C, OPBRA },
    { "BGT ", 0x2E, OPBRA },
    { "BHI ", 0x22, OPBRA },
    { "BIT.A ", 0x85, OPBYTE },
    { "BIT.B ", 0xC5, OPBYTE },
    { "BLE ", 0x2F, OPBRA },
    { "BLS ", 0x23, OPBRA },
    { "BLT ", 0x2D, OPBRA },
    { "BMI ", 0x2B, OPBRA },
    { "BNE ", 0x26, OPBRA },
    { "BPL ", 0x2A, OPBRA },
    { "BRA ", 0x20, OPBRA },
    { "BSR ", 0x8D, OPBRA },
    { "BVC ", 0x28, OPBRA },
    { "BVS ", 0x29, OPBRA },
    { "CBA ", 0x11, OPNONE },
    { "CLC ", 0x0C, OPNONE },
    { "CLI ", 0x0E, OPNONE },
    { "CLR.A ", 0x4F, OPNONE },
    { "CLR.B ", 0x5F, OPNONE },
    { "CLR ", 0x4F, OPRMW },
    { "CLV ", 0x0A, OPNONE },
    { "CMP.A ", 0x81, OPBYTE },
    { "CMP.B ", 0xC1, OPBYTE },
    { "COM.A ", 0x43, OPNONE },
    { "COM.B ", 0x53, OPNONE },
    { "COM ", 0x43, OPRMW },
    { "CPX ", 0x8C, OPWORD },
    { "DAA ", 0x19, OPNONE },
    { "DEC.A ", 0x4A, OPNONE },
    { "DEC.B ", 0x5A, OPNONE },
    { "DEC ", 0x4A, OPRMW },
    { "DES ", 0x34, OPNONE },
    { "DEX ", 0x09, OPNONE },
    { "END ", 0, OPEND },
    { "EOR.A ", 0x88, OPBYTE },
    { "EOR.B ", 0xC8, OPBYTE },
    { "EQU ", 0, OPEQU },
    { "FCB ", 0, OPFCB },
    { "FCC ", 0, OPFCC },
    { "FDB ", 0, OPFDB },
    { "INC.A ", 0x4C, OPNONE },
    { "INC.B ", 0x5C, OPNONE },
    { "INC ", 0x4C, OPRMW },
    { "INS ", 0x31, OPNONE },
    { "INX ", 0x08, OPNONE },
    { "JMP ", 0x6E, OPJMP },
    { "JSR ", 0xAD, OPJMP },
    { "LDA.A ", 0x86, OPBYTE },
    { "LDA.B ", 0xC6, OPBYTE },
    { "LDS ", 0x8E, OPWORD },
    { "LDX ", 0xCE, OPWORD },
    { "LSR.A ", 0x44, OPNONE },
    { "LSR.B ", 0x54, OPNONE },
    { "LSR ", 0x44, OPRMW },
    { "NAM ", 0, OPNAM },
    { "NEG.A ", 0x40, OPNONE },
    { "NEG.B ", 0x50, OPNONE },
    { "NEG ", 0x40, OPRMW },
    { "NOP ", 0x01, OPNONE },
    { "OPT ", 0, OPOPT },
    { "ORA.A ", 0x8A, OPBYTE },
    { "ORA.B ", 0xCA, OPBYTE },
    { "ORG ", 0, OPORG },
    { "PAGE ", 0, OPPAGE },
    { "PSH.A ", 0x36, OPNONE },
    { "PSH.B ", 0x37, OPNONE },
    { "PUL.A ", 0x32, OPNONE },
    { "PUL.B ", 0x33, OPNONE },
    { "RMB ", 0, OPRMB },
    { "ROL.A ", 0x49, OPNONE },
    { "ROL.B ", 0x59, OPNONE },
    { "ROL ", 0x49, OPRMW },
    { "ROR.A ", 0x46, OPNONE },
    { "ROR.B ", 0x56, OPNONE },
    { "ROR ", 0x46, OPRMW },
    { "RTI ", 0x3B, OPNONE },
    { "RTS ", 0x39, OPNONE },
    { "SBA ", 0x10, OPNONE },
    { "SBC.A ", 0x82, OPBYTE },
    { "SBC.B ", 0xC2, OPBYTE },
    { "SEC ", 0x0D, OPNONE },
    { "SEI ", 0x0F, OPNONE },
    { "SEV ", 0x0B, OPNONE },
    { "SPC ", 0, OPSPC },
    { "STA.A ", 0x87, OPSAVE },
    { "STA.B ", 0xC7, OPSAVE },
    { "STS ", 0x8F, OPSAVE },
    { "STX ", 0xCF, OPSAVE },
    { "SUB.A ", 0x80, OPBYTE },
    { "SUB.B ", 0xC0, OPBYTE },
    { "SWI ", 0x3F, OPNONE },
    { "TAB ", 0x16, OPNONE },
    { "TAP ", 0x06, OPNONE },
    { "TBA ", 0x17, OPNONE },
    { "TPA ", 0x07, OPNONE },
    { "TST.A ", 0x4D, OPNONE },
    { "TST.B ", 0x5D, OPNONE },
    { "TST ", 0x4D, OPRMW },
    { "TSX ", 0x30, OPNONE },
    { "TXS ", 0x35, OPNONE },
    { "WAI ", 0x3E, OPNONE },
    { "", 0x00, 0x00 }
};

/* Skip whitespace */

void skipws()
{
    while (iswhite(*inptr))
        ++inptr;
}

/* Skip alphanumeric */

void skipalnum()
{
    while ((*inptr >= '0' && *inptr <= '9') ||
           (*inptr >= 'A' && *inptr <= 'Z') ||
           (*inptr >= 'a' && *inptr <= 'z'))
       ++inptr;
}

/* Parse decimal number */

int parse_decimal()
{
    if (*inptr >= '0' && *inptr <= '9')
    {
        val = 0;
        while (*inptr >= '0' && *inptr <= '9')
        {
            val = val * 10 + *inptr - '0';
            ++inptr;
        }
        return 1;
    }
    return 0;
}

/* Parse binary number */

int parse_binary()
{
    if (*inptr >= '0' && *inptr <= '1')
    {
        val = 0;
        while (*inptr >= '0' && *inptr <= '1')
        {
            val = val * 2 + *inptr - '0';
            ++inptr;
        }
        return 1;
    }
    return 0;
}

/* Parse octal number */

int parse_octal()
{
    if (*inptr >= '0' && *inptr <= '7')
    {
        val = 0;
        while (*inptr >= '0' && *inptr <= '7')
        {
            val = val * 8 + *inptr - '0';
            ++inptr;
        }
        return 1;
    }
    return 0;
}

/* Parse hexadecimal number */

int parse_hex()
{
    if ((*inptr >= '0' && *inptr <= '9') ||
        (*inptr >= 'a' && *inptr <= 'f') ||
        (*inptr >= 'A' && *inptr <= 'F'))
    {
        val = 0;
        while ((*inptr >= '0' && *inptr <= '9') ||
               (*inptr >= 'a' && *inptr <= 'f') ||
               (*inptr >= 'A' && *inptr <= 'F'))
        {
            if (*inptr >= '0' && *inptr <= '9')
                val = val * 16 + *inptr - '0';
            else if (*inptr >= 'a' && *inptr <= 'f')
                val = val * 16 + *inptr - 'a' + 10;
            else if (*inptr >= 'A' && *inptr <= 'F')
                val = val * 16 + *inptr - 'A' + 10;
            ++inptr;
        }
        return 1;
    }
    return 0;
}

/* Parse numeric constant */

int parse_numeric()
{
    val = 0;
    if (*inptr == '%')
    {
        /* Binary */
        ++inptr;
        if (!parse_binary())
        {
            error("Bad binary constant");
        }
    }
    else if (*inptr == '$')
    {
        /* Hexadecimal */
        ++inptr;
        if (!parse_hex())
        {
            error("Bad hex constant");
        }
    }
    else if (*inptr == '@')
    {
        /* Octal */
        ++inptr;
        if (!parse_octal())
        {
            error("Bad octal constant");
        }
    }
    else if (*inptr == '\'')
    {
        /* Character */
        ++inptr;
        if (*inptr) {
            val = *(unsigned char *)inptr;
            ++inptr;
#ifdef ALLOW_TRAILING_QUOTE
            if (*inptr == '\'')
                ++inptr;
#endif
        }
        else
        {
            error("Bad character constant");
        }
    }
    else if (*inptr >= '0' && *inptr <= '9')
    {
        char *holdptr = inptr;
        char *eptr;
        /* Decimal, hexadecimal, octal, or binary */
        /* Find last alphanumeric, then decide type based on: H, O, Q, B or decimal. */
        /* Then parse depending on known type */
        skipalnum();
        eptr = inptr - 1;
        inptr = holdptr;
        if (*eptr == 'o' || *eptr == 'O' || *eptr == 'q' || *eptr == 'Q')
        {
            if (parse_octal() && inptr == eptr)
            {
                /* Good */
            }
            else
            {
                error("Bad octal constant");
            }
        }
        else if (*eptr == 'b' || *eptr == 'B')
        {
            if (parse_binary() && inptr == eptr)
            {
                /* Good */
            }
            else
            {
                error("Bad binary constant");
            }
        }
        else if (*eptr == 'h' || *eptr == 'H')
        {
            if (parse_hex() && inptr == eptr)
            {
                /* Good */
            }
            else
            {
                inptr = eptr + 1;
                error("Bad hex constant");
            }
        }
        else if (*inptr >= '0' && *inptr <= '9')
        {
            if (parse_decimal() && inptr == eptr + 1)
            {
                /* Good */
            }
            else
            {
                inptr = eptr + 1;
                error("Bad decimal constant");
            }
        }
        else
        {
            error("Bad numeric constant");
        }
        inptr = eptr + 1;
    }
    else
    {
        return 0;
    }
}

/* Parse an identifier */

int parse_ident()
{
    int i = 0;
    strval[i] = 0;
    if (*inptr >= 'a' && *inptr <= 'z' || *inptr >= 'A' && *inptr <= 'Z'
#ifdef ALLOW_EXTRA_IDENT_CHARS
        || *inptr == '.'
#endif
       )
    {
        while (*inptr >= 'a' && *inptr <= 'z' || *inptr >= 'A' && *inptr <= 'Z' || *inptr >= '0' && *inptr <= '9'
#ifdef ALLOW_EXTRA_IDENT_CHARS
               || *inptr == '.' || *inptr == '$' || *inptr == '_'
#endif
        )
        {
            strval[i++] = *inptr++;
        }
        strval[i] = 0;
        return 1;
    }
    return 0;
}

/* Match an identifier */

int match_ident(char *s)
{
    char *holdptr = inptr;
    if (parse_ident())
    {
        if (!istrcmp(strval, s))
            return 1;
        inptr = holdptr;
    }
    return 0;
}

/* Parse a string */

int parse_string()
{
    if (*inptr)
    {
        int i;
        char delim = *inptr++;
        for (i = 0; *inptr && *inptr != delim; ++i)
            strval[i] = *inptr++;
        strval[i] = 0;
        if (*inptr)
            ++inptr;
        else
            error("Missing string delimiter");
        return 1;
    }
    return 0;
}

/* Parse module name */

int parse_module_name()
{
    if (*inptr && *inptr != ' ' && *inptr != '\t')
    {
        int i;
        for (i = 0; *inptr && *inptr != ' ' && *inptr != '\t'; ++i)
            strval[i] = *inptr++;
        strval[i] = 0;
        return 1;
    }
    return 0;
}

/* Parse an expression, set 'known' */

int parse_term()
{
    if (parse_numeric())
    {
        /* OK */
        return 1;
    }
    else if (parse_ident())
    {
        /* Do not allow X */
        if (!strcmp(strval, "x"))
        {
            /* Hmm */
            --inptr;
            return 0;
        }
        else
        {
            struct symbol *sy = lookup_symbol(strval);
            val = sy->val;
            if (pass)
            {
                if (!(sy->flag & SET))
                {
                    error("Undefined symbol");
                    known = 0;
                }
                if (!(sy->flag & SET_FIRST))
                    knownfirst = 0;
            }
            else
            {
                if (!(sy->flag & SET))
                {
                    /* It's OK if undefined on first pass */
                    known = 0;
                    knownfirst = 0;
                }
            }
            return 1;
        }
    }
    else if (*inptr == '*')
    {
        ++inptr;
        val = location;
        return 1;
    }
    else
    {
        return 0;
    }
}

int parse_expr()
{
    known = 1;
    knownfirst = 1;
    if (parse_term())
    {
        /* Extend it.. */
        char *hold;
        more:
        hold = inptr;
        skipws();
        if (*inptr == '-')
        {
            int tmp = val;
            ++inptr;
            skipws();
            if (parse_term())
            {
                val = tmp - val;
                goto more;
            }
            else
            {
                error("Bad expression");
            }
        }
        else if (*inptr == '+')
        {
            int tmp = val;
            ++inptr;
            skipws();
            if (parse_term())
            {
                val = tmp + val;
                goto more;
            }
            else
            {
                error("Bad expression");
            }
        }
        else if (*inptr == '*')
        {
            int tmp = val;
            ++inptr;
            skipws();
            if (parse_term())
            {
                val = tmp * val;
                goto more;
            }
            else
            {
                error("Bad expression");
            }
        }
        else if (*inptr == '/')
        {
            int tmp = val;
            ++inptr;
            skipws();
            if (parse_term())
            {
                val = tmp / val;
                goto more;
            }
            else
            {
                error("Bad expression");
            }
        }
        else
        {
            inptr = hold;
        }
        return 1;
    }
    return 0;
}

/* Parse instruction */

struct insn *parse_insn()
{
    struct insn *i;
    for (i = &insns[0]; i->name[0]; ++i)
    {
        char *l, *r;
        l = i->name;
        r = inptr;
        while (*l != ' ' && *r && *l == fold(*r))
        {
            ++l;
            ++r;
        }
        if (*l == '.')
        {
            /* Whitespace is allowed here */
            ++l;
            while (iswhite(*r))
                ++r;
        }
        while (*l != ' ' && *r && *l == fold(*r))
        {
            ++l;
            ++r;
        }
        if (*l == ' ' && (!*r || iswhite(*r)))
        {
            /* We found it! */
            inptr = r;
            return i;
        }
    }
    /* Not found. */
    return 0;
}

/* Parse address mode */

int parse_mode()
{
    int mode = 0;
    val = 0;
    if (*inptr == '#')
    {
        ++inptr;
        // Immediate mode
        skipws();
        if (!parse_expr())
        {
            error("Missing immediate operand");
        }
        mode = 1;
    }
    else if (parse_expr())
    {
        // Indexed or direct
        char *hold = inptr;
        skipws();
        if (*inptr == ',')
        {
            // Indexed
            ++inptr;
            hold = inptr;
            skipws();
            if (!match_ident("x"))
            {
                inptr = hold;
            }
            mode = 2;
        }
        else
        {
            inptr = hold;
            // Direct or extended
            mode = 3;
        }
    }
    else if (*inptr == ',')
    {
        // Maybe 0 indexed
        known = 1;
        ++inptr;
        skipws();
        if (!match_ident("x"))
        {
            error("Missing index register");
        }
        mode = 2;
    }
    else if (match_ident("x"))
    {
        // 0 Indexed
        known = 1;
        mode = 2;
    }
    return mode;
}

/* Parse input line */

int parse_line()
{
    int last = 0;
    char *operand_start;
    char *nhold;
    char *hold;

    ++line_number;
    user_number = line_number;
    line_label[0] = 0;

    /* Deal with user supplied line number */
    if (parse_decimal())
    {
        if (line_number == 1)
            line_number_mode = 1;
        user_number = val;

        /* Line number must be followed by a single space */
        if (!iswhite(*inptr))
        {
            error("Missing space after line number");
            return 0;
        }
        else
        {
            ++inptr;
        }

        if (!line_number_mode)
        {
            error("Unexpected line number");
            return 0;
        }
    }
    else
    {
        if (line_number == 1)
            line_number_mode = 0;
        if (line_number_mode)
        {
            error("Missing line number");
            return 0;
        }
    }

    list_start();

#ifdef ALLOW_LEADING_WS
    hold = inptr;
    skipws();
#endif

#ifdef ALLOW_BLANK_LINES
    if (!*inptr)
        goto blank_line;
#endif

    /* Maybe we have a comment? */
#ifdef ALLOW_SEMI_COMMENTS
    if (*inptr == '*' || *inptr == ';')
#else
    if (*inptr == '*')
#endif
    {
#ifdef ALLOW_LEADING_WS
        inptr = hold;
#endif
        /* This is a comment line */
        fieldcpy(listbuf + LIST_LABEL, inptr);
        list_done();
        return 0;
    }
#ifdef ALLOW_LEADING_WS
        inptr = hold;
#endif


    /* Maybe we have a label? */
    if (parse_ident())
    {
        skipws();
        strcpy(line_label, strval);
        list_label(line_label);
    }

    /* Skip whitespace before opcode */
    skipws();

    /* Find instruction */
    line_insn = parse_insn();

    if (!line_insn)
    {
        fieldcpy(listbuf + 11, inptr);
        error("Missing or unknown mnemonic or directive");
        list_done();
        return 0;
    }

    list_insn(line_insn->name);

    if (line_label[0])
    {
        if (line_insn->type == OPORG ||
            line_insn->type == OPNAM ||
            line_insn->type == OPEND ||
            line_insn->type == OPOPT ||
            line_insn->type == OPPAGE ||
            line_insn->type == OPSPC)
        {
            error("Label not allowed");
        }
        else if (line_insn->type != OPEQU)
        {
            struct symbol *sy = lookup_symbol(line_label);
            if (!pass)
            {
                if (sy->flag & SET)
                {
                    error("Multiple defined symbol");
                }
                else
                {
                    sy->flag = SET_FIRST;
                    sy->val = location;
                }
            }
        }
    }

    nhold = inptr;
    skipws();
    operand_start = inptr; // Start of operand for listing

    switch (line_insn->type)
    {
        case OPBRA:
        {
            int offset;
            skipws();
            if (!parse_expr())
            {
                error("Missing operand");
            }
            offset = val - (location + 2);
            if (pass && known && (offset > 127 || offset < -128))
            {
                error("Relative branch out of range");
            }
            emit(line_insn->opcode);
            emit(offset);
            break;
        }
        case OPBYTE:
        {
            int mode;
            skipws();
            mode = parse_mode();
            if (!mode)
            {
                // Missing
                emit(line_insn->opcode + 0x30);
                emitw(0);
                error("Missing operand");
            }
            else if (mode == 1)
            {
                // Immediate
                emit(line_insn->opcode + 0x00);
                if (pass && known && (val < -128 || val >= 256))
                    error("Immediate value out of range");
                emit(val);
            }
            else if (mode == 2)
            {
                // Indexed
                emit(line_insn->opcode + 0x20);
                emit(val);
                if (pass && known && (val < 0 || val >= 256))
                    error("Index out of range");
            }
            else if (mode == 3 && knownfirst && val >= 0 && val < 256)
            {
                // Value fully resolved in first pass and small
                emit(line_insn->opcode + 0x10);
                emit(val);
            }
            else
            {
                // Large value or value not known in first pass
                emit(line_insn->opcode + 0x30);
                emitw(val);
            }
            break;
        }
        case OPEND:
        {
            last = 1;
            break;
        }
        case OPEQU:
        {
            skipws();
            if (!parse_expr())
            {
                error("Missing expression");
            }
            else if (!knownfirst)
            {
                error("Undefined symbol");
            }
            if (line_label[0])
            {
                struct symbol *sy = lookup_symbol(line_label);
                if (!pass)
                {
                    if (sy->flag & SET)
                    {
                        error("Multiply defined symbol");
                    }
                    else
                    {
                        sy->flag = SET_FIRST;
                        sy->val = val;
                    }
                }
                else
                {
                    /* 2nd pass, do nothing except print */
                    list_word(sy->val);
                }
            }
            else
            {
                error("EQU requires a label");
            }
            break;
        }
        case OPFCB:
        {
            char *hold;
            fcb_more:
            hold = inptr;
            skipws();
            if (!parse_expr())
            {
                inptr = hold;
                val = 0;
            }
            if (pass && known && (val > 255 || val < -128))
            {
                error("Value out of range");
            }
            emit(val);
            hold = inptr;
            skipws();
            if (*inptr == ',')
            {
                ++inptr;
                goto fcb_more;
            }
            else
            {
                inptr = hold;
            }
            break;
        }
        case OPFCC:
        {
            skipws();
            if (parse_decimal())
            {
                skipws();
                if (*inptr == ',')
                {
                    ++inptr;
                    skipws();
                    if (parse_string())
                    {
                        int i;
                        for (i = 0; i < val && strval[i]; ++i)
                        {
                            emit(strval[i]);
                        }
                        if (strval[i])
                        {
                            error("String is too long");
                        }
                        while (i < val)
                        {
                            emit(' ');
                            ++i;
                        }
                    }
                    else
                    {
                        int i;
                        for (i = 0; i < val; ++i)
                        {
                            emit(' ');
                        }
                        error("Missing string");
                    }
                }
                else
                {
                    int i;
                    for (i = 0; i < val; ++i)
                    {
                        emit(' ');
                    }
                    error("Missing string");
                }
            }
            else if (parse_string())
            {
                int i;
                for(i = 0; strval[i]; ++i)
                {
                    emit(strval[i]);
                }
            }
            else
            {
                error("Missing string");
            }
            break;
        }
        case OPFDB:
        {
            char *hold;
            fdb_more:
            hold = inptr;
            skipws();
            if (!parse_expr())
            {
                inptr = hold;
                val = 0;
            }
            emit(val >> 8);
            emit(val);
            hold = inptr;
            skipws();
            if (*inptr == ',')
            {
                ++inptr;
                goto fdb_more;
            }
            else
            {
                inptr = hold;
            }
            break;
        }
        case OPJMP:
        {
            int mode;
            skipws();
            mode = parse_mode();
            if (!mode)
            {
                error("Missing operand");
                emit(line_insn->opcode + 0x10);
                emit(0);
                emit(0);
            }
            else if (mode == 1)
            {
                error("Immediate mode not allowed");
                emit(line_insn->opcode + 0x10);
                emitw(0);
            }
            else if (mode == 2)
            {
                // Indexed
                emit(line_insn->opcode);
                emit(val);
                if (pass && known && (val < 0 || val >= 256))
                    error("Index out of range");
            }
            else
            {
                emit(line_insn->opcode + 0x10);
                emitw(val);
            }
            break;
        }
        case OPNAM:
        {
            if (line_number != 1)
            {
                error("NAM must only be on first line");
            }
            else
            {
                skipws();
                if (parse_module_name())
                {
                    emits0(strval);
                }
                else
                {
                    error("Missing module name");
                }
            }
            break;
        }
        case OPNONE:
        {
            emit(line_insn->opcode);
            break;
        }
        case OPOPT:
        {
            more_opt:
            skipws();
            if  (parse_ident())
            {
                if (!strcmp(strval, "L"))
                    option_listing = 1;
                else if (!strcmp(strval, "NOL"))
                    option_listing = 0;
                else if (!strcmp(strval, "G"))
                    option_generate = 1;
                else if (!strcmp(strval, "NOG"))
                    option_generate = 0;
                else if (!strcmp(strval, "S"))
                    option_symbols = 1;
                else if (!strcmp(strval, "NOS"))
                    option_symbols = 0;
                else if (!strcmp(strval, "M"))
                    option_memory = 1;
                else if (!strcmp(strval, "NOM"))
                    option_memory = 0;
                else if (!strcmp(strval, "O"))
                    option_object = 1;
                else if (!strcmp(strval, "NOO"))
                    option_object = 0;
                else if (!strcmp(strval, "CREF"))
                {
                }
                else if (!strcmp(strval, "LLEN"))
                {
                    if (*inptr=='=')
                    {
                        ++inptr;
                        parse_decimal();
                    }
                }
                else
                {
                    error("Unknown option");
                }
                skipws();
                if (*inptr == ',')
                {
                    ++inptr;
                    goto more_opt;
                }
                
            }
            break;
        }
        case OPORG:
        {
            skipws();
            parse_expr();
            if (!pass && !known)
            {
                error("Symbol not defined");
            }
            location = val;
            list_addr(val);
            break;
        }
        case OPPAGE:
        {
            /* Hidden line */
            break;
        }
        case OPRMB:
        {
            skipws();
            parse_expr(0);
            if (!pass && !known)
            {
                error("Symbol not defined");
            }
            list_addr(location);
            if (location + val > 65636)
            {
                error("RMB runs past end of memory");
            }
            else
            {
                location += val;
            }
            break;
        }
        case OPRMW:
        {
            int mode;
            skipws();
            mode = parse_mode();
            if (!mode)
            {
                // Missing
                emit(line_insn->opcode + 0x30);
                emitw(0);
                error("Missing operand");
            }
            else if (mode == 1)
            {
                // Immediate
                error("Immediate mode not allowed");
                emit(line_insn->opcode + 0x00);
                emitw(0);
            }
            else if (mode == 2)
            {
                // Indexed
                emit(line_insn->opcode + 0x20);
                emit(val);
                if (pass && known && (val < 0 || val >= 256))
                    error("Index out of range");
            }
            else
            {
                // Extended
                emit(line_insn->opcode + 0x30);
                emitw(val);
            }
            break;
        }
        case OPSAVE:
        {
            int mode;
            skipws();
            mode = parse_mode();
            if (!mode)
            {
                // Missing
                emit(line_insn->opcode + 0x30);
                emitw(0);
                error("Missing operand");
            }
            else if (mode == 1)
            {
                // Immediate
                error("Immediate mode not allowed");
                emit(line_insn->opcode + 0x00);
                emit(0);
            }
            else if (mode == 2)
            {
                // Indexed
                emit(line_insn->opcode + 0x20);
                emit(val);
                if (pass && known && (val < 0 || val >= 256))
                    error("Index out of range");
            }
            else if (mode == 3 && knownfirst && (val >= 0 && val < 256))
            {
                // Value fully resolved in first pass and small
                emit(line_insn->opcode + 0x10);
                emit(val);
            }
            else
            {
                // Large value or value not known in first pass
                emit(line_insn->opcode + 0x30);
                emitw(val);
            }
            break;
        }
        case OPSPC:
        {
            /* Hidden line */
            skipws();
            parse_expr(1);
            /* Emit 'val' blank lines in listing */
            break;
        }
        case OPWORD:
        {
            int mode;
            skipws();
            mode = parse_mode();
            if (!mode)
            {
                // Missing
                emit(line_insn->opcode + 0x30);
                emitw(0);
                error("Missing operand");
            }
            else if (mode == 1)
            {
                // Immediate
                emit(line_insn->opcode + 0x00);
                emitw(val);
            }
            else if (mode == 2)
            {
                // Indexed
                emit(line_insn->opcode + 0x20);
                emit(val);
                if (pass && known && (val < 0 || val >= 256))
                    error("Index out of range");
            }
            else if (mode == 3 && knownfirst && val >= 0 && val < 256)
            {
                // Value fully resolved in first pass and small
                emit(line_insn->opcode + 0x10);
                emit(val);
            }
            else
            {
                // Large value or value not known in first pass
                emit(line_insn->opcode + 0x30);
                emitw(val);
            }
            break;
        }
    }

    if (inptr != operand_start)
    {
        /* Operand */
        list_operand(operand_start, inptr - operand_start);
    }
    else
    {
        inptr = nhold;
    }

    if (iswhite(*inptr))
    {
        skipws();
        // We might have a comment right here.
        if (*inptr)
            list_comment(inptr);
    }
    else if (*inptr)
    {
        error("Extra characters in line, syntax error");
        if (*inptr)
            list_comment(inptr);
    }
    blank_line:
    list_done();
    return last;
}

void list_symtab()
{
    if (listing)
    {
        int i;
        int col;
        fprintf(listing, "\nSymbol table:\n\n");
        col = 0;
        for (i = 0; i != symtab_idx; ++i)
        {
            fprintf(listing, "     %6s %4.4x", symtab[i].name, symtab[i].val);
            col += 16;
            if (col + 16 >= 80)
            {
                fprintf(listing, "\n");
                col = 0;
            }
        }
        if (col)
        {
            fprintf(listing, "\n");
        }
    }
}

int main(int argc, char *argv[])
{
    int x;
    char *infile_name = 0;
    char *outfile_name = 0;
    char *listing_name = 0;
    for (x = 1; argv[x]; ++x)
        if (!strcmp(argv[x], "-l") && argv[x + 1] && !listing_name)
        {
            ++x;
            listing_name = argv[x];
        }
        else if (!strcmp(argv[x], "-o") && argv[x + 1] && !outfile_name)
        {
            ++x;
            outfile_name = argv[x];
        }
        else if (!strcmp(argv[x], "-h") || !strcmp(argv[x], "--help"))
        {
            printf("asmb [-l name] [-o name] name\n");
            printf("  Assemble a file with optional output and listing files\n");
            return 0;
        }
        else if (argv[x][0] == '-')
        {
            fprintf(stderr, "Unknown option\n");
            return -1;
        }
        else if (!infile_name)
        {
            infile_name = argv[x];
        }
        else
        {
            fprintf(stderr, "Syntax error\n");
            return -1;
        }

    if (!infile_name)
    {
        fprintf(stderr, "Missing source file name\n");
        return -1;
    }

    infile = fopen(infile_name, "r");
    if (!infile)
    {
        fprintf(stderr, "Couldn't open %s\n", infile_name);
        return -1;
    }

    /* First pass */
    printf("Pass 1...\n");

    while (fgets(inbuf, sizeof(inbuf) - 1, infile))
    {
        int l = strlen(inbuf);
        if (l && inbuf[l-1] == '\n')
        {
            inbuf[l-1] = 0;
            --l;
            if (l && inbuf[l-1] == '\r')
                inbuf[l-1] = 0;
        }
        inptr = inbuf;
        if (parse_line())
            break;
    }

    /* Second pass */
    printf("Pass 2...\n");

    rewind(infile);
    line_number = 0;
    error_count = 0;
    location = 0;
    pass = 1;

    if (outfile_name)
    {
        outfile = fopen(outfile_name, "w");
        if (!outfile)
        {
            fprintf(stderr, "Couldn't open %s\n", outfile_name);
            return -1;
        }
    }

    if (listing_name)
    {
        listing = fopen(listing_name, "w");
        if (!listing)
        {
            fprintf(stderr, "Couldn't open %s\n", listing_name);
            return -1;
        }
    }

    while (fgets(inbuf, sizeof(inbuf) - 1, infile))
    {
        int l = strlen(inbuf);
        if (l && inbuf[l-1] == '\n')
        {
            inbuf[l-1] = 0;
            --l;
            if (l && inbuf[l-1] == '\r')
                inbuf[l-1] = 0;
        }
        inptr = inbuf;
        if (parse_line())
            break;
    }

    emit_final();
    printf("%3d Errors\n", error_count);

    list_symtab();

    if (error_count) {
        return -1;
    } else {
        return 0;
    }
}
