# FORTRAN Compiler Notes

This note summarizes the current reverse-engineering read of [`fort.asm`](fort.asm), which was generated from the S19 image in [`fort`](fort).

## Big Picture

The binary appears to be a resident Motorola 6800 FORTRAN compiler with these broad phases:

1. Startup and monitor/device initialization.
2. Read one source line into an internal buffer.
3. Classify the line by keyword and statement descriptor tables.
4. Parse expressions and build temporary 3-byte descriptors.
5. Update symbol tables and fixup chains.
6. Emit compiler output records through a compact template/object-record layer.
7. Print diagnostics and, at the end, a symbol/error summary.

## Call/Data Flow

The main control flow now looks like this:

`L2575` resident driver
-> `L097C` cold start
-> `L26D0` read one source line
-> `L0B83` classify leading keyword
-> `L111C` / `L1205` / `L121E` walk statement descriptors
-> one of the statement families:
   - `STMT_DISPATCH` for declaration/storage/common-data work
   - `STMT_TRANSFER_CODEGEN_DISPATCH` for transfer/fixup-heavy forms
   - `STMT_SOURCE_IO_OR_TRANSFER_DISPATCH` for operand-list statements, especially source I/O
-> `L1EF1` / `L194D` expression parsing and operator reduction as needed
-> `CODEGEN_TEMPLATE_DISPATCH` to choose emitted fragments
-> `OBJECT_FRAGMENT_DISPATCH` / `L28C7` / `L2C42` to build and flush object/listing records
-> `DIAG_FORMAT` / `DIAG_BUILD_AND_PRINT` for diagnostics
-> `STMT_END_AND_SYMBOL_LIST` for the final summary pass

## Major Areas

### 1. Driver and startup

