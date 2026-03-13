; Auto-generated 6800 disassembly from fort S19 image
; Best-effort decode with labels, raw-byte comments, and ASCII data annotations.
; Symbol names below are inferred from usage and should be treated as working hypotheses.
; Handler aliases are intentionally tentative; they describe observed roles, not confirmed source names.

; Inferred workspace / state variables
TABLE_PTR_LO     EQU     $0029    ; Scratch pointer used by the generic table walkers.
TABLE_PTR_HI     EQU     $002A    ; Scratch pointer high byte used by the generic table walkers.
TOKEN_INDEX      EQU     $003E    ; General-purpose token / statement / operator index register.
TOKEN_CLASS      EQU     $003F    ; Current token sub-class or precedence nibble.
ERROR_CODE       EQU     $0040    ; Current diagnostic / formatter code.
HASH_ACCUM       EQU     $0041    ; Identifier hash accumulator / loop counter.
ATTR_BITS        EQU     $0042    ; Attribute bits for the current symbol or descriptor.
EXPR_PTR         EQU     $0043    ; Expression descriptor pointer scratch.
EXPR_PTR_HI      EQU     $0044    ; Expression descriptor pointer scratch high byte.
SYM_SCAN_PTR     EQU     $0049    ; Current symbol-chain scan pointer.
SYM_MATCH_PTR    EQU     $004B    ; Matched symbol/member pointer while comparing names.
KEYWORD_CODE     EQU     $004D    ; Current statement keyword code.
IDENT_LEN        EQU     $004E    ; Current identifier/text length.
STMT_STATE       EQU     $004F    ; Statement-classification state / keyword phase.
VALUE_LO         EQU     $0050    ; Expression value/address low byte.
VALUE_HI         EQU     $0051    ; Expression value/address high byte.
BASE_TEXT_PTR    EQU     $0052    ; Base pointer for generated text/object fragments.
SYM_INSERT_MODE  EQU     $0054    ; Symbol insertion/update mode flag.
SYM_PTR          EQU     $0055    ; Pointer to the current symbol-table entry.
DEFAULT_ATTR     EQU     $0057    ; Default descriptor attribute byte.
EXPR_DEPTH       EQU     $0058    ; Expression descriptor stack depth.
EXPR_STACK_PTR   EQU     $0059    ; Top of the temporary 3-byte expression descriptor stack.
OPERAND_LO       EQU     $005B    ; Temporary operand/address low byte.
OPERAND_HI       EQU     $005C    ; Temporary operand/address high byte.
OPERAND_TAG      EQU     $005D    ; Temporary operand/operator tag.
PREC_SCAN_INDEX  EQU     $005E    ; Expression precedence scan index.
MUL_ACC_LO       EQU     $005F    ; Low byte of multiply/shift accumulator.
MUL_ACC_HI       EQU     $0060    ; High byte of multiply/shift accumulator.
CODE_PTR         EQU     $0061    ; Current code-generation write pointer.
CODE_PTR_HI      EQU     $0062    ; Current code-generation write pointer high byte.
TEMP_BASE_PTR    EQU     $0063    ; Base pointer for current temporary/code block.
FIXUP_PTR        EQU     $0065    ; Pointer to current fixup/relocation chain entry.
TMP_COUNTER      EQU     $0067    ; Small per-line counter used by code generation.
PAGE_COUNT       EQU     $0068    ; Listing page / error-page counter.
LIST_REC_LEN     EQU     $0963    ; Length of the current printable/listing record.
LIST_REC_PTR     EQU     $0961    ; Pointer to the current printable/listing record.
LIST_HEX_BUF     EQU     $0964    ; Scratch ASCII buffer used for listing/object text.
OBJ_QUEUE_PTR    EQU     $0977    ; Pointer to deferred object-output descriptors.
OBJ_QUEUE_BASE   EQU     $096E    ; Deferred object-output descriptor storage.
PREC_STACK_PTR   EQU     $1EC7    ; Pointer to the operator-precedence stack.
IOCB_PTR         EQU     $0638    ; Pointer to the active monitor I/O control block.
IOCB_DATA        EQU     $063A    ; Current monitor I/O data/address field.
EMIT_OPCODE      EQU     $063C    ; Current output fragment opcode/major type.
EMIT_FLAGS       EQU     $063D    ; Current output fragment flags.
EMIT_TARGET      EQU     $0641    ; Target address/pointer for the fragment being emitted.
EMIT_WIDTH       EQU     $0646    ; Encoded width/type for the fragment being emitted.
REC_BYTES_LEFT   EQU     $064A    ; Remaining payload bytes before the current record must flush.
REC_OPEN         EQU     $064B    ; Nonzero when an output record is open.
REC_ADDR         EQU     $064C    ; Current output record base address.
REC_DATA_PTR     EQU     $064E    ; Current write pointer inside the output record buffer.
RELOC_ROOM       EQU     $0650    ; Remaining relocation room/count for the current record.
RELOC_PENDING    EQU     $0651    ; Flush-needed flag for relocation state.
RELOC_ACTIVE     EQU     $0652    ; Nonzero when relocation collection is active.
RELOC_DIRTY      EQU     $0653    ; Relocation metadata has been added to the current record.
RELOC_BITS_OPEN  EQU     $0654    ; A relocation bitmap byte is currently being packed.
RELOC_BASE       EQU     $0655    ; Base address used when computing relocation deltas.
RELOC_BASE_HI    EQU     $0656    ; High byte of relocation base.
RELOC_CLASS      EQU     $0657    ; Packed relocation-class bits for the current record.
RELOC_REC_PTR    EQU     $0658    ; Pointer to the current relocation descriptor record.
RELOC_SLOT_A     EQU     $065A    ; First relocation slot/type used in the current record.
RELOC_SLOT_B     EQU     $065B    ; Second relocation slot/type used in the current record.
RELOC_SLOT_C     EQU     $065C    ; Third relocation slot/type used in the current record.
RELOC_COUNT      EQU     $065D    ; Count of relocation descriptor bytes emitted.
RELOC_BITCOUNT   EQU     $065E    ; Number of packed two-bit relocation classes in the current byte.
RELOC_PAYLOAD_PTR EQU     $065F    ; Pointer to relocation payload bytes.
RELOC_BIT_PTR    EQU     $0661    ; Pointer to the relocation bitmap stream.
RELOC_BIT_PHASE  EQU     $0663    ; Position inside the current relocation bitmap byte.
REC_HEADER       EQU     $0664    ; Beginning of the object record header buffer.
REC_HEADER_LEN   EQU     $0665    ; Object record header length/count byte.
REC_HEADER_ARG0  EQU     $0666    ; Object record header argument / subtype.
REC_HEADER_ARG1  EQU     $0667    ; Object record header argument / subtype.
REC_ADDR_SAVE    EQU     $0668    ; Saved record address used when building prologue records.
REC_BUFFER       EQU     $066A    ; Object record payload buffer.
PROLOGUE_TYPE    EQU     $0664    ; Record-type byte while constructing object-module prologue records.
PROLOGUE_LEN     EQU     $0665    ; Payload length for the current prologue/header record.
PROLOGUE_FRAG0   EQU     $0666    ; First copied fragment byte in the prologue scratch record.
PROLOGUE_FRAG1   EQU     $0667    ; Second copied fragment/control byte in the prologue scratch record.
PROLOGUE_ADDR0   EQU     $0668    ; Address field used by the 0x34 prologue record.
PROLOGUE_KIND0   EQU     $066A    ; Subtype/opcode byte used in prologue records 0x33/0x34.
PROLOGUE_TAG_OB  EQU     $066C    ; ASCII tag fragment 'OB' written into the 0x32 prologue record.
PROLOGUE_KIND1   EQU     $066D    ; Second subtype/opcode byte used in prologue records 0x33/0x34.
PROLOGUE_ADDR1   EQU     $066E    ; Code/start address field used in prologue records 0x33/0x34.
PROLOGUE_KIND2   EQU     $0670    ; Third subtype byte in the 0x33 prologue record.
PROLOGUE_ADDR2   EQU     $0671    ; Entry/code-base address field in the 0x33 prologue record.
PROLOGUE_KIND3   EQU     $0673    ; Fourth subtype byte in the 0x33 prologue record.
PROLOGUE_TEXT_BASE EQU     $0674    ; Saved BASE_TEXT_PTR written into the 0x33 prologue record.
PROLOGUE_RUNTIME_0 EQU     $0676    ; ASCII '0' prefix in the 0x33 runtime-tag field.
PROLOGUE_RUNTIME_RU EQU     $0677    ; ASCII 'RU' in the 0x33 runtime-tag field.
PROLOGUE_RUNTIME_NSP EQU     $0679    ; ASCII 'N ' in the 0x33 runtime-tag field.
PROLOGUE_RUNTIME_SP2 EQU     $067B    ; Trailing spaces in the 0x33 runtime-tag field.

        ORG     $0020

; Reset entry from the S-record image.
; Control transfers directly into the resident compiler driver at L2575.
L0020:
        JMP     L2575            ; 0020: 7E 25 75

        ORG     $006A
        FCB     $00,$00,$2A ; '..*'

        ORG     $0071
        FCB     $20,$20 ; '  '

; Statement keyword table used during statement classification.
; Entries are 0x04-terminated strings packed back-to-back.
; Tentative keyword map:
;   AGO        likely GOTO / transfer statement family
;   BTO        likely auxiliary TO-token used by DO/GOTO parsing
;   CIF        likely IF statement family
;   DSTOP      likely STOP statement
;   EEND       likely END statement
;   FDO        likely DO statement family
;   GCONTINUE  likely CONTINUE statement
;   HCALL      likely CALL statement
;   ISUBROUTINE likely SUBROUTINE declaration/header
;   JFUNCTION  likely FUNCTION declaration/header
;   KRETURN    likely RETURN statement
;   LREAD      likely READ statement family
;   MPRINT     likely PRINT statement family
;   MWRITE     likely WRITE statement family
;   NDIMENSION likely DIMENSION declaration
;   OCOMMON    likely COMMON declaration
;   PDATA      likely DATA statement
;   QFORMAT    likely FORMAT statement
;   RNOT/SAND/TOR/UEQ/VNE/WLE/XGE/YLT/ZGT are expression/logical operators
;   0ENDFILE/0REWIND are likely file-control statements
        ORG     $01EC
        FCC     "AGO BTO CIF DSTOP EEND FDO GCONTINUE HCALL ISUBROUTINE JFUNCTION KRETURN LREAD MPRINT NDIMENSION OCOMMON PDATA QFORMAT RNOT SAND TOR UEQ VNE WLE XGE YLT ZGT 0ENDFILE 0REWIND MWRITE "
        FCB     $04            ; 01EC: 41 47 4F 20 42 54 4F 20 43 49 46 20 44 53 54 4F 50 20 45 45 4E 44 20 46 44 4F 20 47 43 4F 4E 54 49 4E 55 45 20 48 43 41 4C 4C 20 49 53 55 42 52 4F 55 54 49 4E 45 20 4A 46 55 4E 43 54 49 4F 4E 20 4B 52 45 54 55 52 4E 20 4C 52 45 41 44 20 4D 50 52 49 4E 54 20 4E 44 49 4D 45 4E 53 49 4F 4E 20 4F 43 4F 4D 4D 4F 4E 20 50 44 41 54 41 20 51 46 4F 52 4D 41 54 20 52 4E 4F 54 20 53 41 4E 44 20 54 4F 52 20 55 45 51 20 56 4E 45 20 57 4C 45 20 58 47 45 20 59 4C 54 20 5A 47 54 20 30 45 4E 44 46 49 4C 45 20 30 52 45 57 49 4E 44 20 4D 57 52 49 54 45 20 0x04 terminator

; Per-keyword metadata table indexed from the statement keyword table.
; The compiler uses this to classify statements and route them to handlers.
; Best current read: this is a packed set of parallel descriptor slices rather than one
; flat record per keyword. Early bytes act like family/dispatch selectors; later bytes carry
; flags, secondary template selectors, and small literal parameters consumed by L111C/L121E.
        ORG     $02D1
        FCB     $05,$00,$00,$01,$06,$00,$00,$02
        FCB     $06,$06,$06,$06,$06,$06,$07,$06
        FCB     $04,$04,$04,$04,$04,$04,$04,$04
        FCB     $04,$04,$00,$01,$00,$06,$00,$00
        FCB     $00,$03,$03,$03,$03,$03,$03,$03
        FCB     $03,$03,$03,$03,$03,$03,$03,$03
        FCB     $03,$03,$03,$03,$03,$03,$03,$03
        FCB     $03,$03,$03,$80,$00,$00,$00,$00
        FCB     $00,$00,$00,$00,$00,$00,$40,$00 ; '......@.'
        FCB     $00,$00,$00,$00,$00,$00,$00,$C2
        FCB     $02,$01,$01,$04,$01,$01,$01,$01
        FCB     $01,$C3,$11,$00,$05,$05,$01,$01
        FCB     $04,$05,$05,$C5,$21,$00,$05,$05 ; '....!...'
        FCB     $05,$01,$04,$05,$05,$80,$32,$00 ; '......2.'
        FCB     $05,$05,$05,$05,$02,$05,$05,$00
        FCB     $00,$00,$00,$00,$00,$00,$00,$00
        FCB     $00,$80,$16,$00,$00,$00,$01,$00
        FCB     $02,$00,$04

        ORG     $035E
        FCC     "UNDEFINED SYMBOLS"
        FCB     $04            ; 035E: 55 4E 44 45 46 49 4E 45 44 20 53 59 4D 42 4F 4C 53 0x04 terminator
        FCC     "      ERRORS"
        FCB     $04            ; 0370: 20 20 20 20 20 20 45 52 52 4F 52 53 0x04 terminator

; Primary token / operator descriptor table.
; Many parser helpers index through this block with L2519/L2562.
        ORG     $03A7
        FCB     $12,$02,$FF,$80,$31,$03,$FF,$80 ; '....1...'
        FCB     $1D,$04,$FF,$D8,$47,$05,$FF,$00 ; '....G...'
        FCB     $74,$06,$FF,$80,$7D,$07,$FF,$80 ; 't...}...'
        FCB     $26,$08,$FF,$80,$5D,$09,$FF,$80 ; '&...]...'
        FCB     $7C,$0A,$FF,$9D,$14,$0B,$FF,$80 ; '|.......'
        FCB     $44,$0C,$FF,$12,$56,$0D,$FF,$AC ; 'D...V...'
        FCB     $4B,$0E,$FF,$29,$3E,$0F,$FF,$E7 ; 'K..)>...'
        FCB     $3B,$10,$FF,$80,$2F,$11,$FF,$80 ; ';.../...'
        FCB     $45,$00,$FF,$1B,$24,$00,$13,$1A ; 'E...$...'
        FCB     $05,$02,$02,$11,$41,$00,$15,$00 ; '....A...'
        FCB     $42,$00,$16,$00,$05,$17,$FF,$21 ; 'B......!'
        FCB     $28,$00,$18,$22,$05,$00,$19,$23 ; '(.."...#'
        FCB     $2C,$1A,$18,$00,$29,$00,$1B,$00 ; ',...)...'
        FCB     $2C,$00,$1C,$00,$88,$00,$FF,$A4 ; ',.......'
        FCB     $46,$00,$1E,$00,$05,$00,$1F,$53 ; 'F......S'
        FCB     $88,$00,$20,$D4,$3D,$00,$21,$00 ; '.. .=.!.'
        FCB     $88,$00,$22,$D5,$2C,$00,$23,$00 ; '..".,.#.'
        FCB     $88,$00,$24,$D6,$2C,$FF,$25,$00 ; '..$.,.%.'
        FCB     $88,$00,$FF,$D7,$50,$00,$27,$00 ; '....P.'.'
        FCB     $03,$2D,$28,$17,$2F,$00,$29,$00 ; '.-(./.).'
        FCB     $95,$2E,$2A,$98,$2C,$2B,$29,$00 ; '..*.,+).'
        FCB     $2F,$00,$2C,$00,$2C,$FF,$27,$00 ; '/.,.,.'.'
        FCB     $04,$00,$28,$17,$2D,$00,$29,$37 ; '..(.-.)7'
        FCB     $30,$00,$30,$1E,$88,$00,$FF,$9F ; '0.0.....'
        FCB     $43,$00,$32,$00,$28,$00,$33,$4C ; 'C.2.(.3L'
        FCB     $98,$00,$34,$C7,$29,$00,$35,$51 ; '..4.).5Q'
        FCB     $05
        FCC     ":6R,"
        FCB     $04            ; 0478: 3A 36 52 2C 0x04 terminator
        FCB     $00,$37,$00,$05,$00,$38,$52,$2C ; '.7...8R,'
        FCB     $00,$39,$00,$05,$00,$FF,$52,$0A ; '.9....R.'
        FCB     $00,$FF,$DB,$88,$00,$3C,$CA,$3D ; '.....<.='
        FCB     $00,$3D,$45,$98,$00,$FF,$C7 ; '.=E....'
        FCC     "L?EaM"
        FCB     $04            ; 049B: 4C 3F 45 61 4D 0x04 terminator
        FCB     $00,$45,$62,$05,$00,$41,$63,$2C ; '.Eb..Ac,'
        FCB     $FF
        FCC     "BjJCD"
        FCB     $04            ; 04A9: 42 6A 4A 43 44 0x04 terminator
        FCB     $80,$88,$FF,$44,$E4,$2C,$FF,$42 ; '...D.,.B'
        FCB     $00
        FCC     "(@Fj"
        FCB     $04            ; 04B7: 28 40 46 6A 0x04 terminator
        FCB     $88,$00,$47,$E9,$2C,$00,$48,$00 ; '..G.,.H.'
        FCB     $05,$00,$49,$63,$29,$00,$42,$00 ; '..Ic).B.'
        FCB     $28,$00 ; '(.'
        FCC     "KeJML"
        FCB     $04            ; 04CD: 4B 65 4A 4D 4C 0x04 terminator
        FCB     $80,$2C,$00,$4B,$00,$88,$00,$4E ; '.,.K...N'
        FCB     $80
        FCC     ",OKd="
        FCB     $04            ; 04DB: 2C 4F 4B 64 3D 0x04 terminator
        FCB     $00,$50,$54,$88,$00,$51,$E6,$2C ; '.PT..Q.,'
        FCB     $00,$52,$00,$88,$00,$53,$D6,$2C ; '.R...S.,'
        FCB     $55,$54,$00,$88,$00,$55,$D7,$29 ; 'UT...U.)'
        FCB     $00,$FF,$68,$48,$00,$57,$00,$03 ; '..hH.W..'
        FCB     $58,$FF,$2A,$07,$00,$59,$2A,$28 ; 'X.*..Y*('
        FCB     $00,$5A,$6A,$86,$00,$5B,$AB,$2C ; '.Zj..[.,'
        FCB     $5C,$5A,$00,$29,$00,$FF,$00,$51 ; '\Z.)...Q'
        FCB     $00,$5E,$71,$28,$00,$5F,$00,$61 ; '.^q(._.a'
        FCB     $00,$60,$80,$29,$00,$FF,$7A,$05 ; '.`.)..z.'
        FCC     "bbr/casXdesPees,fa"
        FCB     $04            ; 0528: 62 62 72 2F 63 61 73 58 64 65 73 50 65 65 73 2C 66 61 0x04 terminator
        FCB     $00
        FCC     "(igta"
        FCB     $04            ; 053B: 28 69 67 74 61 0x04 terminator
        FCB     $00,$68,$80,$29,$00 ; '.h.).'
        FCC     "ruEjkxFnkx"
        FCB     $04            ; 0545: 72 75 45 6A 6B 78 46 6E 6B 78 0x04 terminator
        FCB     $05,$00,$6C,$76,$2E,$00,$6D,$00 ; '..lv..m.'
        FCB     $05,$00
        FCC     "rwIopxAqpx"
        FCB     $04            ; 0559: 72 77 49 6F 70 78 41 71 70 78 0x04 terminator
        FCB     $05,$00,$72,$77,$02,$FF ; '..rw..'
        FCC     "ry,sa"
        FCB     $04            ; 0569: 72 79 2C 73 61 0x04 terminator
        FCB     $00,$2F,$FF ; './.'
        FCC     "asIvu3"
        FCB     $04            ; 0571: 61 73 49 76 75 33 0x04 terminator
        FCB     $03,$77,$FF,$25,$4A,$00,$77,$33 ; '.w.%J.w3'
        FCB     $07,$00,$78,$25,$28,$00,$79,$26 ; '..x%(.y&'
        FCB     $03,$00
        FCC     "z',{y"
        FCB     $04            ; 0589: 7A 27 2C 7B 79 0x04 terminator
        FCB     $00,$29,$00,$FF,$28,$4F,$00,$7E ; '.)..(O.~'
        FCB     $1C,$4E,$00,$80,$00,$03,$7F,$85 ; '.N......'
        FCB     $31,$04,$80,$81,$13,$07,$00,$81 ; '1.......'
        FCB     $13,$28,$00,$82,$00,$05,$00,$83 ; '.(......'
        FCB     $14,$2C,$84,$82,$00,$29,$00,$85 ; '.,...)..'
        FCB     $15,$2C,$FF,$7E,$00,$2D,$88,$87 ; '.,.~.-..'
        FCB     $37,$95,$00,$FF,$BB,$89,$87,$FF ; '7.......'
        FCB     $BD,$03,$8A,$FF,$31,$07,$8C,$8B ; '....1...'
        FCB     $32,$59,$00,$FF,$AC,$04,$00,$8D ; '2Y......'
        FCB     $34,$28,$FF,$8E,$00,$05,$8F,$93 ; '4(......'
        FCB     $35,$03,$00,$90,$36,$2B,$91,$92 ; '5...6+..'
        FCB     $00,$2D,$93,$92,$37,$05,$00,$93 ; '.-..7...'
        FCB     $38,$2C,$94,$8E,$00,$29,$00,$FF ; '8,...)..'
        FCB     $39,$05,$96,$FF,$3A,$02,$97,$FF ; '9...:...'
        FCB     $3A,$2E,$00,$FF,$3C,$2D,$99,$9A ; ':...<-..'
        FCB     $40,$52,$9A,$9A,$4D,$28,$9D,$9B ; '@R..M(..'
        FCB     $48,$98,$00,$9C,$80,$29,$00,$9E ; 'H....)..'
        FCB     $46,$88,$00,$9E,$CB,$3D,$9F,$98 ; 'F....=..'
        FCB     $45,$09,$A0,$98,$4D,$2B,$A1,$98 ; 'E...M+..'
        FCB     $43,$2D,$A2,$98,$44,$2F,$A3,$98 ; 'C-..D/..'
        FCB     $42,$2A,$FF,$A4,$41,$2A,$98,$98 ; 'B*..A*..'
        FCB     $49 ; 'I'

; Resident banner strings printed when the compiler starts.
        ORG     $0863
        FCC     "M6800 RESIDENT FORTRAN 1.0"
        FCB     $04            ; 0863: 4D 36 38 30 30 20 52 45 53 49 44 45 4E 54 20 46 4F 52 54 52 41 4E 20 31 2E 30 0x04 terminator
        FCC     "COPYRIGHT BY MOTOROLA 1976"
        FCB     $04            ; 087E: 43 4F 50 59 52 49 47 48 54 20 42 59 20 4D 4F 54 4F 52 4F 4C 41 20 31 39 37 36 0x04 terminator

; Print the startup banner and copyright strings.
L0899:
        LDX     #$0898           ; 0899: CE 08 98
        JSR     L26BF            ; 089C: BD 26 BF
        LDX     #$0863           ; 089F: CE 08 63
        JSR     L26BF            ; 08A2: BD 26 BF
        LDX     #$087E           ; 08A5: CE 08 7E
        JSR     L26BF            ; 08A8: BD 26 BF
        LDX     #$0898           ; 08AB: CE 08 98
        JSR     L26BF            ; 08AE: BD 26 BF
        LDX     #$0898           ; 08B1: CE 08 98
        JSR     L26BF            ; 08B4: BD 26 BF
        RTS                      ; 08B7: 39

; Default object-module header text for the generated output.
        ORG     $0964
        FCC     "30      3$MAIN  "
        FCB     $04            ; 0964: 33 30 20 20 20 20 20 20 33 24 4D 41 49 4E 20 20 0x04 terminator
        FCB     $00,$00

        ORG     $097C

; Compiler cold-start initialization.
; Sets the runtime stack, seeds descriptor pointers, clears state,
; copies keyword data into working storage, and enters the main compile loop.
L097C:
        LDS     #$01EB           ; 097C: 8E 01 EB
        LDX     #$03A7           ; 097F: CE 03 A7
        STX     $02A1            ; 0982: FF 02 A1
        LDX     $1293            ; 0985: FE 12 93
        STX     $61              ; 0988: DF 61
        STX     $52              ; 098A: DF 52
        STX     $63              ; 098C: DF 63
        CLR     $0068            ; 098E: 7F 00 68
        CLR     $01C2            ; 0991: 7F 01 C2
        LDAA    #$07             ; 0994: 86 07
        STAA    $67              ; 0996: 97 67
        LDX     #$2E2B           ; 0998: CE 2E 2B
        STX     $01BF            ; 099B: FF 01 BF
        LDAA    #$55             ; 099E: 86 55
L09A0:
        INX                      ; 09A0: 08
        STAA    $00,X            ; 09A1: A7 00
        LDAB    $00,X            ; 09A3: E6 00
        CMPB    #$55             ; 09A5: C1 55
        BEQ     L09A0            ; 09A7: 27 F7
        DEX                      ; 09A9: 09
        STX     $035C            ; 09AA: FF 03 5C
        LDAA    #$01             ; 09AD: 86 01
L09AF:
        STAA    $3E              ; 09AF: 97 3E
        LDAB    $1293            ; 09B1: F6 12 93
        LDAA    $1294            ; 09B4: B6 12 94
        LDX     #$019F           ; 09B7: CE 01 9F
        JSR     L2519            ; 09BA: BD 25 19
        BRA     L09C2            ; 09BD: 20 03
        FCB     $02,$00,$3E ; '..>'
L09C2:
        STAB    $00,X            ; 09C2: E7 00
        STAA    $01,X            ; 09C4: A7 01
        LDAA    $3E              ; 09C6: 96 3E
        CMPA    #$10             ; 09C8: 81 10
        BCC     L09D0            ; 09CA: 24 04
        INCA                     ; 09CC: 4C
        JMP     L09AF            ; 09CD: 7E 09 AF
L09D0:
        LDX     $1291            ; 09D0: FE 12 91
        STX     $0139            ; 09D3: FF 01 39
        CLR     $004E            ; 09D6: 7F 00 4E
        LDAA    #$01             ; 09D9: 86 01
L09DB:
        STAA    $3E              ; 09DB: 97 3E
        LDX     #$01EC           ; 09DD: CE 01 EC
        LDAB    $3E              ; 09E0: D6 3E
        JSR     L2562            ; 09E2: BD 25 62
        LDAA    $00,X            ; 09E5: A6 00
        CMPA    #$20             ; 09E7: 81 20
        BNE     L09EE            ; 09E9: 26 03
        JMP     L0A08            ; 09EB: 7E 0A 08
L09EE:
        LDX     #$01EC           ; 09EE: CE 01 EC
        LDAB    $3E              ; 09F1: D6 3E
        JSR     L2562            ; 09F3: BD 25 62
        LDAA    $00,X            ; 09F6: A6 00
        LDX     #$00C4           ; 09F8: CE 00 C4
        LDAB    $4E              ; 09FB: D6 4E
        JSR     L2562            ; 09FD: BD 25 62
        STAA    $00,X            ; 0A00: A7 00
        INC     $004E            ; 0A02: 7C 00 4E
        JMP     L0A1F            ; 0A05: 7E 0A 1F
L0A08:
        DEC     $004E            ; 0A08: 7A 00 4E
        CLRA                     ; 0A0B: 4F
        JSR     L0A4B            ; 0A0C: BD 0A 4B
        LDAA    #$80             ; 0A0F: 86 80
        JSR     L0ADF            ; 0A11: BD 0A DF
        LDAA    $C3              ; 0A14: 96 C3
        LDX     $55              ; 0A16: DE 55
        STAA    $04,X            ; 0A18: A7 04
        CLR     $03,X            ; 0A1A: 6F 03
        CLR     $004E            ; 0A1C: 7F 00 4E
L0A1F:
        LDAA    $3E              ; 0A1F: 96 3E
        CMPA    #$B5             ; 0A21: 81 B5
        BCC     L0A29            ; 0A23: 24 04
        INCA                     ; 0A25: 4C
        JMP     L09DB            ; 0A26: 7E 09 DB
L0A29:
        CLR     $004F            ; 0A29: 7F 00 4F
        CLR     $0380            ; 0A2C: 7F 03 80
        JSR     L0B83            ; 0A2F: BD 0B 83
L0A32:
        CLR     $0058            ; 0A32: 7F 00 58
        LDX     $128F            ; 0A35: FE 12 8F
        STX     $59              ; 0A38: DF 59
        LDX     #$C311           ; 0A3A: CE C3 11
        STX     $032A            ; 0A3D: FF 03 2A
        LDX     $128D            ; 0A40: FE 12 8D
        STX     $65              ; 0A43: DF 65
        JSR     L111C            ; 0A45: BD 11 1C
        JMP     L0A32            ; 0A48: 7E 0A 32

; Checksum / hash helper for the current identifier text in $00C4..
; Produces a mixed value in $41/$55/$56 used while walking symbol chains.
L0A4B:
        STAA    $42              ; 0A4B: 97 42
        LDX     #$00C4           ; 0A4D: CE 00 C4
        LDAB    $4E              ; 0A50: D6 4E
        CLRA                     ; 0A52: 4F
L0A53:
        ADDA    $00,X            ; 0A53: AB 00
        INX                      ; 0A55: 08
        DECB                     ; 0A56: 5A
        BNE     L0A53            ; 0A57: 26 FA
        STAA    $41              ; 0A59: 97 41
        ANDA    #$0F             ; 0A5B: 84 0F
        ASLA                     ; 0A5D: 48
        STAA    $41              ; 0A5E: 97 41
        CLRB                     ; 0A60: 5F
        TSTA                     ; 0A61: 4D
        BGE     L0A65            ; 0A62: 2C 01
        COMB                     ; 0A64: 53
