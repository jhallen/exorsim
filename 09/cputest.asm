*
*  MC6809 CPU Emulation Validation
*
* Tested on an SGS Thomson EF6809P Processor
*
* W. Schwotzer                     20.07.2003
*

*PDATA1 EQU   2     ; Output EOT terminated string: ASSIST09 SWI

ACIAD  EQU   $FCF5

EXTREG EQU   $8000
EOT    EQU   $4

       ORG   $2000

BEGIN  BRA   START

PSTRNG	LDA	,X+
	CMPA	#EOT
	BEQ	PSDONE
	STA	ACIAD
	BRA	PSTRNG
PSDONE	RTS


VERSIO FCB   1
REG161 FDB   0


* Call from ASSIST or something
START  CLR   ERRFLG ; Clear Error flag
       JSR   TDAA
*
* Test Addressing modes
       JSR  TAD1
       JSR  TAD2
       JSR  TAD3
       JSR  TAD4
       JSR  TAD5
       JSR  TAD6
       JSR  TAD7
       JSR  TAD8
       JSR  TAD9
       JSR  TADA
       JSR  TADB
       JSR  TADI1
       JSR  TADI3
       JSR  TADI5
       JSR  TADI6
       JSR  TADI7
       JSR  TADI8
       JSR  TADI9
       JSR  TADIA
       JSR  TADIB
*       JSR  TADIC
* Test Instructions
       JSR   TNEG
       JSR   TCOM
       JSR   TDEC
       JSR   TINC
       JSR   TCLR
       JSR   TADD
       JSR   TADDD
       JSR   TADC
       JSR   TMUL
       JSR   TSEX
       JSR   TSUB
       JSR   TSUBD
       JSR   TSBC
       JSR   TDAA
       JSR   TCMP
       JSR   TCMPD
       JSR   TTST
       JSR   TBIT
       JSR   TLSR
       JSR   TLSL
       JSR   TASR
       JSR   TROL
       JSR   TROR
       JSR   TLD
       JSR   TST
       JSR   TLDD
       JSR   TSTD
       JSR   TLEA
       JSR   TTFR
       JSR   TEXG
*
       JSR   OUTSUC
*	JMP START
       RTS
*       JMP  WARMS

**************************************************
* Test MUL
**************************************************
TMUL   LDU   #LMUL
       LDX   #TBMUL
TMUL1  LDD   ,X++
       MUL
       TFR   CC,DP
       CMPD  ,X++
       LBNE  OUTERR
       TFR   DP,B
       ANDB  #$05   ; Only check Bits ZC
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LMUL
       LBNE  TMUL1
       RTS

TBMUL  FDB   $0000,$0000
       FCB   $04
       FDB   $8080,$4000
       FCB   $00
       FDB   $01FF,$00FF
       FCB   $01
       FDB   $FF01,$00FF
       FCB   $01
       FDB   $7F81,$3FFF
       FCB   $01
       FDB   $817F,$3FFF
       FCB   $01
       FDB   $C040,$3000
       FCB   $00
       FDB   $40C0,$3000
       FCB   $00
       FDB   $FFFF,$FE01
       FCB   $00
       FDB   $7F7F,$3F01
       FCB   $00
       FDB   $0101,$0001
       FCB   $00
LMUL   FCC   'MUL'
	FCB	13,10,EOT

**************************************************
* Test SEX
**************************************************
TSEX   LDU   #LSEX
       LDX   #TBSEX
TSEX1  LDB   ,X+
       SEX
       TFR   CC,DP
       CMPD  ,X++
       LBNE  OUTERR
       TFR   DP,B
       ANDB  #$0C   ; Only check Bits NZ
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LSEX
       LBNE  TSEX1
       RTS


TBSEX  FCB   $00
       FDB   $0000
       FCB   $04
       FCB   $01
       FDB   $0001
       FCB   $00
       FCB   $7F
       FDB   $007F
       FCB   $00
       FCB   $80
       FDB   $FF80
       FCB   $08
       FCB   $FF
       FDB   $FFFF
       FCB   $08
LSEX   FCC   'SEX'
	FCB	13,10,EOT

**************************************************
* Test TST
**************************************************
TTST   LDU   #LTST
       LDX   #TBTST
TTST1  LDA   ,X+
       TSTA
       TFR   CC,B
       ANDB  #$0E   ; Only check Bits NZV
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LTST
       LBNE  TTST1
       RTS

TBTST  FCB   $00,$04
       FCB   $80,$08
       FCB   $01,$00
       FCB   $FF,$08
       FCB   $7F,$00
       FCB   $81,$08
       FCB   $C0,$08
       FCB   $40,$00
LTST   FCC   'TST'
	FCB	13,10,EOT

**************************************************
* Test LD
**************************************************
TLD    LDU   #LLD
       LDX   #TBTST
TLD1   LDA   ,X+
       TFR   CC,B
       ANDB  #$0E   ; Only check Bits NZV
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LTST
       LBNE  TLD1
       RTS
LLD    FCC   'LD'
	FCB	13,10,EOT

**************************************************
* Test ST
**************************************************
TST    LDU   #LST
       LDX   #TBTST
TST1   LDA   ,X+
       STA   EXTREG
       TFR   CC,B
       ANDB  #$0E   ; Only check Bits NZV
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LTST
       LBNE  TST1
       RTS
LST    FCC   'ST'
	FCB	13,10,EOT

**************************************************
* Test LDD
**************************************************
TLDD   LDU   #LLDD
       LDX   #TBLDD
TLDD1  LDD   ,X++
       TFR   CC,B
       ANDB  #$0E   ; Only check Bits NZV
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LLDD
       LBNE  TLDD1
       RTS
TBLDD  FDB   $0000
       FCB   $04
       FDB   $8000
       FCB   $08
       FDB   $0001
       FCB   $00
       FDB   $FFFF
       FCB   $08
       FDB   $7FFF
       FCB   $00
       FDB   $8001
       FCB   $08
       FDB   $C000
       FCB   $08
       FDB   $4000
       FCB   $00
LLDD   FCC   'LDD'
	FCB	13,10,EOT

**************************************************
* Test STD
**************************************************
TSTD   LDU   #LSTD
       LDX   #TBLDD
TSTD1  LDD   ,X++
       STD   EXTREG
       TFR   CC,B
       ANDB  #$0E   ; Only check Bits NZV
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LLDD
       LBNE  TSTD1
       RTS
LSTD   FCC   'STD'
	FCB	13,10,EOT

**************************************************
* Test LEA
**************************************************
TLEA   LDU   #LLEA
       LDX   #TBLEA
TLEA1  LDY   ,X++
       LEAY  ,Y
       TFR   CC,B
       ANDB  #$04   ; Only check Bit Z
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LLEA
       LBNE  TLEA1
       RTS

