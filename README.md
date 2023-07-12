# EXORsim - Motorola EXORciser Simulator

## Introduction

EXORsim Linux/Cygwin program that simulates a Motorola EXORciser, which was
a development system that [Motorola](http://en.wikipedia.org/wiki/Motorola)
sold in the late 1970s and early 1980s based on the 8-bit
[6800](http://en.wikipedia.org/wiki/Motorola_6800) and
[6809](https://en.wikipedia.org/wiki/Motorola_6809) microprocessors.  Some
pictures of this development system can be found at Pekka Tanskanen's
website [here](http://www.exorciser.net/index_en.htm).

The EXORciser was influential in the M6800 world:

* The [Fairlight CMI](https://en.wikipedia.org/wiki/Fairlight_CMI) sampling synthesize was developed on it
* The [First Digital Camera](https://thephotographeronline.com/current-issue/meet-the-inventor-of-the-first-digital-camera) was developed on it

Documentation for the EXORciser can be found here:

[http://www.bitsavers.org/components/motorola/6800/exorciser/](http://www.bitsavers.org/components/motorola/6800/exorciser/)

Disk images for the EXORciser can be found here:

[http://www.bitsavers.org/bits/Motorola/Exorcisor/](http://www.bitsavers.org/bits/Motorola/Exorcisor/)

You need to use [ImageDisk](http://www.classiccmp.org/dunfield/img/index.htm) to extract the binary data from from the .IMD
files (requires MS-DOS).

	imdu disk.imd disk.dsk /b

Or you can use "imdx" provided in this repository:

	imdx disk.imd

The EXORciser was designed primarily to help you write assembly language
programs for Motorola's 8-bit microprocessors, including 6800, 6801, 6805 and
6809.  But it also supported some high level language compilers including
BASIC, FORTRAN, COBOL, [MPL](https://github.com/jhallen/exorsim/blob/master/mpl/readme.md) and PASCAL.

The EXORciser could be used without any disk drives.  In this case, you
would use a paper tape punch and reader, usually part of an ASR33 terminal. 
But it also could be used much more pleasantly with disk drives.  In this
case, there were a number of available operating systems:

* EDOS-I  (primitive OS with no file names- files are refered to by number, for EXORdisk-I)
* EDOS-II   (primitive OS with 5 character file names, for EXORdisk-I)
* MDOS   (CP/M-like OS with 8 character file names with two character extensions, for EXORdisk-II/-III)
* MDOS09  (same as MDOS, but for 6809-based EXORcisers)

So far, EXORsim only runs MDOS: because EXORsim only simulates a 6800 and
I've not found any bootable EDOS disks.  Note that even though MDOS is for
6800-based EXORcisers, you can write 6809 programs with it.

EXORsim also simulates a [SWTPC 6800 Computer System](https://deramp.com/swtpc.html), which allows you to run
the [TSC FLEX](http://en.wikipedia.org/wiki/FLEX_%28operating_system%29) operating system.

## EXORsim

EXORsim is a terminal program- it's invoked at the UNIX shell and runs in
the user's ANSI terminal emulator (such as XTerm).  This allows you to use
terminal emulator features such as scroll-back.  I find this to be more
useful than some emulators which run under MS-Windows, but provide only a
very rudimentary terimnal.

EXORsim Simulates 6800 CPU and the following peripherals:

* ACIA-based serial port (UART)
  * EXORsim emulates an EXORterm 155 by converting its control sequences into ANSI ones
    * This allows the [EDITORM Resident Editor](https://github.com/jhallen/exorsim/blob/master/doc/mdos-intro.md#e-editorm-resident-editor) to operate in screen mode.
* Line printer port
  * Output is appended to a file "listing.lp"
  * "listing.lp" can be converted to a .pdf file using the included "lpf" program
* Floppy disk controller
  * EXORdisk-II floppy diskette controller emulated by intercepting calls to the controller ROM
  * SWTPC [MF68](https://deramp.com/swtpc.com/MF_68/MF_68_Index.htm) with DC_1 emulated by hardware emulation of the [FD1771](https://en.wikipedia.org/wiki/Western_Digital_FD1771) controller.

<h3>Here is an example session.  This is one way to write an assembly language "Hello, World!" program:</h3>

<p>0. Boot MDOS</p>

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>
~/exor-1.0$ ./exor
Load facts file 'facts'
'exbug.bin' loaded.
'mdos.dsk' opened for drive 0 (single sided)

Hit Ctrl-C for simulator command line.  Starting simulation...

MDOS 03.00
=
</code></pre>

'=' is the MDOS command prompt.

Hit Ctrl-C once to access the simulator's debugger prompt.

Hit Ctrl-C again to exit EXORsim, or use the 'c' command to continue
simulating.

<p>1. Use EDITM to create the assembly language source file:</p>

<p>
<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>=<b>EDITM</b>
EDITM RELEASE 3.10
COPYRIGHT BY MOTOROLA 1978
READY
?<b>BUILD HELLO</b>
HELLO   .SA:0 CREATED
0010 <b> NAM HELLO</b>         <i>Sets module name</i>
0020  <b>TTL HELLO</b>         <i>Sets title on listing</i>
0030  <b>OPT REL,G,S</b>       <i>options: rel for relocatable, g to expand fcc</i>
0040  <b>PSCT</b>              <i>Program section</i>
0050 <b>START LDX #PROMPT</b>  <i>Point to string</i>
0060  <b>SCALL .DSPLY</b>      <i>Display string</i>
0070  <b>SCALL .MDENT</b>      <i>Return to MDOS</i>
0080 <b>PROMPT FCC &quot;HELLO, WORLD!&quot;</b>
0090  <b>FCB 13</b>            <i>Carriage return terminates string</i>
0100  <b>BSZ 80</b>            <i>Allocate space for stack</i>
0110  <b>END START</b>         <i>Label after END is entry point</i>
0120   
?<b>RESE N</b>                 <i>Eliminate line numbers</i>
READY
?<b>LIST</b>
 NAM HELLO
 TTL HELLO
 OPT REL,G
 PSCT
START LDX #PROMPT
 SCALL .DSPLY    
 SCALL .MDENT
PROMPT FCC &quot;HELLO, WORLD!&quot;
 FCB 13   
 BSZ 80   
 END START
?<b>Q</b>
SAVE IN HELLO1  .SA:0 (Y/N)?<b>Y</b>
READY
?<b>Q</b>
=</code></pre>

<p>2. Assemble, including EQU.SA file.  Note that there is no
<code>include</code> directive, so header files like EQU.SA have to be
specified on the command line.</p>

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>=<b>RASM EQU,HELLO1;LO=HELLO1</b>
<i>...490 lines from the EQU.SA file...</i>
00491                         NAM    HELLO
00492                         TTL    HELLO
00493                         OPT    REL,G
00494P 0000                   PSCT
00495P 0000 CE 0007  P START  LDX    #PROMPT
00496P 0003                   SCALL  .DSPLY
00497P 0005                   SCALL  .MDENT
00498P 0007    48    A PROMPT FCC    "HELLO, WORLD!"
     P 0008    45    A
     P 0009    4C    A
     P 000A    4C    A
     P 000B    4F    A
     P 000C    2C    A
     P 000D    20    A
     P 000E    57    A
     P 000F    4F    A
     P 0010    52    A
     P 0011    4C    A
     P 0012    44    A
     P 0013    21    A
00499P 0014    0D    A        FCB    13
00500P 0015    0050  A        BSZ    80
00501          0000  P        END    START
TOTAL ERRORS 00000--00000
=</code></pre>

<p>3. Link</p>

<p><pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>=<b>RLOAD</b>
MDOS LINKING LOADER REV F3.P0
COPYRIGHT BY MOTOROLA 1977
?<b>IF=T</b>			<i>Gives name of temporary file for second pass</i>
?<b>BASE</b>			<i>Set base addresses for MDOS</i>
?<b>IDON</b>			<i>Enable display of module name information</i>
?<b>LOAD=HELLO</b>		<i>Load object file, first pass</i>
  HELLO   
?<b>OBJA=HELLO</b>		<i>Give name of output file, start second pass</i>
  HELLO   
?<b>MAPF</b>			<i>Show map file</i>
  NO UNDEFINED SYMBOLS
MAP
 S SIZE  STR  END COMN
 B 0000 0020 0020 0000		<i>&lt;-- Base section, for code below $100</i>
 C 0000 2000 2000 0000		<i>&lt;-- Common section</i>
 D 0000 2000 2000 0000		<i>&lt;-- Data section</i>
 P 0065 2000 2064 0000		<i>&lt;-- The code is here, in program section</i>
MODULE NAME BSCT DSCT PSCT
  HELLO     0020 2000 2000
?<b>EXIT</b>			<i>Exit RLOAD</i>
=</code></pre>
</p>

<p>4. Load and run</p>

<p><pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>=<b>LOAD HELLO;G</b>
HELLO, WORLD!
</code></pre>
</p>

<p>5. Rename it to a command file</p>

<p><pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>=NAME HELLO.LO .CM</code></pre></p>

<p>6. Now just type the command:</p>
<p><pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>=HELLO
HELLO, WORLD!
=</code></pre></p>

## SWTPC FLEX

To simulate SWTPC instead, run with the --swtpc option:

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>
~/exor-1.0$ ./exor --swtpc
Load facts file 'facts'
'swtbug.bin' loaded.
'flex.dsk' opened for drive 0 (tracks=80 sectors=72)

Hit Ctrl-C for simulator command line.  Starting simulation...

FLEX 2.0

DATE (MM,DD,YY)? 02,07,11

+++
</code></pre>


You can get FLEX2 disk images here:

[http://www.flexusergroup.com/flexusergroup/fugdflx2.htm](http://www.flexusergroup.com/flexusergroup/fugdflx2.htm)

[https://deramp.com/downloads/swtpc/software/FLEX/FLEX%202.0%20and%203.0%20Disk%20Images/](https://deramp.com/downloads/swtpc/software/FLEX/FLEX%202.0%20and%203.0%20Disk%20Images/)


# Build instructions

Just type "make" to compile it:

	exor	This is the simulator

	mdos	This provides file access to MDOS diskette images

	edos	This provides file access to EDOS-II diskette images

	unasm	This is a stand-alone 6800 disassember

	imdx	Utility to extract disk images from .imd files

Right now the exbug.bin and swtbug.bin files must be in the current
directory.

### Links

[MDOS quick start](doc/mdos-intro.md)
<br>
[MDOS reference](doc/mdos-ref.md)
<br>
[MDOS technical information](doc/mdos-tech.md)
<br>
[EXORsim usage guide](doc/usage.md)
<br>
[MPL language](mpl/readme.md)
<br>
[EXORterm info](doc/exorterm.md)