L0A65:
        ADDA    $128C            ; 0A65: BB 12 8C
        ADCB    $128B            ; 0A68: F9 12 8B
        STAB    $55              ; 0A6B: D7 55
        STAA    $56              ; 0A6D: 97 56
        LDAA    #$FF             ; 0A6F: 86 FF
        STAA    $54              ; 0A71: 97 54
        LDX     $55              ; 0A73: DE 55
        LDX     $00,X            ; 0A75: EE 00
        STX     $49              ; 0A77: DF 49
L0A79:
        LDX     $1293            ; 0A79: FE 12 93
        CPX     $49              ; 0A7C: 9C 49
        BNE     L0A81            ; 0A7E: 26 01
        RTS                      ; 0A80: 39
L0A81:
        CLR     $0054            ; 0A81: 7F 00 54
        LDAA    $4E              ; 0A84: 96 4E
        ORAA    $42              ; 0A86: 9A 42
        LDX     $49              ; 0A88: DE 49
        EORA    $00,X            ; 0A8A: A8 00
        ANDA    #$1F             ; 0A8C: 84 1F
        BEQ     L0A9B            ; 0A8E: 27 0B
L0A90:
        LDX     $49              ; 0A90: DE 49
        STX     $55              ; 0A92: DF 55
        LDX     $01,X            ; 0A94: EE 01
        STX     $49              ; 0A96: DF 49
        JMP     L0A79            ; 0A98: 7E 0A 79
L0A9B:
        LDX     $49              ; 0A9B: DE 49
        LDAA    $00,X            ; 0A9D: A6 00
        ANDA    #$60             ; 0A9F: 84 60
        BNE     L0AA7            ; 0AA1: 26 04
        STX     $4B              ; 0AA3: DF 4B
        BRA     L0AB3            ; 0AA5: 20 0C
L0AA7:
        LDX     $49              ; 0AA7: DE 49
        INX                      ; 0AA9: 08
        INX                      ; 0AAA: 08
        INX                      ; 0AAB: 08
        INX                      ; 0AAC: 08
        INX                      ; 0AAD: 08
        INX                      ; 0AAE: 08
        INX                      ; 0AAF: 08
        INX                      ; 0AB0: 08
        STX     $4B              ; 0AB1: DF 4B
L0AB3:
        LDAA    #$01             ; 0AB3: 86 01
L0AB5:
        STAA    $41              ; 0AB5: 97 41
        LDX     #$00C4           ; 0AB7: CE 00 C4
        TAB                      ; 0ABA: 16
        JSR     L2562            ; 0ABB: BD 25 62
        LDAA    $00,X            ; 0ABE: A6 00
        LDX     $4B              ; 0AC0: DE 4B
        CMPA    $05,X            ; 0AC2: A1 05
        BEQ     L0AC9            ; 0AC4: 27 03
        JMP     L0A90            ; 0AC6: 7E 0A 90
L0AC9:
        INX                      ; 0AC9: 08
        STX     $4B              ; 0ACA: DF 4B
        LDAA    $41              ; 0ACC: 96 41
        CMPA    $4E              ; 0ACE: 91 4E
        BCC     L0AD6            ; 0AD0: 24 04
        INCA                     ; 0AD2: 4C
        JMP     L0AB5            ; 0AD3: 7E 0A B5
L0AD6:
        LDAA    #$01             ; 0AD6: 86 01
        STAA    $54              ; 0AD8: 97 54
        LDX     $49              ; 0ADA: DE 49
        STX     $55              ; 0ADC: DF 55
        RTS                      ; 0ADE: 39

; Create or extend a symbol-table entry at $01BF.
; Stores class bits from A plus source-location and auxiliary descriptor fields.
L0ADF:
        STAA    $42              ; 0ADF: 97 42
        LDAA    $54              ; 0AE1: 96 54
        BNE     L0AF3            ; 0AE3: 26 0E
        LDAB    $01BF            ; 0AE5: F6 01 BF
        LDAA    $01C0            ; 0AE8: B6 01 C0
        LDX     $55              ; 0AEB: DE 55
        STAB    $01,X            ; 0AED: E7 01
        STAA    $02,X            ; 0AEF: A7 02
        BRA     L0AFF            ; 0AF1: 20 0C
L0AF3:
        LDAB    $01BF            ; 0AF3: F6 01 BF
        LDAA    $01C0            ; 0AF6: B6 01 C0
        LDX     $55              ; 0AF9: DE 55
        STAB    $00,X            ; 0AFB: E7 00
        STAA    $01,X            ; 0AFD: A7 01
L0AFF:
        LDX     $01BF            ; 0AFF: FE 01 BF
        STX     $55              ; 0B02: DF 55
        LDAA    $4E              ; 0B04: 96 4E
        ORAA    $42              ; 0B06: 9A 42
        STAA    $00,X            ; 0B08: A7 00
        LDAB    $1293            ; 0B0A: F6 12 93
        LDAA    $1294            ; 0B0D: B6 12 94
        STAB    $01,X            ; 0B10: E7 01
        STAA    $02,X            ; 0B12: A7 02
        LDAB    $50              ; 0B14: D6 50
        LDAA    $51              ; 0B16: 96 51
        STAB    $03,X            ; 0B18: E7 03
        STAA    $04,X            ; 0B1A: A7 04
        LDAA    $00,X            ; 0B1C: A6 00
        ANDA    #$60             ; 0B1E: 84 60
        BEQ     L0B3F            ; 0B20: 27 1D
        LDAB    $1293            ; 0B22: F6 12 93
        LDAA    $1294            ; 0B25: B6 12 94
        STAB    $05,X            ; 0B28: E7 05
        STAA    $06,X            ; 0B2A: A7 06
        STAB    $07,X            ; 0B2C: E7 07
        STAA    $08,X            ; 0B2E: A7 08
        STAB    $09,X            ; 0B30: E7 09
        STAA    $0A,X            ; 0B32: A7 0A
        INX                      ; 0B34: 08
        INX                      ; 0B35: 08
        INX                      ; 0B36: 08
        INX                      ; 0B37: 08
        INX                      ; 0B38: 08
        INX                      ; 0B39: 08
        INX                      ; 0B3A: 08
        INX                      ; 0B3B: 08
        STX     $01BF            ; 0B3C: FF 01 BF
L0B3F:
        LDAA    #$01             ; 0B3F: 86 01
L0B41:
        STAA    $41              ; 0B41: 97 41
        LDX     #$00C4           ; 0B43: CE 00 C4
        LDAB    $41              ; 0B46: D6 41
        JSR     L2562            ; 0B48: BD 25 62
        LDAA    $00,X            ; 0B4B: A6 00
        LDX     $01BF            ; 0B4D: FE 01 BF
        STAA    $05,X            ; 0B50: A7 05
        INX                      ; 0B52: 08
        STX     $01BF            ; 0B53: FF 01 BF
        LDAA    $41              ; 0B56: 96 41
        CMPA    $4E              ; 0B58: 91 4E
        BCC     L0B60            ; 0B5A: 24 04
        INCA                     ; 0B5C: 4C
        JMP     L0B41            ; 0B5D: 7E 0B 41
L0B60:
        LDX     $01BF            ; 0B60: FE 01 BF
        INX                      ; 0B63: 08
        INX                      ; 0B64: 08
        INX                      ; 0B65: 08
        INX                      ; 0B66: 08
        INX                      ; 0B67: 08
        STX     $01BF            ; 0B68: FF 01 BF
        LDAB    $01BF            ; 0B6B: F6 01 BF
        LDAA    $01C0            ; 0B6E: B6 01 C0
        CMPB    $035C            ; 0B71: F1 03 5C
        BCS     L0B82            ; 0B74: 25 0C
        BNE     L0B7D            ; 0B76: 26 05
        CMPA    $035D            ; 0B78: B1 03 5D
        BCS     L0B82            ; 0B7B: 25 05
L0B7D:
        LDAA    #$02             ; 0B7D: 86 02
        JSR     L0D7F            ; 0B7F: BD 0D 7F
L0B82:
        RTS                      ; 0B82: 39

; Classify the current statement keyword and load its dispatch metadata.
; This is the front door from statement text to statement-specific handlers.
L0B83:
        LDAA    $4F              ; 0B83: 96 4F
        BNE     L0B8A            ; 0B85: 26 03
        JMP     L0D6B            ; 0B87: 7E 0D 6B
L0B8A:
        CLR     $004E            ; 0B8A: 7F 00 4E
        LDAA    #$20             ; 0B8D: 86 20
        STAA    $C5              ; 0B8F: 97 C5
        LDX     #$0073           ; 0B91: CE 00 73
        LDAB    $4F              ; 0B94: D6 4F
        JSR     L2562            ; 0B96: BD 25 62
        LDAA    $00,X            ; 0B99: A6 00
        STAA    $01C1            ; 0B9B: B7 01 C1
        STAA    $4D              ; 0B9E: 97 4D
        CMPA    #$20             ; 0BA0: 81 20
        BGE     L0BA8            ; 0BA2: 2C 04
        LDAA    #$3B             ; 0BA4: 86 3B
        STAA    $4D              ; 0BA6: 97 4D
L0BA8:
        LDAA    $4D              ; 0BA8: 96 4D
        CMPA    #$5A             ; 0BAA: 81 5A
        BLE     L0BB2            ; 0BAC: 2F 04
        CLRA                     ; 0BAE: 4F
        JSR     L0D7F            ; 0BAF: BD 0D 7F
L0BB2:
        LDAA    $4D              ; 0BB2: 96 4D
        SUBA    #$1F             ; 0BB4: 80 1F
        STAA    $3E              ; 0BB6: 97 3E
        LDX     #$02D1           ; 0BB8: CE 02 D1
        LDAB    $3E              ; 0BBB: D6 3E
        JSR     L2562            ; 0BBD: BD 25 62
        LDAA    $00,X            ; 0BC0: A6 00
        INCA                     ; 0BC2: 4C
        STAA    $3E              ; 0BC3: 97 3E
L0BC5:
        LDX     #$030C           ; 0BC5: CE 03 0C
        JSR     L2519            ; 0BC8: BD 25 19
        BRA     L0BD0            ; 0BCB: 20 03
        FCB     $0A,$00,$3E ; '..>'
L0BD0:
        LDAA    $00,X            ; 0BD0: A6 00
        ANDA    #$40             ; 0BD2: 84 40
        BEQ     L0BE7            ; 0BD4: 27 11
        LDX     #$030C           ; 0BD6: CE 03 0C
        JSR     L2519            ; 0BD9: BD 25 19
        BRA     L0BE1            ; 0BDC: 20 03
        FCB     $0A,$00,$3E ; '..>'
L0BE1:
        LDAA    $00,X            ; 0BE1: A6 00
        ANDA    #$3F             ; 0BE3: 84 3F
        STAA    $4D              ; 0BE5: 97 4D
L0BE7:
        LDX     #$030C           ; 0BE7: CE 03 0C
        JSR     L2519            ; 0BEA: BD 25 19
        BRA     L0BF2            ; 0BED: 20 03
        FCB     $0A,$00,$3E ; '..>'
L0BF2:
        LDAA    $00,X            ; 0BF2: A6 00
        ANDA    #$80             ; 0BF4: 84 80
        BNE     L0C2C            ; 0BF6: 26 34
        INC     $004F            ; 0BF8: 7C 00 4F
L0BFB:
        LDX     #$030D           ; 0BFB: CE 03 0D
        JSR     L2519            ; 0BFE: BD 25 19
        BRA     L0C06            ; 0C01: 20 03
        FCB     $0A,$00,$3E ; '..>'
L0C06:
        LDAA    $00,X            ; 0C06: A6 00
        ASRA                     ; 0C08: 47
        ASRA                     ; 0C09: 47
        ASRA                     ; 0C0A: 47
        ASRA                     ; 0C0B: 47
        ANDA    #$0F             ; 0C0C: 84 0F
        INCA                     ; 0C0E: 4C
        STAA    $3F              ; 0C0F: 97 3F
        BRA     L0C1D            ; 0C11: 20 0A
        FCB     $0C,$E2,$0C,$E7,$0D,$25,$0D,$7C ; '.....%.|'
        FCB     $0D,$6B ; '.k'
L0C1D:
        LDAB    $3F              ; 0C1D: D6 3F
        ASLB                     ; 0C1F: 58
        LDX     #$0C11           ; 0C20: CE 0C 11
        JSR     L2567            ; 0C23: BD 25 67
        LDX     $00,X            ; 0C26: EE 00
        JMP     $00,X            ; 0C28: 6E 00
        FCB     $20,$15 ; ' .'
L0C2C:
        LDX     #$030D           ; 0C2C: CE 03 0D
        JSR     L2519            ; 0C2F: BD 25 19
        BRA     L0C37            ; 0C32: 20 03
        FCB     $0A,$00,$3E ; '..>'
L0C37:
        LDAA    $00,X            ; 0C37: A6 00
        ANDA    #$0F             ; 0C39: 84 0F
        INCA                     ; 0C3B: 4C
        STAA    $3F              ; 0C3C: 97 3F
        JMP     L0C75            ; 0C3E: 7E 0C 75
L0C41:
        LDX     #$0073           ; 0C41: CE 00 73
        LDAB    $4F              ; 0C44: D6 4F
        JSR     L2562            ; 0C46: BD 25 62
        LDAA    $00,X            ; 0C49: A6 00
        SUBA    #$1F             ; 0C4B: 80 1F
        STAA    $01C1            ; 0C4D: B7 01 C1
        BPL     L0C54            ; 0C50: 2A 02
        LDAA    #$1C             ; 0C52: 86 1C
L0C54:
        STAA    $3F              ; 0C54: 97 3F
        LDX     #$02D1           ; 0C56: CE 02 D1
        LDAB    $3F              ; 0C59: D6 3F
        JSR     L2562            ; 0C5B: BD 25 62
        LDAA    $00,X            ; 0C5E: A6 00
        STAA    $3F              ; 0C60: 97 3F
        LDX     #$030F           ; 0C62: CE 03 0F
        JSR     L2519            ; 0C65: BD 25 19
        BRA     L0C70            ; 0C68: 20 06
        FCB     $0A,$00,$3E,$01,$00,$3F ; '..>..?'
L0C70:
        LDAA    $00,X            ; 0C70: A6 00
        INCA                     ; 0C72: 4C
        STAA    $3F              ; 0C73: 97 3F
L0C75:
        BRA     L0C85            ; 0C75: 20 0E
        FCB     $0C,$92,$0C,$96,$0C,$B8,$0C,$96
        FCB     $0C,$B8,$0B,$FB,$0C,$C7
L0C85:
        LDAB    $3F              ; 0C85: D6 3F
        ASLB                     ; 0C87: 58
        LDX     #$0C75           ; 0C88: CE 0C 75
        JSR     L2567            ; 0C8B: BD 25 67
        LDX     $00,X            ; 0C8E: EE 00
        JMP     $00,X            ; 0C90: 6E 00
L0C92:
        CLRA                     ; 0C92: 4F
        JSR     L0D7F            ; 0C93: BD 0D 7F
L0C96:
        LDAA    $4E              ; 0C96: 96 4E
        CMPA    #$4F             ; 0C98: 81 4F
        BLT     L0CA1            ; 0C9A: 2D 05
        LDAA    #$04             ; 0C9C: 86 04
        JSR     L0D7F            ; 0C9E: BD 0D 7F
L0CA1:
        INC     $004E            ; 0CA1: 7C 00 4E
        LDX     #$0073           ; 0CA4: CE 00 73
        LDAB    $4F              ; 0CA7: D6 4F
        JSR     L2562            ; 0CA9: BD 25 62
        LDAA    $00,X            ; 0CAC: A6 00
        LDX     #$00C4           ; 0CAE: CE 00 C4
        LDAB    $4E              ; 0CB1: D6 4E
        JSR     L2562            ; 0CB3: BD 25 62
        STAA    $00,X            ; 0CB6: A7 00
L0CB8:
        INC     $004F            ; 0CB8: 7C 00 4F
        LDAA    $3F              ; 0CBB: 96 3F
        CMPA    #$04             ; 0CBD: 81 04
        BLT     L0CC4            ; 0CBF: 2D 03
        JMP     L0BFB            ; 0CC1: 7E 0B FB
L0CC4:
        JMP     L0C41            ; 0CC4: 7E 0C 41
L0CC7:
        LDX     #$0074           ; 0CC7: CE 00 74
        LDAB    $4F              ; 0CCA: D6 4F
        JSR     L2562            ; 0CCC: BD 25 62
        LDAA    $00,X            ; 0CCF: A6 00
        CMPA    #$41             ; 0CD1: 81 41
        BCC     L0CDC            ; 0CD3: 24 07
        LDAA    #$07             ; 0CD5: 86 07
        STAA    $3E              ; 0CD7: 97 3E
        JMP     L0BC5            ; 0CD9: 7E 0B C5
L0CDC:
        CLR     $003F            ; 0CDC: 7F 00 3F
        JMP     L0CB8            ; 0CDF: 7E 0C B8
        FCB     $DE,$C4,$DF,$5F,$39,$4F,$BD,$0A ; '..._9O..'
        FCB     $4B,$96,$54,$81,$01,$2D,$11,$DE ; 'K.T..-..'
        FCB     $55,$A6,$04,$97,$4D,$81,$53,$2D ; 'U...M.S-'
        FCB     $06,$97,$C4,$86,$09,$97,$4D,$39 ; '......M9'
        FCB     $86,$10,$BD,$0A,$4B,$B6,$01,$C1 ; '....K...'
        FCB     $81,$09,$26,$04,$86,$07,$97,$4D ; '..&....M'
        FCB     $96,$54,$81,$01,$2D,$0C,$DE,$55 ; '.T..-..U'
        FCB     $A6,$00,$84,$40,$27,$04,$86,$04 ; '...@'...'
        FCB     $97,$4D,$39,$FE,$12,$93,$DF,$5F ; '.M9...._'
        FCB     $86,$01,$97,$3F,$CE,$00,$C4,$D6 ; '...?....'
        FCB     $3F,$BD,$25,$62,$A6,$00,$84,$0F ; '?.%b....'
        FCB     $B7,$01,$C1,$D6,$5F,$96 ; '...._.'
        FCC     "`HYHY"
        FCB     $04            ; 0D40: 60 48 59 48 59 0x04 terminator
        FCB     $9B,$60,$D9,$5F,$48,$59,$D7,$5F ; '.`._HY._'
        FCB     $97,$60,$BB,$01,$C1,$C9,$00,$D7 ; '.`......'
        FCB     $5F,$97,$60,$96,$3F,$91,$4E,$24 ; '_.`.?.N$'
        FCB     $04,$4C,$7E,$0D,$2C,$DE,$5F,$DF ; '.L~.,._.'
        FCB     $C4,$86,$02,$97,$4E,$39 ; '....N9'
L0D6B:
        JSR     L0FB4            ; 0D6B: BD 0F B4
        LDAA    $01C2            ; 0D6E: B6 01 C2
        BLE     L0D78            ; 0D71: 2F 05
        LDAA    #$06             ; 0D73: 86 06
        JSR     L1295            ; 0D75: BD 12 95
L0D78:
        LDAA    #$05             ; 0D78: 86 05
        STAA    $4F              ; 0D7A: 97 4F
        JMP     L0B8A            ; 0D7C: 7E 0B 8A

; Emit an error message for the code in A.
; Formats the line marker, prints '**ERROR', and advances listing state.
L0D7F:
        STAA    $40              ; 0D7F: 97 40
        LDAA    $01C3            ; 0D81: B6 01 C3
        BGE     L0D89            ; 0D84: 2C 03
        JSR     L1048            ; 0D86: BD 10 48
L0D89:
        LDX     #$0DCF           ; 0D89: CE 0D CF
        CLRA                     ; 0D8C: 4F
        LDAB    $40              ; 0D8D: D6 40
        JSR     L2670            ; 0D8F: BD 26 70
        LDAA    #$20             ; 0D92: 86 20
        LDAB    $4F              ; 0D94: D6 4F
        INCB                     ; 0D96: 5C
L0D97:
        STAA    $00,X            ; 0D97: A7 00
        INX                      ; 0D99: 08
        DECB                     ; 0D9A: 5A
        BNE     L0D97            ; 0D9B: 26 FA
        DEX                      ; 0D9D: 09
        LDAA    #$5E             ; 0D9E: 86 5E
        STAA    $00,X            ; 0DA0: A7 00
        LDAA    #$04             ; 0DA2: 86 04
        STAA    $01,X            ; 0DA4: A7 01
        LDX     #$0DC6           ; 0DA6: CE 0D C6
        JSR     L1091            ; 0DA9: BD 10 91
        CLRB                     ; 0DAC: 5F
        LDAA    $4F              ; 0DAD: 96 4F
        CMPA    #$06             ; 0DAF: 81 06
        BNE     L0DB5            ; 0DB1: 26 02
        TAB                      ; 0DB3: 16
        DECB                     ; 0DB4: 5A
L0DB5:
        STAB    $4F              ; 0DB5: D7 4F
        INC     $0068            ; 0DB7: 7C 00 68
        LDX     #$C311           ; 0DBA: CE C3 11
        STX     $032A            ; 0DBD: FF 03 2A
        LDS     #$01EB           ; 0DC0: 8E 01 EB
        JMP     L0A29            ; 0DC3: 7E 0A 29
        FCB     $20,$2A,$2A,$45,$52,$52,$4F,$52 ; ' **ERROR'
        FCB     $20,$20 ; '  '

        ORG     $0E23

; Unpack one encoded descriptor from the parse tables at $0114/$0115.
; Used when decoding statement templates and operand classes.
L0E23:
        LDAA    #$00             ; 0E23: 86 00
        JSR     L0EEE            ; 0E25: BD 0E EE
        LDAA    $40              ; 0E28: 96 40
        ASRA                     ; 0E2A: 47
        ASRA                     ; 0E2B: 47
        ASRA                     ; 0E2C: 47
        ASRA                     ; 0E2D: 47
        ANDA    #$0C             ; 0E2E: 84 0C
        STAA    $40              ; 0E30: 97 40
        LDX     #$0114           ; 0E32: CE 01 14
        JSR     L2519            ; 0E35: BD 25 19
        BRA     L0E3D            ; 0E38: 20 03
        FCB     $03,$00,$5C ; '..\'
L0E3D:
        LDAA    $00,X            ; 0E3D: A6 00
        ASRA                     ; 0E3F: 47
        ASRA                     ; 0E40: 47
        ANDA    #$30             ; 0E41: 84 30
        STAA    $3F              ; 0E43: 97 3F
        LDX     #$0114           ; 0E45: CE 01 14
        JSR     L2519            ; 0E48: BD 25 19
        BRA     L0E50            ; 0E4B: 20 03
        FCB     $03,$00,$5B ; '..['
L0E50:
        LDAA    $00,X            ; 0E50: A6 00
        ANDA    #$C0             ; 0E52: 84 C0
        STAA    $3E              ; 0E54: 97 3E
        ORAA    $3F              ; 0E56: 9A 3F
        ORAA    $40              ; 0E58: 9A 40
        STAA    $42              ; 0E5A: 97 42
        LDAA    $5E              ; 0E5C: 96 5E
        STAA    $41              ; 0E5E: 97 41
        LDAA    #$42             ; 0E60: 86 42
        LDAB    #$00             ; 0E62: C6 00
        LDX     $41              ; 0E64: DE 41
        JSR     L0F08            ; 0E66: BD 0F 08
        LDX     #$0114           ; 0E69: CE 01 14
        JSR     L2519            ; 0E6C: BD 25 19
        BRA     L0E74            ; 0E6F: 20 03
        FCB     $03,$00,$5B ; '..['
L0E74:
        LDAA    $00,X            ; 0E74: A6 00
        ASRA                     ; 0E76: 47
        ASRA                     ; 0E77: 47
        ASRA                     ; 0E78: 47
        ASRA                     ; 0E79: 47
        ANDA    #$03             ; 0E7A: 84 03
        NEGA                     ; 0E7C: 40
        ADDA    #$03             ; 0E7D: 8B 03
        STAA    $3E              ; 0E7F: 97 3E
        LDX     #$0114           ; 0E81: CE 01 14
        JSR     L2519            ; 0E84: BD 25 19
        BRA     L0E8C            ; 0E87: 20 03
        FCB     $03,$00,$5C ; '..\'
L0E8C:
        LDAA    $00,X            ; 0E8C: A6 00
        ASRA                     ; 0E8E: 47
        ASRA                     ; 0E8F: 47
        ASRA                     ; 0E90: 47
        ASRA                     ; 0E91: 47
        ANDA    #$03             ; 0E92: 84 03
        NEGA                     ; 0E94: 40
        ADDA    #$03             ; 0E95: 8B 03
        STAA    $3F              ; 0E97: 97 3F
        LDX     #$0114           ; 0E99: CE 01 14
        JSR     L2519            ; 0E9C: BD 25 19
        BRA     L0EA4            ; 0E9F: 20 03
        FCB     $03,$00,$5D ; '..]'
L0EA4:
        LDAA    $00,X            ; 0EA4: A6 00
        ASRA                     ; 0EA6: 47
        ASRA                     ; 0EA7: 47
        ASRA                     ; 0EA8: 47
        ASRA                     ; 0EA9: 47
        ANDA    #$03             ; 0EAA: 84 03
        NEGA                     ; 0EAC: 40
        ADDA    #$03             ; 0EAD: 8B 03
        STAA    $40              ; 0EAF: 97 40
        LDAA    #$42             ; 0EB1: 86 42
        LDAB    $3E              ; 0EB3: D6 3E
        LDX     #$0115           ; 0EB5: CE 01 15
        JSR     L2519            ; 0EB8: BD 25 19
        BRA     L0EC0            ; 0EBB: 20 03
        FCB     $03,$00,$5B ; '..['
L0EC0:
        LDX     $00,X            ; 0EC0: EE 00
        JSR     L0F08            ; 0EC2: BD 0F 08
        LDAA    #$42             ; 0EC5: 86 42
        LDAB    $3F              ; 0EC7: D6 3F
        LDX     #$0115           ; 0EC9: CE 01 15
        JSR     L2519            ; 0ECC: BD 25 19
        BRA     L0ED4            ; 0ECF: 20 03
        FCB     $03,$00,$5C ; '..\'
L0ED4:
        LDX     $00,X            ; 0ED4: EE 00
        JSR     L0F08            ; 0ED6: BD 0F 08
        LDAA    #$42             ; 0ED9: 86 42
        LDAB    $40              ; 0EDB: D6 40
        LDX     #$0115           ; 0EDD: CE 01 15
        JSR     L2519            ; 0EE0: BD 25 19
        BRA     L0EE8            ; 0EE3: 20 03
        FCB     $03,$00,$5D ; '..]'
L0EE8:
        LDX     $00,X            ; 0EE8: EE 00
        JSR     L0F08            ; 0EEA: BD 0F 08
        RTS                      ; 0EED: 39

; High-level diagnostic formatter.
; Seeds the error code and routes into L0F08 to build/print the message text.
DIAG_FORMAT:
L0EEE:
        STAA    $41              ; 0EEE: 97 41
        CLR     $0043            ; 0EF0: 7F 00 43
        STAA    $44              ; 0EF3: 97 44
        LDAA    #$41             ; 0EF5: 86 41
        CLRB                     ; 0EF7: 5F
        LDX     $1289            ; 0EF8: FE 12 89
        JSR     L0F08            ; 0EFB: BD 0F 08
        LDAA    #$42             ; 0EFE: 86 42
        LDAB    #$05             ; 0F00: C6 05
        LDX     $43              ; 0F02: DE 43
        JSR     L0F08            ; 0F04: BD 0F 08
        RTS                      ; 0F07: 39

; Low-level diagnostic message builder/printer.
; Selects one of several message templates, formats numeric fields into $0DD0..,
; and sends the finished text to the listing/output path.
DIAG_BUILD_AND_PRINT:
L0F08:
        STAA    $41              ; 0F08: 97 41
        STAB    $42              ; 0F0A: D7 42
        STX     $49              ; 0F0C: DF 49
        JSR     L2716            ; 0F0E: BD 27 16
        LDX     $1287            ; 0F11: FE 12 87
        LDAA    $00,X            ; 0F14: A6 00
        LDX     #$0DD0           ; 0F16: CE 0D D0
        JSR     L2DFC            ; 0F19: BD 2D FC
        LDAA    #$20             ; 0F1C: 86 20
        STAA    $0DD2            ; 0F1E: B7 0D D2
        STAA    $0DD5            ; 0F21: B7 0D D5
        STAA    $0DDA            ; 0F24: B7 0D DA
        STAA    $0DDD            ; 0F27: B7 0D DD
        STAA    $0DDE            ; 0F2A: B7 0D DE
        LDAA    #$04             ; 0F2D: 86 04
        STAA    $0DDF            ; 0F2F: B7 0D DF
        LDX     $1285            ; 0F32: FE 12 85
        LDAA    $00,X            ; 0F35: A6 00
        LDX     #$0DD3           ; 0F37: CE 0D D3
        JSR     L2DFC            ; 0F3A: BD 2D FC
        LDX     $128D            ; 0F3D: FE 12 8D
        STX     $4B              ; 0F40: DF 4B
        LDAA    $41              ; 0F42: 96 41
        CMPA    #$40             ; 0F44: 81 40
        BLT     L0F4D            ; 0F46: 2D 05
        LDX     $1283            ; 0F48: FE 12 83
        STX     $4B              ; 0F4B: DF 4B
L0F4D:
        TSTA                     ; 0F4D: 4D
        BGE     L0F55            ; 0F4E: 2C 05
        LDX     $1281            ; 0F50: FE 12 81
        STX     $4B              ; 0F53: DF 4B