TBLEA  FDB   $0000
       FCB   $04
       FDB   $0001
       FCB   $00
       FDB   $FFFF
       FCB   $00
       FDB   $7FFF
       FCB   $00
       FDB   $8000
       FCB   $00
       FDB   $8001
       FCB   $00
LLEA   FCC   'LEA'
	FCB	13,10,EOT

**************************************************
* Test BIT
**************************************************
TBIT   LDU   #LBIT
       LDX   #TBBIT
TBIT1  LDA   ,X+
       BITA  ,X+
       TFR   CC,B
       ANDB  #$0E   ; Only check Bits NZV
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LBIT
       LBNE  TBIT1
       RTS

TBBIT  FCB   $00,$00,$04
       FCB   $AA,$55,$04
       FCB   $AA,$AA,$08
       FCB   $55,$55,$00
       FCB   $FF,$FF,$08
       FCB   $FF,$80,$08
       FCB   $81,$80,$08
       FCB   $FF,$7F,$00
       FCB   $FF,$01,$00
       FCB   $F0,$0F,$04
LBIT   FCC   'BIT'
	FCB	13,10,EOT

**************************************************
* Test DAA
**************************************************
TDAA   LDU   #LDAA
       LDX   #TBDAA
TDAA1  LDA   ,X+
       TFR   CC,B
       ANDB  ,X+
       ORB   ,X+
       TFR   B,CC
       DAA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0D   ; Only check Bits NZC
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LDAA
       LBNE  TDAA1
       RTS

TBDAA
* Carry: 0, Halfcarry: 0
       FCB   $00,$DE,$00,$00,$04
       FCB   $01,$DE,$00,$01,$00
       FCB   $09,$DE,$00,$09,$00
       FCB   $0A,$DE,$00,$10,$00
       FCB   $0F,$DE,$00,$15,$00
       FCB   $10,$DE,$00,$10,$00
       FCB   $4A,$DE,$00,$50,$00
       FCB   $79,$DE,$00,$79,$00
       FCB   $7A,$DE,$00,$80,$08
       FCB   $7F,$DE,$00,$85,$08
       FCB   $81,$DE,$00,$81,$08
       FCB   $99,$DE,$00,$99,$08
       FCB   $A0,$DE,$00,$00,$05
       FCB   $BF,$DE,$00,$25,$01
       FCB   $F0,$DE,$00,$50,$01
       FCB   $FF,$DE,$00,$65,$01
* Carry: 1, Halfcarry: 0
       FCB   $00,$DF,$01,$60,$01
       FCB   $01,$DF,$01,$61,$01
       FCB   $09,$DF,$01,$69,$01
       FCB   $0A,$DF,$01,$70,$01
       FCB   $0F,$DF,$01,$75,$01
       FCB   $10,$DF,$01,$70,$01
       FCB   $4A,$DF,$01,$B0,$09
       FCB   $79,$DF,$01,$D9,$09
       FCB   $7A,$DF,$01,$E0,$09
       FCB   $7F,$DF,$01,$e5,$09
       FCB   $81,$DF,$01,$E1,$09
       FCB   $99,$DF,$01,$F9,$09
       FCB   $A0,$DF,$01,$00,$05
       FCB   $BF,$DF,$01,$25,$01
       FCB   $F0,$DF,$01,$50,$01
       FCB   $FF,$DF,$01,$65,$01
* Carry: 0, Halfcarry: 1
       FCB   $00,$FE,$20,$06,$00
       FCB   $01,$FE,$20,$07,$00
       FCB   $09,$FE,$20,$0F,$00
       FCB   $0A,$FE,$20,$10,$00
       FCB   $0F,$FE,$20,$15,$00
       FCB   $10,$FE,$20,$16,$00
       FCB   $4A,$FE,$20,$50,$00
       FCB   $79,$FE,$20,$7F,$00
       FCB   $7A,$FE,$20,$80,$08
       FCB   $7F,$FE,$20,$85,$08
       FCB   $81,$FE,$20,$87,$08
       FCB   $99,$FE,$20,$9F,$08
       FCB   $A0,$FE,$20,$06,$01
       FCB   $BF,$FE,$20,$25,$01
       FCB   $F0,$FE,$20,$56,$01
       FCB   $FF,$FE,$20,$65,$01
* Carry: 1, Halfcarry: 1
       FCB   $00,$FF,$21,$66,$01
       FCB   $01,$FF,$21,$67,$01
       FCB   $09,$FF,$21,$6F,$01
       FCB   $0A,$FF,$21,$70,$01
       FCB   $0F,$FF,$21,$75,$01
       FCB   $10,$FF,$21,$76,$01
       FCB   $4A,$FF,$21,$B0,$09
       FCB   $79,$FF,$21,$DF,$09
       FCB   $7A,$FF,$21,$E0,$09
       FCB   $7F,$FF,$21,$E5,$09
       FCB   $81,$FF,$21,$E7,$09
       FCB   $99,$FF,$21,$FF,$09
       FCB   $A0,$FF,$21,$06,$01
       FCB   $BF,$FF,$21,$25,$01
       FCB   $F0,$FF,$21,$56,$01
       FCB   $FF,$FF,$21,$65,$01
LDAA   FCC   'DAA'
	FCB	13,10,EOT

**************************************************
* Test NEG
**************************************************
TNEG   LDU   #LNEG
       LDX   #TBNEG
TNEG1  LDA   ,X+
       NEGA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0F   ; Only check Bits NZVC
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LNEG
       LBNE  TNEG1
       RTS

TBNEG  FCB   $00,$00,$04
       FCB   $80,$80,$0B
       FCB   $01,$FF,$09
       FCB   $FF,$01,$01
       FCB   $7F,$81,$09
       FCB   $81,$7F,$01
       FCB   $C0,$40,$01
       FCB   $40,$C0,$09
LNEG   FCC   'NEG'
	FCB	13,10,EOT

**************************************************
* Test COM
**************************************************
TCOM   LDU   #LCOM
       LDX   #TBCOM
TCOM1  LDA   ,X+
       COMA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0F   ; Only check Bits NZVC
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LCOM
       BNE   TCOM1
       RTS


TBCOM  FCB   $00,$FF,$09
       FCB   $FF,$00,$05
       FCB   $F0,$0F,$01
       FCB   $0F,$F0,$09
       FCB   $55,$AA,$09
       FCB   $AA,$55,$01
       FCB   $01,$FE,$09
       FCB   $FE,$01,$01
       FCB   $80,$7F,$01
       FCB   $7F,$80,$09
LCOM   FCC   'COM'
	FCB	13,10,EOT

**************************************************
* Test LSR
**************************************************
TLSR   LDU   #LLSR
       LDX   #TBLSR
