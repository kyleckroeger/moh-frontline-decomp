#!/usr/bin/env python3
"""Print each class's virtual table from the original image, slot by slot.

The original keeps its relocations, so every slot of every __vt__ symbol
names its target exactly. Canonical class headers must declare virtual
functions in this order (docs/PortReadiness.md, step 3).

  vtables.py                 every class, one line per slot
  vtables.py CPlayerObject   one class
  vtables.py --json          {class: [slot names]} for tools
"""
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF = ROOT / "orig/GMFE69/Moh2RelGC.elf"


def load():
    d = ELF.read_bytes()
    shoff = struct.unpack_from(">I", d, 0x20)[0]
    shnum, shstr = struct.unpack_from(">HH", d, 0x30)
    sections = [struct.unpack_from(">10I", d, shoff + 40 * i) for i in range(shnum)]

    def cstr(at):
        return d[at:d.index(b"\0", at)].decode("latin1")

    symtab = next(i for i, s in enumerate(sections) if s[1] == 2)
    strtab = sections[sections[symtab][6]]
    syms = []
    s = sections[symtab]
    for at in range(s[4], s[4] + s[5], 16):
        name_at, value, size, info, _, shndx = struct.unpack_from(">IIIBBH", d, at)
        syms.append((cstr(strtab[4] + name_at), value, size, shndx))
    targets = {}  # address of a word -> symbol it relocates to
    for sec in sections:
        if sec[1] != 4:  # SHT_RELA
            continue
        base = sections[sec[7]][3]
        for at in range(sec[4], sec[4] + sec[5], 12):
            where, info, addend = struct.unpack_from(">IIi", d, at)
            name, value = syms[info >> 8][0], syms[info >> 8][1]
            if (info & 0xFF) == 1:  # R_PPC_ADDR32
                targets[base + where if where < base else where] = name if not addend else f"{name}+{addend:#x}"
    tables = {}
    for name, value, size, shndx in syms:
        if name.startswith("__vt__") and size:
            cls = name[6:].lstrip("0123456789") if name[6:7].isdigit() else name[6:]
            slots = [targets.get(value + off, "0") for off in range(0, size, 4)]
            tables.setdefault(cls, slots)
    return tables


def main():
    tables = load()
    if "--json" in sys.argv:
        print(json.dumps(tables, indent=1))
        return
    want = [a for a in sys.argv[1:] if not a.startswith("-")]
    for cls in sorted(tables):
        if want and cls not in want:
            continue
        print(f"{cls}: {len(tables[cls])} words")
        for i, slot in enumerate(tables[cls]):
            print(f"  +{i * 4:#05x} {slot}")


if __name__ == "__main__":
    main()
