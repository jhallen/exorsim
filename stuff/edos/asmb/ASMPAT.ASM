* Patches to ASM from the Astral-2000 to run on the Motorola Exorciser
*
* 011E : 7E EE EA	"~  "		jmp	crLfStr		$F024
* 0121 : 7E EE EC	"~  "		jmp	dispStr		$F027
* 0124 : 7E EE F9	"~  "		jmp	crLf		$F021
* 0130 : 7E E8 21	"~ !"		jmp	MONITOR		$F000
* 0157 : BD E8 00	"   "		jsr	charIn		$F015
* 0177 : BD E8 03	"   "		jsr	charOut		$F018
*
* 01D3 : BD F1 AD	"   "		jsr	diskWrt		$E80c
* 0282 : BD F1 35	"  5"		jsr	diskRd		$E809
* 094F : BD F0 3B	"  ;"		jsr	RESTORE		$E82D
* 0956 : 7E F0 23	"~ #"		jmp	UPDATE		$E80F
*
* 0020 : FE E0 14	"   "		ldx	XE014		$FF00 (max RAM stored here)
* 0946 : 8E E0 B8	"   "		lds	#$E0B8  	$FF8A top of stack
*
* 13CB : 2A 1D		"* "		bpl	L13EA		bug, should be bcc $24

PROMLOC	equ	$E800			EDOS Resident PROM
EXBUG	equ	$F000			I/O jump vector table in EXBUG

* These are offsets from PROMLOC

DISKRD	equ	$09			offset to disk read byte
DISKWRT	equ	$0C			offset to disk write byte
UPDATE	equ	$0F			offset to UPDATE vector
RESTORE	equ	$2D			offset to restore file pointers

* These are offsets into the jump table in EXBUG

MONITOR	equ	$00			offset to MONITOR jump vector
CONIN	equ	$15			offset to CONIN jump vector
CONOUT	equ	$18			offset to CONOUT jump vector
CRLF	equ	$21			offset to send CR/LF
CRLFSTR	equ	$24			offset to display string with leader CR/LF
DISPSTR	equ	$27			offset to display string

* Patches

	org	$011F
	fdb	EXBUG+CRLFSTR
	org	$0122
	fdb	EXBUG+DISPSTR
	org	$0125
	fdb	EXBUG+CRLF
	org	$0131
	fdb	EXBUG+MONITOR
	org	$0158
	fdb	EXBUG+CONIN
	org	$0178
	fdb	EXBUG+CONOUT

	org	$01D4
	fdb	PROMLOC+DISKWRT
	org	$0283
	fdb	PROMLOC+DISKRD
	org	$0950
	fdb	PROMLOC+RESTORE
	org	$0957
	fdb	PROMLOC+UPDATE

	org	$0021
	fdb	$FF00			EXBUG keeps top of RAM in $FF00
	org	$0947
	fdb	$FF8A			EXBUG top of stack

	org	$13cb
	fcb	$24			BCC, not BPL

	end