TLSR1  LDA   ,X+
       LSRA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0F   ; Only check Bits NZVC
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LLSR
       BNE   TLSR1
       RTS


TBLSR  FCB   $00,$00,$04
       FCB   $FF,$7F,$01
       FCB   $7F,$3F,$01
       FCB   $3F,$1F,$01
       FCB   $1F,$0F,$01
       FCB   $0F,$07,$01
       FCB   $07,$03,$01
       FCB   $03,$01,$01
       FCB   $01,$00,$05
       FCB   $55,$2A,$01
       FCB   $AA,$55,$00
       FCB   $80,$40,$00
       FCB   $10,$08,$00
       FCB   $C0,$60,$00
       FCB   $E0,$70,$00
       FCB   $F0,$78,$00
       FCB   $F8,$7C,$00
       FCB   $FC,$7E,$00
       FCB   $FE,$7F,$00
LLSR   FCC   'LSR'
	FCB	13,10,EOT

**************************************************
* Test LSL
**************************************************
TLSL   LDU   #LLSL
       LDX   #TBLSL
TLSL1  LDA   ,X+
       LSLA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0F   ; Only check Bits NZVC
       CMPB  ,X+
*       LBNE  OUTERR
       CMPX  #LLSL
       BNE   TLSL1
       RTS


TBLSL  FCB   $00,$00,$04
       FCB   $FF,$FE,$01
       FCB   $7F,$FE,$01
       FCB   $3F,$7E,$01
       FCB   $1F,$3E,$00
       FCB   $0F,$1E,$00
       FCB   $07,$0E,$00
       FCB   $03,$06,$00
       FCB   $01,$02,$00
       FCB   $55,$AA,$00
       FCB   $AA,$54,$00
       FCB   $80,$00,$01
       FCB   $10,$20,$01
       FCB   $C0,$80,$01
       FCB   $E0,$C0,$01
       FCB   $F0,$E0,$01
       FCB   $F8,$F0,$01
       FCB   $FC,$F8,$01
       FCB   $FE,$FC,$01
LLSL   FCC   'LSL'
	FCB	13,10,EOT

**************************************************
* Test ASR
**************************************************
TASR   LDU   #LASR
       LDX   #TBASR
TASR1  LDA   ,X+
       ASRA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0D   ; Only check Bits NZC
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LASR
       BNE   TASR1
       RTS


TBASR  FCB   $00,$00,$04
       FCB   $FF,$FF,$09
       FCB   $7F,$3F,$01
       FCB   $3F,$1F,$01
       FCB   $1F,$0F,$01
       FCB   $0F,$07,$01
       FCB   $07,$03,$01
       FCB   $03,$01,$01
       FCB   $01,$00,$05
       FCB   $55,$2A,$01
       FCB   $AA,$D5,$08
       FCB   $80,$C0,$08
       FCB   $10,$08,$00
       FCB   $C0,$E0,$08
       FCB   $E0,$F0,$08
       FCB   $F0,$F8,$08
       FCB   $F8,$FC,$08
       FCB   $FC,$FE,$08
       FCB   $FE,$FF,$08
LASR   FCC   'ASR'
	FCB	13,10,EOT

**************************************************
* Test ROR
**************************************************
TROR   LDU   #LROR
       LDX   #TBROR
TROR1  LDA   ,X+
       TFR   CC,B
       ANDB  ,X+
       ORB   ,X+
       TFR   B,CC
       RORA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0D   ; Only check Bits NZC
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LROR
       BNE   TROR1
       RTS

TBROR  FCB   $00,$FE,$00,$00,$04
       FCB   $01,$FE,$00,$00,$05
       FCB   $0F,$FE,$00,$07,$01
       FCB   $10,$FE,$00,$08,$00
       FCB   $7F,$FE,$00,$3F,$01
       FCB   $80,$FE,$00,$40,$00
       FCB   $FE,$FE,$00,$7F,$00
       FCB   $FF,$FE,$00,$7F,$01
       FCB   $00,$FF,$01,$80,$08
       FCB   $01,$FF,$01,$80,$09
       FCB   $0F,$FF,$01,$87,$09
       FCB   $10,$FF,$01,$88,$08
       FCB   $7F,$FF,$01,$BF,$09
       FCB   $80,$FF,$01,$C0,$08
       FCB   $FE,$FF,$01,$FF,$08
       FCB   $FF,$FF,$01,$FF,$09
LROR   FCC   'ROR'
	FCB	13,10,EOT

**************************************************
* Test ROL
**************************************************
TROL   LDU   #LROL
       LDX   #TBROL
TROL1  LDA   ,X+
       TFR   CC,B
       ANDB  ,X+
       ORB   ,X+
       TFR   B,CC
       ROLA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0D   ; Only check Bits NZC
       CMPB  ,X+
*       LBNE  OUTERR
       CMPX  #LROL
       BNE   TROL1
       RTS

TBROL  FCB   $00,$FE,$00,$00,$04
       FCB   $01,$FE,$00,$02,$00
       FCB   $0F,$FE,$00,$1E,$00
       FCB   $08,$FE,$00,$10,$00
       FCB   $7F,$FE,$00,$FE,$08
       FCB   $80,$FE,$00,$00,$05
       FCB   $FE,$FE,$00,$FC,$09
       FCB   $FF,$FE,$00,$FE,$09
       FCB   $00,$FF,$01,$01,$00
       FCB   $01,$FF,$01,$03,$00
       FCB   $0F,$FF,$01,$1F,$00
       FCB   $08,$FF,$01,$11,$00
       FCB   $7F,$FF,$01,$FF,$08
       FCB   $80,$FF,$01,$01,$01
       FCB   $FE,$FF,$01,$FD,$09
       FCB   $FF,$FF,$01,$FF,$09
LROL   FCC   'ROL'
	FCB	13,10,EOT

**************************************************
* Test DEC
**************************************************
TDEC   LDU   #LDEC
       LDX   #TBDEC
TDEC1  LDA   ,X+
       DECA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0E   ; Only check Bits NZV
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LDEC
       BNE   TDEC1
       RTS

TBDEC  FCB   $01,$00,$04
       FCB   $00,$FF,$08
       FCB   $FF,$FE,$08
       FCB   $80,$7F,$02
       FCB   $7F,$7E,$00
       FCB   $10,$0F,$00
       FCB   $0F,$0E,$00
LDEC   FCC   'DEC'
	FCB	13,10,EOT

**************************************************
* Test INC
**************************************************
TINC   LDU   #LINC
       LDX   #TBINC
TINC1  LDA   ,X+
       INCA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0E   ; Only check Bits NZV
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LINC
       BNE   TINC1
       RTS