L0F55:
        LDX     $4B              ; 0F55: DE 4B
        LDAA    $00,X            ; 0F57: A6 00
        LDAB    $01,X            ; 0F59: E6 01
        LDX     #$0DD6           ; 0F5B: CE 0D D6
        JSR     L2DF7            ; 0F5E: BD 2D F7
        LDAA    $41              ; 0F61: 96 41
        ANDA    #$07             ; 0F63: 84 07
        STAA    $41              ; 0F65: 97 41
        CMPA    #$01             ; 0F67: 81 01
        BNE     L0F86            ; 0F69: 26 1B
        LDX     $127F            ; 0F6B: FE 12 7F
        LDAA    $00,X            ; 0F6E: A6 00
        LDX     #$0DDB           ; 0F70: CE 0D DB
        JSR     L2DFC            ; 0F73: BD 2D FC
L0F76:
        LDAA    $01C3            ; 0F76: B6 01 C3
        BLE     L0F95            ; 0F79: 2F 1A
        JMP     L0F95            ; 0F7B: 7E 0F 95
        FCB     $CE,$0D,$D0,$BD,$10,$91,$20,$0F ; '...... .'
L0F86:
        LDX     $127F            ; 0F86: FE 12 7F
        LDAA    $00,X            ; 0F89: A6 00
        LDAB    $01,X            ; 0F8B: E6 01
        LDX     #$0DDB           ; 0F8D: CE 0D DB
        JSR     L2DF7            ; 0F90: BD 2D F7
        BRA     L0F76            ; 0F93: 20 E1
L0F95:
        LDX     $1281            ; 0F95: FE 12 81
        CPX     $4B              ; 0F98: 9C 4B
        BEQ     L0FB3            ; 0F9A: 27 17
        LDAA    $42              ; 0F9C: 96 42
        CMPA    #$01             ; 0F9E: 81 01
        BEQ     L0FB3            ; 0FA0: 27 11
        LDAA    $41              ; 0FA2: 96 41
        CLRB                     ; 0FA4: 5F
        TSTA                     ; 0FA5: 4D
        BGE     L0FA9            ; 0FA6: 2C 01
        COMB                     ; 0FA8: 53
L0FA9:
        LDX     $4B              ; 0FA9: DE 4B
        ADDA    $01,X            ; 0FAB: AB 01
        ADCB    $00,X            ; 0FAD: E9 00
        STAB    $00,X            ; 0FAF: E7 00
        STAA    $01,X            ; 0FB1: A7 01
L0FB3:
        RTS                      ; 0FB3: 39
L0FB4:
        CLR     $0078            ; 0FB4: 7F 00 78
        BSR     L0FCF            ; 0FB7: 8D 16
        LDAA    $78              ; 0FB9: 96 78
        CMPA    #$43             ; 0FBB: 81 43
        BEQ     L0FB4            ; 0FBD: 27 F5
        LDAA    $7D              ; 0FBF: 96 7D
        LDAB    #$20             ; 0FC1: C6 20
        STAB    $77              ; 0FC3: D7 77
        STAB    $7D              ; 0FC5: D7 7D
        CBA                      ; 0FC7: 11
        BNE     L0FCE            ; 0FC8: 26 04
        LDAA    #$24             ; 0FCA: 86 24
        STAA    $77              ; 0FCC: 97 77
L0FCE:
        RTS                      ; 0FCE: 39
L0FCF:
        LDAA    $01C3            ; 0FCF: B6 01 C3
        CMPA    #$02             ; 0FD2: 81 02
        BLT     L0FD9            ; 0FD4: 2D 03
        JMP     L10E5            ; 0FD6: 7E 10 E5
L0FD9:
        TST     $01C2            ; 0FD9: 7D 01 C2
        BEQ     L0FDF            ; 0FDC: 27 01
        RTS                      ; 0FDE: 39
L0FDF:
        LDX     $6A              ; 0FDF: DE 6A
        INX                      ; 0FE1: 08
        STX     $6A              ; 0FE2: DF 6A
        LDAA    $6A              ; 0FE4: 96 6A
        LDAB    $6B              ; 0FE6: D6 6B
        LDX     #$0072           ; 0FE8: CE 00 72
        JSR     L2670            ; 0FEB: BD 26 70
        CLR     $0069            ; 0FEE: 7F 00 69
        LDX     #$0078           ; 0FF1: CE 00 78
L0FF4:
        STX     $4B              ; 0FF4: DF 4B
L0FF6:
        JSR     L26BC            ; 0FF6: BD 26 BC
        BCC     L1005            ; 0FF9: 24 0A
        TSTA                     ; 0FFB: 4D
        JSR     L264D            ; 0FFC: BD 26 4D
        BCC     L0FF6            ; 0FFF: 24 F5
        INC     $01C2            ; 1001: 7C 01 C2
        RTS                      ; 1004: 39
L1005:
        TSTA                     ; 1005: 4D
        BEQ     L0FF6            ; 1006: 27 EE
        CMPA    #$0A             ; 1008: 81 0A
        BEQ     L0FF6            ; 100A: 27 EA
        LDX     $4B              ; 100C: DE 4B
        CMPA    #$20             ; 100E: 81 20
        BNE     L1039            ; 1010: 26 27
        TST     $0069            ; 1012: 7D 00 69
        BNE     L1025            ; 1015: 26 0E
L1017:
        LDAB    #$20             ; 1017: C6 20
L1019:
        CPX     #$007D           ; 1019: 8C 00 7D
        BEQ     L1023            ; 101C: 27 05
        STAB    $00,X            ; 101E: E7 00
        INX                      ; 1020: 08
        BRA     L1019            ; 1021: 20 F6
L1023:
        STAA    $69              ; 1023: 97 69
L1025:
        STAA    $00,X            ; 1025: A7 00
        INX                      ; 1027: 08
        CMPA    #$0D             ; 1028: 81 0D
        BNE     L0FF4            ; 102A: 26 C8
        DEX                      ; 102C: 09
        LDAA    #$04             ; 102D: 86 04
        STAA    $00,X            ; 102F: A7 00
        LDAA    $01C3            ; 1031: B6 01 C3
        BLE     L1038            ; 1034: 2F 02
        BRA     L1048            ; 1036: 20 10
L1038:
        RTS                      ; 1038: 39
L1039:
        CPX     #$0078           ; 1039: 8C 00 78
        BNE     L1025            ; 103C: 26 E7
        CMPA    #$43             ; 103E: 81 43
        BEQ     L1023            ; 1040: 27 E1
        CMPA    #$26             ; 1042: 81 26
        BEQ     L1017            ; 1044: 27 D1
        BRA     L1025            ; 1046: 20 DD
L1048:
        LDAA    $52              ; 1048: 96 52
        LDAB    $53              ; 104A: D6 53
        LDX     #$006D           ; 104C: CE 00 6D
        JSR     L2DF7            ; 104F: BD 2D F7
        LDX     #$006C           ; 1052: CE 00 6C
        LDAA    $77              ; 1055: 96 77
        PSHA                     ; 1057: 36
        LDAA    #$20             ; 1058: 86 20
        STAA    $77              ; 105A: 97 77
        JSR     L1091            ; 105C: BD 10 91
        PULA                     ; 105F: 32
        STAA    $77              ; 1060: 97 77
        RTS                      ; 1062: 39
        FCB     $3A,$01 ; ':.'
        FCC     "PAGE"
        FCB     $04            ; 1065: 50 41 47 45 0x04 terminator

        ORG     $106B
        FCC     "        M6800 FORTRAN RELEASE 1.0"
        FCB     $04            ; 106B: 20 20 20 20 20 20 20 20 4D 36 38 30 30 20 46 4F 52 54 52 41 4E 20 52 45 4C 45 41 53 45 20 31 2E 30 0x04 terminator
        FCB     $2D,$2D,$2D,$04 ; '---.'

; Listing/page-management helper.
; Prints headings and keeps the page / column counters in sync.
L1091:
        LDAA    $01C3            ; 1091: B6 01 C3
        BLT     L10DD            ; 1094: 2D 47
        LDAA    $1063            ; 1096: B6 10 63
        CMPA    #$3A             ; 1099: 81 3A
        BNE     L10D7            ; 109B: 26 3A
        STX     $0643            ; 109D: FF 06 43
L10A0:
        JSR     L10DE            ; 10A0: BD 10 DE
        JSR     L10DE            ; 10A3: BD 10 DE
        JSR     L10DE            ; 10A6: BD 10 DE
        LDX     #$108D           ; 10A9: CE 10 8D
        JSR     L10DA            ; 10AC: BD 10 DA
        JSR     L10DE            ; 10AF: BD 10 DE
        JSR     L10DE            ; 10B2: BD 10 DE
        CLRA                     ; 10B5: 4F
        LDAB    $1064            ; 10B6: F6 10 64
        LDX     #$1069           ; 10B9: CE 10 69
        JSR     L2670            ; 10BC: BD 26 70
        LDX     #$2020           ; 10BF: CE 20 20
        STX     $1069            ; 10C2: FF 10 69
        LDX     #$1065           ; 10C5: CE 10 65
        JSR     L10DA            ; 10C8: BD 10 DA
        INC     $1064            ; 10CB: 7C 10 64
        JSR     L10DE            ; 10CE: BD 10 DE
        CLR     $1063            ; 10D1: 7F 10 63
        LDX     $0643            ; 10D4: FE 06 43
L10D7:
        INC     $1063            ; 10D7: 7C 10 63
L10DA:
        JMP     L10DD            ; 10DA: 7E 10 DD
L10DD:
        RTS                      ; 10DD: 39
L10DE:
        LDX     #$108C           ; 10DE: CE 10 8C
        JSR     L10DA            ; 10E1: BD 10 DA
        RTS                      ; 10E4: 39

; Read a source line from the monitor input service into the line buffer.
; Handles erase/end-of-line characters and appends the internal 0x04 terminator.
L10E5:
        LDAA    #$3F             ; 10E5: 86 3F
L10E7:
        PSHA                     ; 10E7: 36
        JSR     L26B9            ; 10E8: BD 26 B9
        LDX     #$0000           ; 10EB: CE 00 00
        STX     $73              ; 10EE: DF 73
        STX     $75              ; 10F0: DF 75
        PULA                     ; 10F2: 32
L10F3:
        JSR     L26B3            ; 10F3: BD 26 B3
L10F6:
        STX     $4B              ; 10F6: DF 4B
        JSR     L26CA            ; 10F8: BD 26 CA
        LDX     $4B              ; 10FB: DE 4B
        CMPA    #$7F             ; 10FD: 81 7F
        BEQ     L1115            ; 10FF: 27 14
        CMPA    #$18             ; 1101: 81 18
        BEQ     L1111            ; 1103: 27 0C
        STAA    $78,X            ; 1105: A7 78
        INX                      ; 1107: 08
        CMPA    #$0D             ; 1108: 81 0D
        BNE     L10F6            ; 110A: 26 EA
        LDAA    #$04             ; 110C: 86 04
        STAA    $77,X            ; 110E: A7 77
        RTS                      ; 1110: 39
L1111:
        LDAA    #$20             ; 1111: 86 20
        BRA     L10E7            ; 1113: 20 D2
L1115:
        DEX                      ; 1115: 09
        BMI     L1111            ; 1116: 2B F9
        LDAA    #$5C             ; 1118: 86 5C
        BRA     L10F3            ; 111A: 20 D7

; Walk the statement descriptor list built from the keyword tables.
; Each descriptor is interpreted by L1205/L121E and routed to the proper handler.
L111C:
        LDX     $127D            ; 111C: FE 12 7D
        STX     $02CB            ; 111F: FF 02 CB
        CLR     $0138            ; 1122: 7F 01 38
        CLR     $02CD            ; 1125: 7F 02 CD
        LDAA    #$01             ; 1128: 86 01
        STAA    $02CE            ; 112A: B7 02 CE
L112D:
        JSR     L1205            ; 112D: BD 12 05
        LDX     $02CF            ; 1130: FE 02 CF
        LDAA    $03,X            ; 1133: A6 03
        ANDA    #$80             ; 1135: 84 80
        BNE     L113C            ; 1137: 26 03
        JMP     L1167            ; 1139: 7E 11 67
L113C:
        LDAA    #$01             ; 113C: 86 01
        LDX     $02CB            ; 113E: FE 02 CB
        STAA    $00,X            ; 1141: A7 00
        LDAA    $02CE            ; 1143: B6 02 CE
        STAA    $14,X            ; 1146: A7 14
        INX                      ; 1148: 08
        STX     $02CB            ; 1149: FF 02 CB
        LDX     $127B            ; 114C: FE 12 7B
        CPX     $02CB            ; 114F: BC 02 CB
        BNE     L1159            ; 1152: 26 05
        LDAA    #$04             ; 1154: 86 04
        JSR     L0D7F            ; 1156: BD 0D 7F
L1159:
        CLR     $02CD            ; 1159: 7F 02 CD
        LDX     $02CF            ; 115C: FE 02 CF
        LDAA    $00,X            ; 115F: A6 00
        STAA    $02CE            ; 1161: B7 02 CE
        JMP     L112D            ; 1164: 7E 11 2D
L1167:
        LDX     $02CF            ; 1167: FE 02 CF
        LDAA    $00,X            ; 116A: A6 00
        CMPA    $4D              ; 116C: 91 4D
        BNE     L1173            ; 116E: 26 03
        JMP     L11B0            ; 1170: 7E 11 B0
L1173:
        LDX     $02CF            ; 1173: FE 02 CF
        LDAA    $01,X            ; 1176: A6 01
        STAA    $02CE            ; 1178: B7 02 CE
        CLR     $02CD            ; 117B: 7F 02 CD
        TSTA                     ; 117E: 4D
        BEQ     L1184            ; 117F: 27 03
        JMP     L11C1            ; 1181: 7E 11 C1
L1184:
        LDX     $127D            ; 1184: FE 12 7D
        CPX     $02CB            ; 1187: BC 02 CB
        BNE     L118F            ; 118A: 26 03
        JMP     L11AB            ; 118C: 7E 11 AB
L118F:
        LDX     $02CB            ; 118F: FE 02 CB
        DEX                      ; 1192: 09
        STX     $02CB            ; 1193: FF 02 CB
        LDAA    $00,X            ; 1196: A6 00
        STAA    $02CD            ; 1198: B7 02 CD
        LDAA    $14,X            ; 119B: A6 14
        STAA    $02CE            ; 119D: B7 02 CE
        JSR     L1205            ; 11A0: BD 12 05
        LDAA    $02CD            ; 11A3: B6 02 CD
        BEQ     L11AB            ; 11A6: 27 03
        JMP     L1173            ; 11A8: 7E 11 73
L11AB:
        LDAA    #$05             ; 11AB: 86 05
        JSR     L0D7F            ; 11AD: BD 0D 7F
L11B0:
        JSR     L121E            ; 11B0: BD 12 1E
        JSR     L0B83            ; 11B3: BD 0B 83
L11B6:
        CLR     $02CD            ; 11B6: 7F 02 CD
        LDX     $02CF            ; 11B9: FE 02 CF
        LDAA    $02,X            ; 11BC: A6 02
        STAA    $02CE            ; 11BE: B7 02 CE
L11C1:
        LDAA    $02CE            ; 11C1: B6 02 CE
        CMPA    #$FF             ; 11C4: 81 FF
        BEQ     L11CB            ; 11C6: 27 03
        JMP     L112D            ; 11C8: 7E 11 2D
L11CB:
        LDX     $02CB            ; 11CB: FE 02 CB
        DEX                      ; 11CE: 09
        STX     $02CB            ; 11CF: FF 02 CB
        LDX     $1279            ; 11D2: FE 12 79
        CPX     $02CB            ; 11D5: BC 02 CB
        BNE     L11DB            ; 11D8: 26 01
        RTS                      ; 11DA: 39
L11DB:
        LDX     $02CB            ; 11DB: FE 02 CB
        LDAA    $00,X            ; 11DE: A6 00
        STAA    $02CD            ; 11E0: B7 02 CD
        LDAA    $14,X            ; 11E3: A6 14
        STAA    $02CE            ; 11E5: B7 02 CE
        LDAA    $02CD            ; 11E8: B6 02 CD
        BNE     L11F0            ; 11EB: 26 03
        JMP     L11CB            ; 11ED: 7E 11 CB
L11F0:
        JSR     L1205            ; 11F0: BD 12 05
        JSR     L121E            ; 11F3: BD 12 1E
        CLR     $02CD            ; 11F6: 7F 02 CD
        LDX     $127D            ; 11F9: FE 12 7D
        CPX     $02CB            ; 11FC: BC 02 CB
        BEQ     L1204            ; 11FF: 27 03
        JMP     L11B6            ; 1201: 7E 11 B6
L1204:
        RTS                      ; 1204: 39

; Map a 1-based statement descriptor index in $02CE to its table entry.
L1205:
        LDAA    $02CE            ; 1205: B6 02 CE
        DECA                     ; 1208: 4A
        CLRB                     ; 1209: 5F
        ASLA                     ; 120A: 48
        ADCB    #$00             ; 120B: C9 00
        ASLB                     ; 120D: 58
        ASLA                     ; 120E: 48
        ADCB    #$00             ; 120F: C9 00
        ADDA    $02A2            ; 1211: BB 02 A2
        STAA    $02D0            ; 1214: B7 02 D0
        ADCB    $02A1            ; 1217: F9 02 A1
        STAB    $02CF            ; 121A: F7 02 CF
        RTS                      ; 121D: 39

; Interpret one statement descriptor entry and optionally queue code generation.
L121E:
        LDX     $02CF            ; 121E: FE 02 CF
        LDAA    $03,X            ; 1221: A6 03
        ANDA    #$7F             ; 1223: 84 7F
        STAA    $3F              ; 1225: 97 3F
        BNE     L122A            ; 1227: 26 01
        RTS                      ; 1229: 39
L122A:
        LDAA    $3F              ; 122A: 96 3F
        ANDA    #$0F             ; 122C: 84 0F
        STAA    $3E              ; 122E: 97 3E
        LDAA    $3F              ; 1230: 96 3F
        ASRA                     ; 1232: 47
        ASRA                     ; 1233: 47
        ASRA                     ; 1234: 47
        ASRA                     ; 1235: 47
        STAA    $3F              ; 1236: 97 3F
        LDAA    $01C3            ; 1238: B6 01 C3
        CMPA    #$02             ; 123B: 81 02
        BLT     L1253            ; 123D: 2D 14
        LDAA    $3F              ; 123F: 96 3F
        LDAB    $3E              ; 1241: D6 3E
        LDX     #$0DD0           ; 1243: CE 0D D0
        JSR     L2DF7            ; 1246: BD 2D F7
        LDAA    #$04             ; 1249: 86 04
        STAA    $00,X            ; 124B: A7 00
        LDX     $1277            ; 124D: FE 12 77
        JSR     L1091            ; 1250: BD 10 91
L1253:
        LDAA    $3E              ; 1253: 96 3E
        BRA     L1269            ; 1255: 20 12
        FCB     $12,$95,$16,$F0,$19,$4D,$1E,$F1 ; '.....M..'
        FCB     $20,$C0,$22,$C2,$23,$D1,$0D,$7F ; ' .".#...'
        FCB     $0D,$7F
L1269:
        LDAB    $3F              ; 1269: D6 3F
        ASLB                     ; 126B: 58
        LDX     #$1255           ; 126C: CE 12 55
        JSR     L2567            ; 126F: BD 25 67
        LDX     $00,X            ; 1272: EE 00
        JMP     $00,X            ; 1274: 6E 00
        FCB     $39,$0D,$D0,$02,$A2,$02,$B7,$02 ; '9.......'
        FCB     $A3,$00,$49,$00,$47,$00,$52,$00 ; '..I.G.R.'
        FCB     $42,$00,$41,$BD,$00,$01,$9F,$00 ; 'B.A.....'
        FCB     $61,$01,$11,$01,$31,$00,$00 ; 'a...1..'

; Statement handler dispatcher.
; The jump table below fans out to the major FORTRAN statement compilers.
; These aliases are tentative, but this cluster looks heavily skewed toward
; declaration/dimension/common-data handling plus the end-of-compilation pass.
; Working family map:
;   declaration/data side: STMT_DECLARE_OR_REFERENCE, STMT_DECLARATION_SETUP,
;     STMT_DIMENSION_STORE_BOUND, STMT_DIMENSION_OR_COMMON_FINALIZE,
;     STMT_DATA_OR_COMMON_PREP, STMT_DATA_OR_COMMON_EMIT_FIELDS, STMT_REQUIRE_SYMBOL_ENTRY
;   summary/fixup side: STMT_TRANSFER_FIXUP_MERGE, STMT_END_AND_SYMBOL_LIST, STMT_SET_FIXUP_BASE
;   codegen side: STMT_TRANSFER_CODEGEN_DISPATCH plus STMT_SOURCE_IO_OR_TRANSFER_DISPATCH
; Best current guess:
;   DIMENSION/COMMON/DATA/FORMAT cluster lives mainly in the declaration/data side above.
;   READ/PRINT/WRITE likely flow through STMT_SOURCE_IO_OR_TRANSFER_DISPATCH.
;   GOTO/IF/CALL/RETURN likely lean on STMT_TRANSFER_CODEGEN_DISPATCH and fixup helpers.
STMT_DISPATCH:
L1295:
        STAA    $3E              ; 1295: 97 3E
        BRA     L12B7            ; 1297: 20 1E
        FCB     $12,$C4,$13,$10,$13,$16,$13,$82
        FCB     $13,$A3,$14,$33,$14,$36,$14,$67 ; '...3.6.g'
        FCB     $14,$9F,$14,$AD,$14,$D6,$16,$DA
        FCB     $16,$DF,$16,$E0,$16,$E6
L12B7:
        LDAB    $3E              ; 12B7: D6 3E
        ASLB                     ; 12B9: 58
        LDX     #$1297           ; 12BA: CE 12 97
        JSR     L2567            ; 12BD: BD 25 67
        LDX     $00,X            ; 12C0: EE 00
        JMP     $00,X            ; 12C2: 6E 00

; Allocate / finalize a symbol reference entry for the current identifier.
; One of the early statement handlers: resolves the current symbol,
; creates a table entry if needed, and seeds the generated descriptor fields.
STMT_DECLARE_OR_REFERENCE:
L12C4:
        LDAA    #$08             ; 12C4: 86 08
        JSR     L0A4B            ; 12C6: BD 0A 4B
        LDAA    $54              ; 12C9: 96 54
        BGT     L12D0            ; 12CB: 2E 03
        JSR     L12F8            ; 12CD: BD 12 F8
L12D0:
        LDX     $55              ; 12D0: DE 55
        LDAA    $00,X            ; 12D2: A6 00
        ANDA    #$80             ; 12D4: 84 80
        BEQ     L12DD            ; 12D6: 27 05
        LDAA    #$0A             ; 12D8: 86 0A
        JSR     L0D7F            ; 12DA: BD 0D 7F
L12DD:
        LDX     $55              ; 12DD: DE 55
        LDAA    $00,X            ; 12DF: A6 00
        ORAA    #$80             ; 12E1: 8A 80
        STAA    $00,X            ; 12E3: A7 00
        LDX     $03,X            ; 12E5: EE 03
        STX     $47              ; 12E7: DF 47
        LDAA    #$B2             ; 12E9: 86 B2
        LDAB    #$04             ; 12EB: C6 04
        LDX     $52              ; 12ED: DE 52
        JSR     L0F08            ; 12EF: BD 0F 08
        LDX     $5F              ; 12F2: DE 5F
        STX     $038E            ; 12F4: FF 03 8E
        RTS                      ; 12F7: 39
L12F8:
        LDAA    #$08             ; 12F8: 86 08
        JSR     L0ADF            ; 12FA: BD 0A DF
        LDAB    $61              ; 12FD: D6 61
        LDAA    $62              ; 12FF: 96 62
        LDX     $55              ; 1301: DE 55
        STAB    $03,X            ; 1303: E7 03
        STAA    $04,X            ; 1305: A7 04
        ADDA    #$02             ; 1307: 8B 02
        ADCB    #$00             ; 1309: C9 00
        STAA    $62              ; 130B: 97 62
        STAB    $61              ; 130D: D7 61
        RTS                      ; 130F: 39

; Report a syntax/semantic error 0x1B.
STMT_DIAG_1B:
L1310:
        LDAA    #$1B             ; 1310: 86 1B
        JSR     L0EEE            ; 1312: BD 0E EE
        RTS                      ; 1315: 39

; Set symbol attributes for the current declaration or assignment context.
STMT_DECLARATION_SETUP:
L1316:
        LDAA    $54              ; 1316: 96 54
        BLE     L1334            ; 1318: 2F 1A
        LDX     $55              ; 131A: DE 55
        LDAA    $00,X            ; 131C: A6 00
        ANDA    #$20             ; 131E: 84 20
        CMPA    #$20             ; 1320: 81 20
        BEQ     L132B            ; 1322: 27 07
        LDAA    #$0B             ; 1324: 86 0B
        JSR     L0D7F            ; 1326: BD 0D 7F
        BRA     L1334            ; 1329: 20 09
L132B:
        LDAA    $00,X            ; 132B: A6 00
        ORAA    #$40             ; 132D: 8A 40
        STAA    $00,X            ; 132F: A7 00
        JMP     L1339            ; 1331: 7E 13 39
L1334:
        LDAA    #$50             ; 1334: 86 50
        JSR     L0ADF            ; 1336: BD 0A DF
L1339:
        LDAA    #$01             ; 1339: 86 01
        STAA    $40              ; 133B: 97 40
        LDAB    $2517            ; 133D: F6 25 17
        LDAA    $2518            ; 1340: B6 25 18
        LDX     $55              ; 1343: DE 55
        STAB    $09,X            ; 1345: E7 09
        STAA    $0A,X            ; 1347: A7 0A
        STAB    $07,X            ; 1349: E7 07
        STAA    $08,X            ; 134B: A7 08
        LDAA    #$40             ; 134D: 86 40
        STAA    $41              ; 134F: 97 41
        LDX     $2515            ; 1351: FE 25 15
        CPX     $65              ; 1354: 9C 65
        BNE     L1366            ; 1356: 26 0E
        LDAA    #$C0             ; 1358: 86 C0
        STAA    $41              ; 135A: 97 41
        LDX     $55              ; 135C: DE 55
        LDAA    $63              ; 135E: 96 63
        LDAB    $64              ; 1360: D6 64
        STAA    $0B,X            ; 1362: A7 0B
        STAB    $0C,X            ; 1364: E7 0C
L1366:
        LDX     $55              ; 1366: DE 55
        LDAA    $00,X            ; 1368: A6 00
        ORAA    $41              ; 136A: 9A 41
        STAA    $00,X            ; 136C: A7 00
        JSR     L1E95            ; 136E: BD 1E 95
        LDAA    $57              ; 1371: 96 57
        BNE     L137C            ; 1373: 26 07
        LDX     $2513            ; 1375: FE 25 13
        STX     $47              ; 1378: DF 47
        BRA     L1381            ; 137A: 20 05
L137C:
        LDX     $2511            ; 137C: FE 25 11
        STX     $47              ; 137F: DF 47
L1381:
        RTS                      ; 1381: 39

; Store one computed offset/extent pair into the current symbol entry.
; Used by declaration-style handlers after expression extents are evaluated.
STMT_DIMENSION_STORE_BOUND:
L1382:
        LDAB    $47              ; 1382: D6 47
        LDAA    $48              ; 1384: 96 48
        LDX     $55              ; 1386: DE 55
        JSR     L2519            ; 1388: BD 25 19
        BRA     L1390            ; 138B: 20 03
        FCB     $02,$00,$40 ; '..@'
L1390:
        STAB    $05,X            ; 1390: E7 05
        STAA    $06,X            ; 1392: A7 06
        LDX     $47              ; 1394: DE 47
        STX     $4B              ; 1396: DF 4B
        JSR     L1A4E            ; 1398: BD 1A 4E
        STAB    $47              ; 139B: D7 47
        STAA    $48              ; 139D: 97 48
        INC     $0040            ; 139F: 7C 00 40
        RTS                      ; 13A2: 39

; Finalize address/extent data for the current declared symbol.
; Links the symbol to the active code/fixup pointer and emits descriptor bytes.
STMT_DIMENSION_OR_COMMON_FINALIZE:
L13A3:
        LDX     $2517            ; 13A3: FE 25 17
        STX     $4B              ; 13A6: DF 4B
        LDX     $55              ; 13A8: DE 55
        LDAA    $00,X            ; 13AA: A6 00
        ANDA    #$20             ; 13AC: 84 20
        BNE     L13DC            ; 13AE: 26 2C
        LDAB    $61              ; 13B0: D6 61
        LDAA    $62              ; 13B2: 96 62
        STAB    $03,X            ; 13B4: E7 03
        STAA    $04,X            ; 13B6: A7 04
        LDAA    #$02             ; 13B8: 86 02
        STAA    $3E              ; 13BA: 97 3E
        LDX     $250F            ; 13BC: FE 25 0F
        CPX     $65              ; 13BF: 9C 65
        BNE     L13CC            ; 13C1: 26 09
        LDX     $250D            ; 13C3: FE 25 0D
        STX     $4B              ; 13C6: DF 4B
        LDAA    #$03             ; 13C8: 86 03
        STAA    $3E              ; 13CA: 97 3E
L13CC:
        LDAA    $4C              ; 13CC: 96 4C
        LDAB    $4B              ; 13CE: D6 4B
        LDX     $65              ; 13D0: DE 65
        ADDA    $01,X            ; 13D2: AB 01
        ADCB    $00,X            ; 13D4: E9 00
        STAB    $4B              ; 13D6: D7 4B
        STAA    $4C              ; 13D8: 97 4C
        BRA     L13ED            ; 13DA: 20 11
L13DC:
        LDAA    #$03             ; 13DC: 86 03
        STAA    $3E              ; 13DE: 97 3E
        LDX     $61              ; 13E0: DE 61
        STX     $0E21            ; 13E2: FF 0E 21
        LDX     $55              ; 13E5: DE 55
        LDX     $03,X            ; 13E7: EE 03
        STX     $4B              ; 13E9: DF 4B
        STX     $61              ; 13EB: DF 61
L13ED:
        LDAB    $3E              ; 13ED: D6 3E
        LDX     $4B              ; 13EF: DE 4B
        JSR     L193B            ; 13F1: BD 19 3B
        LDX     $47              ; 13F4: DE 47
        JSR     L193F            ; 13F6: BD 19 3F
        LDAA    #$01             ; 13F9: 86 01
