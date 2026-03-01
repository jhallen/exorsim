# EDOS

EDOS and EDOS-II were the first disk operating systems available for the
EXORciser.  These operating systems were specifically for the EXORdisk-I
which was a re-badged iCOM FD360.

Here is Motorola's manual for this disk drive:

[M68FD3601-3604_EXORdisk_Floppy_Disk_System_Users_Guide_1975](https://www.bitsavers.org/components/motorola/6800/exorciser/M68FD3601-3604_EXORdisk_Floppy_Disk_System_Users_Guide_1975.pdf)

iCOM's manuals can be found here:

[iCOM FD360](https://deramp.com/downloads/mfe_archive/020-Disk%20Drives/iCom/iCOM%20FD360/)

The iCOM FD360 was a dual 8-inch disk drive with a built-in controller made
from a large number of TTL ICs.  It was attached via ribbon cables to a
relatively simple parallel interface board within the EXORciser.  This board
has a 1K PROM at address $E800 with some low level interface routines. 

iCOM developed FDOS to go along with FD360, for use with the 8080-based
Intellec and MITS Altair computers.  [FDOS was the first microprocessor disk
operating system](https://www.youtube.com/watch?v=TpHKygZ7OHY).
EDOS is a 6800 port of FDOS.

## Running EDOS-II from exorsim

Just type "exor1" to simulate an EXORciser with an EXORdisk-I with an
EDOS-II boot disk in drive 0.  '!' is the EDOS-II command prompt:

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>
$ <b>exor1 -x</b>
:9
EXBUG 1.1 <b>MAID</b>
*<b>E800;G</b>
M6800 EDOS VER 2.6

!<b>CDIR</b>

NAME   ATTR TRAK SCTR  SIZE

FORT    01   05   01   00C7
SORT    01   0C   12   0006
FORLB   01   0C   18   007A
RLOAD   01   11   10   00B4
EDIT    01   18   0E   0031
SORO1   00   1A   0B   000A
SORTL   00   1A   15   0066
MESS5   00   1E   13   000B
WAVS1   00   1F   04   0017
WAVS2   00   20   01   000F
INT1    00   20   10   0069
RTL1    00   24   11   009E
INT2    00   2A   13   0069
RTL2    00   2E   14   009E
CONTB   00   34   16   0002
NASMB   00   34   18   006F
ASMB    00   39   05   0079
HELLO   00   3D   16   0003
BASIC   00   3D   19   00DA


!
</code></pre>

## How EDOS works

EDOS is an interesting operating system- it is designed to use an absolute
minimum amount of memory.

EDOS is composed of two parts: the 1K PROM on the interface card and EXEC,
which is the main command-line user interface which is booted from a floppy. 

EXEC and other programs are stored in S19 format on the floppy.  S19 is not
very space efficient, but is versatile in that it can load any number of
segments of the file, each to any address.

The PROM has a number of entry points:

* E800: Boot EDOS EXEC from the floppy and jump to it
* E815: Load S19 from already open input file and jump to EXBUG
* E81B: Load S19 from already open input file, restore and jump to $0400
* E824: Load S19 from already open input file, restore and jump to $0020
* E809: Read a byte from an already open input file
* E82D: Restore: reset input file
* E80C: Write a byte to an already open output file
* E80F: "Update"- this means Boot EDOS EXEC, but with a flag set so that the first thing it does is close the output file.
* If the program wants to discard the output file, it should call the normal "Boot EDOS" function ($E800;G).

"Restore" resets the input file.  This means that it overwrites the input
file pointer from a temporary file pointer.  The temporary file pointer is
initialized to the start of the user specified input file, so "Restore"
effectively rewinds the input file.

These PROM subroutines are the only parts of EDOS which are available to
user programs.  For console I/O, the user's program should use the routines
provided by EXBUG.  For line printer output, the EXORdisk-I PROM provides a
few more entry points:

* EAB0: Print one character to the line printer
* EAD6: Print a line to the line printer, including added CR-LF sequence

The PROM uses RAM between $00 and $0F.  The rest of the RAM is free for use
by the user's program, even if that program is reading and writing files.

Sector buffers are required for floppy access.  These buffers are located in
the controller in the FD360 itself: one 128 byte buffer for input file
reading and one 128 byte buffer for output file writing.

Each file on the disk is stored in one contiguous extent: all you need for
reading is the starting track and sector numbers, plus the file size. 
Multiple files are packed contiguously together starting at the beginning. 
The one output file you may have open for writing can use the remaining
contiguous space.

## Command Syntax

EDOS-I refers to files by number, it's very primitive.  The documentation
for this is in the EXORdisk-I manual.  We do not have any EDOS-I boot disks.

EDOS-II has a conventional directory and allows you to refer to files by
name, but the name size is only 5 characters.  Unfortunately, we do not have
Motorola documentation for it.

However, the FDOS-II manual is here:

[https://bitsavers.trailing-edge.com/pdf/icom/FDOS-II_Manual_197610.pdf](https://bitsavers.trailing-edge.com/pdf/icom/FDOS-II_Manual_197610.pdf).

Unfortunately, EDOS-II doesn't quite match FDOS-II, so here is a quick
summary of the available EDOS-II commands:

[EDOS-II Commands](https://deramp.com/downloads/altair/software/icom_floppy/Exorciser/EDOS-II%20Commands.pdf)

## Programs

Here are programs that are available on "edos.dsk".

### FORT

FORT is the MC6800 FORTRAN 1.0 compiler.  We have this from an original
Motorola FORTRAN language disk.

An ad for it here:

[Resident FORTRAN Compiler](https://www.bitsavers.org/components/motorola/_catalogs/1979_Microcomputer_Development_Systems.pdf#page=157)

The manual for it is here:


FORTRAN 1.0 is an earlier version than documented in the 1976 and 1977 manuals.
One area in which v1.0 differs is in the optional numeric parameter that can
follow the listing device name.

FORT,#CN[x],objFn,srcFn

 #CN specifies the console as the listing device. #LP specifies the printer
as the listing device. The device names allowed are the same, however, the
meaning of the optional numeric parameter 'x' is different.

'0': Normal listing is generated (same as no number at all).

'1': Suppress the source, only the symbol table for each module is listed.

'5': Show the data generated by the compiler for each source line. 

Any other digit displays '?' like it wants additional input to continue.
I have not yet investigated what the expected input is.

### RLOAD

RLOAD is the linker.  We have this from an original Motorola FORTRAN
language disk.  The manual for it is here:

[M6800_Linking_Loader](https://www.bitsavers.org/components/motorola/6800/exorciser/M6800_Linking_Loader.pdf)

### EDIT

<p>EDIT is a much simplified <a href="http://en.wikipedia.org/wiki/Text_Editor_and_Corrector">TECO</a>
clone.  TECO is like a screen editor, but without the screen.  Instead you
have to use your imagination to remember where the cursor is.</p>

EDIT was available on the Motorola FORTRAN language disk.

A manual for EDIT is here:

[M6800_Co-Resident_Editor_Reference_Manual_1977](https://www.bitsavers.org/components/motorola/6800/exorciser/M6800_Co-Resident_Editor_Reference_Manual_1977.pdf)

### ASM (file ASMB)

ASM is the absolute assembler, meaning it generates S19 files directly (or
assembles directly to memory) but can not generate relocatable object files
that work with RLOAD.

Mike Douglas (aka "Deramp") ported this from the Astral-2000.

A manual for ASM is here:

[M6800_Co-Resident_Assembler_Reference_Manual_1976](https://www.bitsavers.org/components/motorola/6800/exorciser/M6800_Co-Resident_Assembler_Reference_Manual_1976.pdf)

Patch notes are here: [../stuff/edos/asmb](../stuff/edos/asmb)

There is an assembly language "Hello, world!" program on edos.dsk.  You can
try it like this:

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>

!<b>ASM,2,,OUT,HELLO</b>

M6800 ASSEMBLER VERSION 1.2

00001              * Hello
00002                     OPT    O
00003                     OPT    NOM
00004      E800    FDOS   EQU    $E800
00005      F018    XOUTC  EQU    $F018
00006 0020                ORG    $20
00007 0020 7E 0112        JMP    PRINTIT
00008 0100                ORG    $100
00009      0100    HELLO  EQU    *
00010 0100 0D             FCB    13
00011 0101 0A             FCB    $0A
00012 0102 48             FCC    'HELLO, WORLD!'
      0103 45
      0104 4C
      0105 4C
      0106 4F
      0107 2C
      0108 20
      0109 57
      010A 4F
      010B 52
      010C 4C
      010D 44
      010E 21
00013 010F 0D             FCB    $0D
00014 0110 0A             FCB    $0A
00015 0111 00             FCB    $00
00016 0112 CE 0100 PRINTI LDX    #HELLO
00017 0115 8D 03          BSR    PUTS
00018 0117 7E E800        JMP    FDOS
00019 011A A6 00   PUTS   LDA A  0,X
00020 011C 27 0C          BEQ    DONE
00021 011E FF 012B        STX    TMP
00022 0121 BD F018        JSR    XOUTC
00023 0124 FE 012B        LDX    TMP
00024 0127 08             INX
00025 0128 20 F0          BRA    PUTS
00026 012A 39      DONE   RTS
00027 012B 0000    TMP    FDB    0
00028                     END

TOTAL ERRORS 00000

!<b>CDIR</b>

NAME   ATTR TRAK SCTR  SIZE

FORT    01   05   01   00C7
SORT    01   0C   12   0006
FORLB   01   0C   18   007A
RLOAD   01   11   10   00B4
EDIT    01   18   0E   0031
SORO1   00   1A   0B   000A
SORTL   00   1A   15   0066
MESS5   00   1E   13   000B
WAVS1   00   1F   04   0017
WAVS2   00   20   01   000F
INT1    00   20   10   0069
RTL1    00   24   11   009E
INT2    00   2A   13   0069
RTL2    00   2E   14   009E
CONTB   00   34   16   0002
NASMB   00   34   18   006F
ASMB    00   39   05   0079
HELLO   00   3D   16   0003
BASIC   00   3D   19   00DA
OUT     00   46   09   0002

!<b>CLIST,OUT</b>
S00B00002020202020202020F4
S10600207E011248
S11E01000D0A48454C4C4F2C20574F524C44210D0A00CE01008D037EE800A6DE
S115011B00270CFF012BBDF018FE012B0820F039000030
S9030000FC

!<b>LOAD,OUT</b>
:9
EXBUG 1.1 <b>MAID</b>
*<b>20;G</b>
HELLO, WORLD!

M6800 EDOS VER 2.6

!
</code></pre>


### BASIC

This is the pre-MDOS version of BASIC (MDOS BASIC starts with version 2.0). 
It has similar language features to early 1970s Dartmouth and HP-2000 BASIC,
except no matrix functions.

Mike Douglas ported this "Astral disk BASIC" from the Astral-2000.

The Astral-2000 manual for it is here:

[Astral 2000 System Reference](https://deramp.com/downloads/altair/software/icom_floppy/Astral%202000%20FDOS-II/Astral%202000%20System%20Reference%20Manual.pdf)

It's unclear if this is exactly the same as Motorola's BASIC, but Motorola
definitely sold a version of BASIC for EDOS, there are ads for it:

[Resident BASIC Interpreter](https://www.bitsavers.org/components/motorola/_catalogs/1979_Microcomputer_Development_Systems.pdf#page=153)

S19 for it is here: [../stuff/edos/basic](../stuff/edos/basic)

<pre style="font-family: Andale Mono, Lucida Console, Monaco, fixed,
monospace; color: #000000; background-color: #eee;font-size: 12px;border:
1px dashed #999999;line-height: 14px;padding: 5px; overflow: auto; width:
100%"><code>

!<b>BASIC,,OUT</b>

READY
:<b>10 PRINT "HELLO, WORLD!"</b>
:<b>RUN
HELLO, WORLD!

READY
:<b>EDOS</b>

M6800 EDOS VER 2.6

!
</code></pre>


### Missing

One important program we are missing is the EDOS-II version of RASM (the
file name is RSMB).  This is the relocatable assembler that can generate
object files that can be linked with RLOAD.  If we had this, we could link
assembly language programs with FORTRAN.

## Source code

Mike Douglas entered the EDOS PROM source code by hand from the EXORdisk-I
user manual. It's here: [../stuff/edos/exordisk1](../stuff/edos/exordisk1)

EDOS-II EXEC source code is available here: [../stuff/edos/exec](../stuff/edos/exec)  It's not clear where this is
from, but it has the comment: "THIS PROGRAM WAS COMMENTED BY AUSTRALIAN 6800
USER'S GROUP"

## Disk Format

EDOS and EDOS-II use IBM format single sided, single density floppy disks:

* Tracks 77 (0 - 76)
* Sectors per track: 26 (1 - 26)
* Sector size: 128 bytes
* Interleave factor: 9 sectors
* Size: 256,256 bytes

Like CP/M, EDOS interleaves the sectors in software (vs. interleaved
formatting).

Track 0 holds the 6-sector directory.  It's in sectors 3 - 8. 
Directory entries are 11 bytes, and each directory sector has 11 entries.
Directory entries look like this:

~~~
struct dirent {
    unsigned char name[5]; /* 5 '*'s if deleted */
    unsigned char mark; /* 0xFF means end of directory, 0x80 means file was deleted */
    unsigned char track; /* Starting track number of file */
    unsigned char sect; /* Starting sector number of file (1 based) */
    unsigned char size_hi; /* Size high byte */
    unsigned char size_lo; /* Size low byte: size is one larger than actual size */
    unsigned char extra; /* This is observed to have 0xFF, 0x02 or 0x00 */
};
~~~

Tracks 1 - 4 hold the EDOS executive in S19 format.  The PROM is hard-coded
to load 78 sectors from tracks 1 - 3, but EDOS-II is larger than this- 89
sectors.  During the load, there is an S1 record that overwrites the file
size to 0x68 (it is written to address 0x04).

Tracks 5 - 76 is the space for file data.  It is broken in to two parts:
allocated space followed by free space.  When you create a new file, it is
written to the beginning of the free space.

Files can not be directly deleted.  Instead, you mark them for deletion by
using the "ATTR,filename,80" command.  Then you use the PURGE command to
delete the names from the directory and delete the gaps in the allocated
space.