TBINC  FCB   $00,$01,$00
       FCB   $FE,$FF,$08
       FCB   $FF,$00,$04
       FCB   $7F,$80,$0A
       FCB   $80,$81,$08
       FCB   $0F,$10,$00
       FCB   $10,$11,$00
LINC   FCC   'INC'
	FCB	13,10,EOT

**************************************************
* Test CLR
**************************************************
TCLR   LDU   #LCLR
       LDX   #TBCLR
TCLR1  LDA   ,X+
       CLRA
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0F   ; Only check Bits NZVC
       CMPB  ,X+
*       LBNE  OUTERR
       CMPX  #LCLR
       BNE   TCLR1
       RTS

TBCLR  FCB   $01,$00,$04
       FCB   $00,$00,$04
       FCB   $FF,$00,$04
       FCB   $80,$00,$04
       FCB   $7F,$00,$04
       FCB   $10,$00,$04
       FCB   $0F,$00,$04
LCLR   FCC   'CLR'
	FCB	13,10,EOT

**************************************************
* Test ADDD
**************************************************
TADDD  LDU   #LADDD
       LDX   #TBADDD
TADDD1 LDD   ,X++
       ADDD  ,X++
       TFR   CC,DP
       CMPD  ,X++
       LBNE  OUTERR
       TFR   DP,A
       ANDA  #$0F   ; Only check Bits NZVC
       CMPA  ,X+
       LBNE  OUTERR
       CMPX  #LADDD
       BNE   TADDD1
       RTS

TBADDD FDB   $0000,$0000,$0000
       FCB   $04
       FDB   $0000,$0001,$0001
       FCB   $00
       FDB   $4000,$4000,$8000
       FCB   $0A
       FDB   $0000,$8000,$8000
       FCB   $08
       FDB   $8000,$8000,$0000
       FCB   $07
       FDB   $4000,$8000,$C000
       FCB   $08
       FDB   $7FFF,$7FFF,$FFFE
       FCB   $0A
       FDB   $7FFF,$8000,$FFFF
       FCB   $08
       FDB   $7FFF,$0001,$8000
       FCB   $0A
       FDB   $8000,$0001,$8001
       FCB   $08
       FDB   $4000,$C000,$0000
       FCB   $05
       FDB   $4000,$C001,$0001
       FCB   $01
       FDB   $FFFF,$FFFF,$FFFE
       FCB   $09
       FDB   $8001,$8000,$0001
       FCB   $03
LADDD  FCC   'ADDD'
	FCB	13,10,EOT

**************************************************
* Test ADD
**************************************************
TADD   LDU   #LADD
       LDX   #TBADD
TADD1  LDB   ,X+
       ADDB  ,X+
       TFR   CC,A
       CMPB  ,X+
       LBNE  OUTERR
       ANDA  #$2F   ; Only check Bits HNZVC
       CMPA  ,X+
       LBNE  OUTERR
       CMPX  #LADD
       BNE   TADD1
       RTS

TBADD  FCB   $00,$00,$00,$04
       FCB   $00,$01,$01,$00
       FCB   $40,$40,$80,$0A
       FCB   $00,$80,$80,$08
       FCB   $80,$80,$00,$07
       FCB   $40,$80,$C0,$08
       FCB   $7F,$7F,$FE,$2A
       FCB   $7F,$80,$FF,$08
       FCB   $80,$7F,$FF,$08
       FCB   $7F,$01,$80,$2A
       FCB   $80,$01,$81,$08
       FCB   $40,$C0,$00,$05
       FCB   $40,$C1,$01,$01
       FCB   $FF,$FF,$FE,$29
       FCB   $81,$80,$01,$03
LADD   FCC   'ADD'
	FCB	13,10,EOT

**************************************************
* Test ADC
**************************************************
TADC   LDU   #LADC
       LDX   #TBADC
TADC1  LDA   ,X+
       TFR   CC,B
       ANDB  ,X+
       ORB   ,X+
       TFR   B,CC
       ADCA  ,X+
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$2F   ; Only check Bits HNZVC
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LADC
       BNE   TADC1
       RTS

TBADC  FCB   $00,$FE,$00,$00,$00,$04
       FCB   $00,$FE,$00,$01,$01,$00
       FCB   $40,$FE,$00,$40,$80,$0A
       FCB   $00,$FE,$00,$80,$80,$08
       FCB   $80,$FE,$00,$80,$00,$07
       FCB   $40,$FE,$00,$80,$C0,$08
       FCB   $7F,$FE,$00,$7F,$FE,$2A
       FCB   $7F,$FE,$00,$80,$FF,$08
       FCB   $80,$FE,$00,$7F,$FF,$08
       FCB   $7F,$FE,$00,$01,$80,$2A
       FCB   $80,$FE,$00,$01,$81,$08
       FCB   $40,$FE,$00,$C0,$00,$05
       FCB   $40,$FE,$00,$C1,$01,$01
       FCB   $FF,$FE,$00,$FF,$FE,$29
       FCB   $81,$FE,$00,$80,$01,$03
       FCB   $00,$FF,$01,$00,$01,$00
       FCB   $00,$FF,$01,$01,$02,$00
       FCB   $40,$FF,$01,$40,$81,$0A
       FCB   $00,$FF,$01,$80,$81,$08
       FCB   $80,$FF,$01,$80,$01,$03
       FCB   $40,$FF,$01,$80,$C1,$08
       FCB   $7F,$FF,$01,$7F,$FF,$2A
       FCB   $7F,$FF,$01,$80,$00,$25
       FCB   $80,$FF,$01,$7F,$00,$25
       FCB   $7F,$FF,$01,$01,$81,$2A
       FCB   $80,$FF,$01,$01,$82,$08
       FCB   $40,$FF,$01,$C0,$01,$01
       FCB   $40,$FF,$01,$C1,$02,$01
       FCB   $FF,$FF,$01,$FF,$FF,$29
       FCB   $81,$FF,$01,$80,$02,$03
LADC   FCC   'ADC'
	FCB	13,10,EOT

**************************************************
* Test CMP
**************************************************
TCMP   LDU   #LCMP
       LDX   #TBSUB
TCMP1  LDB   ,X+
       CMPB  ,X+
       TFR   CC,A
       LEAX  1,X    ; Skip result Byte
       ANDA  #$0F   ; Only check Bits NZVC
       CMPA  ,X+
       LBNE  OUTERR
       CMPX  #LSUB
       BNE   TCMP1
       RTS

LCMP   FCC   'CMP'
	FCB	13,10,EOT

**************************************************
* Test SUB
**************************************************
TSUB   LDU   #LSUB
       LDX   #TBSUB