L13FB:
        STAA    $3E              ; 13FB: 97 3E
        LDX     $55              ; 13FD: DE 55
        JSR     L2519            ; 13FF: BD 25 19
        BRA     L1407            ; 1402: 20 03
        FCB     $02,$00,$3E ; '..>'
L1407:
        LDX     $05,X            ; 1407: EE 05
        JSR     L193F            ; 1409: BD 19 3F
        LDAA    $3E              ; 140C: 96 3E
        CMPA    #$03             ; 140E: 81 03
        BCC     L1416            ; 1410: 24 04
        INCA                     ; 1412: 4C
        JMP     L13FB            ; 1413: 7E 13 FB
L1416:
        LDX     $55              ; 1416: DE 55
        LDAA    $00,X            ; 1418: A6 00
        ANDA    #$20             ; 141A: 84 20
        BNE     L142D            ; 141C: 26 0F
        LDX     $65              ; 141E: DE 65
        LDAA    $01,X            ; 1420: A6 01
        LDAB    $00,X            ; 1422: E6 00
        ADDA    $48              ; 1424: 9B 48
        ADCB    $47              ; 1426: D9 47
        STAB    $00,X            ; 1428: E7 00
        STAA    $01,X            ; 142A: A7 01
        RTS                      ; 142C: 39
L142D:
        LDX     $0E21            ; 142D: FE 0E 21
        STX     $61              ; 1430: DF 61
        RTS                      ; 1432: 39

; Abort the current statement and return to the main source-line loop.
STMT_ABORT_LINE:
L1433:
        JMP     L25D1            ; 1433: 7E 25 D1

; Push the current symbol/value descriptor and derive a base address for later codegen.
; Used before emitting array/string extents or storage-allocation fields.
STMT_DATA_OR_COMMON_PREP:
L1436:
        LDX     $55              ; 1436: DE 55
        LDAA    $00,X            ; 1438: A6 00
        ANDA    #$80             ; 143A: 84 80
        BEQ     L1443            ; 143C: 27 05
        LDAA    #$0D             ; 143E: 86 0D
        JSR     L0D7F            ; 1440: BD 0D 7F
L1443:
        LDAA    #$01             ; 1443: 86 01
        JSR     L1DD5            ; 1445: BD 1D D5
        LDX     $59              ; 1448: DE 59
        LDX     $01,X            ; 144A: EE 01
        STX     $47              ; 144C: DF 47
        LDX     $55              ; 144E: DE 55
        LDAA    $00,X            ; 1450: A6 00
        ANDA    #$40             ; 1452: 84 40
        BEQ     L1464            ; 1454: 27 0E
        LDAA    $48              ; 1456: 96 48
        LDAB    $47              ; 1458: D6 47
        ADDA    $250E            ; 145A: BB 25 0E
        ADCB    $250D            ; 145D: F9 25 0D
        STAB    $47              ; 1460: D7 47
        STAA    $48              ; 1462: 97 48
L1464:
        JMP     L1499            ; 1464: 7E 14 99

; Emit one or two extent/address fields from the descriptor on the expression stack.
; The second field is emitted only for wider or multiword descriptors.
STMT_DATA_OR_COMMON_EMIT_FIELDS:
L1467:
        LDX     $59              ; 1467: DE 59
        LDAA    $00,X            ; 1469: A6 00
        ASRA                     ; 146B: 47
        ASRA                     ; 146C: 47
        ASRA                     ; 146D: 47
        ASRA                     ; 146E: 47
        ASRA                     ; 146F: 47
        ASRA                     ; 1470: 47
        ANDA    #$02             ; 1471: 84 02
        ADDA    #$02             ; 1473: 8B 02
        STAA    $3E              ; 1475: 97 3E
        LDAA    #$B2             ; 1477: 86 B2
        LDX     $0385            ; 1479: FE 03 85
        JSR     L1941            ; 147C: BD 19 41
        LDX     $47              ; 147F: DE 47
        INX                      ; 1481: 08
        INX                      ; 1482: 08
        STX     $47              ; 1483: DF 47
        LDAA    $3E              ; 1485: 96 3E
        CMPA    #$04             ; 1487: 81 04
        BNE     L1499            ; 1489: 26 0E
        LDAA    #$B2             ; 148B: 86 B2
        LDX     $0387            ; 148D: FE 03 87
        JSR     L1941            ; 1490: BD 19 41
        LDX     $47              ; 1493: DE 47
        INX                      ; 1495: 08
        INX                      ; 1496: 08
        STX     $47              ; 1497: DF 47
L1499:
        CLR     $0380            ; 1499: 7F 03 80
        JMP     L1E8A            ; 149C: 7E 1E 8A

; Ensure the current identifier has a symbol-table entry, creating it if needed.
STMT_REQUIRE_SYMBOL_ENTRY:
L149F:
        LDAA    #$08             ; 149F: 86 08
        JSR     L0A4B            ; 14A1: BD 0A 4B
        LDAA    $54              ; 14A4: 96 54
        BLE     L14A9            ; 14A6: 2F 01
        RTS                      ; 14A8: 39
L14A9:
        JSR     L12F8            ; 14A9: BD 12 F8
        RTS                      ; 14AC: 39

; Deduplicate a patched address against the last recorded fixup location.
; If the same location recurs, dispatches through L20C0 to merge or update it.
STMT_TRANSFER_FIXUP_MERGE:
L14AD:
        LDX     $2517            ; 14AD: FE 25 17
        CPX     $038E            ; 14B0: BC 03 8E
        BNE     L14B6            ; 14B3: 26 01
        RTS                      ; 14B5: 39
L14B6:
        LDX     $0139            ; 14B6: FE 01 39
        LDAB    $00,X            ; 14B9: E6 00
        LDAA    $01,X            ; 14BB: A6 01
        CMPB    $038E            ; 14BD: F1 03 8E
        BNE     L14CF            ; 14C0: 26 0D
        CMPA    $038F            ; 14C2: B1 03 8F
        BNE     L14CF            ; 14C5: 26 08
        LDAA    #$09             ; 14C7: 86 09
        JSR     L20C0            ; 14C9: BD 20 C0
        JMP     L14B6            ; 14CC: 7E 14 B6
L14CF:
        LDX     $2517            ; 14CF: FE 25 17
        STX     $038E            ; 14D2: FF 03 8E
        RTS                      ; 14D5: 39

; End-of-compilation symbol summary/listing pass.
; Walks the per-letter symbol chains, formats names and values into the listing buffer,
; and likely emits the 'UNDEFINED SYMBOLS ... ERRORS' report.
; The two nested loops first print symbol names, then print associated numeric values/page counts.
STMT_END_AND_SYMBOL_LIST:
L14D6:
        LDAA    L2716            ; 14D6: B6 27 16
        CMPA    #$39             ; 14D9: 81 39
        BEQ     L14F1            ; 14DB: 27 14
        LDAA    $0649            ; 14DD: B6 06 49
        BNE     L14E7            ; 14E0: 26 05
        LDAA    #$1B             ; 14E2: 86 1B
        JSR     L0EEE            ; 14E4: BD 0E EE
L14E7:
        LDAA    #$0F             ; 14E7: 86 0F
        LDAB    #$01             ; 14E9: C6 01
        LDX     #$0000           ; 14EB: CE 00 00
        JSR     L0F08            ; 14EE: BD 0F 08
L14F1:
        LDAA    $01C3            ; 14F1: B6 01 C3
        BGE     L14F9            ; 14F4: 2C 03
        JMP     L15D5            ; 14F6: 7E 15 D5
L14F9:
        CLR     $0040            ; 14F9: 7F 00 40
        LDX     $250B            ; 14FC: FE 25 0B
        JSR     L1091            ; 14FF: BD 10 91
        LDX     $2509            ; 1502: FE 25 09
        JSR     L1091            ; 1505: BD 10 91
        LDX     $2507            ; 1508: FE 25 07
        STX     $0E21            ; 150B: FF 0E 21
        LDAA    #$01             ; 150E: 86 01
L1510:
        STAA    $3E              ; 1510: 97 3E
        LDX     #$019F           ; 1512: CE 01 9F
        JSR     L2519            ; 1515: BD 25 19
        BRA     L151D            ; 1518: 20 03
        FCB     $02,$00,$3E ; '..>'
L151D:
        LDX     $00,X            ; 151D: EE 00
        STX     $43              ; 151F: DF 43
L1521:
        LDX     $2517            ; 1521: FE 25 17
        CPX     $43              ; 1524: 9C 43
        BNE     L152B            ; 1526: 26 03
        JMP     L15CB            ; 1528: 7E 15 CB
L152B:
        LDX     $43              ; 152B: DE 43
        LDAA    $00,X            ; 152D: A6 00
        ANDA    #$10             ; 152F: 84 10
        BNE     L1536            ; 1531: 26 03
        JMP     L15C2            ; 1533: 7E 15 C2
L1536:
        INX                      ; 1536: 08
        INX                      ; 1537: 08
        INX                      ; 1538: 08
        STX     $47              ; 1539: DF 47
        LDX     $43              ; 153B: DE 43
        LDAA    $00,X            ; 153D: A6 00
        ANDA    #$07             ; 153F: 84 07
        STAA    $3F              ; 1541: 97 3F
        LDAA    $00,X            ; 1543: A6 00
        ANDA    #$60             ; 1545: 84 60
        BEQ     L1553            ; 1547: 27 0A
        INX                      ; 1549: 08
        INX                      ; 154A: 08
        INX                      ; 154B: 08
        INX                      ; 154C: 08
        INX                      ; 154D: 08
        INX                      ; 154E: 08
        INX                      ; 154F: 08
        INX                      ; 1550: 08
        STX     $43              ; 1551: DF 43
L1553:
        LDAA    $40              ; 1553: 96 40
        CMPA    #$05             ; 1555: 81 05
        BNE     L156F            ; 1557: 26 16
        LDAA    #$04             ; 1559: 86 04
        LDX     $0E21            ; 155B: FE 0E 21
        STAA    $00,X            ; 155E: A7 00
        LDX     $2507            ; 1560: FE 25 07
        JSR     L1091            ; 1563: BD 10 91
        CLR     $0040            ; 1566: 7F 00 40
        LDX     $2507            ; 1569: FE 25 07
        STX     $0E21            ; 156C: FF 0E 21
L156F:
        INC     $0040            ; 156F: 7C 00 40
        LDAA    #$01             ; 1572: 86 01
L1574:
        STAA    $41              ; 1574: 97 41
        CMPA    $3F              ; 1576: 91 3F
        BGT     L1585            ; 1578: 2E 0B
        LDX     $43              ; 157A: DE 43
        LDAA    $05,X            ; 157C: A6 05
        LDX     $0E21            ; 157E: FE 0E 21
        STAA    $00,X            ; 1581: A7 00
        BRA     L158C            ; 1583: 20 07
L1585:
        LDAA    #$20             ; 1585: 86 20
        LDX     $0E21            ; 1587: FE 0E 21
        STAA    $00,X            ; 158A: A7 00
L158C:
        LDX     $43              ; 158C: DE 43
        INX                      ; 158E: 08
        STX     $43              ; 158F: DF 43
        LDX     $0E21            ; 1591: FE 0E 21
        INX                      ; 1594: 08
        STX     $0E21            ; 1595: FF 0E 21
        LDAA    $41              ; 1598: 96 41
        CMPA    #$07             ; 159A: 81 07
        BCC     L15A2            ; 159C: 24 04
        INCA                     ; 159E: 4C
        JMP     L1574            ; 159F: 7E 15 74
L15A2:
        LDX     $47              ; 15A2: DE 47
        LDAA    $00,X            ; 15A4: A6 00
        LDAB    $01,X            ; 15A6: E6 01
        LDX     $0E21            ; 15A8: FE 0E 21
        JSR     L2DF7            ; 15AB: BD 2D F7
        LDAB    #$05             ; 15AE: C6 05
        LDAA    #$20             ; 15B0: 86 20
L15B2:
        STAA    $00,X            ; 15B2: A7 00
        INX                      ; 15B4: 08
        DECB                     ; 15B5: 5A
        BNE     L15B2            ; 15B6: 26 FA
        STX     $0E21            ; 15B8: FF 0E 21
        LDX     $47              ; 15BB: DE 47
        DEX                      ; 15BD: 09
        DEX                      ; 15BE: 09
        DEX                      ; 15BF: 09
        STX     $43              ; 15C0: DF 43
L15C2:
        LDX     $43              ; 15C2: DE 43
        LDX     $01,X            ; 15C4: EE 01
        STX     $43              ; 15C6: DF 43
        JMP     L1521            ; 15C8: 7E 15 21
L15CB:
        LDAA    $3E              ; 15CB: 96 3E
        CMPA    #$10             ; 15CD: 81 10
        BCC     L15D5            ; 15CF: 24 04
        INCA                     ; 15D1: 4C
        JMP     L1510            ; 15D2: 7E 15 10
L15D5:
        LDAA    $40              ; 15D5: 96 40
        BEQ     L15E6            ; 15D7: 27 0D
        LDAA    #$04             ; 15D9: 86 04
        LDX     $0E21            ; 15DB: FE 0E 21
        STAA    $00,X            ; 15DE: A7 00
        LDX     $2507            ; 15E0: FE 25 07
        JSR     L1091            ; 15E3: BD 10 91
L15E6:
        LDX     $2507            ; 15E6: FE 25 07
        STX     $0E21            ; 15E9: FF 0E 21
        LDAA    #$FF             ; 15EC: 86 FF
        STAA    $69              ; 15EE: 97 69
        LDAA    #$01             ; 15F0: 86 01
L15F2:
        STAA    $3E              ; 15F2: 97 3E
        LDX     #$019F           ; 15F4: CE 01 9F
        JSR     L2519            ; 15F7: BD 25 19
        BRA     L15FF            ; 15FA: 20 03
        FCB     $02,$00,$3E ; '..>'
L15FF:
        LDX     $00,X            ; 15FF: EE 00
        STX     $43              ; 1601: DF 43
L1603:
        LDX     $2517            ; 1603: FE 25 17
        CPX     $43              ; 1606: 9C 43
        BNE     L160D            ; 1608: 26 03
        JMP     L168F            ; 160A: 7E 16 8F
L160D:
        LDX     $43              ; 160D: DE 43
        LDAA    $00,X            ; 160F: A6 00
        ANDA    #$80             ; 1611: 84 80
        BNE     L161B            ; 1613: 26 06
        LDAA    $00,X            ; 1615: A6 00
        CMPA    #$0A             ; 1617: 81 0A
        BEQ     L161E            ; 1619: 27 03
L161B:
        JMP     L1686            ; 161B: 7E 16 86
L161E:
        INC     $0068            ; 161E: 7C 00 68
        LDAA    $69              ; 1621: 96 69
        CMPA    #$FF             ; 1623: 81 FF
        BEQ     L162A            ; 1625: 27 03
        JMP     L163F            ; 1627: 7E 16 3F
L162A:
        LDX     $250B            ; 162A: FE 25 0B
        JSR     L1091            ; 162D: BD 10 91
        LDX     $2503            ; 1630: FE 25 03
        JSR     L1091            ; 1633: BD 10 91
        LDX     $2507            ; 1636: FE 25 07
        STX     $0E21            ; 1639: FF 0E 21
        JMP     L1658            ; 163C: 7E 16 58
L163F:
        LDAA    $69              ; 163F: 96 69
        CMPA    #$0A             ; 1641: 81 0A
        BEQ     L1648            ; 1643: 27 03
        JMP     L165B            ; 1645: 7E 16 5B
L1648:
        LDAA    #$04             ; 1648: 86 04
        LDX     $0E21            ; 164A: FE 0E 21
        STAA    $00,X            ; 164D: A7 00
        LDX     #$0DD0           ; 164F: CE 0D D0
        STX     $0E21            ; 1652: FF 0E 21
        JSR     L1091            ; 1655: BD 10 91
L1658:
        CLR     $0069            ; 1658: 7F 00 69
L165B:
        INC     $0069            ; 165B: 7C 00 69
        LDX     $43              ; 165E: DE 43
        LDAA    $05,X            ; 1660: A6 05
        LDAB    $06,X            ; 1662: E6 06
        LDX     $0E21            ; 1664: FE 0E 21
        STX     $037D            ; 1667: FF 03 7D
        JSR     L2670            ; 166A: BD 26 70
        LDAB    #$20             ; 166D: C6 20
        STAB    $00,X            ; 166F: E7 00
        INX                      ; 1671: 08
        STX     $0E21            ; 1672: FF 0E 21
        LDX     $037D            ; 1675: FE 03 7D
L1678:
        LDAA    $00,X            ; 1678: A6 00
        CMPA    #$30             ; 167A: 81 30
        BNE     L1686            ; 167C: 26 08
        STAB    $00,X            ; 167E: E7 00
        INX                      ; 1680: 08
        CPX     $0E21            ; 1681: BC 0E 21
        BNE     L1678            ; 1684: 26 F2
L1686:
        LDX     $43              ; 1686: DE 43
        LDX     $01,X            ; 1688: EE 01
        STX     $43              ; 168A: DF 43
        JMP     L1603            ; 168C: 7E 16 03
L168F:
        LDAA    $3E              ; 168F: 96 3E
        CMPA    #$10             ; 1691: 81 10
        BCC     L1699            ; 1693: 24 04
        INCA                     ; 1695: 4C
        JMP     L15F2            ; 1696: 7E 15 F2
L1699:
        LDAA    $69              ; 1699: 96 69
        BLS     L16AA            ; 169B: 23 0D
        LDAA    #$04             ; 169D: 86 04
        LDX     $0E21            ; 169F: FE 0E 21
        STAA    $00,X            ; 16A2: A7 00
        LDX     $2507            ; 16A4: FE 25 07
        JSR     L1091            ; 16A7: BD 10 91
L16AA:
        LDX     $250B            ; 16AA: FE 25 0B
        JSR     L1091            ; 16AD: BD 10 91
        LDAA    #$00             ; 16B0: 86 00
        LDAB    $68              ; 16B2: D6 68
        LDX     $2501            ; 16B4: FE 25 01
        JSR     L2670            ; 16B7: BD 26 70
        LDX     $2501            ; 16BA: FE 25 01
        JSR     L1091            ; 16BD: BD 10 91
        LDX     $16AB            ; 16C0: FE 16 AB
        LDX     $00,X            ; 16C3: EE 00
        JSR     L1091            ; 16C5: BD 10 91
L16C8:
        LDAA    $1063            ; 16C8: B6 10 63
        CMPA    #$3A             ; 16CB: 81 3A
        BEQ     L16D7            ; 16CD: 27 08
        LDX     $250B            ; 16CF: FE 25 0B
        JSR     L1091            ; 16D2: BD 10 91
        BRA     L16C8            ; 16D5: 20 F1
L16D7:
        JMP     L097C            ; 16D7: 7E 09 7C

; Seed FIXUP_PTR from the current code-generation base pointer.
STMT_SET_FIXUP_BASE:
L16DA:
        LDX     $2515            ; 16DA: FE 25 15
        STX     $65              ; 16DD: DF 65

; No-op / early-return helper used by the statement dispatcher.
STMT_NOOP:
L16DF:
        RTS                      ; 16DF: 39

; Report diagnostic 0x1E.
STMT_DIAG_1E:
L16E0:
        LDAA    #$1E             ; 16E0: 86 1E
        JSR     L0EEE            ; 16E2: BD 0E EE
        RTS                      ; 16E5: 39

; Emit a 3-byte descriptor referenced from the top of the expression stack.
STMT_EMIT_STACK_DESC:
L16E6:
        LDAB    #$03             ; 16E6: C6 03
        LDX     $59              ; 16E8: DE 59
        LDX     $01,X            ; 16EA: EE 01
        JSR     L1937            ; 16EC: BD 19 37
        RTS                      ; 16EF: 39

; Late statement-handler dispatcher for code emission and fixup resolution.
; This cluster appears after primary parsing and mainly drives object-output templates.
; It looks more transfer/fixup-oriented than L22C2: many entries feed pending branch/call
; locations into the template/object layer rather than walking source-I/O item lists.
; Strongest current statement mapping:
;   L1719  assigned/indirect transfer via a stored target address
;   L1763  computed-GOTO-style multi-target dispatcher
;   L1899  operand-free transfer, most likely RETURN
STMT_TRANSFER_CODEGEN_DISPATCH:
L16F0:
        STAA    $3E              ; 16F0: 97 3E
        BRA     L170C            ; 16F2: 20 18
        FCB     $17,$19,$17,$34,$17,$3B,$17,$63 ; '...4.;.c'
        FCB     $17,$F9,$18,$5E,$18,$64,$18,$92 ; '...^.d..'
        FCB     $18,$99,$18,$A0,$18,$CE,$18,$F3
L170C:
        LDAB    $3E              ; 170C: D6 3E
        ASLB                     ; 170E: 58
        LDX     #$16F2           ; 170F: CE 16 F2
        JSR     L2567            ; 1712: BD 25 67
        LDX     $00,X            ; 1715: EE 00
        JMP     $00,X            ; 1717: 6E 00

; Transfer subentry 0: emit a simple 3-byte symbol/address reference followed by a fixed trailer template.
; The emitted FE..6E00 pattern looks like 'load X from variable; JMP 0,X'.
; Best current fit: assigned/indirect GOTO through a stored statement address.
TRANSFER_EMIT_ASSIGNED_GOTO:
L1719:
        LDAA    #$09             ; 1719: 86 09
        JSR     L1295            ; 171B: BD 12 95
        LDX     $24FF            ; 171E: FE 24 FF
        JSR     L1945            ; 1721: BD 19 45
        LDAB    #$03             ; 1724: C6 03
        LDX     $55              ; 1726: DE 55
        LDX     $03,X            ; 1728: EE 03
        JSR     L1937            ; 172A: BD 19 37
        LDX     $24FD            ; 172D: FE 24 FD
        JSR     L1949            ; 1730: BD 19 49
        RTS                      ; 1733: 39

; Transfer subentry 1: initialize the temporary transfer-target list pointer at $0390.
TRANSFER_LIST_INIT:
L1734:
        LDX     $24FB            ; 1734: FE 24 FB
        STX     $0390            ; 1737: FF 03 90
        RTS                      ; 173A: 39

; Transfer subentry 2: append the current symbol target address to the list at $0390.
; Raises diagnostic 0x14 if the temporary target-list buffer reaches its limit.
TRANSFER_LIST_APPEND_SYMBOL:
L173B:
        LDX     $0390            ; 173B: FE 03 90
        INX                      ; 173E: 08
        INX                      ; 173F: 08
        STX     $0390            ; 1740: FF 03 90
        LDX     $24F9            ; 1743: FE 24 F9
        CPX     $0390            ; 1746: BC 03 90
        BNE     L1750            ; 1749: 26 05
        LDAA    #$14             ; 174B: 86 14
        JSR     L0D7F            ; 174D: BD 0D 7F
L1750:
        LDAA    #$09             ; 1750: 86 09
        JSR     L1295            ; 1752: BD 12 95
        LDX     $55              ; 1755: DE 55
        LDAB    $03,X            ; 1757: E6 03
        LDAA    $04,X            ; 1759: A6 04
        LDX     $0390            ; 175B: FE 03 90
        STAB    $00,X            ; 175E: E7 00
        STAA    $01,X            ; 1760: A7 01
        RTS                      ; 1762: 39

; Transfer subentry 3: build and emit the inline multi-target transfer scaffold.
; This is the strongest computed-GOTO candidate in the transfer cluster: it emits
; a dispatcher skeleton and then serializes the accumulated target list from $0390.
TRANSFER_EMIT_COMPUTED_GOTO:
L1763:
        LDX     $59              ; 1763: DE 59
        LDX     $01,X            ; 1765: EE 01
        INX                      ; 1767: 08
        STX     $43              ; 1768: DF 43
        LDX     $24F7            ; 176A: FE 24 F7
        JSR     L1945            ; 176D: BD 19 45
        LDAB    #$03             ; 1770: C6 03
        LDX     $43              ; 1772: DE 43
        JSR     L1937            ; 1774: BD 19 37
        LDAA    $53              ; 1777: 96 53
        LDAB    $52              ; 1779: D6 52
        ADDA    #$16             ; 177B: 8B 16
        ADCB    #$00             ; 177D: C9 00
        STAB    $43              ; 177F: D7 43
        STAA    $44              ; 1781: 97 44
        LDX     $24F5            ; 1783: FE 24 F5
        JSR     L1945            ; 1786: BD 19 45
        LDAB    #$04             ; 1789: C6 04
        LDX     $43              ; 178B: DE 43
        JSR     L1937            ; 178D: BD 19 37
        LDAA    $0391            ; 1790: B6 03 91
        SUBA    $24F4            ; 1793: B0 24 F4
        ADDA    #$02             ; 1796: 8B 02
        ASRA                     ; 1798: 47
        STAA    $0E22            ; 1799: B7 0E 22
        LDAA    #$C6             ; 179C: 86 C6
        STAA    $0E21            ; 179E: B7 0E 21
        LDX     $0E21            ; 17A1: FE 0E 21
        JSR     L1949            ; 17A4: BD 19 49
        LDX     #$1100           ; 17A7: CE 11 00
        JSR     L1945            ; 17AA: BD 19 45
        LDX     #$2F01           ; 17AD: CE 2F 01
        JSR     L1949            ; 17B0: BD 19 49
        LDX     #$174A           ; 17B3: CE 17 4A
        JSR     L1949            ; 17B6: BD 19 49
        LDX     #$2F04           ; 17B9: CE 2F 04
        JSR     L1949            ; 17BC: BD 19 49
        LDX     #$0808           ; 17BF: CE 08 08
        JSR     L1949            ; 17C2: BD 19 49
        LDX     #$20F9           ; 17C5: CE 20 F9
        JSR     L1949            ; 17C8: BD 19 49
        LDX     #$EE00           ; 17CB: CE EE 00
        JSR     L1949            ; 17CE: BD 19 49
        LDX     #$EE00           ; 17D1: CE EE 00
        JSR     L1949            ; 17D4: BD 19 49
        LDX     #$6E00           ; 17D7: CE 6E 00
        JSR     L1949            ; 17DA: BD 19 49
        LDX     $24F3            ; 17DD: FE 24 F3
L17E0:
        STX     $43              ; 17E0: DF 43
        LDAB    #$03             ; 17E2: C6 03
        LDX     $00,X            ; 17E4: EE 00
        JSR     L1937            ; 17E6: BD 19 37
        LDX     $43              ; 17E9: DE 43
        CPX     $0390            ; 17EB: BC 03 90
        BEQ     L17F8            ; 17EE: 27 08
        LDAB    #$02             ; 17F0: C6 02
        JSR     L2567            ; 17F2: BD 25 67
        JMP     L17E0            ; 17F5: 7E 17 E0
L17F8:
        RTS                      ; 17F8: 39

; Transfer subentry 4: resolve or create one transfer target symbol and emit its current code address.
; Likely used by a plain one-target transfer statement when the destination may still be undefined.
TRANSFER_RESOLVE_AND_EMIT_TARGET:
L17F9:
        LDAA    $4E              ; 17F9: 96 4E
        ORAA    #$E0             ; 17FB: 8A E0
        STAA    $41              ; 17FD: 97 41
        LDAB    #$01             ; 17FF: C6 01
        LDX     $24F1            ; 1801: FE 24 F1
        JSR     L0F08            ; 1804: BD 0F 08
        LDAA    $54              ; 1807: 96 54
        BLE     L1824            ; 1809: 2F 19
        LDX     $55              ; 180B: DE 55
        LDAA    $00,X            ; 180D: A6 00
        ANDA    #$80             ; 180F: 84 80
        BEQ     L181A            ; 1811: 27 07
        LDAA    #$15             ; 1813: 86 15
        JSR     L0D7F            ; 1815: BD 0D 7F
        BRA     L1822            ; 1818: 20 08
L181A:
        LDX     $55              ; 181A: DE 55
        LDAA    $00,X            ; 181C: A6 00
        ORAA    #$80             ; 181E: 8A 80
        STAA    $00,X            ; 1820: A7 00
L1822:
        BRA     L1833            ; 1822: 20 0F
L1824:
        LDAA    #$30             ; 1824: 86 30
        JSR     L0ADF            ; 1826: BD 0A DF
        LDAB    $61              ; 1829: D6 61
        LDAA    $62              ; 182B: 96 62
        LDX     $55              ; 182D: DE 55
        STAB    $03,X            ; 182F: E7 03
        STAA    $04,X            ; 1831: A7 04
L1833:
        LDX     $24EF            ; 1833: FE 24 EF
        JSR     L1945            ; 1836: BD 19 45
        LDAB    #$03             ; 1839: C6 03
        LDX     $61              ; 183B: DE 61
        JSR     L1937            ; 183D: BD 19 37
        LDAA    #$02             ; 1840: 86 02
        STAA    $62              ; 1842: 97 62
        RTS                      ; 1844: 39
L1845:
        LDX     $55              ; 1845: DE 55
        INX                      ; 1847: 08
        INX                      ; 1848: 08
        INX                      ; 1849: 08
        INX                      ; 184A: 08
        INX                      ; 184B: 08
        STX     $47              ; 184C: DF 47
        LDX     $55              ; 184E: DE 55
        LDAA    $00,X            ; 1850: A6 00
        ANDA    #$07             ; 1852: 84 07
        STAA    $41              ; 1854: 97 41
        LDAB    #$01             ; 1856: C6 01
        LDX     $47              ; 1858: DE 47
        JSR     L0F08            ; 185A: BD 0F 08
        RTS                      ; 185D: 39

; Transfer subentry 5: report diagnostic 0x0F.
TRANSFER_DIAG_0F:
L185E:
        LDAA    #$0F             ; 185E: 86 0F
        JSR     L0EEE            ; 1860: BD 0E EE
        RTS                      ; 1863: 39

; Transfer subentry 6: create/resolve a direct target symbol, emit its address, and advance CODE_PTR.
; Best current fit: direct one-target transfer or call form that reserves a fixed-width inline target field.
TRANSFER_DEFINE_AND_EMIT_TARGET:
L1864:
        LDAA    $54              ; 1864: 96 54
        BLE     L186D            ; 1866: 2F 05
        LDAA    #$16             ; 1868: 86 16
        JSR     L0D7F            ; 186A: BD 0D 7F
