/*	Line printer filter for postscript printers
 *	Copyright
 *		(C) 2023 Joseph H. Allen
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

/* Convert ASCII or PCL to Postscript, or pass Postscript right through */

/* Usage:
 *  ./lpf <listing.lp >listing.ps    As a filter
 *  ./lpf listing.lp listing.ps      Or give file names on command line
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *infile;
FILE *outfile;

/* For converting line printer output to postscript-
   We need original height of line printer and number
   of margin lines to skip each page */

int lp_lines = 66;
int lp_topskip = 6; /* Skip first 6 lines of each page */
int lp_botskip = 0; /* Skip last n lines of each page */

/* Set-up file parameters */
int fontsize=8;	/* Font size */
int ncopies=1;

/* Current state */

int res=720;		/* Resolution (dots per inch) */
int pres=72;		/* Post-script units */
int state=0;		/* Input state */
int num=0;		/* Input number */
int rel=0;
int cursor_y;		/* Cursor y position */
int cursor_x;		/* Cursor x position */
int height;		/* Character height */
int width;		/* Character width */
int lmargin;
int tmargin;
char *font = "Courier";
/* char *font = "Courier-Bold"; */

void izpcl()
 {
 fprintf(outfile, "%%!PS-Adobe-2.0\n");
 fprintf(outfile, "/#copies %d def\n",ncopies);
 fprintf(outfile, "/%s findfont %d scalefont setfont\n", font, fontsize);
 cursor_x=0;
 cursor_y=0;
 height=res/6;
 width=res/10;
 lmargin=res/4;
 tmargin=res/2+res/6;
 }

char line[1024];
int linep=0;

void flshopen()
 {
 fprintf(outfile, "%d %d moveto (",(cursor_x+lmargin)*pres/res,
                        (res*11-tmargin-cursor_y)*pres/res);
 }

void flshc(int c)
 {
 if(c=='\\') fputc('\\', outfile), fputc('\\', outfile);
 else if(c=='(') fputc('\\', outfile), fputc('(', outfile);
 else if(c==')') fputc('\\', outfile), fputc(')', outfile);
 else fputc(c, outfile);
 cursor_x+=width;
 }

void flshclose()
 {
 fprintf(outfile, ") show\n");
 }

void flshline()
 {
 int x;
 int state=0;
 for(x=0;x!=linep;++x)
  if(line[x]=='\b')
   {
   if(state) flshclose(), state=0;
   cursor_x-=width;
   }
  else
   {
   if(!state) flshopen(), state=1;
   flshc(line[x]);
   }
 if(state) flshclose();
 linep=0;
 }

void flshpage()
 {
 flshline();
 fprintf(outfile, "showpage\n");
 cursor_y=0;
 }

int line_count;
int skip_count;

void nextline()
 {
 if (line_count >= lp_topskip && line_count < (lp_lines - lp_botskip))
  {
  flshline();
  cursor_y+=height;
  cursor_x=0;
  if(cursor_y>=10*res) flshpage();
  }
 else
  {
  linep = 0; /* Delete line */
  cursor_x = 0;
  }
 ++line_count;
 if (line_count == lp_lines)
  line_count = 0;
 }

void pcl(int c)
 {
 switch(state)
  {
  case 0:
   if(c>=' ' && c<='~') line[linep++]=c;
   else if(c=='\b') line[linep++]=c;
   else if(c=='\t') do line[linep++]=' '; while(linep&7);
   else if(c=='\n') nextline();
   else if(c=='\r')
    {
    if (line_count >= lp_topskip && line_count < (lp_lines - lp_botskip))
     flshline();
    cursor_x = 0;
    linep = 0;
    }
   else if(c=='\f') flshpage();
   else if(c=='\033') state=1;
   break;

  case 1: /* ESC */
   if(c=='&') state=2;
   else if(c=='(') state=3;
   else if(c==')') state=4;
   else if(c=='*') state=5;
   else if(c==27) state=1;
   else state=0;
   break;

  case 2: /* ESC & */
   if(c=='a') state=6, num=0, rel=0;
   else if(c==27) state=1;
   else state=0;
   break;

  case 3: /* ESC ( */
   if(c=='s') state=7, num=0, rel=0;
   else if(c==27) state=1;
   else state=0;
   break;

  case 4: /* ESC ) */

  case 5: /* ESC * */
  
  case 6: /* ESC & a */
   if(c>='0' && c<='9') num=num*10+c-'0';
   else if(c=='C')
    {
    flshline();
    if(rel==0) cursor_x=width*num;
    else if(rel==1) cursor_x+=width*num;
    else if(rel== -1) cursor_x-=width*num;
    state=0;
    }
   else if(c==27) state=1;
   else if(c=='-') rel= -1;
   else if(c=='+') rel=1;
   else state=0;
   break;

  case 7: /* ESC ( s */
   if(c>='0' && c<='9') num=num*10+c-'0';
   else if(c=='-') rel= -1;
   else if(c=='+') rel= 1;
   else if(c==27) state=1;
   else if(c=='P')
    {
    flshline();
    if(num==0) fprintf(outfile, "/Courier-Bold findfont %d scalefont setfont\n",fontsize);
    else if(num==1) fprintf(outfile, "/Times-Roman findfont %d scalefont setfont\n",fontsize-fontsize/6);
    state=0;
    }
   else state=0;
   break;
  }
 }