TSUB1  LDB   ,X+
       SUBB  ,X+
       TFR   CC,A
       CMPB  ,X+
       LBNE  OUTERR
       ANDA  #$0F   ; Only check Bits NZVC
       CMPA  ,X+
       LBNE  OUTERR
       CMPX  #LSUB
       BNE   TSUB1
       RTS

TBSUB  FCB   $00,$00,$00,$04
       FCB   $00,$01,$FF,$09
       FCB   $00,$7F,$81,$09
       FCB   $00,$80,$80,$0B
       FCB   $00,$FF,$01,$01
       FCB   $01,$00,$01,$00
       FCB   $01,$7F,$82,$09
       FCB   $40,$41,$FF,$09
       FCB   $40,$80,$C0,$0B
       FCB   $40,$C0,$80,$0B
       FCB   $40,$C1,$7F,$01
       FCB   $7F,$00,$7F,$00
       FCB   $7F,$01,$7E,$00
       FCB   $7F,$7E,$01,$00
       FCB   $7F,$7F,$00,$04
       FCB   $7F,$80,$FF,$0B
       FCB   $7F,$FF,$80,$0B
       FCB   $80,$01,$7F,$02
       FCB   $80,$40,$40,$02
       FCB   $80,$7F,$01,$02
       FCB   $80,$80,$00,$04
       FCB   $80,$81,$FF,$09
       FCB   $81,$80,$01,$00
       FCB   $FF,$FF,$00,$04
       FCB   $FF,$FE,$01,$00
       FCB   $FF,$00,$FF,$08
LSUB   FCC   'SUB'
	FCB	13,10,EOT

**************************************************
* Test SBC
**************************************************
TSBC   LDU   #LSBC
       LDX   #TBSBC
TSBC1  LDA   ,X+
       TFR   CC,B
       ANDB  ,X+
       ORB   ,X+
       TFR   B,CC
       SBCA  ,X+
       TFR   CC,B
       CMPA  ,X+
       LBNE  OUTERR
       ANDB  #$0F   ; Only check Bits NZVC
       CMPB  ,X+
       LBNE  OUTERR
       CMPX  #LSBC
       BNE   TSBC1
       RTS

TBSBC  FCB   $00,$FE,$00,$00,$00,$04
       FCB   $00,$FE,$00,$01,$FF,$09
       FCB   $00,$FE,$00,$7F,$81,$09
       FCB   $00,$FE,$00,$80,$80,$0B
       FCB   $00,$FE,$00,$FF,$01,$01
       FCB   $01,$FE,$00,$00,$01,$00
       FCB   $01,$FE,$00,$7F,$82,$09
       FCB   $40,$FE,$00,$41,$FF,$09
       FCB   $40,$FE,$00,$80,$C0,$0B
       FCB   $40,$FE,$00,$C0,$80,$0B
       FCB   $40,$FE,$00,$C1,$7F,$01
       FCB   $7F,$FE,$00,$00,$7F,$00
       FCB   $7F,$FE,$00,$01,$7E,$00
       FCB   $7F,$FE,$00,$7E,$01,$00
       FCB   $7F,$FE,$00,$7F,$00,$04
       FCB   $7F,$FE,$00,$80,$FF,$0B
       FCB   $7F,$FE,$00,$FF,$80,$0B
       FCB   $80,$FE,$00,$01,$7F,$02
       FCB   $80,$FE,$00,$40,$40,$02
       FCB   $80,$FE,$00,$7F,$01,$02
       FCB   $80,$FE,$00,$80,$00,$04
       FCB   $80,$FE,$00,$81,$FF,$09
       FCB   $81,$FE,$00,$80,$01,$00
       FCB   $FF,$FE,$00,$FF,$00,$04
       FCB   $FF,$FE,$00,$FE,$01,$00
       FCB   $FF,$FE,$00,$00,$FF,$08
       FCB   $00,$FF,$01,$00,$FF,$09
       FCB   $00,$FF,$01,$01,$FE,$09
       FCB   $00,$FF,$01,$7F,$80,$09
       FCB   $00,$FF,$01,$80,$7F,$01
       FCB   $00,$FF,$01,$FF,$00,$05
       FCB   $01,$FF,$01,$00,$00,$04
       FCB   $01,$FF,$01,$7F,$81,$09
       FCB   $40,$FF,$01,$41,$FE,$09
       FCB   $40,$FF,$01,$80,$BF,$0B
       FCB   $40,$FF,$01,$C0,$7F,$01
       FCB   $40,$FF,$01,$C1,$7E,$01
       FCB   $7F,$FF,$01,$00,$7E,$00
       FCB   $7F,$FF,$01,$01,$7D,$00
       FCB   $7F,$FF,$01,$7E,$00,$04
       FCB   $7F,$FF,$01,$7F,$FF,$09
       FCB   $7F,$FF,$01,$80,$FE,$0B
       FCB   $7F,$FF,$01,$FF,$7F,$01
       FCB   $80,$FF,$01,$01,$7E,$02
       FCB   $80,$FF,$01,$40,$3F,$02
       FCB   $80,$FF,$01,$7F,$00,$06
       FCB   $80,$FF,$01,$80,$FF,$09
       FCB   $80,$FF,$01,$81,$FE,$09
       FCB   $81,$FF,$01,$80,$00,$04
       FCB   $FF,$FF,$01,$FF,$FF,$09
       FCB   $FF,$FF,$01,$FE,$00,$04
       FCB   $FF,$FF,$01,$00,$FE,$08
LSBC   FCC   'SBC'
	FCB	13,10,EOT

**************************************************
* Test SUBD
**************************************************
TSUBD  LDU   #LSUBD
       LDX   #TBSUBD
TSUBD1 LDD   ,X++
       SUBD  ,X++
       TFR   CC,DP
       CMPD  ,X++
       LBNE  OUTERR
       TFR   DP,A
       ANDA  #$0F   ; Only check Bits NZVC
       CMPA  ,X+
       LBNE  OUTERR
       CMPX  #LSUBD
       BNE   TSUBD1
       RTS

