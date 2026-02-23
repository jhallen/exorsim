# RLOAD Object File Format

I've been reverse engineering the RLOAD (relocatable linker) object file
format.  I have no particular reason to do this, except to overcome the
frustration of having lost the official documentation for it.

Around 1985 one of my first programming projects was to make a
cross-reference tool for the external symbols of large, multi-module
assembly language programs.  At that time, I had access to the official
Motorola documentation for it.  I don't remember if this was a stand-alone
document, or if it was an appendix to RLOAD or RASM.  In any case, I've not
been able to find it in any of the available scans of the RLOAD or RASM
manuals.

relobj.c is a program which parses RLOAD object files.  When it runs into
unknown things, it prints "***".

# Intro 

Study the relocatable assembler and RLOAD manuals.  Each module produced by
the assembler has XREFs (external references) and XDEFs (symbols visible by
other modules).

There are a number of sections where code can be generated:

	ASCT Absolute section, use ORG to specify address
	BSCT Relocatable section below $100 for direct mode addressing
	DSCT Relocatable data section
	PSCT Relocatable code section
	CSCT FORTRAN blank common section
	COMM FORTRAN named common section in BSCT, DSCT or PSCT.

The blank and named common sections can not receive object data, you can
only use RMBs in them.  The starting address within each module for a common
section of the same name is the same (ones with the same name overlap).  The
size of each of these common sections is the size of the largest one.

During linking, there are local fixups and external fixups.  Local fixups
are for JMPs to locations within the same module, where the target symbol is
not an XDEF.  External fixups are for JMPs to targets in other modules.

# Records

Object files are composed of binary records.  Each record looks like this:

'D' \<size> \<body> \<checksum> 0x0D [0x0A] [NUL]*

Each record begins with an ASCII D character (0x44) and ends with a carriage
return (0x0D).

In EDOS, each record ends with CR-LF.

Any number of NULs may exist between records, even in MDOS.  Sometimes there
are NULs to the next sector boundary for libraries (a library is just a
bunch of object files concatenated together).

\<size> is one byte which gives the record size.  This size includes the
\<body> and the \<checksum>.

\<checksum> is one byte 2s complement of the sum of each byte in \<size> and
\<body>.  If you sum all the bytes in \<size>, \<body> and \<checksum>, the
result is zero.

# Record types

The format of \<body> is \<record-type> \<payload>

\<record-type> is a single ASCII digit that indicates the record type.  Here
are the known types:

## 0x32 (2) Module header

This is the first record of an object file, and it contains the module name. 
This is the name provided to the NAM assembly language directive.  The
format of its \<payload> is:

    <info>
    <name>  6-byte module name, space padded
    0x4F (unknown)
    0x42 (unknown)

\<info> seems to indicate the source language:

* 0x00: Assembly language
* 0x04: FORTRAN

## 0x33 (3) Symbols

This record provides external symbol names and types.  The format of its
\<payload> is as follows:

    <header>
    <symbols>

The \<header> is present only on the first symbols record in the object
file.  Subsequent symbol records only have \<symbols>

The format of \<header> is:

    0x00 (unknown)
    0x00 (unknown)
    0x00 (unknown)
    0x00 (unknown)
    0x00 (unknown)
    0x01 <size>     Two byte size of BSCT
    0x02 <size>     Two byte size of CSCT
    0x03 <size>     Two byte size of DSCT
    0x04 <size>     Two byte size of PSCT
    <symbols>       List of symbols

\<symbols> is a list of symbols.  There are a bunch of types of them:

    0x00 <size> <offset>      If present, it's the first "symbol".  It gives the size and offset of ASCT.
    0x30 <name>                XREF type "any" with 6-byte space padded name.
    0x31 <name>                XREF type BSCT
    0x33 <name>                XREF type DSCT
    0x34 <name>                XREF type PSCT
    0x20 <name> <offset>      XDEF type ASCT
    0x21 <name> <offset>      XDEF type BSCT
    0x23 <name> <offset>      XDEF type DSCT
    0x24 <name> <offset>      XDEF type PSCT
    0x11 <name> <size>        Named common block in BSCT of specified size
    0x13 <name> <size>        Named common block in DSCT
    0x14 <name> <size>        Named common block in PSCT

