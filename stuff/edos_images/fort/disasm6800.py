#!/usr/bin/env python3
from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
import string


@dataclass(frozen=True)
class Op:
    mnemonic: str
    mode: str
    size: int


OPS: dict[int, Op] = {}


def add(opcodes: list[int], mnemonic: str, mode: str, size: int) -> None:
    for opcode in opcodes:
        OPS[opcode] = Op(mnemonic, mode, size)


# Inherent / control
add([0x01], "NOP", "inh", 1)
add([0x06], "TAP", "inh", 1)
add([0x07], "TPA", "inh", 1)
add([0x08], "INX", "inh", 1)
add([0x09], "DEX", "inh", 1)
add([0x0A], "CLV", "inh", 1)
add([0x0B], "SEV", "inh", 1)
add([0x0C], "CLC", "inh", 1)
add([0x0D], "SEC", "inh", 1)
add([0x0E], "CLI", "inh", 1)
add([0x0F], "SEI", "inh", 1)
add([0x10], "SBA", "inh", 1)
add([0x11], "CBA", "inh", 1)
add([0x16], "TAB", "inh", 1)
add([0x17], "TBA", "inh", 1)
add([0x19], "DAA", "inh", 1)
add([0x1B], "ABA", "inh", 1)
add([0x30], "TSX", "inh", 1)
add([0x31], "INS", "inh", 1)
add([0x32], "PULA", "inh", 1)
add([0x33], "PULB", "inh", 1)
add([0x34], "DES", "inh", 1)
add([0x35], "TXS", "inh", 1)
add([0x36], "PSHA", "inh", 1)
add([0x37], "PSHB", "inh", 1)
add([0x38], "PULX", "inh", 1)
add([0x39], "RTS", "inh", 1)
add([0x3A], "ABX", "inh", 1)
add([0x3B], "RTI", "inh", 1)
add([0x3E], "WAI", "inh", 1)
add([0x3F], "SWI", "inh", 1)

# Relative branches
for opcode, mnemonic in [
    (0x20, "BRA"),
    (0x22, "BHI"),
    (0x23, "BLS"),
    (0x24, "BCC"),
    (0x25, "BCS"),
    (0x26, "BNE"),
    (0x27, "BEQ"),
    (0x28, "BVC"),
    (0x29, "BVS"),
    (0x2A, "BPL"),
    (0x2B, "BMI"),
    (0x2C, "BGE"),
    (0x2D, "BLT"),
    (0x2E, "BGT"),
    (0x2F, "BLE"),
    (0x8D, "BSR"),
]:
    add([opcode], mnemonic, "rel", 2)

# Accumulator A inherent ops
for opcode, mnemonic in [
    (0x40, "NEGA"),
    (0x43, "COMA"),
    (0x44, "LSRA"),
    (0x46, "RORA"),
    (0x47, "ASRA"),
    (0x48, "ASLA"),
    (0x49, "ROLA"),
    (0x4A, "DECA"),
    (0x4C, "INCA"),
    (0x4D, "TSTA"),
    (0x4F, "CLRA"),
]:
    add([opcode], mnemonic, "inh", 1)

# Accumulator B inherent ops
for opcode, mnemonic in [
    (0x50, "NEGB"),
    (0x53, "COMB"),
    (0x54, "LSRB"),
    (0x56, "RORB"),
    (0x57, "ASRB"),
    (0x58, "ASLB"),
    (0x59, "ROLB"),
    (0x5A, "DECB"),
    (0x5C, "INCB"),
    (0x5D, "TSTB"),
    (0x5F, "CLRB"),
]:
    add([opcode], mnemonic, "inh", 1)

# Indexed memory ops
for opcode, mnemonic in [
    (0x60, "NEG"),
    (0x63, "COM"),
    (0x64, "LSR"),
    (0x66, "ROR"),
    (0x67, "ASR"),
    (0x68, "ASL"),
    (0x69, "ROL"),
    (0x6A, "DEC"),
    (0x6C, "INC"),
    (0x6D, "TST"),
    (0x6E, "JMP"),
    (0x6F, "CLR"),
]:
    add([opcode], mnemonic, "idx", 2)

# Extended memory ops
for opcode, mnemonic in [
    (0x70, "NEG"),
    (0x73, "COM"),
    (0x74, "LSR"),
    (0x76, "ROR"),
    (0x77, "ASR"),
    (0x78, "ASL"),
    (0x79, "ROL"),
    (0x7A, "DEC"),
    (0x7C, "INC"),
    (0x7D, "TST"),
    (0x7E, "JMP"),
    (0x7F, "CLR"),
]:
    add([opcode], mnemonic, "ext", 3)


def add_group(base: int, mnems: list[str]) -> None:
    modes = [("imm", 2), ("dir", 2), ("idx", 2), ("ext", 3)]
    for i, mnemonic in enumerate(mnems):
        opcode = base + [0, 0x10, 0x20, 0x30][i]
        mode, size = modes[i]
        add([opcode], mnemonic, mode, size)


# A-side arithmetic / logic
for base, mnemonic in [
    (0x80, "SUBA"),
    (0x81, "CMPA"),
    (0x84, "ANDA"),
    (0x86, "LDAA"),
    (0x88, "EORA"),
    (0x89, "ADCA"),
    (0x8A, "ORAA"),
    (0x8B, "ADDA"),
]:
    add([base], mnemonic, "imm", 2)
    add([base + 0x10], mnemonic, "dir", 2)
    add([base + 0x20], mnemonic, "idx", 2)
    add([base + 0x30], mnemonic, "ext", 3)

add([0x8C], "CPX", "imm16", 3)
add([0x9C], "CPX", "dir", 2)
add([0xAC], "CPX", "idx", 2)
add([0xBC], "CPX", "ext", 3)
add([0x8E], "LDS", "imm16", 3)
add([0x9E], "LDS", "dir", 2)
add([0xAE], "LDS", "idx", 2)
add([0xBE], "LDS", "ext", 3)
add([0x8F], "STS", "imm16", 3)
add([0x9F], "STS", "dir", 2)
add([0xAF], "STS", "idx", 2)
add([0xBF], "STS", "ext", 3)

# B-side
for base, mnemonic in [
    (0xC0, "SUBB"),
    (0xC1, "CMPB"),
    (0xC2, "SBCB"),
    (0xC4, "ANDB"),
    (0xC6, "LDAB"),
    (0xC8, "EORB"),
    (0xC9, "ADCB"),
    (0xCA, "ORAB"),
    (0xCB, "ADDB"),
]:
    add([base], mnemonic, "imm", 2)
    add([base + 0x10], mnemonic, "dir", 2)
    add([base + 0x20], mnemonic, "idx", 2)
    add([base + 0x30], mnemonic, "ext", 3)