TBSUBD FDB   $0000,$0000,$0000
       FCB   $04
       FDB   $0000,$0001,$FFFF
       FCB   $09
       FDB   $0000,$7FFF,$8001
       FCB   $09
       FDB   $0000,$8000,$8000
       FCB   $0B
       FDB   $0000,$FFFF,$0001
       FCB   $01
       FDB   $0001,$0000,$0001
       FCB   $00
       FDB   $0001,$7FFF,$8002
       FCB   $09
       FDB   $4000,$4001,$FFFF
       FCB   $09
       FDB   $4000,$8000,$C000
       FCB   $0B
       FDB   $4000,$C000,$8000
       FCB   $0B
       FDB   $4000,$C001,$7FFF
       FCB   $01
       FDB   $7FFF,$0000,$7FFF
       FCB   $00
       FDB   $7FFF,$0001,$7FFE
       FCB   $00
       FDB   $7FFF,$7FFE,$0001
       FCB   $00
       FDB   $7FFF,$7FFF,$0000
       FCB   $04
       FDB   $7FFF,$8000,$FFFF
       FCB   $0B
       FDB   $7FFF,$FFFF,$8000
       FCB   $0B
       FDB   $8000,$0001,$7FFF
       FCB   $02
       FDB   $8000,$4000,$4000
       FCB   $02
       FDB   $8000,$7FFF,$0001
       FCB   $02
       FDB   $8000,$8000,$0000
       FCB   $04
       FDB   $8000,$8001,$FFFF
       FCB   $09
       FDB   $8001,$8000,$0001
       FCB   $00
       FDB   $FFFF,$FFFF,$0000
       FCB   $04
       FDB   $FFFF,$FFFE,$0001
       FCB   $00
       FDB   $FFFF,$0000,$FFFF
       FCB   $08
LSUBD  FCC   'SUBD'
	FCB	13,10,EOT

**************************************************
* Test CMPD
**************************************************
TCMPD  LDU   #LCMPD
       LDX   #TBSUBD
TCMPD1 LDD   ,X++
       SUBD  ,X++
       TFR   CC,DP
       LEAX  2,X    ; Skip result Word
       TFR   DP,A
       ANDA  #$0F   ; Only check Bits NZVC
       CMPA  ,X+
       LBNE  OUTERR
       CMPX  #LSUBD
       BNE   TCMPD1
       RTS

LCMPD  FCC   'CMPD'
	FCB	13,10,EOT

**************************************************
* Test TFR
**************************************************
TTFR   LDX   #TBTFR
TTFR1  PSHS  X
       LDY   ,X
       JSR   ,Y
       PULS  X
       BNE   FTFR
       LEAX  2,X
       CMPX  #LTFR
       BNE   TTFR1
       RTS
FTFR   LDU   #LTFR
       LBRA  OUTERR

CLRREG CLRA
       CLRB
       LDX   #0
       LDY   #0
       LDU   #0
       TFR   A,DP
       TFR   A,CC
       RTS
TBTFR1 BSR   CLRREG
       LDD   #$5678
       TFR   D,X
       CMPX  #$5678  
       RTS
TBTFR2 BSR   CLRREG
       LDX   #$1234
       TFR   X,Y
       CMPY  #$1234  
       RTS
TBTFR3 BSR   CLRREG
       LDY   #$5500
       TFR   Y,U
       CMPU  #$5500  
       RTS
TBTFR4 BSR   CLRREG
       LDU   #$FEFF
       TFR   U,D
       CMPD  #$FEFF  
       RTS
TBTFR5 BSR   CLRREG
       LDA   #$55
       TFR   A,B
       CMPB  #$55  
       RTS
TBTFR6 BSR   CLRREG
       LDB   #$AA
       TFR   B,DP
       PSHS  DP
       PULS  A
       CMPA  #$AA
       RTS
TBTFR7 BSR   CLRREG
       LDA   #$03
       PSHS  A
       CLRA
       PULS  DP
       TFR   DP,CC
       PSHS  CC
       PULS  A
       CMPA  #$03
       RTS
TBTFR8 BSR   CLRREG
       LDA   #$01
       PSHS  A
       PULS  CC
       TFR   CC,A
       CMPA  #$01
       RTS

TBTFR  FDB   TBTFR1
       FDB   TBTFR2
       FDB   TBTFR3
       FDB   TBTFR4
       FDB   TBTFR5
       FDB   TBTFR6
       FDB   TBTFR7
       FDB   TBTFR8
LTFR   FCC   'TFR'
	FCB	13,10,EOT

**************************************************
* Test EXG
**************************************************
TEXG   LDX   #TBEXG
TEXG1  PSHS  X
       LDY   ,X
       JSR   ,Y
       PULS  X
       BNE   FEXG
       LEAX  2,X
       CMPX  #LEXG
       BNE   TEXG1
       RTS
FEXG   LDU   #LEXG
       LBRA  OUTERR

TBEXG1 LBSR  CLRREG
       LDD   #$5678
       LDX   #$A55A
       EXG   D,X
       CMPX  #$5678
       BNE   TBEXGE
       CMPD  #$A55A  
TBEXGE RTS
TBEXG2 LBSR  CLRREG
       LDX   #$1234
       LDY   #$0815
       EXG   X,Y
       CMPY  #$1234
       BNE   TBEXGE
       CMPX  #$0815
       RTS
TBEXG3 LBSR  CLRREG
       LDY   #$5500
       LDU   #$4711
       EXG   Y,U
       CMPU  #$5500 
       BNE   TBEXGE 
       CMPY  #$4711  
       RTS
TBEXG4 LBSR  CLRREG
       LDU   #$FEFF
       LDD   #$9999
       EXG   U,D
       CMPD  #$FEFF
       BNE   TBEXGE
       CMPU  #$9999  
       RTS
TBEXG5 LBSR  CLRREG
       LDA   #$55
       LDB   #$AA
       EXG   A,B
       CMPB  #$55  
       BNE   TBEXGE
       CMPA  #$AA
       RTS
TBEXG6 LBSR  CLRREG
       LDB   #$AA
       LDA   #$44
       PSHS  A
       CLRA
       PULS  DP
       EXG   B,DP
       PSHS  DP
       PULS  A
       CMPA  #$AA
       BNE   TBEXGE
       CMPB  #$44
       RTS
TBEXG7 LBSR  CLRREG
       LDA   #$03
       PSHS  A
       CLRA
       PULS  DP
       LDA   #$30
       PSHS  A
       CLRA
       PULS  CC
       EXG   DP,CC
       PSHS  CC
       PULS  A
       PSHS  DP
       PULS  B
       CMPA  #$03
       LBNE  TBEXGE
       CMPB  #$30
       RTS
TBEXG8 LBSR  CLRREG
       LDA   #$11
       LDB   #$99
       PSHS  B
       CLRB
       PULS  CC
       EXG   CC,A
       PSHS  CC
       PULS  B
       CMPA  #$99
       LBNE  TBEXGE
       CMPB  #$11
       RTS

TBEXG  FDB   TBEXG1
       FDB   TBEXG2
       FDB   TBEXG3
       FDB   TBEXG4
       FDB   TBEXG5
       FDB   TBEXG6
       FDB   TBEXG7
       FDB   TBEXG8
LEXG   FCC   'EXG'
	FCB	13,10,EOT


**************************************************
* Test Index Addressing modes
**************************************************
CLRMEM PSHS  X
       LDX   #EXTREG-128
       CLRA
CLRM1  CLR   ,X+
       INCA
       BNE   CLRM1
       PULS  X,PC

