#!/usr/bin/env python3
"""reltargets.py OBJ LABEL...: each label's section, size and relocations (offset in object, type,
target symbol + addend; a section-symbol target is shown as the object it lands in)."""
import struct, sys
sys.path.insert(0, "/home/kyle/workspace/moh-frontline/tools")
from formats import Elf32

obj = Elf32(open(sys.argv[1], "rb").read())
syms = list(obj.symbols())


def at(section, addr):
    for s in syms:
        if s["section"] == section and s["type"] in (1, 2) and s["address"] <= addr < s["address"] + max(s["size"], 1):
            return f"{s['name']}+{addr - s['address']:#x}"
    return f"{obj.sections[section]['name']}+{addr:#x}"


for label in sys.argv[2:]:
    s = next(s for s in syms if s["name"] == label and s["section"])
    sec = s["section"]
    out = []
    for rs in obj.sections:
        if rs["type"] != 4 or rs["info"] != sec:
            continue
        data = obj.contents(rs)
        for o in range(0, rs["size"], 12):
            loc, info, add = struct.unpack_from(">IIi", data, o)
            if s["address"] <= loc < s["address"] + s["size"]:
                t = syms[info >> 8]
                name = t["name"] if t["name"] and t["type"] != 3 else None
                tgt = f"{name}+{add:#x}" if name else at(t["section"], t["address"] + add)
                out.append(f"+{loc - s['address']:#x}:{info & 255}:{tgt}")
    print(label, obj.sections[sec]["name"], s["size"], " ".join(out))