add([0x83], "SUBD", "imm16", 3)
add([0x93], "SUBD", "dir", 2)
add([0xA3], "SUBD", "idx", 2)
add([0xB3], "SUBD", "ext", 3)
add([0xCD], "LDX", "imm16", 3)
add([0xDD], "LDX", "dir", 2)
add([0xED], "LDX", "idx", 2)
add([0xFD], "LDX", "ext", 3)
add([0xCE], "LDX", "imm16", 3)
add([0xDE], "LDX", "dir", 2)
add([0xEE], "LDX", "idx", 2)
add([0xFE], "LDX", "ext", 3)
add([0xCF], "STX", "imm16", 3)
add([0xDF], "STX", "dir", 2)
add([0xEF], "STX", "idx", 2)
add([0xFF], "STX", "ext", 3)
add([0xBD], "JSR", "ext", 3)
add([0xAD], "JSR", "idx", 2)

# Missing 6800 ops that do not fit the simple patterns.
add([0x92], "SBCA", "dir", 2)
add([0xA2], "SBCA", "idx", 2)
add([0xB2], "SBCA", "ext", 3)
add([0x97], "STAA", "dir", 2)
add([0xA7], "STAA", "idx", 2)
add([0xB7], "STAA", "ext", 3)
add([0xD6], "LDAB", "dir", 2)
add([0xE6], "LDAB", "idx", 2)
add([0xF6], "LDAB", "ext", 3)
add([0xD7], "STAB", "dir", 2)
add([0xE7], "STAB", "idx", 2)
add([0xF7], "STAB", "ext", 3)
add([0xDA], "ORAB", "dir", 2)
add([0xDB], "ADDB", "dir", 2)
add([0xE9], "ADCB", "idx", 2)
add([0xF9], "ADCB", "ext", 3)
add([0x98], "EORA", "dir", 2)
add([0x99], "ADCA", "dir", 2)
add([0x9A], "ORAA", "dir", 2)
add([0x9B], "ADDA", "dir", 2)
add([0x94], "ANDA", "dir", 2)
add([0x95], "BITA", "dir", 2)
add([0xA4], "ANDA", "idx", 2)
add([0xA5], "BITA", "idx", 2)
add([0xB4], "ANDA", "ext", 3)
add([0xB5], "BITA", "ext", 3)
add([0xC5], "BITB", "imm", 2)
add([0xD5], "BITB", "dir", 2)
add([0xE5], "BITB", "idx", 2)
add([0xF5], "BITB", "ext", 3)
add([0xD2], "SBCB", "dir", 2)
add([0xE2], "SBCB", "idx", 2)
add([0xF2], "SBCB", "ext", 3)
add([0xD3], "ADDD", "dir", 2)
add([0xE3], "ADDD", "idx", 2)
add([0xF3], "ADDD", "ext", 3)
add([0x96], "LDAA", "dir", 2)
add([0xA6], "LDAA", "idx", 2)
add([0xB6], "LDAA", "ext", 3)


