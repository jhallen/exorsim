/*	Utilities
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
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "utils.h"

/* Skip over whitespace */

int skipws(char **at_p)
{
	char *p = *at_p;
	while (*p == ' ' || *p == '\t')
		++p;
	*at_p = p;
	return 1;
}

/* Required whitespace */

int ws(char **at_p)
{
        int ok = 0;
	char *p = *at_p;
	while (*p == ' ' || *p == '\t') {
		++p;
		ok = 1;
        }
	*at_p = p;
	return ok;
}

/* Skip to whitespace */

int skiptows(char **at_p)
{
	char *p = *at_p;
	while (*p && *p != ' ' && *p != '\t')
		++p;
	*at_p = p;
	if (*p == ' ' || *p == '\t')
        	return 1;
        else
                return 0;
}

/* Skip over matching word */

int match_word(char **at_p, char *word)
{
        char *p = *at_p;
        int len = strlen(word);
        if (!strncmp(p, word, len) && (!p[len] || p[len] == ' ' || p[len] == '\t')) {
                p += len;
                skipws(&p);
                *at_p = p;
                return 1;
        } else
                return 0;
}

/* Extract string */

int parse_string(char **at_p, char *buf)
{
        char *p = *at_p;
        if (*p == '"') {
                ++p;
                while (*p && *p != '"') {
                        if (*p == '\\' && p[1]) {
                                ++p;
                                if (*p == 'n') {
                                        *buf++ = '\n';
                                } else if (*p == 'r') {
                                        *buf++ = '\r';
                                } else if (*p == '"') {
                                        *buf++ = '"';
                                } else if (*p == '\'') {
                                        *buf++ = '\'';
                                } else if (*p == '\t') {
                                        *buf++ = '\t';
                                } else if (*p == '\b') {
                                        *buf++ = '\b';
                                }
                                ++p;
                        } else
                                *buf++ = *p++;
                }
                if (*p == '"')
                        ++p;
                *at_p = p;
                *buf = 0;
                return 1;
        } else
                return 0;
}

/* Extract word */

