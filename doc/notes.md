# Various notes

## Missing information

We don't have RASM.CM for MDOS09.

We don't have EDIT.CM for MDOS09: but now we have MEDIT!  This is an EDIT
clone by Dave Hatch, see MEDIT.SA.  Rubout works correctly for this editor.

We don't have the COBOL manual.

We don't have the FORTRAN manual.

We don't have the RLOAD object file format.

We don't have a bootable EDOS-II disk.  We don't have a manual for it
either but some of commands are shown in the EXORdisk-I brochure.

We don't have a bootable EDOS disk.

## CRT-mode patches

### MDOS09

Unfortunately the version of MDOS09 we have (version 3.04) is designed for a
teletype like ASR-33: when you hit Backspace (the key that sends
0x7F), it emits the character deleted.  This is the best you can do on a
teletype.

There is a chain file that is supposed to patch MDOS to emit 0x08, 0x20,
0x08 instead for a CRT terminal (MDOSMODE.CF).  At least this chain file is
available on some of the 6800 versions of MDOS.  Unfortunately, we don't
have it for MDOS09.

So here is  my own patch for this.  It's included in the mdos09.dsk image. 
(I searched for 0xC1 0x7F 0x26 0x1F in the entire disk image to find the
code, but it's in one of the MDOS overlay files).

Original:  this is code that is checking what to do for each entered key.

~~~
1946: C1 7F               CMPB #$7f          Check for Delete
1948: 26 1F               BNE $1969            branch if not delete

194A: BD 1B8D             JSR $1b8d          Check if at start, load cursor ptr to b,a
194D: 27 DD               BEQ $192c            Loop, next char

194F: 4D                  TSTA               Decrement b,a: backup cursor
1950: 26 01               BNE $1953
1952: 5A                  DECB 
1953: 4A                  DECA 

1954: AE 66               LDX 6,S            Get pointer to struct
1956: E7 02               STB 2,X            Save updated cursor
1958: A7 03               STA 3,X

195A: 7F 1BD9             CLR $1bd9          Overflowed line flag?

195D: 3F                  SWI                  B,A -> X
195E: 25                  FCB $25

195F: E6 84               LDB ,X             Get old character at cursor
1961: BD 1AFC             JSR $1afc          Echo deleted character b
1964: 24 C6               BCC $192c            Print OK, loop, get next char
1966: 7E 1AC7             JMP $1ac7          not sure what this does

1969: C1 18               CMPB #$18           Check for Ctrl-X
196B: 26 06               BNE $1973
~~~

This is my replacement code:

~~~
% u 1946
1946: C1 7F               CMPB #$7f
1948: 26 1F               BNE $1969

194A: AE 66               LDX 6,S           Get structure pointer
194C: EC 02               LDD 2,X           Get cursor
194E: 10A3 04             CMPD 4,X          At beginning?
1951: 27 D9               BEQ $192c          branch if yes
1953: 83 0001             SUBD #$0001       Backup cursor
1956: ED 02               STD 2,X
1958: 8D 09               BSR $1963         Print 0x08
195A: C6 20               LDB #$20          Print 0x20
195C: 8D 07               BSR $1965
195E: C6 08               LDB #$08          Print 0x08
1960: 12                  NOP 

$1961 might be an entry point, so preserve its behavior:
1961: 20 C1               BRA $1924         Jump to some other code that calls 1afc then 1ac7.
1963: C6 08               LDB #$08
1965: 7E 1AFC             JMP $1afc
1968: 12                  NOP 

1969: C1 18               CMPB #$18
196B: 26 06               BNE $1973
~~~

### E

The 6800 and 6809 versions of E have the same problem.  There is a chain
file to patch it for CRT mode, but it's for E version 3.14, but we have
version 3.12 so it does not work.

Some other notes about 6800 E:

$251E appears to be the start of the input buffer.

E.CM is 20408 bytes- it loads from $2000 - $6fb7 and starts at $2b6d.
There are two overlay areas:

$637f - $6dae, 2608 bytes.

$6daf - $6fb6, 520 bytes.

The copyright notice is located at $638C.  Unfortunately, this is in the
overlay area, which is too bad because we need a place to fit our patch.

For the 6800 version of E, $3483 is the location of the code that performs
the backspace:

~~~
% u 3483
3483: DE F3               LDX $00F3  Has same pointer as cursor?
3485: 09                  DEX
3486: DF F3               STX $00F3
3488: DE F1               LDX $00F1  Cursor pointer
348A: 09                  DEX
348B: DF F1               STX $00F1
348D: E6 00               LDB $00,X  Get character to delete
348F: BD 38 DF            JSR $38DF  Print character in B
3492: 7A 00 FF            DEC $00FF  Line length?
3495: 39                  RTS
~~~

Notice that this code does not check for the beginning of the buffer before
decrementing the pointers in $F1 and $F3.  Sure enough, the code is buggy:
if you hit Backspace 100 times you'll see junk echoing back (after echoing
back a bunch of NULs which appear before the start of the buffer).