def parse_s19(path: Path) -> dict[int, int]:
    mem: dict[int, int] = {}
    for line in path.read_text().splitlines():
        if not line.startswith("S1"):
            continue
        count = int(line[2:4], 16)
        addr = int(line[4:8], 16)
        data = line[8 : 8 + (count - 3) * 2]
        for i in range(0, len(data), 2):
            mem[addr + i // 2] = int(data[i : i + 2], 16)
    return mem


def mem_ranges(mem: dict[int, int]) -> list[tuple[int, int]]:
    addrs = sorted(mem)
    ranges: list[tuple[int, int]] = []
    start = prev = addrs[0]
    for addr in addrs[1:]:
        if addr == prev + 1:
            prev = addr
        else:
            ranges.append((start, prev))
            start = prev = addr
    ranges.append((start, prev))
    return ranges


def u16(mem: dict[int, int], addr: int) -> int:
    return (mem[addr] << 8) | mem[addr + 1]


def signed8(value: int) -> int:
    return value - 0x100 if value & 0x80 else value


def decode(mem: dict[int, int], pc: int) -> tuple[Op | None, list[int]]:
    op = OPS.get(mem.get(pc, -1))
    if op is None:
        return None, [mem[pc]]
    bytes_ = [mem.get(pc + i, 0) for i in range(op.size)]
    return op, bytes_


def looks_ascii(mem: dict[int, int], start: int, end: int) -> bool:
    length = end - start + 1
    if length < 4:
        return False
    printable = sum(1 for addr in range(start, end + 1) if chr(mem[addr]) in string.printable and mem[addr] not in (0x0B, 0x0C))
    return printable / length > 0.8


def find_ascii_runs(mem: dict[int, int], start: int, end: int) -> list[tuple[int, int]]:
    runs = []
    run_start: int | None = None
    for addr in range(start, end + 1):
        b = mem[addr]
        ok = 32 <= b <= 126
        if ok and run_start is None:
            run_start = addr
        elif not ok and run_start is not None:
            if addr - run_start >= 4:
                runs.append((run_start, addr - 1))
            run_start = None
    if run_start is not None and end + 1 - run_start >= 4:
        runs.append((run_start, end))
    return runs


def discover_code(mem: dict[int, int], entries: list[int]) -> tuple[set[int], set[int], set[int]]:
    code: set[int] = set()
    instr_starts: set[int] = set()
    labels: set[int] = set(entries)
    queue = [entry for entry in entries if entry in mem]
    visited = set()
    while queue:
        pc = queue.pop()
        if pc in visited or pc not in mem:
            continue
        visited.add(pc)
        while pc in mem and pc not in instr_starts:
            op, raw = decode(mem, pc)
            if op is None:
                break
            if any((pc + i) not in mem for i in range(op.size)):
                break
            instr_starts.add(pc)
            for i in range(op.size):
                code.add(pc + i)
            next_pc = pc + op.size
            branch_target = None
            jump_target = None
            if op.mode == "rel":
                branch_target = (next_pc + signed8(raw[1])) & 0xFFFF
                if branch_target in mem:
                    labels.add(branch_target)
                    queue.append(branch_target)
            elif op.mode == "ext" and op.mnemonic in {"JMP", "JSR"}:
                jump_target = (raw[1] << 8) | raw[2]
                if jump_target in mem:
                    labels.add(jump_target)
                    queue.append(jump_target)

            if op.mnemonic in {"RTS", "RTI", "SWI", "WAI"}:
                break
            if op.mnemonic == "JMP":
                break
            if op.mnemonic == "BRA":
                break
            pc = next_pc
    return code, instr_starts, labels


def label_name(addr: int, code_labels: set[int]) -> str:
    return ("L" if addr in code_labels else "D") + f"{addr:04X}"


def fmt_operand(mem: dict[int, int], addr: int, op: Op, raw: list[int], labels: set[int]) -> str:
    if op.mode == "inh":
        return ""
    if op.mode == "imm":
        return f"#$%02X" % raw[1]
    if op.mode == "imm16":
        return f"#$%04X" % ((raw[1] << 8) | raw[2])
    if op.mode == "rel":
        target = (addr + op.size + signed8(raw[1])) & 0xFFFF
        return label_name(target, labels)
    if op.mode == "dir":
        operand = raw[1]
        return f"$%02X" % operand
    if op.mode == "idx":
        return f"$%02X,X" % raw[1]
    if op.mode == "ext":
        operand = (raw[1] << 8) | raw[2]
        return label_name(operand, labels) if operand in labels else f"$%04X" % operand
    raise ValueError(op.mode)


def data_comment(mem: dict[int, int], start: int, end: int) -> str:
    text = "".join(chr(mem[a]) if 32 <= mem[a] <= 126 else "." for a in range(start, end + 1))
    if any(c != "." for c in text):
        return f" ; '{text}'"
    return ""


def emit(path_in: Path, path_out: Path) -> None:
    mem = parse_s19(path_in)
    ranges = mem_ranges(mem)

    # Seed the walker with obvious entry points: the reset jump and the starts of executable ranges.
    seeds = [
        0x0020,
        0x0898,
        0x097C,
        0x0E23,
        0x10A0,
        0x1DAC,
        0x23D1,
        0x2575,
        # Entry points referenced by jump tables.
        0x0BFB,
        0x0C92,
        0x0C96,
        0x0CB8,
        0x0CC7,
        0x0D7F,
        0x1295,
        0x12C4,
        0x1310,
        0x1316,
        0x1382,
        0x13A3,
        0x1433,
        0x1436,
        0x1467,
        0x149F,
        0x14AD,
        0x14D6,
        0x16DA,
        0x16DF,
        0x16E0,
        0x16E6,
        0x16F0,
        0x1719,
        0x1734,
        0x173B,
        0x1763,
        0x17F9,
        0x185E,
        0x1864,
        0x1892,
        0x1899,
        0x18A0,
        0x18CE,
        0x18F3,
        0x194D,
        0x1EF1,
        0x20C0,
        0x22C2,
        0x22E7,
        0x22EB,
        0x22F0,
        0x233A,
        0x2388,
        0x239C,
        0x23AA,
        0x23B0,
        0x23BB,
        0x23C2,
        0x23F6,
        0x2413,
        0x2419,
        0x242B,
        0x2434,
        0x243D,
        0x2443,
        0x247B,
        0x249B,
        0x24C9,
    ]
    code_bytes, instr_starts, labels = discover_code(mem, seeds)

    equates: list[tuple[str, int, str]] = [
        ("TABLE_PTR_LO", 0x29, "Scratch pointer used by the generic table walkers."),
        ("TABLE_PTR_HI", 0x2A, "Scratch pointer high byte used by the generic table walkers."),
        ("TOKEN_INDEX", 0x3E, "General-purpose token / statement / operator index register."),
        ("TOKEN_CLASS", 0x3F, "Current token sub-class or precedence nibble."),
        ("ERROR_CODE", 0x40, "Current diagnostic / formatter code."),
        ("HASH_ACCUM", 0x41, "Identifier hash accumulator / loop counter."),
        ("ATTR_BITS", 0x42, "Attribute bits for the current symbol or descriptor."),
        ("EXPR_PTR", 0x43, "Expression descriptor pointer scratch."),
        ("EXPR_PTR_HI", 0x44, "Expression descriptor pointer scratch high byte."),
        ("SYM_SCAN_PTR", 0x49, "Current symbol-chain scan pointer."),
        ("SYM_MATCH_PTR", 0x4B, "Matched symbol/member pointer while comparing names."),
        ("KEYWORD_CODE", 0x4D, "Current statement keyword code."),
        ("IDENT_LEN", 0x4E, "Current identifier/text length."),
        ("STMT_STATE", 0x4F, "Statement-classification state / keyword phase."),
        ("VALUE_LO", 0x50, "Expression value/address low byte."),
        ("VALUE_HI", 0x51, "Expression value/address high byte."),
        ("BASE_TEXT_PTR", 0x52, "Base pointer for generated text/object fragments."),
        ("SYM_INSERT_MODE", 0x54, "Symbol insertion/update mode flag."),
        ("SYM_PTR", 0x55, "Pointer to the current symbol-table entry."),
        ("DEFAULT_ATTR", 0x57, "Default descriptor attribute byte."),
        ("EXPR_DEPTH", 0x58, "Expression descriptor stack depth."),
        ("EXPR_STACK_PTR", 0x59, "Top of the temporary 3-byte expression descriptor stack."),
        ("OPERAND_LO", 0x5B, "Temporary operand/address low byte."),
        ("OPERAND_HI", 0x5C, "Temporary operand/address high byte."),
        ("OPERAND_TAG", 0x5D, "Temporary operand/operator tag."),
        ("PREC_SCAN_INDEX", 0x5E, "Expression precedence scan index."),
        ("MUL_ACC_LO", 0x5F, "Low byte of multiply/shift accumulator."),
        ("MUL_ACC_HI", 0x60, "High byte of multiply/shift accumulator."),
        ("CODE_PTR", 0x61, "Current code-generation write pointer."),
        ("CODE_PTR_HI", 0x62, "Current code-generation write pointer high byte."),
        ("TEMP_BASE_PTR", 0x63, "Base pointer for current temporary/code block."),
        ("FIXUP_PTR", 0x65, "Pointer to current fixup/relocation chain entry."),
        ("TMP_COUNTER", 0x67, "Small per-line counter used by code generation."),
        ("PAGE_COUNT", 0x68, "Listing page / error-page counter."),
        ("LIST_REC_LEN", 0x0963, "Length of the current printable/listing record."),
        ("LIST_REC_PTR", 0x0961, "Pointer to the current printable/listing record."),
        ("LIST_HEX_BUF", 0x0964, "Scratch ASCII buffer used for listing/object text."),
        ("OBJ_QUEUE_PTR", 0x0977, "Pointer to deferred object-output descriptors."),
        ("OBJ_QUEUE_BASE", 0x096E, "Deferred object-output descriptor storage."),
        ("PREC_STACK_PTR", 0x1EC7, "Pointer to the operator-precedence stack."),
        ("IOCB_PTR", 0x0638, "Pointer to the active monitor I/O control block."),
        ("IOCB_DATA", 0x063A, "Current monitor I/O data/address field."),
        ("EMIT_OPCODE", 0x063C, "Current output fragment opcode/major type."),
        ("EMIT_FLAGS", 0x063D, "Current output fragment flags."),
        ("EMIT_TARGET", 0x0641, "Target address/pointer for the fragment being emitted."),
        ("EMIT_WIDTH", 0x0646, "Encoded width/type for the fragment being emitted."),
        ("REC_BYTES_LEFT", 0x064A, "Remaining payload bytes before the current record must flush."),
        ("REC_OPEN", 0x064B, "Nonzero when an output record is open."),
        ("REC_ADDR", 0x064C, "Current output record base address."),
        ("REC_DATA_PTR", 0x064E, "Current write pointer inside the output record buffer."),
        ("RELOC_ROOM", 0x0650, "Remaining relocation room/count for the current record."),
        ("RELOC_PENDING", 0x0651, "Flush-needed flag for relocation state."),
        ("RELOC_ACTIVE", 0x0652, "Nonzero when relocation collection is active."),
        ("RELOC_DIRTY", 0x0653, "Relocation metadata has been added to the current record."),
        ("RELOC_BITS_OPEN", 0x0654, "A relocation bitmap byte is currently being packed."),
        ("RELOC_BASE", 0x0655, "Base address used when computing relocation deltas."),
        ("RELOC_BASE_HI", 0x0656, "High byte of relocation base."),
        ("RELOC_CLASS", 0x0657, "Packed relocation-class bits for the current record."),
        ("RELOC_REC_PTR", 0x0658, "Pointer to the current relocation descriptor record."),
        ("RELOC_SLOT_A", 0x065A, "First relocation slot/type used in the current record."),
        ("RELOC_SLOT_B", 0x065B, "Second relocation slot/type used in the current record."),
        ("RELOC_SLOT_C", 0x065C, "Third relocation slot/type used in the current record."),
        ("RELOC_COUNT", 0x065D, "Count of relocation descriptor bytes emitted."),
        ("RELOC_BITCOUNT", 0x065E, "Number of packed two-bit relocation classes in the current byte."),
        ("RELOC_PAYLOAD_PTR", 0x065F, "Pointer to relocation payload bytes."),
        ("RELOC_BIT_PTR", 0x0661, "Pointer to the relocation bitmap stream."),
        ("RELOC_BIT_PHASE", 0x0663, "Position inside the current relocation bitmap byte."),
        ("REC_HEADER", 0x0664, "Beginning of the object record header buffer."),
        ("REC_HEADER_LEN", 0x0665, "Object record header length/count byte."),
        ("REC_HEADER_ARG0", 0x0666, "Object record header argument / subtype."),
        ("REC_HEADER_ARG1", 0x0667, "Object record header argument / subtype."),
        ("REC_ADDR_SAVE", 0x0668, "Saved record address used when building prologue records."),
        ("REC_BUFFER", 0x066A, "Object record payload buffer."),
        ("PROLOGUE_TYPE", 0x0664, "Record-type byte while constructing object-module prologue records."),
        ("PROLOGUE_LEN", 0x0665, "Payload length for the current prologue/header record."),
        ("PROLOGUE_FRAG0", 0x0666, "First copied fragment byte in the prologue scratch record."),
        ("PROLOGUE_FRAG1", 0x0667, "Second copied fragment/control byte in the prologue scratch record."),
        ("PROLOGUE_ADDR0", 0x0668, "Address field used by the 0x34 prologue record."),
        ("PROLOGUE_KIND0", 0x066A, "Subtype/opcode byte used in prologue records 0x33/0x34."),
        ("PROLOGUE_TAG_OB", 0x066C, "ASCII tag fragment 'OB' written into the 0x32 prologue record."),
        ("PROLOGUE_KIND1", 0x066D, "Second subtype/opcode byte used in prologue records 0x33/0x34."),
        ("PROLOGUE_ADDR1", 0x066E, "Code/start address field used in prologue records 0x33/0x34."),
        ("PROLOGUE_KIND2", 0x0670, "Third subtype byte in the 0x33 prologue record."),
        ("PROLOGUE_ADDR2", 0x0671, "Entry/code-base address field in the 0x33 prologue record."),
        ("PROLOGUE_KIND3", 0x0673, "Fourth subtype byte in the 0x33 prologue record."),
        ("PROLOGUE_TEXT_BASE", 0x0674, "Saved BASE_TEXT_PTR written into the 0x33 prologue record."),
        ("PROLOGUE_RUNTIME_0", 0x0676, "ASCII '0' prefix in the 0x33 runtime-tag field."),
        ("PROLOGUE_RUNTIME_RU", 0x0677, "ASCII 'RU' in the 0x33 runtime-tag field."),
        ("PROLOGUE_RUNTIME_NSP", 0x0679, "ASCII 'N ' in the 0x33 runtime-tag field."),
        ("PROLOGUE_RUNTIME_SP2", 0x067B, "Trailing spaces in the 0x33 runtime-tag field."),
    ]

    org_comments: dict[int, list[str]] = {
        0x01EC: [
            "Statement keyword table used during statement classification.",
            "Entries are 0x04-terminated strings packed back-to-back.",
            "Tentative keyword map:",
            "  AGO        likely GOTO / transfer statement family",
            "  BTO        likely auxiliary TO-token used by DO/GOTO parsing",
            "  CIF        likely IF statement family",
            "  DSTOP      likely STOP statement",
            "  EEND       likely END statement",
            "  FDO        likely DO statement family",
            "  GCONTINUE  likely CONTINUE statement",
            "  HCALL      likely CALL statement",
            "  ISUBROUTINE likely SUBROUTINE declaration/header",
            "  JFUNCTION  likely FUNCTION declaration/header",
            "  KRETURN    likely RETURN statement",
            "  LREAD      likely READ statement family",
            "  MPRINT     likely PRINT statement family",
            "  MWRITE     likely WRITE statement family",
            "  NDIMENSION likely DIMENSION declaration",
            "  OCOMMON    likely COMMON declaration",
            "  PDATA      likely DATA statement",
            "  QFORMAT    likely FORMAT statement",
            "  RNOT/SAND/TOR/UEQ/VNE/WLE/XGE/YLT/ZGT are expression/logical operators",
            "  0ENDFILE/0REWIND are likely file-control statements",
        ],
        0x02D1: [
            "Per-keyword metadata table indexed from the statement keyword table.",
            "The compiler uses this to classify statements and route them to handlers.",
            "Best current read: this is a packed set of parallel descriptor slices rather than one",
            "flat record per keyword. Early bytes act like family/dispatch selectors; later bytes carry",
            "flags, secondary template selectors, and small literal parameters consumed by L111C/L121E.",
        ],
        0x03A7: [
            "Primary token / operator descriptor table.",
            "Many parser helpers index through this block with L2519/L2562.",
        ],
        0x0863: [
            "Resident banner strings printed when the compiler starts.",
        ],
        0x0964: [
            "Default object-module header text for the generated output.",
        ],
        0x1277: [
            "Fixed pointer/constant block used by the statement walker, diagnostic formatter,",
            "symbol hash seeding, and cold-start initialization.",
            "The words here are not code pointers as a group; most are small workspace or table bases.",
        ],
        0x1EC9: [
            "Operator precedence / class table used by the expression parser around L1EF1.",
        ],
        0x23C8: [
            "Packed name fragment used by the transfer / object trailer logic.",
        ],
    }

    label_comments: dict[int, list[str]] = {
        0x0020: [
            "Reset entry from the S-record image.",
            "Control transfers directly into the resident compiler driver at L2575.",
        ],
        0x0899: [
            "Print the startup banner and copyright strings.",
        ],
        0x097C: [
            "Compiler cold-start initialization.",
            "Sets the runtime stack, seeds descriptor pointers, clears state,",
            "copies keyword data into working storage, and enters the main compile loop.",
        ],
        0x0A4B: [
            "Checksum / hash helper for the current identifier text in $00C4..",
            "Produces a mixed value in $41/$55/$56 used while walking symbol chains.",
        ],
        0x0ADF: [
            "Create or extend a symbol-table entry at $01BF.",
            "Stores class bits from A plus source-location and auxiliary descriptor fields.",
        ],
        0x0B83: [
            "Classify the current statement keyword and load its dispatch metadata.",
            "This is the front door from statement text to statement-specific handlers.",
        ],
        0x0D7F: [
            "Emit an error message for the code in A.",
            "Formats the line marker, prints '**ERROR', and advances listing state.",
        ],
        0x0EEE: [
            "High-level diagnostic formatter.",
            "Seeds the error code and routes into L0F08 to build/print the message text.",
        ],
        0x0F08: [
            "Low-level diagnostic message builder/printer.",
            "Selects one of several message templates, formats numeric fields into $0DD0..,",
            "and sends the finished text to the listing/output path.",
        ],
        0x0E23: [
            "Unpack one encoded descriptor from the parse tables at $0114/$0115.",
            "Used when decoding statement templates and operand classes.",
        ],
        0x1091: [
            "Listing/page-management helper.",
            "Prints headings and keeps the page / column counters in sync.",
        ],
        0x10E5: [
            "Read a source line from the monitor input service into the line buffer.",
            "Handles erase/end-of-line characters and appends the internal 0x04 terminator.",
        ],
        0x111C: [
            "Walk the statement descriptor list built from the keyword tables.",
            "Each descriptor is interpreted by L1205/L121E and routed to the proper handler.",
        ],
        0x1205: [
            "Map a 1-based statement descriptor index in $02CE to its table entry.",
        ],
        0x121E: [
            "Interpret one statement descriptor entry and optionally queue code generation.",
        ],
        0x1295: [
            "Statement handler dispatcher.",
            "The jump table below fans out to the major FORTRAN statement compilers.",
            "These aliases are tentative, but this cluster looks heavily skewed toward",
            "declaration/dimension/common-data handling plus the end-of-compilation pass.",
            "Working family map:",
            "  declaration/data side: STMT_DECLARE_OR_REFERENCE, STMT_DECLARATION_SETUP,",
            "    STMT_DIMENSION_STORE_BOUND, STMT_DIMENSION_OR_COMMON_FINALIZE,",
            "    STMT_DATA_OR_COMMON_PREP, STMT_DATA_OR_COMMON_EMIT_FIELDS, STMT_REQUIRE_SYMBOL_ENTRY",
            "  summary/fixup side: STMT_TRANSFER_FIXUP_MERGE, STMT_END_AND_SYMBOL_LIST, STMT_SET_FIXUP_BASE",
            "  codegen side: STMT_TRANSFER_CODEGEN_DISPATCH plus STMT_SOURCE_IO_OR_TRANSFER_DISPATCH",
            "Best current guess:",
            "  DIMENSION/COMMON/DATA/FORMAT cluster lives mainly in the declaration/data side above.",
            "  READ/PRINT/WRITE likely flow through STMT_SOURCE_IO_OR_TRANSFER_DISPATCH.",
            "  GOTO/IF/CALL/RETURN likely lean on STMT_TRANSFER_CODEGEN_DISPATCH and fixup helpers.",
        ],
        0x12C4: [
            "Allocate / finalize a symbol reference entry for the current identifier.",
            "One of the early statement handlers: resolves the current symbol,",
            "creates a table entry if needed, and seeds the generated descriptor fields.",
        ],
        0x1310: [
            "Report a syntax/semantic error 0x1B.",
        ],
        0x1316: [
            "Set symbol attributes for the current declaration or assignment context.",
        ],
        0x1382: [
            "Store one computed offset/extent pair into the current symbol entry.",
            "Used by declaration-style handlers after expression extents are evaluated.",
        ],
        0x13A3: [
            "Finalize address/extent data for the current declared symbol.",
            "Links the symbol to the active code/fixup pointer and emits descriptor bytes.",
        ],
        0x1433: [
            "Abort the current statement and return to the main source-line loop.",
        ],
        0x1436: [
            "Push the current symbol/value descriptor and derive a base address for later codegen.",
            "Used before emitting array/string extents or storage-allocation fields.",
        ],
        0x1467: [
            "Emit one or two extent/address fields from the descriptor on the expression stack.",
            "The second field is emitted only for wider or multiword descriptors.",
        ],
        0x149F: [
            "Ensure the current identifier has a symbol-table entry, creating it if needed.",
        ],
        0x14AD: [
            "Deduplicate a patched address against the last recorded fixup location.",
            "If the same location recurs, dispatches through L20C0 to merge or update it.",
        ],
        0x14D6: [
            "End-of-compilation symbol summary/listing pass.",
            "Walks the per-letter symbol chains, formats names and values into the listing buffer,",
            "and likely emits the 'UNDEFINED SYMBOLS ... ERRORS' report.",
            "The two nested loops first print symbol names, then print associated numeric values/page counts.",
        ],
        0x16DA: [
            "Seed FIXUP_PTR from the current code-generation base pointer.",
        ],
        0x16DF: [
            "No-op / early-return helper used by the statement dispatcher.",
        ],
        0x16E0: [
            "Report diagnostic 0x1E.",
        ],
        0x16E6: [
            "Emit a 3-byte descriptor referenced from the top of the expression stack.",
        ],
        0x16F0: [
            "Late statement-handler dispatcher for code emission and fixup resolution.",
            "This cluster appears after primary parsing and mainly drives object-output templates.",
            "It looks more transfer/fixup-oriented than L22C2: many entries feed pending branch/call",
            "locations into the template/object layer rather than walking source-I/O item lists.",
            "Strongest current statement mapping:",
            "  L1719  assigned/indirect transfer via a stored target address",
            "  L1763  computed-GOTO-style multi-target dispatcher",
            "  L1899  operand-free transfer, most likely RETURN",
        ],
        0x1719: [
            "Transfer subentry 0: emit a simple 3-byte symbol/address reference followed by a fixed trailer template.",
            "The emitted FE..6E00 pattern looks like 'load X from variable; JMP 0,X'.",
            "Best current fit: assigned/indirect GOTO through a stored statement address.",
        ],
        0x1734: [
            "Transfer subentry 1: initialize the temporary transfer-target list pointer at $0390.",
        ],
        0x173B: [
            "Transfer subentry 2: append the current symbol target address to the list at $0390.",
            "Raises diagnostic 0x14 if the temporary target-list buffer reaches its limit.",
        ],
        0x1763: [
            "Transfer subentry 3: build and emit the inline multi-target transfer scaffold.",
            "This is the strongest computed-GOTO candidate in the transfer cluster: it emits",
            "a dispatcher skeleton and then serializes the accumulated target list from $0390.",
        ],
        0x17F9: [
            "Transfer subentry 4: resolve or create one transfer target symbol and emit its current code address.",
            "Likely used by a plain one-target transfer statement when the destination may still be undefined.",
        ],
        0x185E: [
            "Transfer subentry 5: report diagnostic 0x0F.",
        ],
        0x1864: [
            "Transfer subentry 6: create/resolve a direct target symbol, emit its address, and advance CODE_PTR.",
            "Best current fit: direct one-target transfer or call form that reserves a fixed-width inline target field.",
        ],
        0x1892: [
            "Transfer subentry 7: emit the current CODE_PTR through the template layer.",
        ],
        0x1899: [
            "Transfer subentry 8: emit the fixed template at $24ED.",
            "The fixed word at $24ED is 0x3900, so this is the clearest RETURN-style emitter in the cluster.",
        ],
        0x18A0: [
            "Transfer subentry 9: allocate a numbered local transfer target and append its address to the list at $0390.",
            "Strong candidate for the label-list half of a computed/assigned transfer statement.",
        ],
        0x18CE: [
            "Transfer subentry 10: append the current expression descriptor to the transfer list buffer.",
            "Pairs naturally with the computed/assigned transfer helpers around L18A0/L18F3.",
        ],
        0x18F3: [
            "Transfer subentry 11: walk the accumulated transfer-list buffer and emit each target entry.",
            "Likely the final serialization pass for computed/assigned transfer tables.",
        ],
        0x194D: [
            "Expression-operator dispatcher.",
            "Routes parsed operators to code-generation fragments and stack manipulation helpers.",
        ],
        0x1A4E: [
            "16-bit shift/add multiply helper used during expression evaluation.",
        ],
        0x1DAC: [
            "Commit the current expression result descriptor back into the symbol/reference entry.",
        ],
        0x1DD5: [
            "Push one descriptor onto the parser's temporary stack at $59.",
            "Descriptors are three bytes wide and carry type / address / attribute bits.",
        ],
        0x1E8A: [
            "Pop one 3-byte descriptor from the temporary expression stack.",
        ],
        0x1E95: [
            "Derive default attribute bits for the current token.",
            "This decides whether a pushed descriptor should be marked relocatable/immediate.",
        ],
        0x1EF1: [
            "Operator-precedence parser core.",
            "Uses the precedence table at $1EC9 and a descriptor stack rooted at $1EC7.",
        ],
        0x1FA0: [
            "Insert an operator into the precedence stack or collapse pending operators first.",
        ],
        0x20C0: [
            "Secondary statement/expression sub-dispatcher.",
            "Used by several statement handlers once the lead keyword has been identified.",
            "The call from L14AD suggests it also merges or rewrites pending fixups.",
        ],
        0x22C2: [
            "Tertiary statement dispatch used by a mixed family of transfer and source-I/O statements.",
            "This block performs operand classification and then routes into the code/template emitters.",
            "Its strongest READ/PRINT/WRITE/FORMAT evidence is in subentries 2 and 3, which walk operand",
            "descriptors from the expression stack. The outer family still shares templates with transfer code.",
            "It is a better candidate for source-language I/O handling than the 2C67 record code.",
        ],
        0x22E7: [
            "Subentry 0: clear the local I/O substate counter in $40.",
        ],
        0x22EB: [
            "Subentry 1: initialize the local I/O substate counter to 1.",
        ],
        0x22F0: [
            "Subentry 2: classify one I/O list item, emit its descriptor, and queue the related fixup.",
            "This looks like the main READ/WRITE/PRINT item-list path.",
        ],
        0x233A: [
            "Subentry 3: normalize an I/O control/format descriptor from the expression stack.",
            "The bit-masking and width selection here make this a good candidate for FORMAT/control-list handling.",
        ],
        0x2388: [
            "Subentry 4: emit a fixed-width template via L20C0 and advance BASE_TEXT_PTR by six bytes.",
        ],
        0x239C: [
            "Subentry 5: emit template 0x0A through L20C0 without consuming the caller's BASE_TEXT_PTR.",
        ],
        0x23AA: [
            "Subentry 6: report diagnostic 0x18.",
        ],
        0x23B0: [
            "Subentry 7: emit templates 0x08 then 0x09 via L20C0.",
            "Likely one of the paired source-I/O statement forms.",
        ],
        0x23BB: [
            "Subentry 8: bump the local I/O substate counter by two.",
        ],
        0x23C2: [
            "Subentry 9: increment the per-statement item counter at $0138.",
        ],
        0x23D1: [
            "Low-level code-generation dispatcher.",
            "Selects one of several output templates and patches addresses / fixups.",
        ],
        0x23F6: [
            "Template subentry 0: reset the current I/O/codegen template state.",
            "Clears the running template bytes, snapshots BASE_TEXT_PTR, and zeros the local template latches.",
        ],
        0x2413: [
            "Template subentry 1: latch MUL_ACC_HI/$60 as the first template parameter byte.",
        ],
        0x2419: [
            "Template subentry 2: resolve the keyword-driven trailer code and emit the current parameter byte.",
        ],
        0x242B: [
            "Template subentry 3: emit the template pointer held at $23C6 and finish.",
        ],
        0x2434: [
            "Template subentry 4: emit the fixed template pointer at $24DF and finish.",
        ],
        0x243D: [
            "Template subentry 5: latch MUL_ACC_HI/$60 as the secondary template accumulator.",
        ],
        0x2443: [
            "Template subentry 6: pack the latched template nibbles/bytes and emit one or more template fields.",
        ],
        0x247B: [
            "Template helper: scan the small keyword fragment table at $23C8 ('/XPIAFE')",
            "and convert KEYWORD_CODE into a compact template selector in $23CF.",
        ],
        0x249B: [
            "Template subentry 7: emit a string/name reference using IDENT_LEN and the text buffer at $00C4.",
        ],
        0x24C9: [
            "Template subentry 8: emit the current source-location/fixup record and reseed $032A.",
        ],
        0x2716: [
            "Dispatch one encoded object-fragment descriptor to the object/listing emitters.",
            "The jumps to L2D9E/L2DC1/L2C67 are compiler-output record formats, not FORTRAN READ/WRITE handlers.",
        ],
        0x2519: [
            "Generic descriptor-table walker.",
            "Follows a compact table of (count, scale, base) records and returns X = base + index.",
        ],
        0x254E: [
            "8-bit by small-constant multiply helper used by L2519.",
        ],
        0x2562: [
            "Add B to X, treating B=0 as 'select the byte before the table'.",
        ],
        0x2567: [
            "Add B to X and return the adjusted pointer.",
        ],
        0x2575: [
            "Resident compiler top-level driver.",
            "Validates source/list devices, initializes I/O vectors, then iterates over source lines.",
        ],
        0x2637: [
            "Decode monitor device mnemonic in X ('CN', 'LP', etc.).",
        ],
        0x264D: [
            "Fetch the next I/O control block and mirror it into zero-page work pointers.",
        ],
        0x2670: [
            "Convert a binary value in A:B to ASCII decimal at X.",
        ],
        0x26D0: [
            "Fetch the next source line and reset all per-line compiler state.",
        ],
        0x2716: [
            "Dispatch one encoded object record / pseudo-op fragment to the output builder.",
        ],
        0x2783: [
            "Append a compact six-byte deferred output record into the resident queue at $0863.",
        ],
        0x27C4: [
            "Flush the deferred output queue built by L2783.",
        ],
        0x285B: [
            "Prime the source input work pointers by reading the initial monitor control words.",
        ],
        0x286D: [
            "Read one 5-byte monitor control block into $0638..$063C.",
        ],
        0x288A: [
            "Checksum-print helper for listing/object records.",
            "Emits: record-type byte in A, payload bytes from LIST_REC_PTR/LIST_REC_LEN,",
            "two's-complement checksum, CR, LF, and a trailing NUL byte.",
        ],
        0x28C7: [
            "Main object-record emitter.",
            "Accepts a descriptor in A/B/X and turns it into packed bytes / relocation records.",
        ],
        0x2936: [
            "Assign relocation slots and build packed relocation descriptors for an output record.",
        ],
        0x2A40: [
            "Start a new output record buffer at $066A.",
        ],
        0x2A5E: [
            "Append one byte to the current output record, flushing if the block is full or discontiguous.",
        ],
        0x2A93: [
            "Flush the raw-byte payload currently buffered by L2A5E.",
        ],
        0x2AB2: [
            "Initialize relocation/fixup collection for the current output record.",
        ],
        0x2AC4: [
            "Begin the per-record relocation bitmap and descriptor area.",
        ],
        0x2B1A: [
            "Finalize relocation metadata for the current record.",
        ],
        0x2BC4: [
            "Pack two-bit relocation classes into the current relocation bitmap.",
        ],
        0x2C11: [
            "Flush the current relocation bitmap byte when it is partially filled.",
        ],
        0x2C42: [
            "Flush both payload and relocation metadata for the current object record.",
        ],
        0x2C67: [
            "Emit the object-module prologue/header records.",
            "This is part of the compiler's own object-file emitter, not a source-language I/O statement handler.",
            "The visible prologue sequence emits record types 0x36, 0x32, 0x33, and optionally 0x34,",
            "and seeds the default module text around $0964 ('30      3$MAIN  ').",
            "The 0x33 record also captures CODE_PTR, BASE_TEXT_PTR, and the ASCII runtime tag '0RUN   '.",
            "The optional 0x34 record packages a small bootstrap stub at load address $0020,",
            "with data bytes that decode as an 8E/7E-style loader handoff sequence.",
        ],
        0x2D9E: [
            "Emit a standard object/listing text record from the scratch buffer at $0964/$0966.",
            "Reached from L2716 for one class of compiler-generated output record.",
        ],
        0x2DC1: [
            "Emit the alternate object/listing text record format used by another L2716 fragment class.",
            "Also part of the compiler's output-record machinery rather than FORTRAN READ/WRITE semantics.",
        ],
        0x2DE3: [
            "Add B to the 16-bit pointer in X and return the adjusted pointer.",
            "Used heavily while laying out record headers and relocation payload fields.",
        ],
        0x2DF7: [
            "Store A:B as ASCII hexadecimal at X.",
        ],
        0x2DFC: [
            "Store one byte in A as two ASCII hex digits at X.",
        ],
        0x2E17: [
            "Copy B bytes from the fragment stack rooted at OBJ_QUEUE_PTR into the buffer at X.",
            "Used by the object-record prologue/text emitters to materialize small descriptor fragments.",
        ],
        0x1277: [
            "Pointer to the temporary diagnostic/listing buffer at $0DD0.",
            "Used when statement-descriptor tracing is enabled.",
        ],
        0x1279: [
            "Lower sentinel for the statement-descriptor work stack near $02A3.",
        ],
        0x127B: [
            "Upper limit for the statement-descriptor work stack.",
        ],
        0x127D: [
            "Initial/top pointer for the statement-descriptor work stack.",
        ],
        0x127F: [
            "Pointer constant selecting SYM_SCAN_PTR ($0049) as a diagnostic numeric source.",
        ],
        0x1281: [
            "Pointer constant selecting TMP_DESC_PTR ($0047) as a diagnostic numeric source.",
        ],
        0x1283: [
            "Pointer constant selecting BASE_TEXT_PTR ($0052) as a diagnostic numeric source.",
        ],
        0x1285: [
            "Pointer constant selecting ATTR_BITS ($0042) as a diagnostic source byte.",
        ],
        0x128B: [
            "Base pointer constant for the per-letter symbol-chain table at $019F.",
            "Added to the identifier hash in L0A4B to seed symbol lookup.",
        ],
        0x128D: [
            "Pointer constant selecting CODE_PTR ($0061).",
            "Used as the cold-start seed for FIXUP_PTR and some diagnostics.",
        ],
        0x128F: [
            "Cold-start pointer to the expression-descriptor stack area at $0111.",
        ],
        0x1291: [
            "Cold-start pointer to the pending-fixup / patch-list area at $0131.",
        ],
        0x1293: [
            "Literal 16-bit zero constant used to clear many symbol and descriptor fields.",
        ],
    }

    alias_labels: dict[int, list[str]] = {
        0x0EEE: ["DIAG_FORMAT"],
        0x0F08: ["DIAG_BUILD_AND_PRINT"],
        0x1295: ["STMT_DISPATCH"],
        0x12C4: ["STMT_DECLARE_OR_REFERENCE"],
        0x1310: ["STMT_DIAG_1B"],
        0x1316: ["STMT_DECLARATION_SETUP"],
        0x1382: ["STMT_DIMENSION_STORE_BOUND"],
        0x13A3: ["STMT_DIMENSION_OR_COMMON_FINALIZE"],
        0x1433: ["STMT_ABORT_LINE"],
        0x1436: ["STMT_DATA_OR_COMMON_PREP"],
        0x1467: ["STMT_DATA_OR_COMMON_EMIT_FIELDS"],
        0x149F: ["STMT_REQUIRE_SYMBOL_ENTRY"],
        0x14AD: ["STMT_TRANSFER_FIXUP_MERGE"],
        0x14D6: ["STMT_END_AND_SYMBOL_LIST"],
        0x16DA: ["STMT_SET_FIXUP_BASE"],
        0x16DF: ["STMT_NOOP"],
        0x16E0: ["STMT_DIAG_1E"],
        0x16E6: ["STMT_EMIT_STACK_DESC"],
        0x16F0: ["STMT_TRANSFER_CODEGEN_DISPATCH"],
        0x1719: ["TRANSFER_EMIT_ASSIGNED_GOTO"],
        0x1734: ["TRANSFER_LIST_INIT"],
        0x173B: ["TRANSFER_LIST_APPEND_SYMBOL"],
        0x1763: ["TRANSFER_EMIT_COMPUTED_GOTO"],
        0x17F9: ["TRANSFER_RESOLVE_AND_EMIT_TARGET"],
        0x185E: ["TRANSFER_DIAG_0F"],
        0x1864: ["TRANSFER_DEFINE_AND_EMIT_TARGET"],
        0x1892: ["TRANSFER_EMIT_CODE_PTR"],
        0x1899: ["TRANSFER_EMIT_RETURN"],
        0x18A0: ["TRANSFER_ALLOC_LOCAL_TARGET"],
        0x18CE: ["TRANSFER_LIST_APPEND_EXPR_DESC"],
        0x18F3: ["TRANSFER_LIST_SERIALIZE"],
        0x20C0: ["STMT_SECONDARY_DISPATCH"],
        0x22C2: ["STMT_SOURCE_IO_OR_TRANSFER_DISPATCH"],
        0x22E7: ["IO_SUBSTATE_CLEAR"],
        0x22EB: ["IO_SUBSTATE_SET_ONE"],
        0x22F0: ["IO_ITEM_EMIT"],
        0x233A: ["IO_CONTROL_OR_FORMAT_ITEM"],
        0x2388: ["IO_TEMPLATE_ADVANCE6"],
        0x239C: ["IO_TEMPLATE_PRESERVE_PTR"],
        0x23AA: ["IO_DIAG_18"],
        0x23B0: ["IO_TEMPLATE_08_09"],
        0x23BB: ["IO_SUBSTATE_ADD_TWO"],
        0x23C2: ["IO_ITEM_COUNT_INC"],
        0x23D1: ["CODEGEN_TEMPLATE_DISPATCH"],
        0x23F6: ["TEMPLATE_RESET_STATE"],
        0x2413: ["TEMPLATE_LATCH_PRIMARY"],
        0x2419: ["TEMPLATE_EMIT_KEYWORD_TRAILER"],
        0x242B: ["TEMPLATE_EMIT_PTR_23C6"],
        0x2434: ["TEMPLATE_EMIT_PTR_24DF"],
        0x243D: ["TEMPLATE_LATCH_SECONDARY"],
        0x2443: ["TEMPLATE_PACK_AND_EMIT"],
        0x247B: ["TEMPLATE_KEYWORD_SCAN"],
        0x249B: ["TEMPLATE_EMIT_TEXT_REF"],
        0x24C9: ["TEMPLATE_EMIT_FIXUP_RECORD"],
        0x1277: ["TRACE_DIAG_BUF_PTR"],
        0x1279: ["STMT_DESC_STACK_FLOOR"],
        0x127B: ["STMT_DESC_STACK_LIMIT"],
        0x127D: ["STMT_DESC_STACK_BASE"],
        0x127F: ["DIAG_PTR_SYM_SCAN"],
        0x1281: ["DIAG_PTR_TMP_DESC"],
        0x1283: ["DIAG_PTR_BASE_TEXT"],
        0x1285: ["DIAG_PTR_ATTR_BITS"],
        0x128B: ["SYM_HASH_BASE_PTR"],
        0x128D: ["INIT_CODE_PTR"],
        0x128F: ["INIT_EXPR_DESC_STACK_PTR"],
        0x1291: ["INIT_PATCH_LIST_PTR"],
        0x1293: ["CONST_ZERO_WORD"],
        0x2716: ["OBJECT_FRAGMENT_DISPATCH"],
        0x2D9E: ["OBJECT_TEXT_RECORD_A"],
        0x2DC1: ["OBJECT_TEXT_RECORD_B"],
        0x2DE3: ["ADD_B_TO_X"],
        0x2DF7: ["STORE_HEX16"],
        0x2DFC: ["STORE_HEX8"],
        0x2E17: ["COPY_FRAGMENT_BYTES"],
    }

    lines: list[str] = []
    lines.append("; Auto-generated 6800 disassembly from fort S19 image")
    lines.append("; Best-effort decode with labels, raw-byte comments, and ASCII data annotations.")
    lines.append("; Symbol names below are inferred from usage and should be treated as working hypotheses.")
    lines.append("; Handler aliases are intentionally tentative; they describe observed roles, not confirmed source names.")
    lines.append("")
    lines.append("; Inferred workspace / state variables")
    for name, value, comment in equates:
        lines.append(f"{name:<16} EQU     ${value:04X}    ; {comment}")
    lines.append("")

    ascii_runs = set()
    for start, end in ranges:
        for run in find_ascii_runs(mem, start, end):
            ascii_runs.add(run)

    for start, end in ranges:
        if start in org_comments:
            for comment in org_comments[start]:
                lines.append(f"; {comment}")
        lines.append(f"        ORG     ${start:04X}")
        addr = start
        while addr <= end:
            if addr in labels or addr in alias_labels:
                if addr in label_comments:
                    lines.append("")
                    for comment in label_comments[addr]:
                        lines.append(f"; {comment}")
                if addr in alias_labels:
                    for alias in alias_labels[addr]:
                        lines.append(f"{alias}:")
                if addr in labels:
                    lines.append(f"{label_name(addr, labels)}:")

            op, raw = decode(mem, addr) if addr in instr_starts else (None, [mem[addr]])
            if addr in instr_starts and op is not None:
                operand = fmt_operand(mem, addr, op, raw, labels)
                raw_hex = " ".join(f"{b:02X}" for b in raw)
                text = f"        {op.mnemonic:<7}"
                if operand:
                    text += f" {operand:<12}"
                lines.append(f"{text:<32} ; {addr:04X}: {raw_hex}")
                addr += op.size
                continue

            ascii_run = next((run for run in ascii_runs if run[0] == addr), None)
            if ascii_run is not None:
                s, e = ascii_run
                text = "".join(chr(mem[a]) for a in range(s, e + 1))
                escaped = text.replace('\"', '\\\"')
                raw_hex = " ".join(f"{mem[a]:02X}" for a in range(s, e + 1))
                lines.append(f'        FCC     "{escaped}"')
                lines.append(f"        FCB     ${0x04:02X}            ; {s:04X}: {raw_hex} 0x04 terminator")
                addr = e + 1
                if addr <= end and mem[addr] == 0x04:
                    addr += 1
                continue

            # Group plain data bytes.
            chunk = [mem[addr]]
            next_addr = addr + 1
            while next_addr <= end and next_addr not in instr_starts and all(run[0] != next_addr for run in ascii_runs) and len(chunk) < 8:
                if next_addr in labels:
                    break
                chunk.append(mem[next_addr])
                next_addr += 1
            bytes_text = ",".join(f"${b:02X}" for b in chunk)
            lines.append(f"        FCB     {bytes_text}{data_comment(mem, addr, next_addr - 1)}")
            addr = next_addr
        lines.append("")

    path_out.write_text("\n".join(lines) + "\n")


if __name__ == "__main__":
    emit(Path("fort"), Path("fort.asm"))