- [`L2575`](fort.asm#L3425) is the top-level resident driver.
- [`L0899`](fort.asm#L287) prints the startup banner.
- [`L097C`](fort.asm#L311) performs cold-start initialization.
- [`L26D0`](fort.asm#L3595) reads the next source line and resets per-line state.

The compiler talks to monitor ROM entry points through wrappers around `F0xx` and `E8xx`, so this is not a standalone hosted tool; it expects a resident monitor/runtime environment.

### 2. Statement recognition

- The keyword table starts at [`$01EC`](fort.asm#L134).
- Per-keyword metadata lives at [`$02D1`](fort.asm#L143).
- [`L0B83`](fort.asm#L594) is the statement classifier front door.
- [`L111C`](fort.asm#L1240), [`L1205`](fort.asm#L1349), and [`L121E`](fort.asm#L1365) walk descriptor tables derived from the statement metadata.

The compiler does not appear to parse by a large hand-coded cascade alone; instead, it leans heavily on compact tables plus generic indexed lookup helpers.

### 3. Statement families

- [`STMT_DISPATCH`](fort.asm#L1425) at `L1295` is the main statement-handler jump table.
- The declaration/data side appears to include:
  - [`STMT_DECLARE_OR_REFERENCE`](fort.asm#L1444)
  - [`STMT_DECLARATION_SETUP`](fort.asm#L1494)
  - [`STMT_DIMENSION_STORE_BOUND`](fort.asm#L1555)
  - [`STMT_DIMENSION_OR_COMMON_FINALIZE`](fort.asm#L1576)
  - [`STMT_DATA_OR_COMMON_PREP`](fort.asm#L1661)
  - [`STMT_DATA_OR_COMMON_EMIT_FIELDS`](fort.asm#L1690)
- The transfer/codegen side appears to include:
  - [`STMT_TRANSFER_FIXUP_MERGE`](fort.asm#L1738)
  - [`STMT_TRANSFER_CODEGEN_DISPATCH`](fort.asm#L2065)
- The source-I/O side appears to include:
  - [`STMT_SOURCE_IO_OR_TRANSFER_DISPATCH`](fort.asm#L2989)

Confidence is highest for the family grouping and lower for exact one-to-one statement names.
The current best split is:

- `STMT_TRANSFER_CODEGEN_DISPATCH` looks more transfer-oriented. It sits later in the pipeline and appears to hand branch/call/fixup state directly to the template/object layer.
- `STMT_SOURCE_IO_OR_TRANSFER_DISPATCH` looks more source-I/O-oriented. Its strongest evidence is the operand-list work in `IO_ITEM_EMIT` and the control/format normalization in `IO_CONTROL_OR_FORMAT_ITEM`.
- Both families still share `L20C0` and `CODEGEN_TEMPLATE_DISPATCH`, so the separation is functional rather than absolute.

Inside [`STMT_TRANSFER_CODEGEN_DISPATCH`](fort.asm#L2065), the subentries are now more usable:

- [`TRANSFER_EMIT_ASSIGNED_GOTO`](fort.asm#L2083) is now the best assigned-`GOTO` candidate. The emitted `FE .. 6E 00` pattern looks like “load X from variable; jump indirect through X”.
- [`TRANSFER_LIST_INIT`](fort.asm#L2098) and [`TRANSFER_LIST_APPEND_SYMBOL`](fort.asm#L2106) manage a temporary target list at `$0390`.
- [`TRANSFER_EMIT_COMPUTED_GOTO`](fort.asm#L2131) is the strongest computed-`GOTO` candidate so far. It emits an inline dispatcher skeleton and then serializes the accumulated target list.
- [`TRANSFER_RESOLVE_AND_EMIT_TARGET`](fort.asm#L2197) and [`TRANSFER_DEFINE_AND_EMIT_TARGET`](fort.asm#L2264) look like direct-target helpers for plain branch/call-style forms.
- [`TRANSFER_EMIT_RETURN`](fort.asm#L2298) is now the clearest `RETURN`-style emitter in the transfer family, because its fixed template is the word `0x3900` (`RTS` plus terminator/padding in this object format).
- [`TRANSFER_ALLOC_LOCAL_TARGET`](fort.asm#L2306), [`TRANSFER_LIST_APPEND_EXPR_DESC`](fort.asm#L2332), and [`TRANSFER_LIST_SERIALIZE`](fort.asm#L2354) reinforce the idea that this family handles transfer statements with explicit target lists.

### 4. Expression parser

- [`L1EF1`](fort.asm#L2793) is the clearest expression-parser core.
- Operator precedence/class data is stored at [`$1EC9`](fort.asm#L2784).
- Temporary expression descriptors are pushed/popped with:
  - [`L1DD5`](fort.asm#L2641)
  - [`L1E8A`](fort.asm#L2748)
- Operator/action dispatch goes through:
  - [`L194D`](fort.asm#L2413)

Descriptors look to be 3-byte records carrying type/class bits plus an address/value pointer.

### 5. Symbols and fixups

- Symbol lookup/creation is centered around:
  - [`L0A4B`](fort.asm#L410)
  - [`L0ADF`](fort.asm#L504)
- The current symbol pointer is tracked through `SYM_PTR` (`$55`).
- Fixup and relocation work is tied to:
  - `FIXUP_PTR` (`$65`)
  - `RELOC_*` workspace in the `$0650` range

The compiler appears to maintain symbol chains and to annotate entries with attribute bits, extents, and emitted-address/fixup information as parsing proceeds.

### 6. Diagnostics

- [`DIAG_FORMAT`](fort.asm#L953) at `L0EEE` is the higher-level diagnostic formatter.
- [`DIAG_BUILD_AND_PRINT`](fort.asm#L971) at `L0F08` builds and prints the message text.
- [`L0D7F`](fort.asm#L798) emits error messages using the current line/listing context.

Diagnostics are not just simple prints; the compiler formats message fragments and numeric fields into a scratch buffer before sending the result to the listing path.

The fixed pointer block at [`$1277`](fort.asm#L1391) is now clearer:

- `TRACE_DIAG_BUF_PTR` points to the temporary diagnostic/listing buffer at `$0DD0`.
- `DIAG_PTR_SYM_SCAN`, `DIAG_PTR_TMP_DESC`, `DIAG_PTR_BASE_TEXT`, and `DIAG_PTR_ATTR_BITS` are selector constants that let `L0F08` choose which workspace value to format.
- `SYM_HASH_BASE_PTR` holds the `$019F` base used by the symbol hash code.
- `INIT_CODE_PTR`, `INIT_EXPR_DESC_STACK_PTR`, `INIT_PATCH_LIST_PTR`, and `CONST_ZERO_WORD` are cold-start constants rather than executable code.

The keyword metadata table at [`$02D1`](fort.asm#L143) is also a little clearer now:

- It does not look like a single flat record array.
- The current best read is that it is several tightly packed parallel slices:
  - early bytes act like family/dispatch selectors,
  - middle bytes carry flags and small class values,
  - later bytes carry secondary template selectors or literal parameters consumed by `L111C` / `L121E`.
- That explains why the statement walker treats the table as descriptor space rather than directly indexing fixed-size structures.

### 7. Source I/O statement subdispatch

- [`STMT_SOURCE_IO_OR_TRANSFER_DISPATCH`](fort.asm#L2989) at `L22C2` is the current best candidate for `READ`/`PRINT`/`WRITE`/`FORMAT`-family handling.
- Notable subentries:
  - [`IO_ITEM_EMIT`](fort.asm#L3019)
  - [`IO_CONTROL_OR_FORMAT_ITEM`](fort.asm#L3060)
  - [`IO_TEMPLATE_ADVANCE6`](fort.asm#L3107)
  - [`IO_TEMPLATE_PRESERVE_PTR`](fort.asm#L3124)
  - [`IO_TEMPLATE_08_09`](fort.asm#L3143)

`IO_ITEM_EMIT` looks like the strongest candidate for ordinary I/O list items. `IO_CONTROL_OR_FORMAT_ITEM` looks like the strongest candidate for format/control-item normalization.

### 8. Output-template and object-record layer

There are two different “output” concepts in this compiler:

- Source-language I/O statement handling.
- Compiler-generated object/listing record emission.

The second one lives in the template/object-record layer:

- [`CODEGEN_TEMPLATE_DISPATCH`](fort.asm#L3175) at `L23D1`
- [`OBJECT_FRAGMENT_DISPATCH`](fort.asm#L3626) at `L2716`
- [`L28C7`](fort.asm#L3836) as the main object-record emitter
- [`L2A40`](fort.asm#L4011) / [`L2A5E`](fort.asm#L4025) for record buffering
- [`L2C42`](fort.asm#L4285) for flush/finalization

Useful template subentries include:

- [`TEMPLATE_RESET_STATE`](fort.asm#L3193)
- [`TEMPLATE_EMIT_KEYWORD_TRAILER`](fort.asm#L3216)
- [`TEMPLATE_PACK_AND_EMIT`](fort.asm#L3247)
- [`TEMPLATE_KEYWORD_SCAN`](fort.asm#L3278)
- [`TEMPLATE_EMIT_TEXT_REF`](fort.asm#L3298)
- [`TEMPLATE_EMIT_FIXUP_RECORD`](fort.asm#L3323)

The important point is that this layer emits the compiler's object/listing records, not the user program's `READ` or `WRITE` runtime operations.

The emitted record conventions are a bit clearer now:

- [`L288A`](fort.asm#L3800) outputs a printable/object record as:
  - one record-type byte in `A`
  - `LIST_REC_LEN` bytes from `LIST_REC_PTR`
  - a two's-complement checksum
  - `CR`, `LF`, and a trailing NUL
- [`L2A40`](fort.asm#L4011) starts a raw payload buffer at `$066A`.
- [`L2A5E`](fort.asm#L4025) appends contiguous payload bytes.
- [`L2AB2`](fort.asm#L4069) and neighbors collect relocation metadata as packed two-bit classes plus short descriptor bytes.
- [`L2C42`](fort.asm#L4285) flushes the raw payload and then any relocation side-record.
- [`L2C67`](fort.asm#L4309) emits the module prologue, including visible record types `0x36`, `0x32`, `0x33`, and optionally `0x34`, plus the default module text around `$0964` (`"30      3$MAIN  "`).
- [`COPY_FRAGMENT_BYTES`](fort.asm#L4522) at `L2E17` copies small fragment descriptors from the temporary stack into record buffers, which helps explain how `OBJECT_FRAGMENT_DISPATCH` materializes its mini-templates.
- The optional `0x34` prologue record now looks like a tiny bootstrap stub placed at load address `$0020`, using `8E` / `7E` opcodes as a loader handoff sequence.

The prologue scratch fields at [`$0664`](fort.asm#L74) are now partly named in the assembly:

- `PROLOGUE_TYPE` / `PROLOGUE_LEN` are the visible record-type and length bytes.
- `PROLOGUE_TAG_OB` is the `OB` tag written into the `0x32` record.
- `PROLOGUE_ADDR1`, `PROLOGUE_ADDR2`, and `PROLOGUE_TEXT_BASE` capture `CODE_PTR` and `BASE_TEXT_PTR`.
- `PROLOGUE_RUNTIME_0`, `PROLOGUE_RUNTIME_RU`, `PROLOGUE_RUNTIME_NSP`, and `PROLOGUE_RUNTIME_SP2` spell the ASCII tail `0RUN   ` used in the `0x33` record.
- Several `PROLOGUE_KIND*` bytes are still role-based names because the exact external object-module format is not fully reconstructed yet.

### 9. End-of-compilation report

- [`STMT_END_AND_SYMBOL_LIST`](fort.asm#L1764) at `L14D6` appears to produce the end-of-compilation summary.

This path likely:

- walks symbol chains,
- prints symbol names,
- prints associated values/counts,
- and emits the `UNDEFINED SYMBOLS ... ERRORS` section.

## Best Current Keyword Map

The following is the best current high-level map from the packed keyword table:

- `AGO`: likely `GOTO`
- `BTO`: likely `TO` token used inside `DO`/`GOTO` parsing
- `CIF`: likely `IF`
- `DSTOP`: likely `STOP`
- `EEND`: likely `END`
- `FDO`: likely `DO`
- `GCONTINUE`: likely `CONTINUE`
- `HCALL`: likely `CALL`
- `ISUBROUTINE`: likely `SUBROUTINE`
- `JFUNCTION`: likely `FUNCTION`
- `KRETURN`: likely `RETURN`
- `LREAD`: likely `READ`
- `MPRINT`: likely `PRINT`
- `MWRITE`: likely `WRITE`
- `NDIMENSION`: likely `DIMENSION`
- `OCOMMON`: likely `COMMON`
- `PDATA`: likely `DATA`
- `QFORMAT`: likely `FORMAT`
- `0ENDFILE`: likely `ENDFILE`
- `0REWIND`: likely `REWIND`

Operator-like entries:

- `RNOT`
- `SAND`
- `TOR`
- `UEQ`
- `VNE`
- `WLE`
- `XGE`
- `YLT`
- `ZGT`

These almost certainly feed expression parsing rather than top-level statement compilation.

## Caveats

- Many labels are behavior-based aliases, not confirmed original source names.
- The disassembly is table-heavy and uses indirect dispatch, so some regions are still ambiguous.
- The strongest conclusions are about subsystem boundaries and data flow, not exact source statements for every handler.

## Status Of Earlier Next Steps

- The compact call/data-flow diagram is now included above.
- The fixed pointer/constant block around [`$1277`](fort.asm#L1391) now has working labels and comments in the assembly listing.
- The transfer-vs-source-I/O split is clearer, though still not statement-perfect.
- The object-record layer now has a working record-format summary and named helper routines.
- The `STMT_TRANSFER_CODEGEN_DISPATCH` subentries now have tentative per-entry names instead of one generic dispatcher label.
- The strongest transfer mappings are now in place: assigned `GOTO`, computed `GOTO`, and `RETURN`.
- The prologue scratch area at `$0664..$067B` now has role-based field names in the assembly listing.
- The `$02D1` keyword metadata table now has a rough structural hypothesis in the note and listing comments.

## Good Next Steps

- Separate the remaining direct-target transfer helpers into likely `IF` versus `CALL` versus plain `GOTO` forms.
- Name the remaining `PROLOGUE_KIND*` bytes byte-for-byte against the compiler's external object-module convention.
- Reconstruct the `$02D1` keyword metadata table more concretely so each keyword can be tied to a statement family with less guesswork.