L186D:
        LDAA    #$30             ; 186D: 86 30
        JSR     L0ADF            ; 186F: BD 0A DF
        LDAB    $61              ; 1872: D6 61
        LDAA    $62              ; 1874: 96 62
        LDX     $55              ; 1876: DE 55
        STAB    $03,X            ; 1878: E7 03
        STAA    $04,X            ; 187A: A7 04
        LDAB    #$03             ; 187C: C6 03
        LDX     $61              ; 187E: DE 61
        JSR     L1937            ; 1880: BD 19 37
        LDAA    $62              ; 1883: 96 62
        LDAB    $61              ; 1885: D6 61
        ADDA    $250E            ; 1887: BB 25 0E
        ADCB    $250D            ; 188A: F9 25 0D
        STAB    $61              ; 188D: D7 61
        STAA    $62              ; 188F: 97 62
        RTS                      ; 1891: 39

; Transfer subentry 7: emit the current CODE_PTR through the template layer.
TRANSFER_EMIT_CODE_PTR:
L1892:
        LDX     $2517            ; 1892: FE 25 17
        JSR     L1949            ; 1895: BD 19 49
        RTS                      ; 1898: 39

; Transfer subentry 8: emit the fixed template at $24ED.
; The fixed word at $24ED is 0x3900, so this is the clearest RETURN-style emitter in the cluster.
TRANSFER_EMIT_RETURN:
L1899:
        LDX     $24ED            ; 1899: FE 24 ED
        JSR     L1945            ; 189C: BD 19 45
        RTS                      ; 189F: 39

; Transfer subentry 9: allocate a numbered local transfer target and append its address to the list at $0390.
; Strong candidate for the label-list half of a computed/assigned transfer statement.
TRANSFER_ALLOC_LOCAL_TARGET:
L18A0:
        LDAA    #$18             ; 18A0: 86 18
        JSR     L0A4B            ; 18A2: BD 0A 4B
        LDAA    $54              ; 18A5: 96 54
        BGT     L18BA            ; 18A7: 2E 11
        LDAA    #$18             ; 18A9: 86 18
        JSR     L0ADF            ; 18AB: BD 0A DF
        LDAA    $67              ; 18AE: 96 67
        LDX     $55              ; 18B0: DE 55
        STAA    $03,X            ; 18B2: A7 03
        INC     $0067            ; 18B4: 7C 00 67
        JSR     L1845            ; 18B7: BD 18 45
L18BA:
        LDX     $24F3            ; 18BA: FE 24 F3
        STX     $0390            ; 18BD: FF 03 90
        LDX     $55              ; 18C0: DE 55
        LDAB    $03,X            ; 18C2: E6 03
        LDAA    $04,X            ; 18C4: A6 04
        LDX     $0390            ; 18C6: FE 03 90
        STAB    $00,X            ; 18C9: E7 00
        STAA    $01,X            ; 18CB: A7 01
        RTS                      ; 18CD: 39

; Transfer subentry 10: append the current expression descriptor to the transfer list buffer.
; Pairs naturally with the computed/assigned transfer helpers around L18A0/L18F3.
TRANSFER_LIST_APPEND_EXPR_DESC:
L18CE:
        LDX     $0390            ; 18CE: FE 03 90
        INX                      ; 18D1: 08
        INX                      ; 18D2: 08
        STX     $0390            ; 18D3: FF 03 90
        LDX     $59              ; 18D6: DE 59
        LDAA    $00,X            ; 18D8: A6 00
        LDX     $0390            ; 18DA: FE 03 90
        STAA    $00,X            ; 18DD: A7 00
        INX                      ; 18DF: 08
        STX     $0390            ; 18E0: FF 03 90
        LDX     $59              ; 18E3: DE 59
        LDAB    $01,X            ; 18E5: E6 01
        LDAA    $02,X            ; 18E7: A6 02
        LDX     $0390            ; 18E9: FE 03 90
        STAB    $00,X            ; 18EC: E7 00
        STAA    $01,X            ; 18EE: A7 01
        JMP     L1E8A            ; 18F0: 7E 1E 8A

; Transfer subentry 11: walk the accumulated transfer-list buffer and emit each target entry.
; Likely the final serialization pass for computed/assigned transfer tables.
TRANSFER_LIST_SERIALIZE:
L18F3:
        LDX     $24F3            ; 18F3: FE 24 F3
        STX     $43              ; 18F6: DF 43
        LDX     $24EB            ; 18F8: FE 24 EB
        JSR     L1945            ; 18FB: BD 19 45
        LDX     $43              ; 18FE: DE 43
        LDAB    $00,X            ; 1900: E6 00
        LDX     $2517            ; 1902: FE 25 17
        JSR     L1937            ; 1905: BD 19 37
L1908:
        LDX     $0390            ; 1908: FE 03 90
        CPX     $43              ; 190B: 9C 43
        BNE     L1910            ; 190D: 26 01
        RTS                      ; 190F: 39
L1910:
        LDX     $43              ; 1910: DE 43
        INX                      ; 1912: 08
        INX                      ; 1913: 08
        STX     $43              ; 1914: DF 43
        LDX     $00,X            ; 1916: EE 00
        JSR     L1945            ; 1918: BD 19 45
        LDX     $43              ; 191B: DE 43
        LDAA    $00,X            ; 191D: A6 00
        ASRA                     ; 191F: 47
        ASRA                     ; 1920: 47
        ASRA                     ; 1921: 47
        ASRA                     ; 1922: 47
        ANDA    #$03             ; 1923: 84 03
        NEGA                     ; 1925: 40
        ADDA    #$03             ; 1926: 8B 03
        STAA    $3E              ; 1928: 97 3E
        INX                      ; 192A: 08
        STX     $43              ; 192B: DF 43
        LDAB    $3E              ; 192D: D6 3E
        LDX     $00,X            ; 192F: EE 00
        JSR     L1937            ; 1931: BD 19 37
        JMP     L1908            ; 1934: 7E 19 08
L1937:
        LDAA    #$42             ; 1937: 86 42
        BRA     L1942            ; 1939: 20 07
L193B:
        LDAA    #$32             ; 193B: 86 32
        BRA     L1942            ; 193D: 20 03
L193F:
        LDAA    #$32             ; 193F: 86 32
L1941:
        CLRB                     ; 1941: 5F
L1942:
        JMP     L0F08            ; 1942: 7E 0F 08
L1945:
        LDAA    #$41             ; 1945: 86 41
        BRA     L1941            ; 1947: 20 F8
L1949:
        LDAA    #$42             ; 1949: 86 42
        BRA     L1941            ; 194B: 20 F4

; Expression-operator dispatcher.
; Routes parsed operators to code-generation fragments and stack manipulation helpers.
L194D:
        STAA    $3E              ; 194D: 97 3E
        BRA     L196B            ; 194F: 20 1A
        FCB     $19,$78,$19,$83,$19,$B6,$19,$BB ; '.x......'
        FCB     $19,$F6,$1A,$6B,$1A,$B5,$1A,$24 ; '...k...$'
        FCB     $1A,$B9,$1B,$DA,$1D,$AC,$1B,$D2
        FCB     $1B,$76 ; '.v'
L196B:
        LDAB    $3E              ; 196B: D6 3E
        ASLB                     ; 196D: 58
        LDX     #$194F           ; 196E: CE 19 4F
        JSR     L2567            ; 1971: BD 25 67
        LDX     $00,X            ; 1974: EE 00
        JMP     $00,X            ; 1976: 6E 00
        FCB     $86,$10,$BD,$0A,$4B,$86,$01,$BD ; '....K...'
        FCB     $1D,$D5,$39,$7F,$00,$43,$B6,$03 ; '..9..C..'
        FCB     $92,$97,$44,$8B,$04,$B7,$03,$92 ; '..D.....'
        FCB     $FE,$24,$F5,$BD,$19,$45,$DE,$43 ; '.$...E.C'
        FCB     $BD,$19,$49,$86,$FF,$BD,$1D,$D5 ; '..I.....'
        FCB     $D6,$43,$96,$44,$DE,$59,$E7,$01 ; '.C.D.Y..'
        FCB     $A7,$02,$A6,$00,$8A,$30,$A7,$00 ; '.....0..'
        FCB     $86,$0A,$BD,$16,$F0,$39,$96,$4D ; '.....9.M'
        FCB     $97,$40,$39,$86,$01,$BD,$1D,$D5 ; '.@9.....'
        FCB     $86,$01,$97,$40,$B7,$03,$7F,$DE ; '...@....'
        FCB     $59,$FF,$03,$81,$DE,$55,$FF,$03 ; 'Y....U..'
        FCB     $83,$DE,$59,$EE,$01,$DF,$43,$DF ; '..Y...C.'
        FCB     $47,$86,$0A,$5F,$9B,$48,$D9,$47 ; 'G.._.H.G'
        FCB     $97,$48,$D7,$47,$DE,$55,$A6,$00 ; '.H.G.U..'
        FCB     $84,$80,$27,$04,$EE,$0B,$DF,$47 ; '..'....G'
        FCB     $DE,$47,$FF,$0E,$21,$39,$F6,$25 ; '.G..!9.%'
        FCB     $17,$B6,$25,$18,$CE,$03,$85,$BD ; '..%.....'
        FCB     $25,$19,$20,$03,$02,$03,$7F,$E7 ; '%. .....'
        FCB     $00,$A7,$01,$86,$30,$CE,$03,$8B ; '....0...'
        FCB     $F6,$03,$7F,$BD,$25,$62,$A7,$00 ; '....%b..'
        FCB     $7C,$03,$7F,$7A,$00,$60,$7C,$00 ; '|..z.`|.'
        FCB     $40,$7F,$03,$80,$FE,$03,$83,$BD ; '@.......'
        FCB     $25,$19,$20,$03,$02,$03,$7F,$EE ; '%. .....'
        FCB     $03,$DF,$4B,$D6,$4B,$96,$4C,$BD ; '..K.K.L.'
        FCB     $1A,$4E,$7D,$03,$80,$27,$06,$43 ; '.N}..'.C'
        FCB     $53,$89,$00,$C9,$00,$9B,$48,$D9 ; 'S.....H.'
        FCB     $47,$D7,$47,$97,$48,$39 ; 'G.G.H9'

; 16-bit shift/add multiply helper used during expression evaluation.
L1A4E:
        LDX     #$0010           ; 1A4E: CE 00 10
L1A51:
        ASLA                     ; 1A51: 48
        ROLB                     ; 1A52: 59
        ASL     $0060            ; 1A53: 78 00 60
        ROL     $005F            ; 1A56: 79 00 5F
        BCC     L1A5F            ; 1A59: 24 04
        ADDA    $4C              ; 1A5B: 9B 4C
        ADCB    $4B              ; 1A5D: D9 4B
L1A5F:
        DEX                      ; 1A5F: 09
        BNE     L1A51            ; 1A60: 26 EF
        TSTB                     ; 1A62: 5D
        BMI     L1A66            ; 1A63: 2B 01
        RTS                      ; 1A65: 39
L1A66:
        LDAA    #$0C             ; 1A66: 86 0C
        JSR     L0D7F            ; 1A68: BD 0D 7F
        LDAA    #$01             ; 1A6B: 86 01
        JSR     L1DD5            ; 1A6D: BD 1D D5
        LDX     $59              ; 1A70: DE 59
        LDAA    $00,X            ; 1A72: A6 00
        ANDA    #$40             ; 1A74: 84 40
        BEQ     L1A7E            ; 1A76: 27 06
        LDAA    #$01             ; 1A78: 86 01
        STAA    $3F              ; 1A7A: 97 3F
        BRA     L1A81            ; 1A7C: 20 03
L1A7E:
        CLR     $003F            ; 1A7E: 7F 00 3F
L1A81:
        LDAA    $3F              ; 1A81: 96 3F
        CLRB                     ; 1A83: 5F
        TSTA                     ; 1A84: 4D
        BGE     L1A88            ; 1A85: 2C 01
        COMB                     ; 1A87: 53
L1A88:
        LDX     $59              ; 1A88: DE 59
        ADDA    $02,X            ; 1A8A: AB 02
        ADCB    $01,X            ; 1A8C: E9 01
        LDX     #$0385           ; 1A8E: CE 03 85
        JSR     L2519            ; 1A91: BD 25 19
        BRA     L1A99            ; 1A94: 20 03
        FCB     $02,$03,$7F
L1A99:
        STAB    $00,X            ; 1A99: E7 00
        STAA    $01,X            ; 1A9B: A7 01
        LDX     $59              ; 1A9D: DE 59
        LDAA    $00,X            ; 1A9F: A6 00
        LDX     #$038B           ; 1AA1: CE 03 8B
        LDAB    $037F            ; 1AA4: F6 03 7F
        JSR     L2562            ; 1AA7: BD 25 62
        STAA    $00,X            ; 1AAA: A7 00
        INC     $037F            ; 1AAC: 7C 03 7F
        CLR     $0380            ; 1AAF: 7F 03 80
        JMP     L1E8A            ; 1AB2: 7E 1E 8A
        FCB     $7C,$03,$80,$39,$BD,$1B,$BE,$B6 ; '|..9....'
        FCB     $03,$7F,$91,$42,$27,$05,$86,$1F ; '...B'...'
        FCB     $BD,$0D,$7F,$B6,$03,$7F,$91,$40 ; '.......@'
        FCB     $26,$1C,$FE,$03,$83,$A6,$00,$84 ; '&.......'
        FCB     $20,$26,$13,$D6,$47,$96,$48,$DE ; ' &..G.H.'
        FCB     $59,$E7,$01,$A7,$02,$DE,$59,$A6 ; 'Y.....Y.'
        FCB     $00,$84,$BF,$A7,$00,$39,$86,$09 ; '.....9..'
        FCB     $BD,$0E,$EE,$96,$48,$D6,$47,$B0 ; '....H.G.'
        FCB     $0E,$22,$F2,$0E,$21,$D7,$47,$97 ; '."..!.G.'
        FCB     $48,$DE,$47,$BD,$19,$49,$FE,$03 ; 'H.G..I..'
        FCB     $81,$A6,$00
        FCC     "GGGG"
        FCB     $04            ; 1B08: 47 47 47 47 0x04 terminator
        FCB     $84,$03,$40,$8B,$03,$97,$40,$C6 ; '..@...@.'
        FCB     $03,$EE,$01,$BD,$19,$37,$96,$58 ; '.....7.X'
        FCB     $48,$8B,$20,$97,$3F,$DE,$3F,$BD ; 'H. .?.?.'
        FCB     $19,$45,$FE,$03,$81,$A6,$00,$8A ; '.E......'
        FCB     $71,$A7,$00,$6F,$01,$96,$3F,$A7 ; 'q..o..?.'
        FCB     $02,$7A,$03,$7F,$86,$01,$97,$3E ; '.z.....>'
        FCB     $CE,$03,$85,$BD,$25,$19,$20,$03 ; '....%. .'
        FCB     $02,$00,$3E,$EE,$00,$DF,$47,$CE ; '..>...G.'
        FCB     $03,$8B,$D6,$3E,$BD,$25,$62,$A6 ; '...>.%b.'
        FCB     $00
        FCC     "GGGG"
        FCB     $04            ; 1B55: 47 47 47 47 0x04 terminator
        FCB     $84,$03,$40,$8B,$03,$97,$40,$D6 ; '..@...@.'
        FCB     $40,$DE,$47,$BD,$19,$37,$96,$3E ; '@.G..7.>'
        FCB     $B1,$03,$7F,$24,$04,$4C,$7E,$1B ; '...$.L~.'
        FCB     $3A,$7C,$03,$7F,$39,$FE,$03,$83 ; ':|..9...'
        FCB     $A6,$00,$84,$40,$27,$07,$B6,$03 ; '...@'...'
        FCB     $7F,$81,$01,$27,$01,$39,$B6,$01 ; '...'.9..'
        FCB     $38,$26,$05,$86,$1F,$BD,$0D,$7F ; '8&......'
        FCB     $BD,$1B,$BE,$96,$42,$97,$40,$B7 ; '....B.@.'
        FCB     $03,$7F,$FE,$25,$17,$FF,$03,$89 ; '...%....'
        FCB     $FF,$03,$87,$FF,$03,$85,$FE,$03
        FCB     $81,$A6,$00,$8A,$41,$A7,$00,$B6 ; '....A...'
        FCB     $01,$38,$81,$02,$27,$03,$7E,$1A ; '.8..'.~.'
        FCB     $D8,$39,$7E,$1A,$D8,$86,$04,$FE ; '.9~.....'
        FCB     $03,$83,$E6,$09,$EA,$0A,$26,$06 ; '......&.'
        FCB     $4A,$EE,$07,$26,$01,$4A,$97,$42 ; 'J..&.J.B'
        FCB     $39,$FE,$25,$17,$DF,$5F,$7A,$00 ; '9.%.._z.'
        FCB     $4F,$86,$00,$BD,$1D,$D5,$DE,$5F ; 'O......_'
        FCB     $FF,$03,$85,$96,$4F,$4A,$B7,$1D ; '....OJ..'
        FCB     $9C,$BD,$1D,$27,$C1,$2E,$27,$0D ; '...'..'.'
        FCB     $B6,$03,$80,$27,$07,$B6,$03,$85 ; '...'....'
        FCB     $40,$B7,$03,$85,$39,$FE,$1D,$9B ; '@...9...'
        FCB     $C6,$41,$E1,$73,$2E,$05,$E1,$74 ; '.A.s...t'
        FCB     $2E,$01,$39,$DE,$5F,$FF,$1D,$A2 ; '..9._...'
        FCB     $CE,$00,$00,$FF,$1D,$A0,$7F,$1D
        FCB     $9F,$7F,$1D,$9D,$7F,$1D,$9E,$BD
        FCB     $1D,$27,$25,$0B,$BD,$1D,$0B,$BD ; '.'%.....'
        FCB     $1D,$16,$7A,$1D,$9D,$20,$F0,$C1 ; '..z.. ..'
        FCB     $45,$26,$1E,$4F,$BD,$1D,$27,$25 ; 'E&.O..'%'
        FCB     $0C,$B7,$1D,$A8,$48,$48,$BB,$1D ; '....HH..'
        FCB     $A8,$48,$1B,$20,$EF,$7D,$1D,$9F ; '.H. .}..'
        FCB     $27,$01,$40,$BB,$1D,$9D,$B7,$1D ; ''.@.....'
        FCB     $9D,$6F,$04,$6F,$05,$6F,$06,$6F ; '.o.o.o.o'
        FCB     $07,$A6,$00,$AA,$01,$AA,$02,$AA
        FCB     $03,$27,$7E,$86,$04,$64,$00,$66 ; '.'~..d.f'
        FCB     $01,$66,$02,$66,$03,$66,$04,$66 ; '.f.f.f.f'
        FCB     $05,$66,$06,$66,$07,$4A,$26,$ED ; '.f.f.J&.'
        FCB     $7C,$1D,$9E,$A6,$00,$AA,$01,$AA ; '|.......'
        FCB     $02,$AA,$03,$26,$DE,$F6,$1D,$9D ; '...&....'
        FCB     $2F,$0D,$CE,$1D,$A3,$8D,$7B,$CE ; '/.....{.'
        FCB     $1D,$A0,$7A,$1D,$9D,$20,$E4,$27 ; '..z.. .''
        FCB     $48,$CE,$1D,$A4,$BD,$1D,$69,$8A ; 'H.....i.'
        FCB     $20,$B7,$1D,$AB,$6F,$00,$6F,$01 ; ' ...o.o.'
        FCB     $6F,$02,$6F,$03,$BD,$1D,$5C,$C6 ; 'o.o...\.'
        FCB     $08,$BD,$1D,$5C,$BD,$1D,$5C,$BD ; '...\..\.'
        FCB     $1D,$5C,$BD,$1D,$7E,$BD,$1D,$5C ; '.\..~..\'
        FCB     $BD,$1D,$7E,$5A,$26,$EB,$A6,$00 ; '..~Z&...'
        FCB     $84,$F0,$26,$10,$7A,$1D,$9E,$C6 ; '..&.z...'
        FCB     $04,$68,$03,$69,$02,$69,$01,$69 ; '.h.i.i.i'
        FCB     $00,$5A,$26,$F5,$7C,$1D,$9D,$26 ; '.Z&.|..&'
        FCB     $BB,$B6,$1D,$9E,$84,$7F,$F6,$03
        FCB     $80,$27,$02,$8A,$80,$B7,$1D,$A3 ; '.'......'
        FCB     $B6,$1D,$9C,$97,$4F,$FE,$1D,$A3 ; '....O...'
        FCB     $FF,$03,$85,$FE,$1D,$A5,$FF,$03
        FCB     $87,$DE,$59,$A6,$00,$8A,$80,$A7 ; '..Y.....'
        FCB     $00,$39,$8D,$5C,$8D,$44,$8D,$42 ; '.9.\.D.B'
        FCB     $8D,$6B,$8D,$3E,$39,$EB,$03,$E7 ; '.k.>9...'
        FCB     $03,$24,$0A,$6C,$02,$26,$06,$6C ; '.$.l.&.l'
        FCB     $01,$26,$02,$6C,$00,$39,$FE,$1D ; '.&.l.9..'
        FCB     $9B,$E6,$73,$08,$C1,$20,$27,$F9 ; '..s.. '.'
        FCB     $C1,$2D,$26,$0B,$E6,$71,$C0,$45 ; '.-&..q.E'
        FCB     $26,$05,$7C,$1D,$9F,$20,$EA,$FF ; '&.|.. ..'
        FCB     $1D,$9B,$CE,$1D,$A0,$C1,$30,$2D ; '......0-'
        FCB     $07,$C1,$39,$2E,$03,$C0,$30,$39 ; '..9...09'
        FCB     $0D,$39,$68,$03,$69,$02,$69,$01 ; '.9h.i.i.'
        FCB     $69,$00,$39,$74,$1D,$A8,$76,$1D ; 'i.9t..v.'
        FCB     $A9,$76,$1D,$AA,$76,$1D,$AB,$39 ; '.v..v..9'
        FCB     $A6,$00,$B7,$1D,$A8,$A6,$01,$B7
        FCB     $1D,$A9,$A6,$02,$B7,$1D,$AA,$A6
        FCB     $03,$B7,$1D,$AB,$39,$B6,$1D,$AB ; '....9...'
        FCB     $AB,$03,$A7,$03,$B6,$1D,$AA,$A9
        FCB     $02,$A7,$02,$B6,$1D,$A9,$A9,$01
        FCB     $A7,$01,$B6,$1D,$A8,$A9,$00,$A7
        FCB     $00,$39,$00,$00,$00,$00,$00 ; '.9.....'

        ORG     $1DAC

; Commit the current expression result descriptor back into the symbol/reference entry.
L1DAC:
        LDX     $59              ; 1DAC: DE 59
        LDAA    $00,X            ; 1DAE: A6 00
        ASRA                     ; 1DB0: 47
        ASRA                     ; 1DB1: 47
        ASRA                     ; 1DB2: 47
        ASRA                     ; 1DB3: 47
        ASRA                     ; 1DB4: 47
        ASRA                     ; 1DB5: 47
        ANDA    #$02             ; 1DB6: 84 02
        STAA    $3F              ; 1DB8: 97 3F
        LDAB    $61              ; 1DBA: D6 61
        LDAA    $62              ; 1DBC: 96 62
        STAB    $01,X            ; 1DBE: E7 01
        STAA    $02,X            ; 1DC0: A7 02
        LDX     $0385            ; 1DC2: FE 03 85
        JSR     L193F            ; 1DC5: BD 19 3F
        LDAA    $3F              ; 1DC8: 96 3F
        CMPA    #$02             ; 1DCA: 81 02
        BNE     L1DD4            ; 1DCC: 26 06
        LDX     $0387            ; 1DCE: FE 03 87
        JSR     L193F            ; 1DD1: BD 19 3F
L1DD4:
        RTS                      ; 1DD4: 39

; Push one descriptor onto the parser's temporary stack at $59.
; Descriptors are three bytes wide and carry type / address / attribute bits.
L1DD5:
        STAA    $42              ; 1DD5: 97 42
        LDAA    $58              ; 1DD7: 96 58
        CMPA    #$0C             ; 1DD9: 81 0C
        BNE     L1DE2            ; 1DDB: 26 05
        LDAA    #$1E             ; 1DDD: 86 1E
        JSR     L0D7F            ; 1DDF: BD 0D 7F
L1DE2:
        INC     $0058            ; 1DE2: 7C 00 58
        LDX     $59              ; 1DE5: DE 59
        INX                      ; 1DE7: 08
        INX                      ; 1DE8: 08
        INX                      ; 1DE9: 08
        STX     $59              ; 1DEA: DF 59
        CLR     $00,X            ; 1DEC: 6F 00
        CLR     $01,X            ; 1DEE: 6F 01
        CLR     $02,X            ; 1DF0: 6F 02
        LDAA    $42              ; 1DF2: 96 42
        BNE     L1DF7            ; 1DF4: 26 01
        RTS                      ; 1DF6: 39
L1DF7:
        JSR     L1E95            ; 1DF7: BD 1E 95
        LDAA    $57              ; 1DFA: 96 57
        LDX     $59              ; 1DFC: DE 59
        STAA    $00,X            ; 1DFE: A7 00
        LDAA    $42              ; 1E00: 96 42
        BGT     L1E05            ; 1E02: 2E 01
        RTS                      ; 1E04: 39
L1E05:
        LDAA    $54              ; 1E05: 96 54
        BLE     L1E36            ; 1E07: 2F 2D
        LDX     $55              ; 1E09: DE 55
        LDAB    $03,X            ; 1E0B: E6 03
        LDAA    $04,X            ; 1E0D: A6 04
        LDX     $59              ; 1E0F: DE 59
        STAB    $01,X            ; 1E11: E7 01
        STAA    $02,X            ; 1E13: A7 02
        LDX     $55              ; 1E15: DE 55
        LDAA    $00,X            ; 1E17: A6 00
        ANDA    #$60             ; 1E19: 84 60
        BEQ     L1E25            ; 1E1B: 27 08
        LDX     $59              ; 1E1D: DE 59
        LDAA    $00,X            ; 1E1F: A6 00
        ORAA    #$40             ; 1E21: 8A 40
        STAA    $00,X            ; 1E23: A7 00
L1E25:
        LDX     $55              ; 1E25: DE 55
        LDAA    $00,X            ; 1E27: A6 00
        ANDA    #$80             ; 1E29: 84 80
        BEQ     L1E35            ; 1E2B: 27 08
        LDX     $59              ; 1E2D: DE 59
        LDAA    $00,X            ; 1E2F: A6 00
        ORAA    #$10             ; 1E31: 8A 10
        STAA    $00,X            ; 1E33: A7 00
L1E35:
        RTS                      ; 1E35: 39
L1E36:
        LDAA    #$10             ; 1E36: 86 10
        JSR     L0ADF            ; 1E38: BD 0A DF
        LDX     $59              ; 1E3B: DE 59
        LDAA    $00,X            ; 1E3D: A6 00
        ASRA                     ; 1E3F: 47
        ASRA                     ; 1E40: 47
        ASRA                     ; 1E41: 47
        ASRA                     ; 1E42: 47
        ASRA                     ; 1E43: 47
        ASRA                     ; 1E44: 47
        ANDA    #$02             ; 1E45: 84 02
        ADDA    #$02             ; 1E47: 8B 02
        STAA    $42              ; 1E49: 97 42
        LDX     $65              ; 1E4B: DE 65
        LDX     $00,X            ; 1E4D: EE 00
        STX     $4B              ; 1E4F: DF 4B
        LDAB    $4B              ; 1E51: D6 4B
        LDAA    $4C              ; 1E53: 96 4C
        LDX     $59              ; 1E55: DE 59
        STAB    $01,X            ; 1E57: E7 01
        STAA    $02,X            ; 1E59: A7 02
        LDX     $55              ; 1E5B: DE 55
        STAB    $03,X            ; 1E5D: E7 03
        STAA    $04,X            ; 1E5F: A7 04
        LDAA    $42              ; 1E61: 96 42
        CLRB                     ; 1E63: 5F
        TSTA                     ; 1E64: 4D
        BGE     L1E68            ; 1E65: 2C 01
        COMB                     ; 1E67: 53
L1E68:
        ADDA    $4C              ; 1E68: 9B 4C
        ADCB    $4B              ; 1E6A: D9 4B
        LDX     $65              ; 1E6C: DE 65
        STAB    $00,X            ; 1E6E: E7 00
        STAA    $01,X            ; 1E70: A7 01
        LDX     $2515            ; 1E72: FE 25 15
        CPX     $65              ; 1E75: 9C 65
        BNE     L1E89            ; 1E77: 26 10
        LDX     $55              ; 1E79: DE 55
        LDAA    $00,X            ; 1E7B: A6 00
        ORAA    #$80             ; 1E7D: 8A 80
        STAA    $00,X            ; 1E7F: A7 00
        LDX     $59              ; 1E81: DE 59
        LDAA    $00,X            ; 1E83: A6 00
        ORAA    #$10             ; 1E85: 8A 10
        STAA    $00,X            ; 1E87: A7 00
L1E89:
        RTS                      ; 1E89: 39

; Pop one 3-byte descriptor from the temporary expression stack.
L1E8A:
        DEC     $0058            ; 1E8A: 7A 00 58
        LDX     $59              ; 1E8D: DE 59
        DEX                      ; 1E8F: 09
        DEX                      ; 1E90: 09
        DEX                      ; 1E91: 09
        STX     $59              ; 1E92: DF 59
        RTS                      ; 1E94: 39

; Derive default attribute bits for the current token.
; This decides whether a pushed descriptor should be marked relocatable/immediate.
L1E95:
        CLR     $0057            ; 1E95: 7F 00 57
        LDAA    $4D              ; 1E98: 96 4D
        CMPA    #$02             ; 1E9A: 81 02
        BEQ     L1EA2            ; 1E9C: 27 04
        CMPA    #$05             ; 1E9E: 81 05
        BNE     L1EA3            ; 1EA0: 26 01
