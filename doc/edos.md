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

## How EDOS works

EDOS is an interesting operating system- it is designed to use an absolute
minimum amount of memory.

EDOS is composed of two parts: the 1K PROM on the interface card and EXEC,
which is the main command-line user interface which is booted from a floppy. 

EXEC and other programs are stored in S19 format on the floppy.  S19 is not
very space efficient, but is versatile in that it can load any number of
segments of the file to any address.

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
the controller itself: one 128 byte buffer for input file reading and one
128 byte buffer for output file writing.

Each file on the disk is stored in one contiguous segment: all you need for
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
Motorola FORTRAN language disk.  The manual for it is here:

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

### ASM

ASM is the absolute assembler, meaning it generates S19 files directly but
can not generate relocatable object files that work with RLOAD.

Mike Douglas (aka "Deramp") ported this from the Astral-2000.

A manual for ASM is here:

[M6800_Co-Resident_Assembler_Reference_Manual_1976](https://www.bitsavers.org/components/motorola/6800/exorciser/M6800_Co-Resident_Assembler_Reference_Manual_1976.pdf)

### BASIC

This is the pre-MDOS version of BASIC (MDOS BASIC starts with version 2.0). 
It has similar language features to early 1970s Dartmouth and HP-2000 BASIC,
except no matrix functions.

Mike Douglas ported this "Astral disk BASIC" from the Astral-2000.

The Astral-2000 manual for it is here:

[Astral 2000 System Reference](https://deramp.com/downloads/altair/software/icom_floppy/Astral%202000%20FDOS-II/Astral%202000%20System%20Reference%20Manual.pdf)

It's unclear if this is exactly the same as Motorola's BASIC, but Motorola
definitely sold a version of BASIC for EDOS, there are ads for it:

### Missing

One important program we are missing is the EDOS-II version of RASM (the
file name is RSMB).  This is the relocatable assembler that can generate
object files that can be linked with RLOAD.  If we had this, we could link
assembly language programs with FORTRAN.

## Source code

Mike Douglas entered the EDOS PROM source code by hand from the EXORdisk-I
user manual. It's here:

EDOS-II EXEC source code is available here.  It's not clear where this is
from, but it has the comment: "THIS PROGRAM WAS COMMENTED BY AUSTRALIAN 6800
USER'S GROUP"