TAD1   LDU   #LAD1
       LDX   #EXTREG
* LDA ,X
       BSR   CLRMEM
       LDA   #$55
       STA   EXTREG
       LDA   ,X
       CMPA  #$55
       LBNE  OUTERR
* LDA 1,X
       BSR   CLRMEM
       LDA   #$12
       STA   EXTREG+1
       LDA   1,X
       CMPA  #$12
       LBNE  OUTERR
* LDA 127,X
       BSR   CLRMEM
       LDA   #$44
       STA   EXTREG+127
       LDA   127,X
       CMPA  #$44
       LBNE  OUTERR
* LDA -1,X
       BSR   CLRMEM
       LDA   #$34
       STA   EXTREG-1
       LDA   -1,X
       CMPA  #$34
       LBNE  OUTERR
* LDA -128,X
       BSR   CLRMEM
       LDA   #$AA
       STA   EXTREG-128
       LDA   -128,X
       CMPA  #$AA
       LBNE  OUTERR
       RTS

* LDA ,X+
TAD2   LDX   #EXTREG
       LDU   #LAD2
       BSR   CLRMEM
       LDD   #$A55A
       STD   EXTREG
       LDA   ,X+
       CMPA  #$A5
       LBNE  OUTERR
       LDA   ,X+
       CMPA  #$5A
       LBNE  OUTERR
       RTS

* LDD ,X++
TAD3   LDX   #EXTREG
       LDU   #LAD3
       LBSR  CLRMEM
       LDD   #$A55A
       STD   EXTREG
       LDD   #$789A
       STD   EXTREG+2
       LDD   ,X++
       CMPD  #$A55A
       LBNE  OUTERR
       LDD   ,X++
       CMPD  #$789A
       LBNE  OUTERR
       RTS

* LDA ,-X
TAD4   LDX   #EXTREG
       LDU   #LAD4
       LBSR  CLRMEM
       LDD   #$A55A
       STD   EXTREG-2
       LDA   ,-X
       CMPA  #$5A
       LBNE  OUTERR
       LDA   ,-X
       CMPA  #$A5
       LBNE  OUTERR
       RTS

* LDD ,--X
TAD5   LDX   #EXTREG
       LDU   #LAD5
       LBSR  CLRMEM
       LDD   #$A55A
       STD   EXTREG-2
       LDD   #$789A
       STD   EXTREG-4
       LDD   ,--X
       CMPD  #$A55A
       LBNE  OUTERR
       LDD   ,--X
       CMPD  #$789A
       LBNE  OUTERR
       RTS

* LDA A,X
TAD6   LDU   #LAD6
       LDX   #EXTREG
       LBSR  CLRMEM
       LDA   #$12
       STA   EXTREG+1
       LDA   #1
       LDA   A,X
       CMPA  #$12
       LBNE  OUTERR
       LBSR  CLRMEM
       LDA   #$44
       STA   EXTREG+127
       LDA   #127
       LDA   A,X
       CMPA  #$44
       LBNE  OUTERR
       RTS

* LDA B,Y
TAD7   LDU   #LAD7
       LDY   #EXTREG
       LBSR  CLRMEM
       LDA   #$34
       STA   EXTREG-1
       LDB   #-1
       LDA   B,Y
       CMPA  #$34
       LBNE  OUTERR
       LBSR  CLRMEM
       LDA   #$AA
       STA   EXTREG-128
       LDB   #-128
       LDA   B,Y
       CMPA  #$AA
       LBNE  OUTERR
       RTS

* LDD D,X
TAD8   LDU   #LAD8
       LDX   #0
       LBSR  CLRMEM
       LDD   #$1289
       STD   EXTREG
       LDD   #EXTREG
       LDD   D,X
       CMPD  #$1289
       LBNE  OUTERR
       LDX   #$8000
       LBSR  CLRMEM
       LDD   #$2299
       STD   EXTREG
       LDD   #EXTREG-$8000
       LDD   D,X
       CMPD  #$2299
       LBNE  OUTERR
       RTS

* LDD n16,X
TAD9   LDU   #LAD9
       LDX   #0
       LBSR  CLRMEM
       LDD   #$1289
       STD   EXTREG
       LDD   EXTREG,X
       CMPD  #$1289
       LBNE  OUTERR
       LDX   #$8000
       LBSR  CLRMEM
       LDD   #$2299
       STD   EXTREG
       LDD   EXTREG-$8000,X
       CMPD  #$2299
       LBNE  OUTERR
       RTS

* LDA n8,PC
REG81  FCB   0
TADA   LDU   #LADA
       LBSR  CLRMEM
       LDA   #$13
       STA   REG81
       LDA   <REG81,PCR
       CMPA  #$13
       LBNE  OUTERR
       LBSR  CLRMEM
       LDA   #$23
       STA   REG82
       LDA   <REG82,PCR
       CMPA  #$23
       LBNE  OUTERR
       RTS
REG82  FCB   0

* LDA n16,PC
TADB   LDU   #LADB
       LBSR  CLRMEM
       LDD   #$9876
       STD   REG161
       LDD   >REG161,PCR
       CMPD  #$9876
       LBNE  OUTERR
       LBSR  CLRMEM
       LDD   #$5432
       STD   REG162
       LDD   >REG162,PCR
       CMPD  #$5432
       LBNE  OUTERR
       RTS

TADI1  LDU   #LADI1
       LDX   #EXTREG
* LEAY [,X]
       LBSR  CLRMEM
       LDD   #$5522
       STD   EXTREG
       LEAY  [,X]
       CMPY  #$5522
       LBNE  OUTERR
* LEAY [1,X]
       LBSR  CLRMEM
       LDD   #$1234
       STD   EXTREG+1
       LEAY  [1,X]
       CMPY  #$1234
       LBNE  OUTERR
* LEAY [127,X]
       LBSR  CLRMEM
       LDD   #$4444
       STD   EXTREG+127
       LEAY  [127,X]
       CMPY  #$4444
       LBNE  OUTERR
* LEAY [-1,X]
       LBSR  CLRMEM
       LDD   #$3456
       STD   EXTREG-1
       LEAY  [-1,X]
       CMPY  #$3456
       LBNE  OUTERR
* LEAY [-128,X]
       LBSR  CLRMEM
       LDD   #$AAAA
       STD   EXTREG-128
       LEAY  [-128,X]
       CMPY  #$AAAA
       LBNE  OUTERR
       RTS

* LEAY [,X++]
TADI3  LDX   #EXTREG
       LDU   #LADI3
       LBSR  CLRMEM
       LDD   #$A55A
       STD   EXTREG
       LDD   #$5AA5
       STD   EXTREG+2
       LEAY  [,X++]
       CMPY  #$A55A
       LBNE  OUTERR
       LEAY  [,X++]
       CMPY  #$5AA5
       LBNE  OUTERR
       RTS

