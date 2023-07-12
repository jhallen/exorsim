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

	./imdx disk.imd

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

<h3>Example session</h3>

Right now the exbug.bin (or swtbug.bin for SWTPC) files must be in the
current directory.

'=' is the MDOS command prompt.

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>
~/exor-1.0$ ./exor
Load facts file 'facts'
'exbug.bin' loaded.
'mdos.dsk' opened for drive 0 (double sided)

Hit Ctrl-C for simulator command line.  Starting simulation...


MDOS  3.05
=         
WHAT?
=<b>DIR </b>
DRIVE : 0   DISK I.D. : SYSTEM
ECUSTOM .CF                   
MDOSMODE.CF
NEWS    .SA
EDITINFO.SA
TOTAL DIRECTORY ENTRIES SHOWN : 004/$04
=                                      

=<b>DIR;S</b>                     <i>Include hidden system files</i>
DRIVE : 0   DISK I.D. : SYSTEM
BINEX   .CM                   
LIST    .CM
MDOSOV0 .SY
DIR     .CM
MERGE   .CM
RLOAD   .CM

. . . etc . . .

TOTAL DIRECTORY ENTRIES SHOWN : 058/$3A
=

=<b>BASIC FRED</b>                <i>Try BASIC interpreter- it requires a filename</i>

MDOS BASIC 2.01
COPYRIGHT(C)- 1977



READY
#<b>PRINT "HELLO, WORLD!"</b>
HELLO, WORLD!

READY
#<b>EXIT</b>
SAVE(Y/N)
<b>N</b>

=<b>RASM</b>
MDOS MACROASSEMBLER  3.01
COPYRIGHT BY MOTOROLA 1977



** 02 NAME REQUIRED

=<b>RASM09</b>
M6809 MACROASSEMBLER 02.00
COPYRIGHT BY MOTOROLA 1978



** 02 NAME REQUIRED

=<b>ASM</b>

** 02 NAME REQUIRED

=<b>ASM09</b>

AUSTIN,TEXAS--MICROCOMPUTER CAPITAL OF THE WORLD!
M6800-M6809 CROSS-ASSEMBLER 2.2
MOTOROLA-AUSTIN INTERNAL USE ONLY
COPYRIGHT MOTOROLA 1978

**UNIF. I/O ERROR-STATUS=00 AT 2971

=<b>RLOAD</b>
MDOS LINKING LOADER REV  3.02
COPYRIGHT BY MOTOROLA 1977
?EXIT

=
</code></pre>


Hit Ctrl-C once to access the simulator's debugger prompt.

Hit Ctrl-C again to exit EXORsim, or use the 'c' command to continue
simulating.

See the [MDOS quick start](doc/mdos-intro.md) to see how to write simple
assembly langauge programs.

Also see [EXORsim usage guide](doc/usage.md) for more usage information.

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

Just type "make" to compile it.

# Utilities

A number of utilities are built in addition to "exor":

	mdos	This provides file access to MDOS diskette images

	edos	This provides file access to EDOS-II diskette images

	unasm	This is a stand-alone 6800 disassember

	imdx	Utility to extract disk images from .imd files

# Blank disks

	sblank.dsk	Single sided blank disk with filesystem installed (DOSGEN ;U)

	dblank.dsk	Double sided blank disk with filesystem installed (DOSGEN ;U)

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