L1EA2:
        RTS                      ; 1EA2: 39
L1EA3:
        LDAA    $C4              ; 1EA3: 96 C4
        CMPA    #$49             ; 1EA5: 81 49
        BCS     L1EAD            ; 1EA7: 25 04
        CMPA    #$4E             ; 1EA9: 81 4E
        BLS     L1EB1            ; 1EAB: 23 04
L1EAD:
        LDAA    #$80             ; 1EAD: 86 80
        STAA    $57              ; 1EAF: 97 57
L1EB1:
        RTS                      ; 1EB1: 39

        ORG     $1EB3
        FCB     $0A

; Operator precedence / class table used by the expression parser around L1EF1.
        ORG     $1EC9
        FCB     $03,$04,$04,$04,$04,$04,$02,$01
        FCB     $02,$04,$01,$03,$04,$04,$04,$04
        FCB     $04,$04,$04,$04,$0B,$09,$09,$08
        FCB     $08,$03,$02,$01,$02,$0A,$01,$06
        FCB     $05,$04,$07,$07,$07,$07,$07,$07

; Operator-precedence parser core.
; Uses the precedence table at $1EC9 and a descriptor stack rooted at $1EC7.
L1EF1:
        STAA    $3E              ; 1EF1: 97 3E
        STAA    $1EB2            ; 1EF3: B7 1E B2
        CMPA    #$07             ; 1EF6: 81 07
        BGT     L1EFD            ; 1EF8: 2E 03
        JMP     L1FA0            ; 1EFA: 7E 1F A0
L1EFD:
        LDAA    $3E              ; 1EFD: 96 3E
        SUBA    #$07             ; 1EFF: 80 07
        STAA    $3F              ; 1F01: 97 3F
        BRA     L1F11            ; 1F03: 20 0C
        FCB     $1F,$BD,$1F,$1E,$1F,$28,$1F,$42 ; '.....(.B'
        FCB     $1F,$28,$1F,$97 ; '.(..'
L1F11:
        LDAB    $3F              ; 1F11: D6 3F
        ASLB                     ; 1F13: 58
        LDX     #$1F03           ; 1F14: CE 1F 03
        JSR     L2567            ; 1F17: BD 25 67
        LDX     $00,X            ; 1F1A: EE 00
        JMP     $00,X            ; 1F1C: 6E 00
        FCB     $86,$09,$97,$5E,$FE,$1E,$C7,$A7 ; '...^....'
        FCB     $02,$39,$86,$20,$B7,$03,$92,$86 ; '.9. ....'
        FCB     $FF,$97,$5D,$FE,$24,$E9,$FF,$1E ; '..].$...'
        FCB     $C7,$86,$0A,$97,$5E,$96,$3E,$81 ; '....^.>.'
        FCB     $0C,$26,$01,$39,$96,$58,$B7,$1E ; '.&.9.X..'
        FCB     $B2,$81,$01,$26,$03,$7E,$1F,$C2 ; '...&.~..'
        FCB     $FE,$24,$E7,$DF,$43,$86,$01,$97 ; '.$..C...'
        FCB     $40,$DE,$43,$E6,$01,$A6,$02,$DE ; '@.C.....'
        FCB     $59,$E1,$01,$26,$1E,$A1,$02,$26 ; 'Y..&...&'
        FCB     $1A,$A6,$00,$DE,$43,$A1,$00,$26 ; '....C..&'
        FCB     $12,$DE,$59,$96,$40,$B7,$1E,$B2 ; '..Y.@...'
        FCB     $7A,$00,$58,$09,$09,$09,$DF,$59 ; 'z.X....Y'
        FCB     $7E,$1F,$C2,$7C,$00,$40,$DE,$43 ; '~..|.@.C'
        FCB     $08,$08,$08,$DF,$43,$DE,$59,$9C ; '....C.Y.'
        FCB     $43,$27,$03,$7E,$1F,$57,$7E,$1F ; 'C'.~.W~.'
        FCB     $C2,$96,$C4,$80,$52,$8B,$0B,$B7 ; '....R...'
        FCB     $1E,$B2

; Insert an operator into the precedence stack or collapse pending operators first.
L1FA0:
        CLR     $0380            ; 1FA0: 7F 03 80
        LDX     #$1EDE           ; 1FA3: CE 1E DE
        LDAB    $1EB2            ; 1FA6: F6 1E B2
        JSR     L2562            ; 1FA9: BD 25 62
        LDAA    $00,X            ; 1FAC: A6 00
        LDX     #$1EDE           ; 1FAE: CE 1E DE
        LDAB    $5E              ; 1FB1: D6 5E
        JSR     L2562            ; 1FB3: BD 25 62
        CMPA    $00,X            ; 1FB6: A1 00
        BGT     L1FBD            ; 1FB8: 2E 03
        JMP     L1FCF            ; 1FBA: 7E 1F CF
L1FBD:
        LDAA    $1EB2            ; 1FBD: B6 1E B2
        STAA    $5E              ; 1FC0: 97 5E
        LDX     $1EC7            ; 1FC2: FE 1E C7
        INX                      ; 1FC5: 08
        STX     $1EC7            ; 1FC6: FF 1E C7
        LDAA    $1EB2            ; 1FC9: B6 1E B2
        STAA    $02,X            ; 1FCC: A7 02
        RTS                      ; 1FCE: 39
L1FCF:
        LDX     #$1ECA           ; 1FCF: CE 1E CA
        LDAB    $5E              ; 1FD2: D6 5E
        JSR     L2562            ; 1FD4: BD 25 62
        LDAA    $00,X            ; 1FD7: A6 00
        STAA    $3F              ; 1FD9: 97 3F
        BRA     L1FE5            ; 1FDB: 20 08
        FCB     $1F,$F2,$1F,$F3,$20,$03,$20,$13 ; '.... . .'
L1FE5:
        LDAB    $3F              ; 1FE5: D6 3F
        ASLB                     ; 1FE7: 58
        LDX     #$1FDB           ; 1FE8: CE 1F DB
        JSR     L2567            ; 1FEB: BD 25 67
        LDX     $00,X            ; 1FEE: EE 00
        JMP     $00,X            ; 1FF0: 6E 00
        FCB     $39,$FE,$1E,$C7,$A6,$02,$A7,$01 ; '9.......'
        FCB     $09,$FF,$1E,$C7,$A6,$01,$97,$5E ; '.......^'
        FCB     $39,$FE,$1E,$C7,$A6,$02,$97,$5C ; '9......\'
        FCB     $97,$5B,$09,$FF,$1E,$C7,$7E,$20 ; '.[....~ '
        FCB     $23,$FE,$1E,$C7,$A6,$00,$97,$5B ; '#......['
        FCB     $A6,$02,$97,$5C,$09,$09,$FF,$1E ; '...\....'
        FCB     $C7,$FE,$1E,$C7,$A6,$01,$81,$05
        FCB     $26,$21,$B6,$1E,$B2,$81,$07,$26 ; '&!.....&'
        FCB     $1A,$A6,$00,$97,$5D,$09,$09,$FF ; '....]...'
        FCB     $1E,$C7,$CE,$01,$14,$BD,$25,$19 ; '......%.'
        FCB     $20,$03,$03,$00,$5D,$A6,$00,$97 ; ' ...]...'
        FCB     $40,$20,$3E,$96,$5B,$91,$5D,$27 ; '@ >.[.]''
        FCB     $09,$96,$5C,$91,$5D,$27,$03,$BD ; '..\.]'..'
        FCB     $20,$9F,$CE,$01,$14,$BD,$25,$19 ; ' .....%.'
        FCB     $20,$03,$03,$00,$5B,$A6,$00,$CE ; ' ...[...'
        FCB     $01,$14,$BD,$25,$19,$20,$03,$03 ; '...%. ..'
        FCB     $00,$5C,$AA,$00,$84,$80,$97,$40 ; '.\.....@'
        FCB     $CE,$01,$14,$BD,$25,$19,$20,$03 ; '....%. .'
        FCB     $03,$00,$5D,$A6,$00,$9A,$40,$A7 ; '..]...@.'
        FCB     $00,$96,$5D,$FE,$1E,$C7,$A7,$02 ; '..].....'
        FCB     $BD,$0E,$23,$FE,$1E,$C7,$A6,$01 ; '..#.....'
        FCB     $97,$5E,$7E,$1F,$A0,$86,$00,$BD ; '.^~.....'
        FCB     $1D,$D5,$DE,$59,$6F,$01,$B6,$03 ; '...Yo...'
        FCB     $92,$A7,$02,$A6,$00,$8A,$30,$A7 ; '......0.'
        FCB     $00,$B6,$03,$92,$8B,$04,$B7,$03
        FCB     $92,$96,$58,$97,$5D,$39 ; '..X.]9'

; Secondary statement/expression sub-dispatcher.
; Used by several statement handlers once the lead keyword has been identified.
; The call from L14AD suggests it also merges or rewrites pending fixups.
STMT_SECONDARY_DISPATCH:
L20C0:
        STAA    $3E              ; 20C0: 97 3E
        BRA     L20DA            ; 20C2: 20 16
        FCB     $20,$E7 ; ' .'
        FCC     "!2!O!z!"
        FCB     $04            ; 20C6: 21 32 21 4F 21 7A 21 0x04 terminator
        FCB     $D4,$22,$1B,$22,$81,$22,$60,$22 ; '."."."`"'
        FCB     $84,$21,$CD,$21,$41 ; '.!.!A'
L20DA:
        LDAB    $3E              ; 20DA: D6 3E
        ASLB                     ; 20DC: 58
        LDX     #$20C2           ; 20DD: CE 20 C2
        JSR     L2567            ; 20E0: BD 25 67
        LDX     $00,X            ; 20E3: EE 00
        JMP     $00,X            ; 20E5: 6E 00
        FCB     $86,$03,$BD,$0E,$EE,$DE,$59,$EE ; '......Y.'
        FCB     $00,$BD,$19,$45,$96,$5D,$81,$FF ; '...E.]..'
        FCB     $26,$04,$96,$58,$97,$5D,$CE,$01 ; '&..X.]..'
        FCB     $14,$BD,$25,$19,$20,$03,$03,$00 ; '..%. ...'
        FCB     $5D,$A6,$00 ; ']..'
        FCC     "GGGG"
        FCB     $04            ; 210A: 47 47 47 47 0x04 terminator
        FCB     $84,$03,$40,$8B,$03,$97,$40,$D6 ; '..@...@.'
        FCB     $40,$CE,$01,$15,$BD,$25,$19,$20 ; '@....%. '
        FCB     $03,$03,$00,$5D,$EE,$00,$BD,$19 ; '...]....'
        FCB     $37,$DE,$52,$DF,$50,$FE,$25,$17 ; '7.R.P.%.'
        FCB     $BD,$19,$49,$39,$86,$09,$BD,$12 ; '..I9....'
        FCB     $95,$C6,$03,$DE,$55,$EE,$03,$BD ; '....U...'
        FCB     $19,$37,$39,$DE,$50,$DF,$47,$86 ; '.79.P.G.'
        FCB     $C2,$C6,$04,$DE,$52,$BD,$0F,$08 ; '....R...'
        FCB     $39,$B6,$01,$3A,$F6,$01,$39,$BB ; '9..:..9.'
        FCB     $25,$0E,$F9,$25,$0D,$F7,$01,$39 ; '%..%...9'
        FCB     $B7,$01,$3A,$FE,$24,$E5,$BC,$01 ; '..:.$...'
        FCB     $39,$26,$05,$86,$32,$BD,$0D,$7F ; '9&..2...'
        FCB     $D6,$5F,$96,$60,$FE,$01,$39,$E7 ; '._.`..9.'
        FCB     $00,$A7,$01,$39,$DE,$55,$A6,$00 ; '...9.U..'
        FCB     $84,$20,$27,$05,$86,$34,$BD,$0D ; '. '..4..'
        FCB     $7F,$DE,$59,$E6,$01,$A6,$02,$FE ; '..Y.....'
        FCB     $01,$39,$E7,$02,$A7,$03,$DE,$55 ; '.9.....U'
        FCB     $A6,$00,$84,$20,$27,$11,$FE,$01 ; '... '...'
        FCB     $39,$A6,$03,$E6,$02,$BB,$25,$06 ; '9.....%.'
        FCB     $F9,$25,$05,$A7,$03,$E7,$02,$DE ; '.%......'
        FCB     $59,$A6,$00 ; 'Y..'
        FCC     "GGGG"
        FCB     $04            ; 21B1: 47 47 47 47 0x04 terminator
        FCB     $84,$03,$40,$8B,$03,$FE,$01,$39 ; '..@....9'
        FCB     $A7,$08,$DE,$59,$A6,$00,$84,$80 ; '...Y....'
        FCB     $27,$05,$86,$33,$BD,$0D,$7F,$39 ; ''..3...9'
        FCB     $FE,$01,$39,$EE,$00,$DF,$52,$FE ; '..9...R.'
        FCB     $24,$FF,$BD,$19,$45,$DE,$59,$A6 ; '$...E.Y.'
        FCB     $00
        FCC     "GGGG"
        FCB     $04            ; 21DE: 47 47 47 47 0x04 terminator
        FCB     $84,$03,$40,$8B,$03,$97,$3E,$16 ; '..@...>.'
        FCB     $EE,$01,$BD,$19,$37,$DE,$59,$A6 ; '....7.Y.'
        FCB     $00,$84,$40,$27,$06,$CE,$EE,$00 ; '..@'....'
        FCB     $BD,$19,$49,$D6,$52,$96,$53,$FE ; '..I.R.S.'
        FCB     $01,$39,$E7,$04,$A7,$05,$FE,$24 ; '.9.....$'
        FCB     $EF,$BD,$19,$45,$FE,$01,$39,$E6 ; '...E..9.'
        FCB     $08,$EE,$02,$BD,$19,$37,$7E,$21 ; '.....7~!'
        FCB     $BF,$DE,$59,$E6,$01,$A6,$02,$FE ; '..Y.....'
        FCB     $01,$39,$E7,$06,$A7,$07,$DE,$59 ; '.9.....Y'
        FCB     $A6,$00
        FCC     "GGGG"
        FCB     $04            ; 222C: 47 47 47 47 0x04 terminator
        FCB     $84,$03,$40,$8B,$03,$FE,$01,$39 ; '..@....9'
        FCB     $A7,$09,$F6,$25,$17,$B6,$25,$18 ; '...%..%.'
        FCB     $E7,$02,$A7,$03,$DE,$55,$A6,$00 ; '.....U..'
        FCB     $84,$20,$27,$11,$FE,$01,$39,$A6 ; '. '...9.'
        FCB     $07,$E6,$06,$BB,$25,$06,$F9,$25 ; '....%..%'
        FCB     $05,$E7,$06,$A7,$07,$7E,$21,$BF ; '.....~!.'
        FCB     $FE,$01,$39,$E6,$02,$A6,$03,$F1 ; '..9.....'
        FCB     $25,$17,$26,$06,$B1,$25,$18,$27 ; '%.&..%.''
        FCB     $01,$39,$FE,$25,$05,$DF,$5F,$86 ; '.9.%.._.'
        FCB     $0A,$BD,$19,$4D,$86,$0B,$BD,$19 ; '...M....'
        FCB     $4D,$7E,$21,$87,$86,$06,$BD,$0E ; 'M~!.....'
        FCB     $EE,$FE,$01,$39,$E6,$09,$EE,$06 ; '...9....'
        FCB     $BD,$19,$37,$FE,$01,$39,$E6,$08 ; '..7..9..'
        FCB     $EE,$02,$BD,$19,$37,$C6,$04,$FE ; '....7...'
        FCB     $01,$39,$EE,$04,$BD,$19,$37,$7F ; '.9....7.'
        FCB     $00,$58,$FE,$24,$E3,$DF,$59,$B6 ; '.X.$..Y.'
        FCB     $01,$3A,$F6,$01,$39,$B0,$25,$0E ; '.:..9.%.'
        FCB     $F2,$25,$0D,$F7,$01,$39,$B7,$01 ; '.%...9..'
        FCB     $3A,$39 ; ':9'

; Tertiary statement dispatch used by a mixed family of transfer and source-I/O statements.
; This block performs operand classification and then routes into the code/template emitters.
; Its strongest READ/PRINT/WRITE/FORMAT evidence is in subentries 2 and 3, which walk operand
; descriptors from the expression stack. The outer family still shares templates with transfer code.
; It is a better candidate for source-language I/O handling than the 2C67 record code.
STMT_SOURCE_IO_OR_TRANSFER_DISPATCH:
L22C2:
        STAA    $3E              ; 22C2: 97 3E
        BRA     L22DA            ; 22C4: 20 14
        FCB     $22,$E7,$22,$EB,$22,$F0,$23,$3A ; '".".".#:'
        FCB     $23,$88,$23,$9C,$23,$AA,$23,$B0 ; '#.#.#.#.'
        FCB     $23,$BB,$23,$C2 ; '#.#.'
L22DA:
        LDAB    $3E              ; 22DA: D6 3E
        ASLB                     ; 22DC: 58
        LDX     #$22C4           ; 22DD: CE 22 C4
        JSR     L2567            ; 22E0: BD 25 67
        LDX     $00,X            ; 22E3: EE 00
        JMP     $00,X            ; 22E5: 6E 00

; Subentry 0: clear the local I/O substate counter in $40.
IO_SUBSTATE_CLEAR:
L22E7:
        CLR     $0040            ; 22E7: 7F 00 40
        RTS                      ; 22EA: 39

; Subentry 1: initialize the local I/O substate counter to 1.
IO_SUBSTATE_SET_ONE:
L22EB:
        LDAA    #$01             ; 22EB: 86 01
        STAA    $40              ; 22ED: 97 40
        RTS                      ; 22EF: 39

; Subentry 2: classify one I/O list item, emit its descriptor, and queue the related fixup.
; This looks like the main READ/WRITE/PRINT item-list path.
IO_ITEM_EMIT:
L22F0:
        LDAA    #$12             ; 22F0: 86 12
        JSR     L0EEE            ; 22F2: BD 0E EE
        LDX     $40              ; 22F5: DE 40
        JSR     L1945            ; 22F7: BD 19 45
        LDAA    $40              ; 22FA: 96 40
        CMPA    #$01             ; 22FC: 81 01
        BLE     L2329            ; 22FE: 2F 29
        LDX     $55              ; 2300: DE 55
        LDAA    $00,X            ; 2302: A6 00
        ANDA    #$F0             ; 2304: 84 F0
        LDX     $59              ; 2306: DE 59
        LDAB    #$02             ; 2308: C6 02
        CMPA    #$90             ; 230A: 81 90
        BEQ     L2324            ; 230C: 27 16
        LDAB    #$03             ; 230E: C6 03
        CMPA    #$10             ; 2310: 81 10
        BEQ     L2324            ; 2312: 27 10
        CMPA    #$30             ; 2314: 81 30
        BNE     L2324            ; 2316: 26 0C
        LDAA    $01,X            ; 2318: A6 01
        LDAB    $02,X            ; 231A: E6 02
        ADDB    #$01             ; 231C: CB 01
        ADCA    #$00             ; 231E: 89 00
        STAA    $01,X            ; 2320: A7 01
        STAB    $02,X            ; 2322: E7 02
L2324:
        LDX     $01,X            ; 2324: EE 01
        JSR     L1937            ; 2326: BD 19 37
L2329:
        LDAA    #$09             ; 2329: 86 09
        JSR     L1295            ; 232B: BD 12 95
        LDAB    #$03             ; 232E: C6 03
        LDX     $55              ; 2330: DE 55
        LDX     $03,X            ; 2332: EE 03
        JSR     L1937            ; 2334: BD 19 37
        JMP     L23C2            ; 2337: 7E 23 C2

; Subentry 3: normalize an I/O control/format descriptor from the expression stack.
; The bit-masking and width selection here make this a good candidate for FORMAT/control-list handling.
IO_CONTROL_OR_FORMAT_ITEM:
L233A:
        LDAA    #$15             ; 233A: 86 15
        JSR     L0EEE            ; 233C: BD 0E EE
        LDX     $59              ; 233F: DE 59
        LDAA    $00,X            ; 2341: A6 00
        CLR     $0E22            ; 2343: 7F 0E 22
        CMPA    #$F1             ; 2346: 81 F1
        BEQ     L234E            ; 2348: 27 04
        CMPA    #$71             ; 234A: 81 71
        BNE     L2353            ; 234C: 26 05
L234E:
        ANDA    #$C0             ; 234E: 84 C0
        TAB                      ; 2350: 16
        BRA     L235D            ; 2351: 20 0A
L2353:
        TAB                      ; 2353: 16
        ANDB    #$EF             ; 2354: C4 EF
        ANDA    #$01             ; 2356: 84 01
        BEQ     L235D            ; 2358: 27 03
        INC     $0E22            ; 235A: 7C 0E 22
L235D:
        STAB    $0E21            ; 235D: F7 0E 21
        LDX     $0E21            ; 2360: FE 0E 21
        JSR     L1945            ; 2363: BD 19 45
        LDX     $59              ; 2366: DE 59
        LDAA    $00,X            ; 2368: A6 00
        ASRA                     ; 236A: 47
        ASRA                     ; 236B: 47
        ASRA                     ; 236C: 47
        ASRA                     ; 236D: 47
        ANDA    #$03             ; 236E: 84 03
        NEGA                     ; 2370: 40
        ADDA    #$03             ; 2371: 8B 03
        STAA    $40              ; 2373: 97 40
        LDAB    $0E22            ; 2375: F6 0E 22
        BEQ     L237E            ; 2378: 27 04
        LDAB    #$03             ; 237A: C6 03
        BRA     L2380            ; 237C: 20 02
L237E:
        LDAB    $40              ; 237E: D6 40
L2380:
        LDX     $01,X            ; 2380: EE 01
        JSR     L1937            ; 2382: BD 19 37
        JMP     L1E8A            ; 2385: 7E 1E 8A

; Subentry 4: emit a fixed-width template via L20C0 and advance BASE_TEXT_PTR by six bytes.
IO_TEMPLATE_ADVANCE6:
L2388:
        LDX     $52              ; 2388: DE 52
        STX     $5F              ; 238A: DF 5F
        LDAA    #$03             ; 238C: 86 03
        JSR     L20C0            ; 238E: BD 20 C0
        LDX     $52              ; 2391: DE 52
        INX                      ; 2393: 08
        INX                      ; 2394: 08
        INX                      ; 2395: 08
        INX                      ; 2396: 08
        INX                      ; 2397: 08
        INX                      ; 2398: 08
        STX     $52              ; 2399: DF 52
        RTS                      ; 239B: 39

; Subentry 5: emit template 0x0A through L20C0 without consuming the caller's BASE_TEXT_PTR.
IO_TEMPLATE_PRESERVE_PTR:
L239C:
        LDX     $52              ; 239C: DE 52
        STX     $47              ; 239E: DF 47
        LDAA    #$0A             ; 23A0: 86 0A
        JSR     L20C0            ; 23A2: BD 20 C0
        LDX     $47              ; 23A5: DE 47
        STX     $52              ; 23A7: DF 52
        RTS                      ; 23A9: 39

; Subentry 6: report diagnostic 0x18.
IO_DIAG_18:
L23AA:
        LDAA    #$18             ; 23AA: 86 18
        JSR     L0EEE            ; 23AC: BD 0E EE
        RTS                      ; 23AF: 39

; Subentry 7: emit templates 0x08 then 0x09 via L20C0.
; Likely one of the paired source-I/O statement forms.
IO_TEMPLATE_08_09:
L23B0:
        LDAA    #$08             ; 23B0: 86 08
        JSR     L20C0            ; 23B2: BD 20 C0
        LDAA    #$09             ; 23B5: 86 09
        JSR     L20C0            ; 23B7: BD 20 C0
        RTS                      ; 23BA: 39

; Subentry 8: bump the local I/O substate counter by two.
IO_SUBSTATE_ADD_TWO:
L23BB:
        LDAA    $40              ; 23BB: 96 40
        ADDA    #$02             ; 23BD: 8B 02
        STAA    $40              ; 23BF: 97 40
        RTS                      ; 23C1: 39

; Subentry 9: increment the per-statement item counter at $0138.
IO_ITEM_COUNT_INC:
L23C2:
        INC     $0138            ; 23C2: 7C 01 38
        RTS                      ; 23C5: 39
        FCB     $09

; Packed name fragment used by the transfer / object trailer logic.
        ORG     $23C8
        FCC     "/XPIAFE"
        FCB     $04            ; 23C8: 2F 58 50 49 41 46 45 0x04 terminator

        ORG     $23D1

; Low-level code-generation dispatcher.
; Selects one of several output templates and patches addresses / fixups.
CODEGEN_TEMPLATE_DISPATCH:
L23D1:
        STAA    $3E              ; 23D1: 97 3E
        BRA     L23E9            ; 23D3: 20 14
        FCB     $23,$F6,$24,$13,$24,$19 ; '#.$.$.'
        FCC     "$+$4$=$C${$"
        FCB     $04            ; 23DB: 24 2B 24 34 24 3D 24 43 24 7B 24 0x04 terminator
        FCB     $9B,$24,$C9 ; '.$.'
L23E9:
        LDAB    $3E              ; 23E9: D6 3E
        ASLB                     ; 23EB: 58
        LDX     #$23D3           ; 23EC: CE 23 D3
        JSR     L2567            ; 23EF: BD 25 67
        LDX     $00,X            ; 23F2: EE 00
        JMP     $00,X            ; 23F4: 6E 00

; Template subentry 0: reset the current I/O/codegen template state.
; Clears the running template bytes, snapshots BASE_TEXT_PTR, and zeros the local template latches.
TEMPLATE_RESET_STATE:
L23F6:
        LDX     #$0000           ; 23F6: CE 00 00
        STX     $032A            ; 23F9: FF 03 2A
        LDX     $24E1            ; 23FC: FE 24 E1
        JSR     L1945            ; 23FF: BD 19 45
        LDX     $52              ; 2402: DE 52
        STX     $47              ; 2404: DF 47
        LDX     $2517            ; 2406: FE 25 17
        JSR     L1949            ; 2409: BD 19 49
L240C:
        CLR     $23C7            ; 240C: 7F 23 C7
        CLR     $23D0            ; 240F: 7F 23 D0
        RTS                      ; 2412: 39

; Template subentry 1: latch MUL_ACC_HI/$60 as the first template parameter byte.
TEMPLATE_LATCH_PRIMARY:
L2413:
        LDAA    $60              ; 2413: 96 60
        STAA    $23C7            ; 2415: B7 23 C7
        RTS                      ; 2418: 39

; Template subentry 2: resolve the keyword-driven trailer code and emit the current parameter byte.
TEMPLATE_EMIT_KEYWORD_TRAILER:
L2419:
        JSR     L247B            ; 2419: BD 24 7B
        LDAA    $23C7            ; 241C: B6 23 C7
        STAA    $23D0            ; 241F: B7 23 D0
        LDX     $23CF            ; 2422: FE 23 CF
        JSR     L1949            ; 2425: BD 19 49
        JMP     L240C            ; 2428: 7E 24 0C

; Template subentry 3: emit the template pointer held at $23C6 and finish.
TEMPLATE_EMIT_PTR_23C6:
L242B:
        LDX     $23C6            ; 242B: FE 23 C6
        JSR     L1949            ; 242E: BD 19 49
        JMP     L240C            ; 2431: 7E 24 0C

; Template subentry 4: emit the fixed template pointer at $24DF and finish.
TEMPLATE_EMIT_PTR_24DF:
L2434:
        LDX     $24DF            ; 2434: FE 24 DF
        JSR     L1949            ; 2437: BD 19 49
        JMP     L240C            ; 243A: 7E 24 0C

; Template subentry 5: latch MUL_ACC_HI/$60 as the secondary template accumulator.
TEMPLATE_LATCH_SECONDARY:
L243D:
        LDAA    $60              ; 243D: 96 60
        STAA    $23D0            ; 243F: B7 23 D0
        RTS                      ; 2442: 39

; Template subentry 6: pack the latched template nibbles/bytes and emit one or more template fields.
TEMPLATE_PACK_AND_EMIT:
L2443:
        LDAA    $23D0            ; 2443: B6 23 D0
        BEQ     L244D            ; 2446: 27 05
        SUBA    $60              ; 2448: 90 60
        STAA    $23D0            ; 244A: B7 23 D0
L244D:
        LDAA    $23D0            ; 244D: B6 23 D0
        ASLA                     ; 2450: 48
        ASLA                     ; 2451: 48
        ASLA                     ; 2452: 48
        ASLA                     ; 2453: 48
        ORAA    $60              ; 2454: 9A 60
        STAA    $23D0            ; 2456: B7 23 D0
        LDAA    $23C7            ; 2459: B6 23 C7
        BLE     L2472            ; 245C: 2F 14
        LDX     $23C6            ; 245E: FE 23 C6
        JSR     L1949            ; 2461: BD 19 49
        LDX     $23CF            ; 2464: FE 23 CF
        JSR     L1949            ; 2467: BD 19 49
        LDX     $24DF            ; 246A: FE 24 DF
        JSR     L1949            ; 246D: BD 19 49
        BRA     L2478            ; 2470: 20 06
L2472:
        LDX     $23CF            ; 2472: FE 23 CF
        JSR     L1949            ; 2475: BD 19 49
L2478:
        JMP     L240C            ; 2478: 7E 24 0C

; Template helper: scan the small keyword fragment table at $23C8 ('/XPIAFE')
; and convert KEYWORD_CODE into a compact template selector in $23CF.
TEMPLATE_KEYWORD_SCAN:
L247B:
        LDAA    #$01             ; 247B: 86 01
L247D:
        STAA    $23CF            ; 247D: B7 23 CF
        LDAA    $4D              ; 2480: 96 4D
        LDX     #$23C8           ; 2482: CE 23 C8
        LDAB    $23CF            ; 2485: F6 23 CF
        JSR     L2562            ; 2488: BD 25 62
        CMPA    $00,X            ; 248B: A1 00
        BEQ     L249A            ; 248D: 27 0B
        LDAA    $23CF            ; 248F: B6 23 CF
        CMPA    #$07             ; 2492: 81 07
        BCC     L249A            ; 2494: 24 04
        INCA                     ; 2496: 4C
        JMP     L247D            ; 2497: 7E 24 7D