* LEAY [,--X]
TADI5  LDX   #EXTREG
       LDU   #LADI5
       LBSR  CLRMEM
       LDD   #$A55A
       STD   EXTREG-2
       LDD   #$789A
       STD   EXTREG-4
       LEAY  [,--X]
       CMPY  #$A55A
       LBNE  OUTERR
       LEAY  [,--X]
       CMPY  #$789A
       LBNE  OUTERR
       RTS

* LEAY [A,X]
TADI6  LDU   #LADI6
       LDX   #EXTREG
       LBSR  CLRMEM
       LDD   #$1234
       STD   EXTREG+1
       LDA   #1
       LEAY  [A,X]
       CMPY  #$1234
       LBNE  OUTERR
       LBSR  CLRMEM
       LDD   #$4444
       STD   EXTREG+127
       LDA   #127
       LEAY  [A,X]
       CMPY  #$4444
       LBNE  OUTERR
       RTS

* LEAY [B,X]
TADI7  LDU   #LADI7
       LDX   #EXTREG
       LBSR  CLRMEM
       LDD   #$3456
       STD   EXTREG-1
       LDB   #-1
       LEAY  [B,X]
       CMPY  #$3456
       LBNE  OUTERR
       LBSR  CLRMEM
       LDD   #$AAAA
       STD   EXTREG-128
       LDB   #-128
       LEAY  [B,X]
       CMPY  #$AAAA
       LBNE  OUTERR
       RTS

* LEAY [D,X]
TADI8  LDU   #LADI8
       LDX   #0
       LBSR  CLRMEM
       LDD   #$1289
       STD   EXTREG
       LDD   #EXTREG
       LEAY  [D,X]
       CMPY  #$1289
       LBNE  OUTERR
       LDX   #$8000
       LBSR  CLRMEM
       LDD   #$2299
       STD   EXTREG
       LDD   #EXTREG-$8000
       LEAY  [D,X]
       CMPY  #$2299
       LBNE  OUTERR
       RTS

* LEAY [n16,X]
TADI9  LDU   #LADI9
       LDX   #0
       LBSR  CLRMEM
       LDD   #$1289
       STD   EXTREG
       LEAY  [EXTREG,X]
       CMPY  #$1289
       LBNE  OUTERR
       LDX   #$8000
       LBSR  CLRMEM
       LDD   #$2299
       STD   EXTREG
       LEAY  [EXTREG-$8000,X]
       CMPY  #$2299
       LBNE  OUTERR
       RTS

* LEAY [n8,PCR]
REGI81 FDB   0
TADIA  LDU   #LADIA
       LBSR  CLRMEM
       LDD   #$1357
       STD   REGI81
       LEAY  [REGI81,PCR]
       CMPY  #$1357
       LBNE  OUTERR
       LBSR  CLRMEM
       LDD   #$2345
       STD   REGI82
       LEAY  [REGI82,PCR]
       CMPY  #$2345
       LBNE  OUTERR
       RTS
REGI82 FDB   0

* LEAY [n16,PC]
TADIB  LDU   #LADIB
       LBSR  CLRMEM
       LDD   #$9876
       STD   REG161
       LEAY  [REG161,PCR]
       CMPY  #$9876
       LBNE  OUTERR
       LBSR  CLRMEM
       LDD   #$5432
       STD   REG162
       LEAY  [REG162,PCR]
       CMPY  #$5432
       LBNE  OUTERR
       RTS

* LEAY [addr]
*TADIC  LDU   #LADIC
*       LBSR  CLRMEM
*       LDD   #$9876
*       STD   REG161
*       LEAY  [REG161]
*       CMPY  #$9876
*       LBNE  OUTERR
*       LBSR  CLRMEM
*       LDD   #$5432
*       STD   REG162
*       LEAY  [REG162]
*       CMPY  #$5432
*       LBNE  OUTERR
*       RTS

LAD1   FCC   'LDA n8,X'
	FCB	13,10,EOT
LAD2   FCC   'LDA ,X+'
	FCB	13,10,EOT
LAD3   FCC   'LDD ,X++'
	FCB	13,10,EOT
LAD4   FCC   'LDA ,-X'
	FCB	13,10,EOT
LAD5   FCC   'LDD ,--X'
	FCB	13,10,EOT
LAD6   FCC   'LDA A,X'
	FCB	13,10,EOT
LAD7   FCC   'LDA B,Y'
	FCB	13,10,EOT
LAD8   FCC   'LDD D,X'
	FCB	13,10,EOT
LAD9   FCC   'LDD n16,X'
	FCB	13,10,EOT
LADA   FCC   'LDA n8,PC'
	FCB	13,10,EOT
LADB   FCC   'LDD n16,PC'
	FCB	13,10,EOT
LADI1  FCC   'LEAX [n8,X]'
	FCB	13,10,EOT
LADI3  FCC   'LEAX [,X++]'
	FCB	13,10,EOT
LADI5  FCC   'LEAX [,--X]'
	FCB	13,10,EOT
LADI6  FCC   'LEAX [A,X]'
	FCB	13,10,EOT
LADI7  FCC   'LEAX [B,Y]'
	FCB	13,10,EOT
LADI8  FCC   'LEAX [D,X]'
	FCB	13,10,EOT
LADI9  FCC   'LEAX [n16,X]'
	FCB	13,10,EOT
LADIA  FCC   'LEAX [n8,PCR]'
	FCB	13,10,EOT
LADIB  FCC   'LEAX [n16,PCR]'
	FCB	13,10,EOT
LADIC  FCC   'LEAX [addr]'
	FCB	13,10,EOT

**************************************************
* Print error message for a failed Test
* Parameters:
* U: Pointer to Mnemonic
**************************************************
OUTERR LDA   #1
       STA   ERRFLG
       LDX   #ERRM1
       JSR   PSTRNG
*       SWI
*       FCB   PDATA1
       TFR   U,X
       JSR   PSTRNG
*       SWI
*       FCB   PDATA1
       RTS

**************************************************
* Print a success message if all test have
* been successfull
**************************************************
OUTSUC LDA   ERRFLG
       BNE   OUTS9
       LDX   #SUCCES
       JSR   PSTRNG
*       SWI
*       FCB   PDATA1
OUTS9  RTS

**************************************************
* Misc data:
**************************************************

REG162 FDB   0
ERRFLG FCB   0

ERRM1  FCC   'Failed Test: '
	FCB	EOT
SUCCES 
	FCB	13
	FCB	10
	FCB	13
	FCB	10
	FCC   'All Tests succeded'
	FCB	13
	FCB	10
	FCB	13
	FCB	10
	FCB	EOT

       END   BEGIN