void pclflsh()
 {
 if(cursor_y) flshpage();
 }

int main(int argc, char *argv[])
 {
 int a, b;

 char *infile_name = 0;
 char *outfile_name = 0;

 infile = stdin;
 outfile = stdout;

 /* Parse arguments */
 for (a = 1; argv[a]; ++a)
  if (!strcmp(argv[a], "--help"))
   {
   printf("Convert PCL or ASCII to PostScript\n");
   printf("Usage: %s [options] [infile [outfile]]\n", argv[0]);
   printf("Options:\n");
   printf("   --copies nn     Number of copies to print\n");
   printf("   --fontsize 8    Postscript font size\n");
   printf(" Generally we print 60 lines per page.  If you are converting\n");
   printf(" from line printer output, you need to delete the margin lines\n");
   printf(" with these options so that (lpline - (lptopskip + lpbotskip)) is 60:\n");
   printf("   --lplines 66    Number of lines per page of infile\n");
   printf("   --lptopskip 6   Number of lines to skip at top of each page\n");
   printf("   --lpbotskip 0   Number of lines to skip at bottom of each page\n");
   printf("   --tmargin 480   Top margin (720 is one inch)\n");
   printf("   --lmargin 180   Left margin (720 is one inch)\n");
   printf("   --font Courier-Bold\n");
   printf("                   Postscript font name to use\n");
   return 0;
   }
  else if (!strcmp(argv[a], "--copies") && argv[a+1])
   {
   ++a;
   ncopies = atoi(argv[a]);
   }
  else if (!strcmp(argv[a], "--fontsize") && argv[a+1])
   {
   ++a;
   fontsize = atoi(argv[a]);
   }
  else if (!strcmp(argv[a], "--lplines") && argv[a+1])
   {
   ++a;
   lp_lines = atoi(argv[a]);
   }
  else if (!strcmp(argv[a], "--lptopskip") && argv[a+1])
   {
   ++a;
   lp_topskip = atoi(argv[a]);
   }
  else if (!strcmp(argv[a], "--lpbotskip") && argv[a+1])
   {
   ++a;
   lp_botskip = atoi(argv[a]);
   }
  else if (!strcmp(argv[a], "--tmargin") && argv[a+1])
   {
   ++a;
   tmargin = atoi(argv[a]);
   }
  else if (!strcmp(argv[a], "--lmargin") && argv[a+1])
   {
   ++a;
   lmargin = atoi(argv[a]);
   }
  else if (!strcmp(argv[a], "--font") && argv[a+1])
   {
   ++a;
   font = argv[a];
   }
  else if (!infile_name)
   infile_name = argv[a];
  else if (!outfile_name)
   outfile_name = argv[a];
  else
  {
   fprintf(stderr, "Syntax error\n");
   return -1;
  }

 if (infile_name)
 {
   infile = fopen(infile_name, "r");
   if (!infile)
   {
    perror(infile_name);
    return -1;
   }
 }

 if (outfile_name)
 {
   outfile = fopen(outfile_name, "w");
   if (!outfile)
   {
    perror(outfile_name);
    return -1;
   }
 }

 a=fgetc(infile);
 if(a==4) a=fgetc(infile);
 b=fgetc(infile);
 if((a=='%' && b=='!') || (a=='\033' && b=='%'))
  { /* Input is postscript.  Pass it right through */
  int state=0;
  fputc(a, outfile); fputc(b, outfile);
  if(ncopies==1) while((a=fgetc(infile))!= -1) fputc(a, outfile);
  else while((a=fgetc(infile))!= -1)
   switch(state)
    {
    case 0: if(a=='N') ++state;
            else fputc(a, outfile);
            break;

    case 1: if(a=='u') ++state;
            else fputs("N",outfile), fputc(a, outfile), state=0;
            break;

    case 2: if(a=='m') ++state;
            else fputs("Nu",outfile), fputc(a, outfile), state=0;
            break;

    case 3: if(a=='C') ++state;
            else fputs("Num",outfile), fputc(a, outfile), state=0;
            break;

    case 4: if(a=='o') ++state;
            else fputs("NumC",outfile), fputc(a, outfile), state=0;
            break;

    case 5: if(a=='p') ++state;
            else fputs("NumCo",outfile), fputc(a, outfile), state=0;
            break;
            
    case 6: if(a=='i') ++state;
            else fputs("NumCop",outfile), fputc(a, outfile), state=0;
            break;

    case 7: if(a=='e') ++state;
            else fputs("NumCopi",outfile), fputc(a, outfile), state=0;
            break;

    case 8: if(a=='s')
             {
             while((a=fgetc(infile))!=-1) if(a=='p') break;
             if(a!=-1) fprintf(outfile, "NumCopies %d p",ncopies);
             state=0;
             }
            else fputs("NumCopie",outfile), fputc(a, outfile), state=0;
            break;
    }
  }
 else
  { /* Input is PCL.  Give it to PCL interpreter */
  izpcl();
  pcl(a);
  pcl(b);
  while((a=fgetc(infile))!= -1) pcl(a);
  pclflsh();
  }
 return 0;
 }