L249A:
        RTS                      ; 249A: 39

; Template subentry 7: emit a string/name reference using IDENT_LEN and the text buffer at $00C4.
TEMPLATE_EMIT_TEXT_REF:
L249B:
        LDAA    #$08             ; 249B: 86 08
        STAA    $23CF            ; 249D: B7 23 CF
        LDAA    $4E              ; 24A0: 96 4E
        STAA    $23D0            ; 24A2: B7 23 D0
        LDX     $23CF            ; 24A5: FE 23 CF
        JSR     L1949            ; 24A8: BD 19 49
        LDAA    #$01             ; 24AB: 86 01
L24AD:
        STAA    $3E              ; 24AD: 97 3E
        LDX     #$00C4           ; 24AF: CE 00 C4
        LDAB    $3E              ; 24B2: D6 3E
        JSR     L2562            ; 24B4: BD 25 62
        LDX     $00,X            ; 24B7: EE 00
        JSR     L1945            ; 24B9: BD 19 45
        LDAA    $3E              ; 24BC: 96 3E
        CMPA    $4E              ; 24BE: 91 4E
        BCC     L24C6            ; 24C0: 24 04
        INCA                     ; 24C2: 4C
        JMP     L24AD            ; 24C3: 7E 24 AD
L24C6:
        JMP     L240C            ; 24C6: 7E 24 0C

; Template subentry 8: emit the current source-location/fixup record and reseed $032A.
TEMPLATE_EMIT_FIXUP_RECORD:
L24C9:
        LDX     $2517            ; 24C9: FE 25 17
        JSR     L1949            ; 24CC: BD 19 49
        LDAA    #$C2             ; 24CF: 86 C2
        LDAB    #$04             ; 24D1: C6 04
        LDX     $52              ; 24D3: DE 52
        JSR     L0F08            ; 24D5: BD 0F 08
        LDX     #$C311           ; 24D8: CE C3 11
        STX     $032A            ; 24DB: FF 03 2A
        RTS                      ; 24DE: 39
        FCB     $0A,$00,$7E,$00,$01,$11,$01,$9F ; '..~.....'
        FCB     $01,$14,$1E,$B1,$BD,$00,$39,$00 ; '......9.'
        FCB     $FF,$00,$00,$C4,$03,$93,$CE,$00
        FCB     $B6,$00,$16,$F0,$03,$91,$6E,$00 ; '......n.'
        FCB     $FE,$00,$03,$70,$03,$5E,$00,$01 ; '...p.^..'
        FCB     $0D,$D0,$03,$60,$03,$6F,$00,$0A ; '...`.o..'
        FCB     $00,$61,$00,$04,$00,$02,$00,$63 ; '.a.....c'
        FCB     $00,$00

; Generic descriptor-table walker.
; Follows a compact table of (count, scale, base) records and returns X = base + index.
L2519:
        STX     $29              ; 2519: DF 29
        PSHA                     ; 251B: 36
        PSHB                     ; 251C: 37
        TSX                      ; 251D: 30
        LDX     $02,X            ; 251E: EE 02
        LDAA    $01,X            ; 2520: A6 01
L2522:
        PSHA                     ; 2522: 36
        STX     $25              ; 2523: DF 25
        LDAB    $02,X            ; 2525: E6 02
        LDX     $03,X            ; 2527: EE 03
        LDAA    $00,X            ; 2529: A6 00
        BNE     L2532            ; 252B: 26 05
        SBA                      ; 252D: 10
        LDAB    #$FF             ; 252E: C6 FF
        BRA     L2536            ; 2530: 20 04
L2532:
        DECA                     ; 2532: 4A
        JSR     L254E            ; 2533: BD 25 4E
L2536:
        ADDA    $2A              ; 2536: 9B 2A
        ADCB    $29              ; 2538: D9 29
        STAB    $29              ; 253A: D7 29
        STAA    $2A              ; 253C: 97 2A
        LDX     $25              ; 253E: DE 25
        INX                      ; 2540: 08
        INX                      ; 2541: 08
        INX                      ; 2542: 08
        PULA                     ; 2543: 32
        DECA                     ; 2544: 4A
        DECA                     ; 2545: 4A
        DECA                     ; 2546: 4A
        BNE     L2522            ; 2547: 26 D9
        LDX     $29              ; 2549: DE 29
        PULB                     ; 254B: 33
        PULA                     ; 254C: 32
        RTS                      ; 254D: 39

; 8-bit by small-constant multiply helper used by L2519.
L254E:
        PSHB                     ; 254E: 37
        LDAB    #$08             ; 254F: C6 08
        PSHB                     ; 2551: 37
        TSX                      ; 2552: 30
        CLRB                     ; 2553: 5F
        RORA                     ; 2554: 46
L2555:
        BCC     L2559            ; 2555: 24 02
        ADDB    $01,X            ; 2557: EB 01
L2559:
        RORB                     ; 2559: 56
        RORA                     ; 255A: 46
        DEC     $00,X            ; 255B: 6A 00
        BNE     L2555            ; 255D: 26 F6
        INS                      ; 255F: 31
        INS                      ; 2560: 31
        RTS                      ; 2561: 39

; Add B to X, treating B=0 as 'select the byte before the table'.
L2562:
        BNE     L2566            ; 2562: 26 02
        DEX                      ; 2564: 09
        RTS                      ; 2565: 39
L2566:
        DECB                     ; 2566: 5A

; Add B to X and return the adjusted pointer.
L2567:
        STX     $29              ; 2567: DF 29
        ADDB    $2A              ; 2569: DB 2A
        STAB    $2A              ; 256B: D7 2A
        BCC     L2572            ; 256D: 24 03
        INC     $0029            ; 256F: 7C 00 29
L2572:
        LDX     $29              ; 2572: DE 29
        RTS                      ; 2574: 39

; Resident compiler top-level driver.
; Validates source/list devices, initializes I/O vectors, then iterates over source lines.
L2575:
        CLRB                     ; 2575: 5F
        STAB    $01C3            ; 2576: F7 01 C3
        JSR     L285B            ; 2579: BD 28 5B
        LDAA    $063C            ; 257C: B6 06 3C
        BEQ     L25B0            ; 257F: 27 2F
        LDX     $0638            ; 2581: FE 06 38
        BNE     L258B            ; 2584: 26 05
        DEC     $01C3            ; 2586: 7A 01 C3
        BRA     L25B9            ; 2589: 20 2E
L258B:
        LDAA    $063A            ; 258B: B6 06 3A
        CMPA    #$30             ; 258E: 81 30
        BLE     L259D            ; 2590: 2F 0B
        CMPA    #$31             ; 2592: 81 31
        BEQ     L25A0            ; 2594: 27 0A
        CMPA    #$35             ; 2596: 81 35
        BEQ     L25A5            ; 2598: 27 0B
        INC     $01C3            ; 259A: 7C 01 C3
L259D:
        INC     $01C3            ; 259D: 7C 01 C3
L25A0:
        JSR     L2614            ; 25A0: BD 26 14
        BRA     L25B9            ; 25A3: 20 14
L25A5:
        LDX     #$0F7E           ; 25A5: CE 0F 7E
        STX     $0F7C            ; 25A8: FF 0F 7C
        LDX     $0638            ; 25AB: FE 06 38
        BRA     L259D            ; 25AE: 20 ED
L25B0:
        INC     $01C3            ; 25B0: 7C 01 C3
        LDX     #$2834           ; 25B3: CE 28 34
        STX     $10DB            ; 25B6: FF 10 DB
L25B9:
        JSR     L26D0            ; 25B9: BD 26 D0
L25BC:
        JSR     L286D            ; 25BC: BD 28 6D
        LDAA    $0638            ; 25BF: B6 06 38
        CMPA    #$FF             ; 25C2: 81 FF
        BEQ     L2606            ; 25C4: 27 40
        LDAB    $063A            ; 25C6: F6 06 3A
        BEQ     L25BC            ; 25C9: 27 F1
        LDX     #$25F0           ; 25CB: CE 25 F0
        JSR     L26BF            ; 25CE: BD 26 BF
L25D1:
        LDS     $26C8            ; 25D1: BE 26 C8
        LDAA    L2716            ; 25D4: B6 27 16
        CMPA    #$39             ; 25D7: 81 39
        BEQ     L25ED            ; 25D9: 27 12
        LDAA    $0641            ; 25DB: B6 06 41
        STAA    $0B              ; 25DE: 97 0B
        LDAA    $0642            ; 25E0: B6 06 42
        STAA    $0C              ; 25E3: 97 0C
        LDX     $09              ; 25E5: DE 09
        INX                      ; 25E7: 08
        STX     $09              ; 25E8: DF 09
        JMP     L26C2            ; 25EA: 7E 26 C2
L25ED:
        JMP     L26C5            ; 25ED: 7E 26 C5
        FCC     "ILLEGAL SOURCE DEVICE"
        FCB     $04            ; 25F0: 49 4C 4C 45 47 41 4C 20 53 4F 55 52 43 45 20 44 45 56 49 43 45 0x04 terminator
L2606:
        LDAB    #$02             ; 2606: C6 02
        STAB    $0637            ; 2608: F7 06 37
        JSR     L264D            ; 260B: BD 26 4D
        JSR     L0899            ; 260E: BD 08 99
        JMP     L097C            ; 2611: 7E 09 7C
L2614:
        JSR     L2637            ; 2614: BD 26 37
        BCC     L2622            ; 2617: 24 09
        LDX     #$2626           ; 2619: CE 26 26
        JSR     L26BF            ; 261C: BD 26 BF
        JMP     L25D1            ; 261F: 7E 25 D1
L2622:
        STX     $10DB            ; 2622: FF 10 DB
        RTS                      ; 2625: 39
        FCB     $49,$4C,$4C,$45,$47,$41,$4C,$20 ; 'ILLEGAL '
        FCB     $4C,$49,$53,$54,$20,$44,$45,$56 ; 'LIST DEV'
        FCB     $04

; Decode monitor device mnemonic in X ('CN', 'LP', etc.).
L2637:
        CPX     #$434E           ; 2637: 8C 43 4E
        BNE     L2641            ; 263A: 26 05
        LDX     $26C0            ; 263C: FE 26 C0
        BRA     L2649            ; 263F: 20 08
L2641:
        CPX     #$4C50           ; 2641: 8C 4C 50
        BNE     L264B            ; 2644: 26 05
        LDX     #$EAD5           ; 2646: CE EA D5
L2649:
        CLC                      ; 2649: 0C
        RTS                      ; 264A: 39
L264B:
        SEC                      ; 264B: 0D
        RTS                      ; 264C: 39

; Fetch the next I/O control block and mirror it into zero-page work pointers.
L264D:
        LDAB    $0637            ; 264D: F6 06 37
        JSR     L285B            ; 2650: BD 28 5B
        INC     $0637            ; 2653: 7C 06 37
        LDAA    $0638            ; 2656: B6 06 38
        CMPA    #$FF             ; 2659: 81 FF
        BNE     L265F            ; 265B: 26 02
        SEC                      ; 265D: 0D
        RTS                      ; 265E: 39
L265F:
        LDX     $0638            ; 265F: FE 06 38
        STX     $0006            ; 2662: FF 00 06
        LDX     $063A            ; 2665: FE 06 3A
        STX     $0004            ; 2668: FF 00 04
        CLR     $0008            ; 266B: 7F 00 08
        CLC                      ; 266E: 0C
        RTS                      ; 266F: 39

; Convert a binary value in A:B to ASCII decimal at X.
L2670:
        STX     $0638            ; 2670: FF 06 38
        LDX     #$26A9           ; 2673: CE 26 A9
L2676:
        CLR     $063A            ; 2676: 7F 06 3A
L2679:
        SUBB    $01,X            ; 2679: E0 01
        SBCA    $00,X            ; 267B: A2 00
        BCS     L2684            ; 267D: 25 05
        INC     $063A            ; 267F: 7C 06 3A
        BRA     L2679            ; 2682: 20 F5
L2684:
        ADDB    $01,X            ; 2684: EB 01
        ADCA    $00,X            ; 2686: A9 00
        PSHA                     ; 2688: 36
        STX     $063E            ; 2689: FF 06 3E
        LDX     $0638            ; 268C: FE 06 38
        LDAA    $063A            ; 268F: B6 06 3A
        ADDA    #$30             ; 2692: 8B 30
        STAA    $00,X            ; 2694: A7 00
        PULA                     ; 2696: 32
        INX                      ; 2697: 08
        STX     $0638            ; 2698: FF 06 38
        LDX     $063E            ; 269B: FE 06 3E
        INX                      ; 269E: 08
        INX                      ; 269F: 08
        CPX     #$26B3           ; 26A0: 8C 26 B3
        BNE     L2676            ; 26A3: 26 D1
        LDX     $0638            ; 26A5: FE 06 38
        RTS                      ; 26A8: 39
        FCB     $27,$10,$03,$E8,$00,$64,$00,$0A ; ''....d..'
        FCB     $00,$01
L26B3:
        JMP     $F018            ; 26B3: 7E F0 18
L26B6:
        JMP     $E80C            ; 26B6: 7E E8 0C
L26B9:
        JMP     $F021            ; 26B9: 7E F0 21
L26BC:
        JMP     $E809            ; 26BC: 7E E8 09
L26BF:
        JMP     $F024            ; 26BF: 7E F0 24
L26C2:
        JMP     $E80F            ; 26C2: 7E E8 0F
L26C5:
        JMP     $E800            ; 26C5: 7E E8 00
        FCB     $FF,$8A
L26CA:
        JMP     $F015            ; 26CA: 7E F0 15
L26CD:
        JMP     $E9DC            ; 26CD: 7E E9 DC

; Fetch the next source line and reset all per-line compiler state.
L26D0:
        JSR     L286D            ; 26D0: BD 28 6D
        LDX     $0638            ; 26D3: FE 06 38
        BNE     L26DE            ; 26D6: 26 06
        LDAA    #$39             ; 26D8: 86 39
        STAA    L2716            ; 26DA: B7 27 16
        RTS                      ; 26DD: 39
L26DE:
        LDAA    $0638            ; 26DE: B6 06 38
        STAA    $097A            ; 26E1: B7 09 7A
        STAA    $0B              ; 26E4: 97 0B
        LDAA    $0639            ; 26E6: B6 06 39
        STAA    $097B            ; 26E9: B7 09 7B
        STAA    $0C              ; 26EC: 97 0C
        JSR     L26CD            ; 26EE: BD 26 CD
        CLR     $064B            ; 26F1: 7F 06 4B
        CLR     $0651            ; 26F4: 7F 06 51
        CLR     $0652            ; 26F7: 7F 06 52
        CLR     $0653            ; 26FA: 7F 06 53
        CLR     $0860            ; 26FD: 7F 08 60
        CLR     $0665            ; 2700: 7F 06 65
        CLR     $0649            ; 2703: 7F 06 49
        LDAA    #$35             ; 2706: 86 35
        STAA    $0762            ; 2708: B7 07 62
        LDAA    #$34             ; 270B: 86 34
        STAA    $0664            ; 270D: B7 06 64
        LDAA    #$04             ; 2710: 86 04
        STAA    $0666            ; 2712: B7 06 66
        RTS                      ; 2715: 39

; Dispatch one encoded object record / pseudo-op fragment to the output builder.
OBJECT_FRAGMENT_DISPATCH:
L2716:
        STX     $0977            ; 2716: FF 09 77
        STAB    $0646            ; 2719: F7 06 46
        TAB                      ; 271C: 16
        LSRA                     ; 271D: 44
        LSRA                     ; 271E: 44
        LSRA                     ; 271F: 44
        LSRA                     ; 2720: 44
        ANDB    #$0F             ; 2721: C4 0F
        CMPB    #$0F             ; 2723: C1 0F
        BEQ     L2761            ; 2725: 27 3A
        STAB    $0976            ; 2727: F7 09 76
        STAA    $0643            ; 272A: B7 06 43
        CMPA    #$03             ; 272D: 81 03
        BEQ     L2741            ; 272F: 27 10
        CMPA    #$04             ; 2731: 81 04
        BEQ     L273C            ; 2733: 27 07
        LDX     $0047            ; 2735: FE 00 47
        ANDA    #$07             ; 2738: 84 07
        BRA     L2744            ; 273A: 20 08
L273C:
        LDX     $0052            ; 273C: FE 00 52
        BRA     L2744            ; 273F: 20 03
L2741:
        LDX     $0061            ; 2741: FE 00 61
L2744:
        STX     $0641            ; 2744: FF 06 41
        STAA    $0645            ; 2747: B7 06 45
        LDAB    $0646            ; 274A: F6 06 46
        CMPB    #$01             ; 274D: C1 01
        BNE     L2767            ; 274F: 26 16
        LDAB    $0976            ; 2751: F6 09 76
        LDAA    $0643            ; 2754: B6 06 43
        CMPA    #$0E             ; 2757: 81 0E
        BNE     L275E            ; 2759: 26 03
        JMP     L2DC1            ; 275B: 7E 2D C1
L275E:
        JMP     L2D9E            ; 275E: 7E 2D 9E
L2761:
        JSR     L27C4            ; 2761: BD 27 C4
        JMP     L2C67            ; 2764: 7E 2C 67
L2767:
        CMPA    #$03             ; 2767: 81 03
        BEQ     L2783            ; 2769: 27 18
        LDAA    $0977            ; 276B: B6 09 77
        LDAB    $0976            ; 276E: F6 09 76
        CMPB    #$02             ; 2771: C1 02
        BLT     L277F            ; 2773: 2D 0A
        LDX     $0977            ; 2775: FE 09 77
        LDAA    $0646            ; 2778: B6 06 46
        BEQ     L277F            ; 277B: 27 02
        ORAB    #$80             ; 277D: CA 80
L277F:
        JSR     L28C7            ; 277F: BD 28 C7
        RTS                      ; 2782: 39

; Append a compact six-byte deferred output record into the resident queue at $0863.
L2783:
        LDAB    $0860            ; 2783: F6 08 60
        BNE     L278E            ; 2786: 26 06
        LDX     #$0863           ; 2788: CE 08 63
        STX     $0861            ; 278B: FF 08 61
L278E:
        LDX     $0861            ; 278E: FE 08 61
        LDAA    $0646            ; 2791: B6 06 46
        STAA    $01,X            ; 2794: A7 01
        LDAA    $0976            ; 2796: B6 09 76
        STAA    $00,X            ; 2799: A7 00
        LDAA    $0641            ; 279B: B6 06 41
        STAA    $02,X            ; 279E: A7 02
        LDAA    $0642            ; 27A0: B6 06 42
        STAA    $03,X            ; 27A3: A7 03
        LDAA    $0977            ; 27A5: B6 09 77
        STAA    $04,X            ; 27A8: A7 04
        LDAA    $0978            ; 27AA: B6 09 78
        STAA    $05,X            ; 27AD: A7 05
        INX                      ; 27AF: 08
        INX                      ; 27B0: 08
        INX                      ; 27B1: 08
        INX                      ; 27B2: 08
        INX                      ; 27B3: 08
        INX                      ; 27B4: 08
        ADDB    $0976            ; 27B5: FB 09 76
        ADDB    #$04             ; 27B8: CB 04
        STAB    $0860            ; 27BA: F7 08 60
        STX     $0861            ; 27BD: FF 08 61
        CMPB    #$F3             ; 27C0: C1 F3
        BCS     L2811            ; 27C2: 25 4D

; Flush the deferred output queue built by L2783.
L27C4:
        JSR     L2C42            ; 27C4: BD 2C 42
        LDAA    $0860            ; 27C7: B6 08 60
        BEQ     L2811            ; 27CA: 27 45
        LDX     #$0863           ; 27CC: CE 08 63
L27CF:
        STX     $0861            ; 27CF: FF 08 61
        LDAA    #$03             ; 27D2: 86 03
        STAA    $0666            ; 27D4: B7 06 66
        LDAA    $02,X            ; 27D7: A6 02
        STAA    $0641            ; 27D9: B7 06 41
        LDAA    $03,X            ; 27DC: A6 03
        STAA    $0642            ; 27DE: B7 06 42
        LDAA    $01,X            ; 27E1: A6 01
        STAA    $0646            ; 27E3: B7 06 46
        LDAB    $00,X            ; 27E6: E6 00
        LDX     $04,X            ; 27E8: EE 04
        CMPA    #$00             ; 27EA: 81 00
        BEQ     L27F0            ; 27EC: 27 02
        ORAB    #$80             ; 27EE: CA 80
L27F0:
        JSR     L28C7            ; 27F0: BD 28 C7
        LDX     $0861            ; 27F3: FE 08 61
        INX                      ; 27F6: 08
        INX                      ; 27F7: 08
        INX                      ; 27F8: 08
        INX                      ; 27F9: 08
        INX                      ; 27FA: 08
        INX                      ; 27FB: 08
        LDAA    $0860            ; 27FC: B6 08 60
        SUBA    #$06             ; 27FF: 80 06
        STAA    $0860            ; 2801: B7 08 60
        BNE     L27CF            ; 2804: 26 C9
        JSR     L2C42            ; 2806: BD 2C 42
        CLR     $0860            ; 2809: 7F 08 60
        LDAA    #$04             ; 280C: 86 04
        STAA    $0666            ; 280E: B7 06 66
L2811:
        RTS                      ; 2811: 39
        FCB     $CE,$06,$47,$BD,$2D,$FC,$B6,$06 ; '..G.-...'
        FCB     $47,$8D,$06,$B6,$06,$48,$8D,$01 ; 'G....H..'
        FCB     $39,$81,$09,$26,$02,$86,$20,$81 ; '9..&.. .'
        FCB     $0D,$26,$03,$BD,$26,$B9,$BD,$26 ; '.&..&..&'
        FCB     $B3,$39,$37,$FF,$06,$43,$FE,$06 ; '.97..C..'
        FCB     $43,$A6,$00,$81,$04,$27,$09,$08 ; 'C....'..'
        FCB     $FF,$06,$43,$BD,$26,$B6,$20,$EE ; '..C.&. .'
        FCB     $86,$0D,$BD,$26,$B6,$86,$0A,$BD ; '...&....'
        FCB     $26,$B6,$86,$00,$BD,$26,$B6,$33 ; '&....&.3'
        FCB     $39 ; '9'

; Prime the source input work pointers by reading the initial monitor control words.
L285B:
        LDX     #$0002           ; 285B: CE 00 02
        STX     $04              ; 285E: DF 04
        STX     $06              ; 2860: DF 06
        CLR     $0008            ; 2862: 7F 00 08
L2865:
        PSHB                     ; 2865: 37
        BSR     L286D            ; 2866: 8D 05
        PULB                     ; 2868: 33
        DECB                     ; 2869: 5A
        BPL     L2865            ; 286A: 2A F9
        RTS                      ; 286C: 39

; Read one 5-byte monitor control block into $0638..$063C.
L286D:
        LDAA    #$05             ; 286D: 86 05
        STAA    $063E            ; 286F: B7 06 3E
        LDX     #$0638           ; 2872: CE 06 38
        STX     $063F            ; 2875: FF 06 3F
L2878:
        JSR     L26BC            ; 2878: BD 26 BC
        LDX     $063F            ; 287B: FE 06 3F
        STAA    $00,X            ; 287E: A7 00
        INX                      ; 2880: 08
        STX     $063F            ; 2881: FF 06 3F
        DEC     $063E            ; 2884: 7A 06 3E
        BNE     L2878            ; 2887: 26 EF
        RTS                      ; 2889: 39

; Checksum-print helper for listing/object records.
; Emits: record-type byte in A, payload bytes from LIST_REC_PTR/LIST_REC_LEN,
; two's-complement checksum, CR, LF, and a trailing NUL byte.
L288A:
        JSR     L28C4            ; 288A: BD 28 C4
        LDAA    $0963            ; 288D: B6 09 63
        INCA                     ; 2890: 4C
        STAA    $0963            ; 2891: B7 09 63
        CLR     $0979            ; 2894: 7F 09 79
        BSR     L28BD            ; 2897: 8D 24
        BRA     L28A6            ; 2899: 20 0B
L289B:
        LDX     $0961            ; 289B: FE 09 61
        LDAA    $00,X            ; 289E: A6 00
        INX                      ; 28A0: 08
        STX     $0961            ; 28A1: FF 09 61
        BSR     L28BD            ; 28A4: 8D 17
L28A6:
        DEC     $0963            ; 28A6: 7A 09 63
        BNE     L289B            ; 28A9: 26 F0
        LDAA    $0979            ; 28AB: B6 09 79
        NEGA                     ; 28AE: 40
        BSR     L28C4            ; 28AF: 8D 13
        LDAA    #$0D             ; 28B1: 86 0D
        BSR     L28C4            ; 28B3: 8D 0F
        LDAA    #$0A             ; 28B5: 86 0A
        BSR     L28C4            ; 28B7: 8D 0B
        CLRA                     ; 28B9: 4F
        BSR     L28C4            ; 28BA: 8D 08
        RTS                      ; 28BC: 39
L28BD:
        TAB                      ; 28BD: 16
        ADDB    $0979            ; 28BE: FB 09 79
        STAB    $0979            ; 28C1: F7 09 79
L28C4:
        JMP     L26B6            ; 28C4: 7E 26 B6

; Main object-record emitter.
; Accepts a descriptor in A/B/X and turns it into packed bytes / relocation records.
L28C7:
        STAB    $063D            ; 28C7: F7 06 3D
        STAA    $063C            ; 28CA: B7 06 3C
        STX     $063A            ; 28CD: FF 06 3A
        LDX     $0641            ; 28D0: FE 06 41
        LDAA    $064B            ; 28D3: B6 06 4B
        BNE     L28DB            ; 28D6: 26 03
        JSR     L2A40            ; 28D8: BD 2A 40
L28DB:
        LDAB    $063D            ; 28DB: F6 06 3D
        BITB    #$02             ; 28DE: C5 02
        BEQ     L28FD            ; 28E0: 27 1B
        LDAA    $064A            ; 28E2: B6 06 4A
        CMPA    #$01             ; 28E5: 81 01
        BHI     L28EC            ; 28E7: 22 03
        JSR     L2C42            ; 28E9: BD 2C 42
L28EC:
        LDX     $0641            ; 28EC: FE 06 41
        LDAA    $063A            ; 28EF: B6 06 3A
        BSR     L290B            ; 28F2: 8D 17
        LDX     $0641            ; 28F4: FE 06 41
        INX                      ; 28F7: 08
        LDAA    $063B            ; 28F8: B6 06 3B
        BRA     L2907            ; 28FB: 20 0A
L28FD:
        BITB    #$01             ; 28FD: C5 01
        BEQ     L2925            ; 28FF: 27 24
        LDX     $0641            ; 2901: FE 06 41
        LDAA    $063C            ; 2904: B6 06 3C
L2907:
        BSR     L290B            ; 2907: 8D 02
        BRA     L290E            ; 2909: 20 03
L290B:
        JMP     L2A5E            ; 290B: 7E 2A 5E
L290E:
        LDAB    $063D            ; 290E: F6 06 3D
        BITB    #$80             ; 2911: C5 80
        BEQ     L2925            ; 2913: 27 10
        JSR     L2936            ; 2915: BD 29 36
        BSR     L2926            ; 2918: 8D 0C
        LDAA    $0651            ; 291A: B6 06 51
        BEQ     L2925            ; 291D: 27 06
        JSR     L2C42            ; 291F: BD 2C 42
        CLR     $0651            ; 2922: 7F 06 51
L2925:
        RTS                      ; 2925: 39
L2926:
        LDX     $0641            ; 2926: FE 06 41
        LDAB    $063D            ; 2929: F6 06 3D
        BITB    #$02             ; 292C: C5 02
        BEQ     L2931            ; 292E: 27 01
        INX                      ; 2930: 08
L2931:
        INX                      ; 2931: 08
        STX     $0655            ; 2932: FF 06 55
        RTS                      ; 2935: 39

; Assign relocation slots and build packed relocation descriptors for an output record.
L2936:
        LDAA    $0652            ; 2936: B6 06 52
        BNE     L293E            ; 2939: 26 03
        JSR     L2AB2            ; 293B: BD 2A B2
L293E:
        LDAA    $0653            ; 293E: B6 06 53
        BNE     L2946            ; 2941: 26 03
        JSR     L2AC4            ; 2943: BD 2A C4
L2946:
        LDAA    $0646            ; 2946: B6 06 46
        CMPA    $065A            ; 2949: B1 06 5A
        BEQ     L298C            ; 294C: 27 3E
        CMPA    $065B            ; 294E: B1 06 5B
        BEQ     L2998            ; 2951: 27 45
        CMPA    $065C            ; 2953: B1 06 5C
        BEQ     L29A9            ; 2956: 27 51
        CMPA    #$02             ; 2958: 81 02
        BNE     L2963            ; 295A: 26 07
        LDAB    $065A            ; 295C: F6 06 5A
        BNE     L2984            ; 295F: 26 23
        BRA     L2989            ; 2961: 20 26
L2963:
        CMPA    #$03             ; 2963: 81 03
        BNE     L296E            ; 2965: 26 07
        LDAB    $065B            ; 2967: F6 06 5B
        BNE     L2984            ; 296A: 26 18
        BRA     L2995            ; 296C: 20 27
L296E:
        CMPA    #$05             ; 296E: 81 05
        BNE     L2979            ; 2970: 26 07
        LDAB    $065C            ; 2972: F6 06 5C
        BNE     L2984            ; 2975: 26 0D
        BRA     L29A6            ; 2977: 20 2D
L2979:
        CMPA    #$04             ; 2979: 81 04
        BNE     L2984            ; 297B: 26 07
        LDAB    $065C            ; 297D: F6 06 5C
        BNE     L2984            ; 2980: 26 02
        BRA     L29A6            ; 2982: 20 22
L2984:
        LDAB    $065A            ; 2984: F6 06 5A
        BNE     L2990            ; 2987: 26 07
L2989:
        STAA    $065A            ; 2989: B7 06 5A
