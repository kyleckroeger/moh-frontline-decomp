#!/usr/bin/env python3
"""Print each class's virtual table from the original image, slot by slot.

The original keeps its relocations, so every slot of every __vt__ symbol
names its target exactly. Canonical class headers must declare virtual
functions in this order (docs/PortReadiness.md, steps 2.3 and 3.2).

MWCC's layout: each table is one or more parts. A part is the RTTI pointer,
the offset of that subobject in the object (0 for the primary part, minus the
base's offset for a secondary base's part), then one word per virtual. A slot
with no relocation is 0: a pure virtual (MWCC writes no __pure_virtual_called).
A secondary part's slots that the class overrides are thunks (@36@__dt__...).

  vtables.py                 every class, one line per word
  vtables.py CPlayerObject   one class
  vtables.py --json          {class: [slot names]} for tools (every word, as before)
  vtables.py --tsv           write research/layout/vtables.tsv (one row per word)
"""
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF = ROOT / "orig/GMFE69/Moh2RelGC.elf"
TSV = ROOT / "research/layout/vtables.tsv"
DTK = ROOT / "build/tools/dtk"


def class_key(vt_name):
    """__vt__10ISceneNode -> ISceneNode; __vt__Q24RCMP5CODEC -> Q24RCMP5CODEC (as before)."""
    rest = vt_name[6:]
    return rest.lstrip("0123456789") if rest[:1].isdigit() else rest


def load_words():
    """[(vt symbol, class key, address, bind, [(word value, target or None)])], in address order."""
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
        syms.append((cstr(strtab[4] + name_at), value, size, info >> 4, shndx))
    targets = {}  # address of a word -> symbol it relocates to
    for sec in sections:
        if sec[1] != 4:  # SHT_RELA
            continue
        base = sections[sec[7]][3]
        for at in range(sec[4], sec[4] + sec[5], 12):
            where, info, addend = struct.unpack_from(">IIi", d, at)
            name = syms[info >> 8][0]
            if (info & 0xFF) == 1:  # R_PPC_ADDR32
                targets[base + where if where < base else where] = name if not addend else f"{name}+{addend:#x}"

    def word(addr):
        for sec in sections:
            if sec[1] == 1 and sec[3] <= addr < sec[3] + sec[5]:  # SHT_PROGBITS
                return struct.unpack_from(">I", d, sec[4] + addr - sec[3])[0]
        return 0

    out = []
    for name, value, size, bind, shndx in sorted(syms, key=lambda s: s[1]):
        if name.startswith("__vt__") and size:
            words = [(word(value + off), targets.get(value + off)) for off in range(0, size, 4)]
            out.append((name, class_key(name), value, {1: "global", 2: "weak"}.get(bind, str(bind)), words))
    return out


def load():
    """{class: [slot names]} with every word, "0" for a word with no relocation (old format)."""
    tables = {}
    for _, cls, _, _, words in load_words():
        tables.setdefault(cls, [t or "0" for _, t in words])
    return tables


def rows():
    """One dict per word: vtable, class, address, bind, word, part, role, slot, value."""
    out = []
    for vt, cls, addr, bind, words in load_words():
        part, slot = -1, 0
        for i, (value, target) in enumerate(words):
            if target and target.startswith("__RTTI__"):
                part += 1
                role, slot_s, val = "rtti", "", target
            elif i > 0 and words[i - 1][1] and words[i - 1][1].startswith("__RTTI__"):
                role, slot_s, val = "offset", "", str(struct.unpack(">i", struct.pack(">I", value))[0])
                slot = 0
            else:
                role, slot_s, val = "slot", str(slot), target or ("0" if not value else f"{value:#x}")
                slot += 1
            out.append(dict(vtable=vt, cls=cls, address=f"{addr:#010x}", bind=bind, word=f"{i * 4:#x}",
                            part=str(max(part, 0)), role=role, slot=slot_s, value=val))
    return out


def demangle_all(names):
    out = {}
    for n in sorted(set(names)):
        sym = n.split("+")[0]
        if sym.startswith("@"):  # thunk: @offset@target
            sym = sym.split("@", 2)[2]
        r = subprocess.run([str(DTK), "demangle", sym], capture_output=True, text=True)
        out[n] = r.stdout.strip() if r.returncode == 0 and r.stdout.strip() else ""
        if n != sym and out[n]:
            out[n] = f"thunk {n.split('@')[1]}: {out[n]}"
    return out


def write_tsv():
    rs = rows()
    dm = demangle_all(r["value"] for r in rs if r["role"] == "slot" and r["value"] not in ("0",))
    TSV.parent.mkdir(parents=True, exist_ok=True)
    cols = ["vtable", "class", "address", "bind", "word", "part", "role", "slot", "value", "demangled"]
    with open(TSV, "w") as o:
        o.write("\t".join(cols) + "\n")
        for r in rs:
            o.write("\t".join([r["vtable"], r["cls"], r["address"], r["bind"], r["word"], r["part"], r["role"],
                               r["slot"], r["value"], dm.get(r["value"], "")]) + "\n")
    print(f"{TSV.relative_to(ROOT)}: {len({r['vtable'] for r in rs})} tables, {len(rs)} words, "
          f"{sum(r['role'] == 'slot' for r in rs)} slots")


def main():
    if "--tsv" in sys.argv:
        write_tsv()
        return
    if "--json" in sys.argv:
        print(json.dumps(load(), indent=1))
        return
    want = [a for a in sys.argv[1:] if not a.startswith("-")]
    seen = set()
    for vt, cls, addr, bind, words in load_words():
        if (want and cls not in want) or cls in seen:
            continue
        seen.add(cls)
        print(f"{cls}: {len(words)} words ({vt} {addr:#010x} {bind})")
        for r in rows_of(vt):
            extra = f" slot {r['slot']}" if r["role"] == "slot" else f" {r['role']}"
            print(f"  +{int(r['word'], 16):#05x} {r['value']}  [part {r['part']}{extra}]")


_ROWS = None


def rows_of(vt):
    global _ROWS
    if _ROWS is None:
        _ROWS = rows()
    return [r for r in _ROWS if r["vtable"] == vt]


if __name__ == "__main__":
    try:
        main()
    except BrokenPipeError:
        pass