Where: \<name> is 6 bytes, space padded, \<size> is two bytes and \<offset>
is two bytes.

All of the symbols are added to a table (except for the ASCT record which has
no name) and refered to by index number in the fixup records.

Note that there are three types of symbols: section names (from the COMM
directive), XREFs and XDEFs.

The starting index of the first symbol is 5.  This is because 1 - 4 are used
for the built-in section names:

* 0001 is BSCT
* 0002 is CSCT
* 0003 is DSCT
* 0004 is PSCT

If the ASCT "symbol" is present, it is the first one in the list.

## 0x34 (4) Data

This contains object data generated by the assembler.  The format of its \<payload>
is as follows:

     0x00 (unknown)
     <section>  Section for this data: 0x04 PSCT, 0x03 DSCT, or 0x01 BSCT.
     <offset>   Two byte starting offset of this data.
     <data>     The data

## 0x35 (5) Fixups

This record has one or more fixups for the previous Data record.  The format
of its \<payload> is:

    0x00 <offset> <symbol number>    Add XREF word to data at <offset>
    0x04 <offset> <symbol number>    Add XREF byte to data at <offset>
    0x08 <offset> <symbol number>    Subtract address of data from word at <offset>.  <symbol number> is 4.
    0x81 - 0xFF <offset> [<sect1> <sect2> <sect3>] <count> <commands> [<skip> <count> <commands>]*
                                     Apply local fixups starting with byte at <offset>

For fixing up the two byte offset following 6809 long branches, there are two
fixups needed: one of type 0x00 adds the target address to the two bytes and
one of type 0x08 subtracts the offset's own address to make it relative.

Local fixups (types beginning with 0x81 - 0xFF) use a list of commands.  The
type code itself is further encoded in this case:

* Bit 7     Always set
* Bit 6     Use default command code assignments
* Bit 5     Not sure
* Bits 4:1  This is the number of extensions
* Bit 0     Always set

If bit 6 is clear, then \<sect1> \<sect2> and \<sect3> are present.  These
are two byte indices to the symbol representing a section or the COMM block
symbol to be used for a particular command code, see below.

Usually bit 6 is set so that defaults are used, but it will be clear for
CSCT and COMM local fixups.

\<count> is one byte holding the number of commands in the subsequent
\<commands>.  There are two bits per command, and they fill in the bytes
that make up \<commands>, MSB first.  So bits 7:6 of the first byte in
\<commands> has the first command.  Bits 5:4 has the second, and so on. 
\<commands> is padded with 0s to fill up the last byte.

The command codes are:

    00 Skip one byte
    01 Fixup with <sect1> or BSCT if bit 6 is set
    10 Fixup with <sect2> or DSCT if bit 6 is set
    11 Fixup with <sect3> or PSCT if bit 6 is set

DSCT and PSCT fixups assume extended addressing (a two byte fixup).  BSCT
fixups assume direct addressing (a one byte fixup).

If bits 4:1 are non-zero, then there are extensions, so \<skip> \<count>
\<commands> is present.
    
\<skip> is a single byte whose lower 7 bits (?) indicate the number of data
bytes to skip before the extension applies.  Bit 7 of this byte is always
set.

## 0x36 (6) End of module

This is the last record of the object file.  Its payload is 3 bytes. 
Normally all three bytes are zeros.  But if a start address was provided to
the END assembly language directive, then this provides it:

    <section> <offset>

\<section> is one byte indicating the section of the starting address.
\<offset> is two bytes giving the starting address relative that specified
section.