L298C:
        LDAB    #$40             ; 298C: C6 40
        BRA     L29AB            ; 298E: 20 1B
L2990:
        LDAB    $065B            ; 2990: F6 06 5B
        BNE     L299C            ; 2993: 26 07
L2995:
        STAA    $065B            ; 2995: B7 06 5B
L2998:
        LDAB    #$80             ; 2998: C6 80
        BRA     L29AB            ; 299A: 20 0F
L299C:
        LDAB    $065C            ; 299C: F6 06 5C
        BEQ     L29A6            ; 299F: 27 05
        JSR     L2B1A            ; 29A1: BD 2B 1A
        BRA     L2936            ; 29A4: 20 90
L29A6:
        STAA    $065C            ; 29A6: B7 06 5C
L29A9:
        LDAB    #$C0             ; 29A9: C6 C0
L29AB:
        STAB    $063F            ; 29AB: F7 06 3F
        LDAB    $0642            ; 29AE: F6 06 42
        SUBB    $0656            ; 29B1: F0 06 56
        STAB    $063E            ; 29B4: F7 06 3E
        BEQ     L2A32            ; 29B7: 27 79
        CMPB    #$07             ; 29B9: C1 07
        BLS     L2A19            ; 29BB: 23 5C
        LDAA    $0650            ; 29BD: B6 06 50
        CMPA    #$03             ; 29C0: 81 03
        BLS     L2A11            ; 29C2: 23 4D
        JSR     L2C11            ; 29C4: BD 2C 11
        LDAA    $063E            ; 29C7: B6 06 3E
        FCB     $85,$80,$27,$20,$B6,$06,$5D,$81 ; '..' ..].'
        FCB     $0D,$22,$36,$FE,$06,$5F,$86,$FF ; '."6.._..'
        FCB     $A7,$00,$08,$FF,$06,$5F,$7C,$06 ; '....._|.'
        FCB     $5D,$7A,$06,$50,$F6,$06,$3E,$C0 ; ']z.P..>.'
        FCB     $7F,$F7,$06,$3E,$B6,$06,$5D,$81 ; '...>..].'
        FCB     $0E,$22,$16,$FE,$06,$5F,$B6,$06 ; '."..._..'
        FCB     $3E,$8A,$80,$A7,$00,$08,$FF,$06 ; '>.......'
        FCB     $5F,$7A,$06,$50,$7C,$06,$5D,$20 ; '_z.P|.] '
        FCB     $2E
L2A0B:
        JSR     L2B1A            ; 2A0B: BD 2B 1A
L2A0E:
        JMP     L2936            ; 2A0E: 7E 29 36
L2A11:
        INC     $0651            ; 2A11: 7C 06 51
        JSR     L2C42            ; 2A14: BD 2C 42
        BRA     L2A0E            ; 2A17: 20 F5
L2A19:
        LDAA    $065D            ; 2A19: B6 06 5D
        CMPA    #$0D             ; 2A1C: 81 0D
        BHI     L2A0B            ; 2A1E: 22 EB
        LDAA    $0650            ; 2A20: B6 06 50
        CMPA    #$02             ; 2A23: 81 02
        BLS     L2A11            ; 2A25: 23 EA
L2A27:
        CLRA                     ; 2A27: 4F
        JSR     L2BC4            ; 2A28: BD 2B C4
        DEC     $063E            ; 2A2B: 7A 06 3E
        BNE     L2A27            ; 2A2E: 26 F7
        BRA     L2A39            ; 2A30: 20 07
L2A32:
        LDAA    $0650            ; 2A32: B6 06 50
        CMPA    #$01             ; 2A35: 81 01
        BLS     L2A11            ; 2A37: 23 D8
L2A39:
        LDAA    $063F            ; 2A39: B6 06 3F
        JSR     L2BC4            ; 2A3C: BD 2B C4
        RTS                      ; 2A3F: 39

; Start a new output record buffer at $066A.
L2A40:
        INC     $064B            ; 2A40: 7C 06 4B
        CLR     $0665            ; 2A43: 7F 06 65
        CLR     $0667            ; 2A46: 7F 06 67
        LDX     $0641            ; 2A49: FE 06 41
        STX     $0668            ; 2A4C: FF 06 68
        STX     $064C            ; 2A4F: FF 06 4C
        LDX     #$066A           ; 2A52: CE 06 6A
        STX     $064E            ; 2A55: FF 06 4E
        LDAA    #$F8             ; 2A58: 86 F8
        STAA    $064A            ; 2A5A: B7 06 4A
        RTS                      ; 2A5D: 39

; Append one byte to the current output record, flushing if the block is full or discontiguous.
L2A5E:
        STX     $063E            ; 2A5E: FF 06 3E
        STAA    $0640            ; 2A61: B7 06 40
        LDAA    $064B            ; 2A64: B6 06 4B
        BEQ     L2A7A            ; 2A67: 27 11
        CPX     $064C            ; 2A69: BC 06 4C
        BNE     L2A75            ; 2A6C: 26 07
        LDAA    $064A            ; 2A6E: B6 06 4A
        BEQ     L2A75            ; 2A71: 27 02
        BRA     L2A7C            ; 2A73: 20 07
L2A75:
        BSR     L2A93            ; 2A75: 8D 1C
        JSR     L2C42            ; 2A77: BD 2C 42
L2A7A:
        BSR     L2A40            ; 2A7A: 8D C4
L2A7C:
        LDX     $064E            ; 2A7C: FE 06 4E
        LDAA    $0640            ; 2A7F: B6 06 40
        STAA    $00,X            ; 2A82: A7 00
        INX                      ; 2A84: 08
        STX     $064E            ; 2A85: FF 06 4E
        LDX     $064C            ; 2A88: FE 06 4C
        INX                      ; 2A8B: 08
        STX     $064C            ; 2A8C: FF 06 4C
        DEC     $064A            ; 2A8F: 7A 06 4A
        RTS                      ; 2A92: 39

; Flush the raw-byte payload currently buffered by L2A5E.
L2A93:
        LDAA    $064B            ; 2A93: B6 06 4B
        BEQ     L2AB1            ; 2A96: 27 19
        CLR     $064B            ; 2A98: 7F 06 4B
        LDAA    #$FE             ; 2A9B: 86 FE
        SUBA    $064A            ; 2A9D: B0 06 4A
        STAA    $064A            ; 2AA0: B7 06 4A
        STAA    $0963            ; 2AA3: B7 09 63
        LDX     #$0664           ; 2AA6: CE 06 64
        STX     $0961            ; 2AA9: FF 09 61
        LDAA    #$44             ; 2AAC: 86 44
        JSR     L288A            ; 2AAE: BD 28 8A
L2AB1:
        RTS                      ; 2AB1: 39

; Initialize relocation/fixup collection for the current output record.
L2AB2:
        INC     $0652            ; 2AB2: 7C 06 52
        LDAA    #$FD             ; 2AB5: 86 FD
        STAA    $0650            ; 2AB7: B7 06 50
        CLR     $0653            ; 2ABA: 7F 06 53
        LDX     #$0763           ; 2ABD: CE 07 63
        STX     $0658            ; 2AC0: FF 06 58
        RTS                      ; 2AC3: 39

; Begin the per-record relocation bitmap and descriptor area.
L2AC4:
        LDAA    $0650            ; 2AC4: B6 06 50
        CMPA    #$0A             ; 2AC7: 81 0A
        BHI     L2AD0            ; 2AC9: 22 05
        BSR     L2B0C            ; 2ACB: 8D 3F
        LDAA    $0650            ; 2ACD: B6 06 50
L2AD0:
        SUBA    #$09             ; 2AD0: 80 09
        STAA    $0650            ; 2AD2: B7 06 50
        LDAB    #$80             ; 2AD5: C6 80
        STAB    $0657            ; 2AD7: F7 06 57
        CLRA                     ; 2ADA: 4F
        STAA    $065A            ; 2ADB: B7 06 5A
        STAA    $065B            ; 2ADE: B7 06 5B
        STAA    $065C            ; 2AE1: B7 06 5C
        STAA    $0654            ; 2AE4: B7 06 54
        STAA    $065D            ; 2AE7: B7 06 5D
        LDX     $0658            ; 2AEA: FE 06 58
        LDAB    #$09             ; 2AED: C6 09
        JSR     L2DE3            ; 2AEF: BD 2D E3
        STX     $065F            ; 2AF2: FF 06 5F
        INC     $0653            ; 2AF5: 7C 06 53
        LDX     $0658            ; 2AF8: FE 06 58
        LDAA    $0641            ; 2AFB: B6 06 41
        LDAB    $0642            ; 2AFE: F6 06 42
        STAA    $01,X            ; 2B01: A7 01
        STAB    $02,X            ; 2B03: E7 02
        STAA    $0655            ; 2B05: B7 06 55
        STAB    $0656            ; 2B08: F7 06 56
        RTS                      ; 2B0B: 39
L2B0C:
        JSR     L2C42            ; 2B0C: BD 2C 42
        JSR     L2AB2            ; 2B0F: BD 2A B2
        INC     $0651            ; 2B12: 7C 06 51
        RTS                      ; 2B15: 39
L2B16:
        RTS                      ; 2B16: 39
L2B17:
        JMP     L2BB2            ; 2B17: 7E 2B B2

; Finalize relocation metadata for the current record.
L2B1A:
        LDAA    $0653            ; 2B1A: B6 06 53
        BEQ     L2B16            ; 2B1D: 27 F7
        LDAA    $0652            ; 2B1F: B6 06 52
        BEQ     L2B16            ; 2B22: 27 F2
        LDAA    $0657            ; 2B24: B6 06 57
        ANDA    #$E0             ; 2B27: 84 E0
        BEQ     L2B17            ; 2B29: 27 EC
        CMPA    #$C0             ; 2B2B: 81 C0
        BEQ     L2B17            ; 2B2D: 27 E8
        CMPA    #$80             ; 2B2F: 81 80
        BNE     L2BA7            ; 2B31: 26 74
        JSR     L2C11            ; 2B33: BD 2C 11
        LDX     $0658            ; 2B36: FE 06 58
        CLRA                     ; 2B39: 4F
        LDAB    $065A            ; 2B3A: F6 06 5A
        STAB    $04,X            ; 2B3D: E7 04
        CMPB    #$02             ; 2B3F: C1 02
        BLS     L2B44            ; 2B41: 23 01
        INCA                     ; 2B43: 4C
L2B44:
        LDAB    $065B            ; 2B44: F6 06 5B
        STAB    $06,X            ; 2B47: E7 06
        BEQ     L2B50            ; 2B49: 27 05
        CMPB    #$03             ; 2B4B: C1 03
        BEQ     L2B50            ; 2B4D: 27 01
        INCA                     ; 2B4F: 4C
L2B50:
        LDAB    $065C            ; 2B50: F6 06 5C
        STAB    $08,X            ; 2B53: E7 08
        BEQ     L2B6A            ; 2B55: 27 13
        CMPB    #$05             ; 2B57: C1 05
        BEQ     L2B6A            ; 2B59: 27 0F
        LDAB    $065A            ; 2B5B: F6 06 5A
        CMPB    #$02             ; 2B5E: C1 02
        BEQ     L2B6D            ; 2B60: 27 0B
        LDAB    $065C            ; 2B62: F6 06 5C
        CMPB    #$04             ; 2B65: C1 04
        BEQ     L2B6A            ; 2B67: 27 01
        INCA                     ; 2B69: 4C
L2B6A:
        TSTA                     ; 2B6A: 4D
        BEQ     L2B76            ; 2B6B: 27 09
L2B6D:
        CLRA                     ; 2B6D: 4F
        STAA    $03,X            ; 2B6E: A7 03
        STAA    $05,X            ; 2B70: A7 05
        STAA    $07,X            ; 2B72: A7 07
        BRA     L2BA9            ; 2B74: 20 33
L2B76:
        LDX     $0661            ; 2B76: FE 06 61
        DEX                      ; 2B79: 09
        DEX                      ; 2B7A: 09
        DEX                      ; 2B7B: 09
        DEX                      ; 2B7C: 09
        DEX                      ; 2B7D: 09
        DEX                      ; 2B7E: 09
        STX     $0661            ; 2B7F: FF 06 61
        LDX     $0658            ; 2B82: FE 06 58
        INX                      ; 2B85: 08
        INX                      ; 2B86: 08
        INX                      ; 2B87: 08
L2B88:
        LDAA    $06,X            ; 2B88: A6 06
        STAA    $00,X            ; 2B8A: A7 00
        INX                      ; 2B8C: 08
        CPX     $0661            ; 2B8D: BC 06 61
        BNE     L2B88            ; 2B90: 26 F6
        LDAA    $0650            ; 2B92: B6 06 50
        ADDA    #$06             ; 2B95: 8B 06
        STAA    $0650            ; 2B97: B7 06 50
        LDAA    #$C0             ; 2B9A: 86 C0
        CMPB    #$04             ; 2B9C: C1 04
        BEQ     L2BA2            ; 2B9E: 27 02
        LDAA    #$E0             ; 2BA0: 86 E0
L2BA2:
        STAA    $0657            ; 2BA2: B7 06 57
        BRA     L2BA9            ; 2BA5: 20 02
L2BA7:
        BSR     L2C11            ; 2BA7: 8D 68
L2BA9:
        LDAA    $0657            ; 2BA9: B6 06 57
        ORAA    $065D            ; 2BAC: BA 06 5D
        STAA    $0657            ; 2BAF: B7 06 57
L2BB2:
        LDX     $0658            ; 2BB2: FE 06 58
        LDAA    $0657            ; 2BB5: B6 06 57
        STAA    $00,X            ; 2BB8: A7 00
        LDX     $0661            ; 2BBA: FE 06 61
        STX     $0658            ; 2BBD: FF 06 58
        CLR     $0653            ; 2BC0: 7F 06 53
        RTS                      ; 2BC3: 39

; Pack two-bit relocation classes into the current relocation bitmap.
L2BC4:
        PSHA                     ; 2BC4: 36
        LDAA    $0654            ; 2BC5: B6 06 54
        BEQ     L2BD3            ; 2BC8: 27 09
        LDAA    $065E            ; 2BCA: B6 06 5E
        CMPA    #$7F             ; 2BCD: 81 7F
        BNE     L2BE7            ; 2BCF: 26 16
        BSR     L2C11            ; 2BD1: 8D 3E
L2BD3:
        LDX     $065F            ; 2BD3: FE 06 5F
        INX                      ; 2BD6: 08
        STX     $0661            ; 2BD7: FF 06 61
        CLRA                     ; 2BDA: 4F
        STAA    $0663            ; 2BDB: B7 06 63
        STAA    $065E            ; 2BDE: B7 06 5E
        DEC     $0650            ; 2BE1: 7A 06 50
        INC     $0654            ; 2BE4: 7C 06 54
L2BE7:
        LDAA    $0663            ; 2BE7: B6 06 63
        BNE     L2BEF            ; 2BEA: 26 03
        DEC     $0650            ; 2BEC: 7A 06 50
L2BEF:
        INC     $065E            ; 2BEF: 7C 06 5E
        LDX     $0661            ; 2BF2: FE 06 61
        LDAB    $00,X            ; 2BF5: E6 00
        PULA                     ; 2BF7: 32
        ASLA                     ; 2BF8: 48
        ROLB                     ; 2BF9: 59
        ASLA                     ; 2BFA: 48
        ROLB                     ; 2BFB: 59
        STAB    $00,X            ; 2BFC: E7 00
        LDAA    $0663            ; 2BFE: B6 06 63
        INCA                     ; 2C01: 4C
        STAA    $0663            ; 2C02: B7 06 63
        CMPA    #$04             ; 2C05: 81 04
        BNE     L2C10            ; 2C07: 26 07
        INX                      ; 2C09: 08
        STX     $0661            ; 2C0A: FF 06 61
        CLR     $0663            ; 2C0D: 7F 06 63
L2C10:
        RTS                      ; 2C10: 39

; Flush the current relocation bitmap byte when it is partially filled.
L2C11:
        LDAA    $0654            ; 2C11: B6 06 54
        BEQ     L2C41            ; 2C14: 27 2B
        LDX     $065F            ; 2C16: FE 06 5F
        LDAA    $065E            ; 2C19: B6 06 5E
        STAA    $00,X            ; 2C1C: A7 00
        LDX     $0661            ; 2C1E: FE 06 61
        LDAA    $0663            ; 2C21: B6 06 63
        BEQ     L2C38            ; 2C24: 27 12
        LDAB    $00,X            ; 2C26: E6 00
L2C28:
        ASLB                     ; 2C28: 58
        ASLB                     ; 2C29: 58
        INCA                     ; 2C2A: 4C
        CMPA    #$04             ; 2C2B: 81 04
        BNE     L2C28            ; 2C2D: 26 F9
        STAB    $00,X            ; 2C2F: E7 00
        CLR     $0663            ; 2C31: 7F 06 63
        INX                      ; 2C34: 08
        STX     $0661            ; 2C35: FF 06 61
L2C38:
        STX     $065F            ; 2C38: FF 06 5F
        INC     $065D            ; 2C3B: 7C 06 5D
        CLR     $0654            ; 2C3E: 7F 06 54
L2C41:
        RTS                      ; 2C41: 39

; Flush both payload and relocation metadata for the current object record.
L2C42:
        JSR     L2A93            ; 2C42: BD 2A 93
        LDAA    $0652            ; 2C45: B6 06 52
        BEQ     L2C66            ; 2C48: 27 1C
        JSR     L2B1A            ; 2C4A: BD 2B 1A
        CLR     $0652            ; 2C4D: 7F 06 52
        LDAA    #$FE             ; 2C50: 86 FE
        SUBA    $0650            ; 2C52: B0 06 50
        STAA    $0650            ; 2C55: B7 06 50
        LDX     #$0762           ; 2C58: CE 07 62
        STX     $0961            ; 2C5B: FF 09 61
        STAA    $0963            ; 2C5E: B7 09 63
        LDAA    #$44             ; 2C61: 86 44
        JSR     L288A            ; 2C63: BD 28 8A
L2C66:
        RTS                      ; 2C66: 39

; Emit the object-module prologue/header records.
; This is part of the compiler's own object-file emitter, not a source-language I/O statement handler.
; The visible prologue sequence emits record types 0x36, 0x32, 0x33, and optionally 0x34,
; and seeds the default module text around $0964 ('30      3$MAIN  ').
; The 0x33 record also captures CODE_PTR, BASE_TEXT_PTR, and the ASCII runtime tag '0RUN   '.
; The optional 0x34 record packages a small bootstrap stub at load address $0020,
; with data bytes that decode as an 8E/7E-style loader handoff sequence.
L2C67:
        LDAA    #$36             ; 2C67: 86 36
        STAA    $0664            ; 2C69: B7 06 64
        LDAA    #$04             ; 2C6C: 86 04
        STAA    $0665            ; 2C6E: B7 06 65
        CLR     $0666            ; 2C71: 7F 06 66
        CLR     $0667            ; 2C74: 7F 06 67
        LDAA    #$04             ; 2C77: 86 04
        STAA    $0963            ; 2C79: B7 09 63
        LDX     #$0664           ; 2C7C: CE 06 64
        STX     $0961            ; 2C7F: FF 09 61
        LDAA    #$44             ; 2C82: 86 44
        JSR     L288A            ; 2C84: BD 28 8A
        LDAA    $0B              ; 2C87: 96 0B
        STAA    $0641            ; 2C89: B7 06 41
        LDAA    $0C              ; 2C8C: 96 0C
        STAA    $0642            ; 2C8E: B7 06 42
        JSR     L2DD7            ; 2C91: BD 2D D7
        LDAA    $0B              ; 2C94: 96 0B
        STAA    $0638            ; 2C96: B7 06 38
        LDAA    $0C              ; 2C99: 96 0C
        STAA    $0639            ; 2C9B: B7 06 39
        LDAA    $097A            ; 2C9E: B6 09 7A
        STAA    $0B              ; 2CA1: 97 0B
        LDAA    $097B            ; 2CA3: B6 09 7B
        STAA    $0C              ; 2CA6: 97 0C
        LDX     #$096E           ; 2CA8: CE 09 6E
        STX     $0977            ; 2CAB: FF 09 77
        LDX     #$0666           ; 2CAE: CE 06 66
        LDAB    #$06             ; 2CB1: C6 06
        JSR     L2E17            ; 2CB3: BD 2E 17
        LDX     #$4F42           ; 2CB6: CE 4F 42
        STX     $066C            ; 2CB9: FF 06 6C
        LDAA    #$32             ; 2CBC: 86 32
        STAA    $0664            ; 2CBE: B7 06 64
        LDAA    #$0A             ; 2CC1: 86 0A
        STAA    $0963            ; 2CC3: B7 09 63
        LDX     #$0664           ; 2CC6: CE 06 64
        STX     $0961            ; 2CC9: FF 09 61
        LDAA    #$44             ; 2CCC: 86 44
        JSR     L288A            ; 2CCE: BD 28 8A
        LDAA    #$33             ; 2CD1: 86 33
        STAA    $0664            ; 2CD3: B7 06 64
        CLRA                     ; 2CD6: 4F
        LDAB    #$11             ; 2CD7: C6 11
        LDX     #$0665           ; 2CD9: CE 06 65
L2CDC:
        STAA    $00,X            ; 2CDC: A7 00
        INX                      ; 2CDE: 08
        DECB                     ; 2CDF: 5A
        BNE     L2CDC            ; 2CE0: 26 FA
        LDAA    #$01             ; 2CE2: 86 01
        STAA    $066A            ; 2CE4: B7 06 6A
        INCA                     ; 2CE7: 4C
        STAA    $066D            ; 2CE8: B7 06 6D
        LDX     $0063            ; 2CEB: FE 00 63
        STX     $066E            ; 2CEE: FF 06 6E
        INCA                     ; 2CF1: 4C
        STAA    $0670            ; 2CF2: B7 06 70
        LDX     $0061            ; 2CF5: FE 00 61
        LDAB    $0649            ; 2CF8: F6 06 49
        BNE     L2D05            ; 2CFB: 26 08
        LDAB    #$28             ; 2CFD: C6 28
        JSR     L2DE3            ; 2CFF: BD 2D E3
        STX     $0061            ; 2D02: FF 00 61
L2D05:
        STX     $0671            ; 2D05: FF 06 71
        LDAA    #$04             ; 2D08: 86 04
        STAA    $0673            ; 2D0A: B7 06 73
        LDX     $0052            ; 2D0D: FE 00 52
        STX     $0674            ; 2D10: FF 06 74
        LDAA    #$30             ; 2D13: 86 30
        STAA    $0676            ; 2D15: B7 06 76
        LDX     #$5255           ; 2D18: CE 52 55
        STX     $0677            ; 2D1B: FF 06 77
        LDX     #$4E20           ; 2D1E: CE 4E 20
        STX     $0679            ; 2D21: FF 06 79
        LDX     #$2020           ; 2D24: CE 20 20
        STX     $067B            ; 2D27: FF 06 7B
        LDAA    #$19             ; 2D2A: 86 19
        STAA    $0963            ; 2D2C: B7 09 63
        LDX     #$0664           ; 2D2F: CE 06 64
        STX     $0961            ; 2D32: FF 09 61
        LDAA    #$44             ; 2D35: 86 44
        JSR     L288A            ; 2D37: BD 28 8A
        LDAA    #$0A             ; 2D3A: 86 0A
        LDX     #$096C           ; 2D3C: CE 09 6C
        JSR     L2DA9            ; 2D3F: BD 2D A9
        LDAA    $0649            ; 2D42: B6 06 49
        BNE     L2D92            ; 2D45: 26 4B
        LDAA    #$34             ; 2D47: 86 34
        STAA    $0664            ; 2D49: B7 06 64
        CLR     $0666            ; 2D4C: 7F 06 66
        LDX     #$0020           ; 2D4F: CE 00 20
        STX     $0668            ; 2D52: FF 06 68
        LDAA    #$8E             ; 2D55: 86 8E
        STAA    $066A            ; 2D57: B7 06 6A
        LDX     $0061            ; 2D5A: FE 00 61
        DEX                      ; 2D5D: 09
        STX     $066B            ; 2D5E: FF 06 6B
        LDAA    #$7E             ; 2D61: 86 7E
        STAA    $066D            ; 2D63: B7 06 6D
        CLR     $066E            ; 2D66: 7F 06 6E
        CLR     $066F            ; 2D69: 7F 06 6F
        LDAA    #$0C             ; 2D6C: 86 0C
        LDX     #$0664           ; 2D6E: CE 06 64
        JSR     L2DA9            ; 2D71: BD 2D A9
        LDAA    #$C1             ; 2D74: 86 C1
        STAA    $0763            ; 2D76: B7 07 63
        LDX     #$0021           ; 2D79: CE 00 21
        STX     $0764            ; 2D7C: FF 07 64
        LDAA    #$03             ; 2D7F: 86 03
        STAA    $0766            ; 2D81: B7 07 66
        LDX     #$8C00           ; 2D84: CE 8C 00
        STX     $0767            ; 2D87: FF 07 67
        LDAA    #$06             ; 2D8A: 86 06
        LDX     #$0762           ; 2D8C: CE 07 62
        JSR     L2DA9            ; 2D8F: BD 2D A9
L2D92:
        JSR     L2DD7            ; 2D92: BD 2D D7
        LDX     $09              ; 2D95: DE 09
        INX                      ; 2D97: 08
        STX     $09              ; 2D98: DF 09
        JSR     L26DE            ; 2D9A: BD 26 DE
        RTS                      ; 2D9D: 39

; Emit a standard object/listing text record from the scratch buffer at $0964/$0966.
; Reached from L2716 for one class of compiler-generated output record.
OBJECT_TEXT_RECORD_A:
L2D9E:
        LDX     #$0966           ; 2D9E: CE 09 66
        JSR     L2E17            ; 2DA1: BD 2E 17
        LDAA    #$08             ; 2DA4: 86 08
        LDX     #$0964           ; 2DA6: CE 09 64
L2DA9:
        STX     $0961            ; 2DA9: FF 09 61
        STAA    $0963            ; 2DAC: B7 09 63
        LDAA    #$44             ; 2DAF: 86 44
        JSR     L288A            ; 2DB1: BD 28 8A
        LDX     #$2020           ; 2DB4: CE 20 20
        STX     $0966            ; 2DB7: FF 09 66
        STX     $0968            ; 2DBA: FF 09 68
        STX     $096A            ; 2DBD: FF 09 6A
        RTS                      ; 2DC0: 39

; Emit the alternate object/listing text record format used by another L2716 fragment class.
; Also part of the compiler's output-record machinery rather than FORTRAN READ/WRITE semantics.
OBJECT_TEXT_RECORD_B:
L2DC1:
        LDX     #$2020           ; 2DC1: CE 20 20
        STX     $096E            ; 2DC4: FF 09 6E
        STX     $0970            ; 2DC7: FF 09 70
        STX     $0972            ; 2DCA: FF 09 72
        LDX     #$096E           ; 2DCD: CE 09 6E
        JSR     L2E17            ; 2DD0: BD 2E 17
        INC     $0649            ; 2DD3: 7C 06 49
        RTS                      ; 2DD6: 39
L2DD7:
        TST     $000D            ; 2DD7: 7D 00 0D
        BNE     L2DDD            ; 2DDA: 26 01
        RTS                      ; 2DDC: 39
L2DDD:
        CLRA                     ; 2DDD: 4F
        JSR     L26B6            ; 2DDE: BD 26 B6
        BRA     L2DD7            ; 2DE1: 20 F4

; Add B to the 16-bit pointer in X and return the adjusted pointer.
; Used heavily while laying out record headers and relocation payload fields.
ADD_B_TO_X:
L2DE3:
        STX     $0643            ; 2DE3: FF 06 43
        CLRA                     ; 2DE6: 4F
        ADDB    $0644            ; 2DE7: FB 06 44
        ADCA    $0643            ; 2DEA: B9 06 43
        STAA    $0643            ; 2DED: B7 06 43
        STAB    $0644            ; 2DF0: F7 06 44
        LDX     $0643            ; 2DF3: FE 06 43
        RTS                      ; 2DF6: 39

; Store A:B as ASCII hexadecimal at X.
STORE_HEX16:
L2DF7:
        BSR     L2DFC            ; 2DF7: 8D 03
        TBA                      ; 2DF9: 17
        BRA     L2DFC            ; 2DFA: 20 00

; Store one byte in A as two ASCII hex digits at X.
STORE_HEX8:
L2DFC:
        PSHB                     ; 2DFC: 37
        TAB                      ; 2DFD: 16
        LSRA                     ; 2DFE: 44
        LSRA                     ; 2DFF: 44
        LSRA                     ; 2E00: 44
        LSRA                     ; 2E01: 44
        BSR     L2E09            ; 2E02: 8D 05
        TBA                      ; 2E04: 17
        BSR     L2E09            ; 2E05: 8D 02
        PULB                     ; 2E07: 33
        RTS                      ; 2E08: 39
L2E09:
        ANDA    #$0F             ; 2E09: 84 0F
        ORAA    #$30             ; 2E0B: 8A 30
        CMPA    #$3A             ; 2E0D: 81 3A
        BCS     L2E13            ; 2E0F: 25 02
        ADDA    #$07             ; 2E11: 8B 07
L2E13:
        STAA    $00,X            ; 2E13: A7 00
        INX                      ; 2E15: 08
        RTS                      ; 2E16: 39

; Copy B bytes from the fragment stack rooted at OBJ_QUEUE_PTR into the buffer at X.
; Used by the object-record prologue/text emitters to materialize small descriptor fragments.
COPY_FRAGMENT_BYTES:
L2E17:
        STS     $0643            ; 2E17: BF 06 43
        SEI                      ; 2E1A: 0F
        LDS     $0977            ; 2E1B: BE 09 77
        DES                      ; 2E1E: 34
L2E1F:
        PULA                     ; 2E1F: 32
        STAA    $00,X            ; 2E20: A7 00
        INX                      ; 2E22: 08
        DECB                     ; 2E23: 5A
        BNE     L2E1F            ; 2E24: 26 F9
        LDS     $0643            ; 2E26: BE 06 43
        CLI                      ; 2E29: 0E
        RTS                      ; 2E2A: 39

