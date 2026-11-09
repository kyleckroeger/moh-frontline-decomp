#!/usr/bin/env python3
"""Disassemble Medal of Honor: Frontline (GameCube) behaviour scripts.

Usage:
  bs_disasm.py Moh2RelGC.elf LEVEL.VIV CLASS       print one class
  bs_disasm.py Moh2RelGC.elf --opcodes             print the opcode table
  bs_disasm.py Moh2RelGC.elf --verify LEVEL.VIV... check every class

Everything comes from the GameCube game:

- Opcode numbers are read from the PowerPC code of BSInitMachine in the ELF.
- Operand meanings follow the BSOpCodeFunc_* functions in the moh-frontline decomp.
- Built-in function names come from the BIFunc_ symbols in the ELF, matched by
  the CRC-32 of the upper-case name stored in bsbifunc.dat.

Script files are little-endian. Output is derived from game data: do not commit it.
"""

import struct
import sys
import zlib

SHT_SYMTAB = 2
STATE_RECORD = 20
TEXT_START = 0xC8


class Elf:
    def __init__(self, path):
        with open(path, "rb") as f:
            self.d = f.read()
        shoff = struct.unpack_from(">I", self.d, 0x20)[0]
        shnum = struct.unpack_from(">H", self.d, 0x30)[0]
        self.sections = [struct.unpack_from(">10I", self.d, shoff + 40 * i) for i in range(shnum)]
        self.symbols = {}
        for s in self.sections:
            if s[1] != SHT_SYMTAB:
                continue
            strtab = self.sections[s[6]]
            for at in range(s[4], s[4] + s[5], 16):
                name_at, value, size, _, _, _ = struct.unpack_from(">IIIBBH", self.d, at)
                start = strtab[4] + name_at
                name = self.d[start:self.d.index(b"\0", start)].decode("latin1")
                self.symbols[name] = (value, size)

    def code(self, name):
        value, size = self.symbols[name]
        for s in self.sections:
            if s[1] == 1 and s[3] <= value < s[3] + s[5]:
                return struct.unpack_from(">%dI" % (size // 4), self.d, s[4] + value - s[3])
        raise ValueError("No section holds " + name)


def opcode_table(elf):
    """Recover {opcode: name} from the stores in BSInitMachine's PowerPC code."""
    table, table_size = elf.symbols["g_bocfOpCodeFuncs"]
    by_address = {v[0]: n for n, v in elf.symbols.items() if n.startswith("BSOpCodeFunc_")}
    regs, result = {}, {}
    for word in elf.code("BSInitMachine__Fv"):
        op, rd, ra = word >> 26, (word >> 21) & 31, (word >> 16) & 31
        simm = struct.unpack(">h", struct.pack(">H", word & 0xFFFF))[0]
        if op == 15:  # addis
            regs[rd] = (0 if ra == 0 else regs.get(ra, 0)) + (simm << 16)
        elif op == 14:  # addi
            regs[rd] = (0 if ra == 0 else regs.get(ra, 0)) + simm
        elif op == 36:  # stw
            address = (regs.get(ra, 0) + simm) & 0xFFFFFFFF
            if table <= address < table + table_size:
                target = regs.get(rd, 0) & 0xFFFFFFFF
                name = by_address[target]
                result[(address - table) // 4] = name[len("BSOpCodeFunc_"):].split("__")[0]
    return result


def builtin_table(elf, dat):
    """Return {number: (name, net stack pops)} from bsbifunc.dat and the ELF symbols."""
    names = {}
    for symbol in elf.symbols:
        if symbol.startswith("BIFunc_"):
            name = symbol[len("BIFunc_"):].split("__")[0]
            names[zlib.crc32(name.upper().encode())] = name
    count = struct.unpack_from("<I", dat, 0x0C)[0]
    table = {}
    for i in range(count):
        _, crc, number, pops = struct.unpack_from("<IIHh", dat, 0x14 + 16 * i)
        table[number] = (names.get(crc, "builtin_%x" % number), pops)
    return table


def read_viv(path):
    with open(path, "rb") as f:
        d = f.read()
    if d[:2] != b"\xc0\xfb":
        raise ValueError("Expected a C0FB archive: " + path)
    count = struct.unpack_from(">H", d, 4)[0]
    at, files = 6, {}
    for _ in range(count):
        offset = int.from_bytes(d[at:at + 3], "big")
        size = int.from_bytes(d[at + 3:at + 6], "big")
        end = d.index(b"\0", at + 6)
        files[d[at + 6:end].decode("latin1").lower()] = d[offset:offset + size]
        at = end + 1
    return files


def parse_sin(d):
    """Return (levels, end offset). Fields follow the state view in bsmachin.cpp."""
    u32 = lambda o: struct.unpack_from("<I", d, o)[0]
    level_count = struct.unpack_from("<H", d, 0xBC)[0]
    text_count, table = u32(0xC0), u32(0xC4)
    at = TEXT_START + 4 * text_count
    for i in range(text_count):
        start = u32(table + 4 * i)
        length = d.index(b"\0", start) - start + 1
        at += (length + 3) & ~3
    base, levels = at, []
    at += 28 * level_count
    for k in range(level_count):
        _, code_words, _, state_count, _, _, variables, depth = struct.unpack_from(
            "<IIIHHIIB", d, base + 28 * k)
        at += 4 * (depth + 1)
        pointers = struct.unpack_from("<%dI" % state_count, d, at)
        at += 4 * state_count
        states = []
        for p in pointers:
            if p >= 0xFFFFFFFE:
                states.append(None)
                continue
            code, _, _, parent, state_depth, messages, events = struct.unpack_from("<IIIHBBB", d, at)
            states.append(dict(code=code, parent=parent, depth=state_depth,
                               nmsg=messages, nev=events))
            at += STATE_RECORD
        for s in filter(None, states):
            s["events"] = [struct.unpack_from("<IHH", d, at + 8 * i) for i in range(s["nev"])]
            at += 8 * s["nev"]
        for s in filter(None, states):
            s["messages"] = [struct.unpack_from("<III", d, at + 12 * i) for i in range(s["nmsg"])]
            at += 12 * s["nmsg"]
        levels.append(dict(code_words=code_words, variables=variables, states=states))
    return levels, at


def parse_cbs(d):
    count = struct.unpack_from("<I", d, 0x64)[0]
    at, functions = 0x68, []
    for _ in range(count):
        n = struct.unpack_from("<I", d, at)[0]
        functions.append(struct.unpack_from("<%dI" % n, d, at + 4))
        at += 4 + 4 * n
    return functions


def signed16(word):
    return struct.unpack(">h", struct.pack(">H", word >> 16))[0]


def decode(words, opcodes, builtins):
    """Yield (position, length, text) for each instruction."""
    i = 0
    while i < len(words):
        word = words[i]
        name = opcodes.get(word & 0xFF)
        if name is None:
            raise ValueError("Unknown opcode %#x at %d" % (word & 0xFF, i))
        arg = signed16(word)
        length, text = 1, name
        if name == "PUSH":
            length, value = 2, words[i + 1]
            as_float = struct.unpack("<f", struct.pack("<I", value))[0]
            text = "PUSH %d" % struct.unpack("<i", struct.pack("<I", value))[0]
            if value >> 23 not in (0, 0x1FF) and abs(as_float) < 1e7:
                text += " (float %g)" % as_float
        elif name == "FNCALL":
            length = 2
            text = "FNCALL %#x args=%d" % (words[i + 1], (word >> 16) & 0xFF)
        elif name == "BIFNCALL":
            builtin, pops = builtins.get(arg & 0xFFFF, ("builtin_%x" % (arg & 0xFFFF), 0))
            text = "CALL %s (net pops %d)" % (builtin, pops)
        elif name in ("JMP", "BNE", "BREAK"):
            text = "%s -> %d" % (name, i + arg)
        elif name in ("LW", "SW"):
            text = "%s frame[%d]" % (name, arg)
        elif name in ("LG", "SG"):
            text = "%s var[%d]" % (name, arg)
        elif name in ("LM", "SM"):
            text = "%s field[%d]" % (name, arg)
        elif name in ("LS", "POP", "CAST"):
            text = "%s %d" % (name, arg)
        elif name == "GP":
            text = "GP field=%d trigger=%d" % ((word >> 16) & 0xFF, word >> 24)
        elif name in ("ADD", "SUB", "MULT", "DIV", "NEG", "EQ", "NE", "LT", "GT", "LTE", "GTE"):
            text = "%s types=%04x" % (name, word >> 16)
        yield i, length, text
        i += length


def print_class(elf, viv, cls):
    files = read_viv(viv)
    opcodes = opcode_table(elf)
    builtins = builtin_table(elf, files["bsbifunc.dat"])
    levels, _ = parse_sin(files[cls.lower() + ".sin"])
    functions = parse_cbs(files[cls.lower() + ".cbs"])
    own_depth = len(class_chain(cls.lower(), files)) - 1
    for machine, words in enumerate(functions):
        labels = {}
        states = levels[machine]["states"] if machine < len(levels) else []
        for number, state in enumerate(states):
            if not state:
                continue
            labels.setdefault(state["code"], []).append("state %d: enter (parent %s, depth %d)" % (
                number, "-" if state["parent"] == 0xFFFF else state["parent"], state["depth"]))
            for code, event, flags in state["events"]:
                labels.setdefault(code, []).append("state %d: event %d (flags %#x)" % (number, event, flags))
            for code, kind, message in state["messages"]:
                labels.setdefault(code, []).append("state %d: message %d (kind %#x)" % (
                    number, message & 0xFFFF, kind >> 16))
        print("machine %d (%d words, class depth %d)" % (machine, len(words), own_depth))
        for position, _, text in decode(words, opcodes, builtins):
            for label in labels.get((own_depth << 24) | position, []):
                print("  # " + label)
            print("  %5d  %s" % (position, text))


def parent_class(sin, files):
    """Return the lower-case parent class named at 0x24, if the archive has it."""
    end = sin.index(b"\0", 0x24)
    name = sin[0x24:end].decode("latin1").lower()
    return name if name + ".cbs" in files else None


def class_chain(name, files):
    """Return class names from the root class down to name."""
    chain = [name]
    while True:
        parent = parent_class(files[chain[0] + ".sin"], files)
        if parent is None or parent in chain:
            return chain
        chain.insert(0, parent)


def verify(elf, vivs):
    opcodes = opcode_table(elf)
    classes = handlers = unresolved = 0
    for viv in vivs:
        files = read_viv(viv)
        builtins = builtin_table(elf, files["bsbifunc.dat"])
        starts = {}
        for name in sorted(n[:-4] for n in files if n.endswith(".cbs")):
            starts[name] = [{p for p, _, _ in decode(words, opcodes, builtins)}
                            for words in parse_cbs(files[name + ".cbs"])]
        for name in starts:
            levels, end = parse_sin(files[name + ".sin"])
            tail = files[name + ".sin"][end:]
            if tail and b"PADDING" not in tail:
                raise ValueError("%s: %d unread .sin bytes" % (name, len(tail)))
            chain = class_chain(name, files)
            # A code position is a class depth in the chain plus a word offset
            # in that class's code for the same machine (level).
            for machine, level in enumerate(levels):
                for state in filter(None, level["states"]):
                    codes = [state["code"]] + [e[0] for e in state["events"]] + [m[0] for m in state["messages"]]
                    for code in codes:
                        if code == 0xFFFFFFFF:
                            continue
                        depth = code >> 24
                        if depth >= len(chain) or machine >= len(starts[chain[depth]]):
                            unresolved += 1
                            continue
                        handlers += 1
                        if (code & 0xFFFFFF) not in starts[chain[depth]][machine]:
                            raise ValueError("%s: handler %#x is not an instruction start in %s machine %d" % (
                                name, code, chain[depth], machine))
            classes += 1
    print("OK: %d archives, %d classes, %d handler positions, %d unresolved, opcodes %d" % (
        len(vivs), classes, handlers, unresolved, len(opcodes)))


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    elf = Elf(sys.argv[1])
    if sys.argv[2] == "--opcodes":
        for number, name in sorted(opcode_table(elf).items()):
            print(number, name)
    elif sys.argv[2] == "--verify":
        verify(elf, sys.argv[3:])
    else:
        print_class(elf, sys.argv[2], sys.argv[3])


if __name__ == "__main__":
    main()