int parse_word(char **at_p, char *buf)
{
	char *p = *at_p;
	if (*p && *p != ' ' && *p != '\t' && *p != ',' && *p != '+' && *p != '-' && *p != '[' && *p != ']') {
		int x = 0;
		while (*p && *p != ' ' && *p != '\t' && *p != ',' && *p != '+' && *p != '-' && *p != ']' && *p != '[') {
			buf[x++] = *p++;
		}
		buf[x] = 0;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

int parse_ident(char **at_p, char *buf)
{
	char *p = *at_p;
	if ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '.') {
	        int x = 0;
		while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_' || *p == '$' || *p == '.') {
			buf[x++] = *p++;
		}
		buf[x] = 0;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse one hex digit */

int parse_hex1(char **at_p, int *hex)
{
	char *p = *at_p;
	if ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) {
		unsigned short val = 0;
                if (*p >= '0' && *p <= '9')
                        val = val * 16 + *p++ - '0';
                else if (*p >= 'A' && *p <= 'F')
                        val = val * 16 + *p++ - 'A' + 10;
                else if (*p >= 'a' && *p <= 'f')
                        val = val * 16 + *p++ - 'a' + 10;
		*hex = val;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse hex digits */

int parse_hex(char **at_p, int *hex)
{
	char *p = *at_p;
	if ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) {
		unsigned short val = 0;
		int dig;
		while (parse_hex1(&p, &dig))
		        val = val * 16 + dig;
		*hex = val;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse two hex digits */

int parse_hex2(char **at_p, int *hex)
{
	char *p = *at_p;
	int a = 0;
	int b = 0;
	if (parse_hex1(&p, &a) && parse_hex1(&p, &b)) {
		*hex = (a << 4) + b;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse four hex digits */

int parse_hex4(char **at_p, int *hex)
{
	char *p = *at_p;
	int a = 0;
	int b = 0;
	if (parse_hex2(&p, &a) && parse_hex2(&p, &b)) {
		*hex = (a << 8) + b;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse binary digits */

int parse_bin(char **at_p, unsigned short *hex)
{
	char *p = *at_p;
	if (*p >= '0' && *p <= '1') {
		unsigned short val = 0;
		while (*p >= '0' && *p <= '1')
			val = val * 2 + *p++ - '0';
		*hex = val;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Parse ddd, $hhh, %bbbb */

int parse_dec(char **at_p, int *dec)
{
	char *p = *at_p;
	int neg = 0;
	while (*p == '-') {
	        ++p;
	        neg = !neg;
	}
	if (*p == '$') {
	        int rtn;
	        int hex;
	        ++p;
	        rtn = parse_hex(&p, &hex);
	        if (rtn) {
	                *at_p = p;
	                *dec = neg ? -hex : hex;
	                return 1;
	        } else
	                return 0;
	} else if (*p == '%') {
	        int rtn;
	        unsigned short bin;
	        ++p;
	        rtn = parse_bin(&p, &bin);
	        if (rtn) {
	                *at_p = p;
	                *dec = neg ? -bin : bin;
	                return 1;
	        } else
	                return 0;
        } else if (*p == '\'' && p[1]) {
                ++p;
                unsigned char c = *p++;
                *dec = neg ? -c : c;
                if (*p == '\'')
                        ++p;
                *at_p = p;
                return 1;
	} else if (*p >= '0' && *p <= '9') {
		int val = 0;
		while (*p >= '0' && *p <= '9')
			val = val * 10 + *p++ - '0';
		*dec = neg ? -val : val;
		*at_p = p;
		return 1;
	} else {
		return 0;
	}
}

/* Get input line with editing (for when cooked is off).
 * Returns -1 if Ctrl-C hit
 */

int jgetline(FILE *f, char *buf)
{
        int x = 0;

        if (f != stdin) {
                if (fgets(buf, 179, f)) {
                        if (strlen(buf))
                                buf[strlen(buf) - 1] = 0;
                        return 0;
                } else
                        return -1;
        }

        fflush(stdout);
        for (;;) {
                char c;
                if (read(fileno(stdin), &c, 1) < 0)
                        return -1;
                if (c == 8 || c == 127) {
                        if (x) {
                                fputs("\b \b", stdout);
                                fflush(stdout);
                                --x;
                        }
                } else if (c == 13) {
                        putchar('\n');
                        buf[x] = 0;
                        return 0;
                } else if (c == 3) {
                        return -1;
                } else if (c == 'Z' - '@') {
                        restore_termios();
        		fprintf(stderr, "You have suspended the program.  Type 'fg' to return\n");
        		kill(0, SIGTSTP);
        		sim_termios();
        		nosig_termios();
                } else {
                        putchar(c); fflush(stdout);
                        buf[x++] = c;
                }
        }
}

/* Hex to integer */

int hatoi(unsigned char *buf)
{
        int addr;
        sscanf((char *)buf, "%x", &addr);
        return addr;
}

/* Hex dump */

void hd(FILE *out, unsigned char *mem, int start, int len)
{
        int y;
        int skip = (start & 0x0F);
        int skip1 = skip;
        start &= ~0x0F;

        len += skip;

        y = 0;
        while (len > 0) {
                int x;
                int len1 = len;
                fprintf(out, "%4.4X:", start + y);
                for (x = 0; x != 16; ++x) {
                        if (skip || len <= 0) {
                                --skip;
                                fprintf(out, "   ");
                        } else {
                                fprintf(out, " %2.2X", mem[start +y + x]);
                        }
                        if (x == 7)
                                fprintf(out, " ");
                        --len;
                }
                fprintf(out, " ");
                for (x = 0; x != 16; ++x) {
                        unsigned char c = mem[start + y + x];
                        if (c < 32 || c > 126) c = '.';
                        if (skip1 || len1 <= 0) {
                                --skip1;
                                fprintf(out, " ");
                        } else {
                                fprintf(out, "%c", c);
                        }
                        --len1;
                }
                fprintf(out, "\n");
                y += 16;
        }
}

/* Break up whitespace separated words into an array of string pointers */

int fields(char *buf, char *words[])
{
        int n = 0;
        while (skipws(&buf) && *buf) {
                words[n++] = buf;
                skiptows(&buf);
                if (*buf)
                        *buf++ = 0;
        }
        return n;
}

/* Copy a string, src and dest may overlap */

char *jstrcpy(char *d, char *s)
{
        char *org_d = d;
        while ((*d++ = *s++));
        return org_d;
}

/* Convert to upper case */

int to_upper(int c)
{
        if (c >= 'a' && c <= 'z')
                c += 'A' - 'a';
        return c;
}

/* Convert to lower case */

int to_lower(int c)
{
        if (c >= 'A' && c <= 'Z')
                return c - 'A' + 'a';
        else
                return c;
}

/* stricmp not univeral.. */

int jstricmp(char *d, char *s)
{
        while (*d && *s && to_upper(*d) == to_upper(*s)) {
                ++d;
                ++s;
        }
        if (!*d && !*s)
                return 0;
        else
                return 1;
}

/* Save current TERMIOS settings */

static struct termios saved; // Original attributes of local system
int termios_good; // Set if 'saved' is valid.

void save_termios()
{
        if (!tcgetattr(fileno(stdin), &saved))
                termios_good = 1;
        else
                termios_good = 0;
}

/* Restore TERMIOS settings */

void restore_termios()   
{
        if (termios_good)
                tcsetattr(fileno(stdin),TCSADRAIN,&saved);
}
 
/* Set TERMIOS for simulation */

void sim_termios()
{
        struct termios attr;
        if (!tcgetattr(fileno(stdin), &attr)) {
                /* attr.c_oflag&=~ONLCR; */
                attr.c_iflag&=~ICRNL; /* Yes */
                attr.c_lflag&=~ICANON;
                attr.c_lflag&=~ECHO; /* Yes */
                attr.c_cc[VSUSP] = -1; /* Prevent Ctrl-Z */
                tcsetattr(fileno(stdin),TCSADRAIN,&attr);
        }
}

/* stty -isig */

void nosig_termios()
{
        struct termios attr;
        if (!tcgetattr(fileno(stdin), &attr)) {
                attr.c_lflag&=~ISIG;
                tcsetattr(fileno(stdin),TCSADRAIN,&attr);
        }
}

/* stty sig */

void sig_termios()
{
        struct termios attr;
        if (!tcgetattr(fileno(stdin), &attr)) {
                attr.c_lflag|=ISIG;
                tcsetattr(fileno(stdin),TCSADRAIN,&attr);
        }
}

/* extract file name from path: /home/joe/foo -> foo, foo -> foo */

void jbasename(char *dest, const char *src)
{
        int l = strlen(src);
        const char *s = src + l; /* Point to end */
        /* Scan backwards for / */
        while (s != src && s[-1] != '/')
                --s;
        /* Copy name to dest */
        while (*s)
                *dest++ = *s++;
        *dest++ = 0;
}

/* extract directory from path: /home/joe/foo -> /home/joe/, foo -> "" */

void jdirname(char *dest, const char *src)
{
        int l = strlen(src);
        const char *s = src + l; /* Point to end */
        /* Scan backwards for / */
        while (s != src && s[-1] != '/')
                --s;
        /* Copy dir to dest */
        while (src != s)
                *dest++ = *src++;
        *dest++ = 0;
}

/* Copy a file by name */

int copyfile(const char *src, const char *dest)
{
        FILE *f, *g;
        printf("Copying %s to %s...\n", src, dest);
        f = fopen(src, "r");
        if (f)
        {
                g = fopen(dest, "w");
                if (g)
                {
                        char buf[1024];
                        size_t len;
                        while ((len = fread(buf, 1, sizeof(buf), f)))
                                fwrite(buf, 1, len, g);
                        fclose(g);
                        fclose(f);
                        return 0;
                }
                else
                {
                        fclose(f);
                        return -1;
                }
        }
        else
        {
                return -1;
        }
}

/* Find setup file:
     1. Use one in current directory if it exists
     2. Use one in $HOME/.exorsim if it exists and is newer or same as system one
     3. Install system one into $HOME/.exorsim and use it
*/

const char *local_prefix;

const char *choose_config_file(const char *name)
{
        FILE *f;

        /* Create path $HOME/.exorsim */
        if (!local_prefix)
        {
                char *home = getenv("HOME");
                char *tmp = malloc(strlen(home) + strlen("/.exorsim") + 1);
                sprintf(tmp, "%s/.exorsim", home);
                local_prefix = tmp;
                mkdir(local_prefix, 0700); /* Create directory in case it doesn't exist */
        }

        /* First try current directory */
        f = fopen(name, "r");
        if (f)
        {
                /* It exists here, so use this one */
                fclose(f);
                return name;
        }
        else
        {
                /* Does not exist in local directory */
                struct stat local_stat;
                struct stat sys_stat;
                char *local;
                char *sys;
                char *bkup;

                /* Create $HOME/.exorsim/name */
                local = malloc(strlen(local_prefix) + 1 + strlen(name) + 1);
                sprintf(local, "%s/%s", local_prefix, name);

                /* Create $HOME/.exorsim/name.old */
                bkup = malloc(strlen(local) + 5);
                sprintf(bkup, "%s.old", name);

                /* Create /usr/local/share/exorsim/name */
                sys = malloc(strlen(DATADIR) + strlen(name) + 1);
                sprintf(sys, "%s%s", DATADIR, name);

                if (!stat(sys, &sys_stat))
                {
                        /* System version exists */
                        if (!stat(local, &local_stat))
                        {
                                /* Local version exists */
                                if (local_stat.st_mtime >= sys_stat.st_mtime)
                                {
                                        /* Local one is newer or same, so use it */
                                        return local;
                                }
                                else
                                {
                                        printf("%s is out of date! ", local);
                                        /* Save out of date existing one by renaming it */
                                        if (rename(local, bkup))
                                        {
                                                printf("\n");
                                                /* Couldn't rename? */
                                                perror("choose_config_file");
                                                exit(-1);
                                        }
                                }
                        }
                        else
                        {
                                printf("%s is missing! ", local);
                        }
                        /* Copy system to local */
                        copyfile(sys, local);
                }
                else
                {
                        /* System one doesn't exist? */
                }
                return local;
        }
}

/* Install config file into current directory if it's
   not already there or if the system one is newer.  */

void install_config_file(const char *name)
{
        char *sys = malloc(strlen(DATADIR) + strlen(name) + 1);
        char *bkup = malloc(strlen(name) + 5);
        struct stat cur_stat;
        struct stat sys_stat;

        sprintf(sys, "%s%s", DATADIR, name);
        sprintf(bkup, "%s.old", name);

        if (!stat(sys, &sys_stat))
        {
                // printf("%s exists\n", sys);
                /* System version exists */
                if (!stat(name, &cur_stat))
                {
                        // printf("%s exists\n", name);
                        if (cur_stat.st_mtime >= sys_stat.st_mtime)
                        {
                                // printf("%s is good\n", name);
                                /* Leave existing one alone */
                                return;
                        }
                        else
                        {
                                printf("%s is out of date! ", name);
                                /* Save out of date existing one by renaming it */
                                if (rename(name, bkup))
                                {
                                        printf("\n");
                                        /* Couldn't rename, use existing one.. */
                                        perror("intall_config_file");
                                        exit(-1);
                                }
                        }
                }
                else
                {
                        printf("%s does not exist! ", name);
                }
                /* Copy system to current */
                copyfile(sys, name);
        }
        else
        {
                /* System one doesn't exist? */
        }
}