Here is the patched version of 6800 E:

~~~
% u 3483
3483: 7E 2D 9A            JMP $2D9A

% u 2d9a
2D9A: DE F1               LDX $00F1
2D9C: 8C 25 1E            CPX #$251E   Don't backspace before start
2D9F: 27 16               BEQ $2DB7
2DA1: 09                  DEX          Update cursor
2DA2: DF F1               STX $00F1
2DA4: DE F3               LDX $00F3
2DA6: 09                  DEX          Update other pointer
2DA7: DF F3               STX $00F3
2DA9: 7A 00 FF            DEC $00FF    Decrement line length?
2DAC: 8D 04               BSR $2DB2    Emit 8
2DAE: C6 20               LDB #$20     Emit 20
2DB0: 8D 02               BSR $2DB4
2DB2: C6 08               LDB #$08     Emit 8
2DB4: 7E 38 DF            JMP $38DF
2DB7: 39                  RTS
~~~

The region 0x2d9a - 0x2db7 was all zeros to start with.  There is some
evidence that this area is safe to use because the existing ECUSTOM.CF used
it.  Even so it's risky since ECUSTOM.CF is for E version 3.14...

For the 6809 version of E, $34A7 is the location of the code that performs
the backspace:

~~~
% u 34a7
34A7: 9E F3               LDX $f3    Has same pointer as cursor?
34A9: 30 1F               LEAX -1,X
34AB: 9F F3               STX $f3
34AD: 9E F1               LDX $f1    Cursor pointer
34AF: 30 1F               LEAX -1,X
34B1: 9F F1               STX $f1
34B3: E6 84               LDB ,X     Get character to delete
34B5: BD 3911             JSR $3911  Print character in B
34B8: 0A FF               DEC $ff    Line length?
34BA: 39                  RTS 
~~~

Here is the patched version of 6809 E:

~~~
% u 34a7
34A7: 7E 2DBE             JMP $2dbe

% u 2dbe
2DBE: 9E F1               LDX $f1
2DC0: 8C 251E             CMPX #$251e
2DC3: 27 17               BEQ $2ddc
2DC5: 30 1F               LEAX -1,X
2DC7: 9F F1               STX $f1
2DC9: 9E F3               LDX $f3
2DCB: 30 1F               LEAX -1,X
2DCD: 9F F3               STX $f3
2DCF: 0A FF               DEC $ff
2DD1: 8D 04               BSR $2dd7
2DD3: C6 20               LDB #$20
2DD5: 8D 02               BSR $2dd9
2DD7: C6 08               LDB #$08
2DD9: 7E 3911             JMP $3911
2DDC: 39                  RTS 
~~~

Again the area 0x2DBE - 0x2DDC was originally all zeros.
