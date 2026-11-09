#!/usr/bin/env python3
"""List data symbols that no accepted unit owns, grouped by kind.

A PC build needs every data symbol defined in source. Symbols outside every
unit's section ranges in config/GMFE69/*.json still come from the original
image. Usage: unowned_data.py [--list KIND] (KIND as printed, e.g. "named").
"""
import bisect
import collections
import glob
import json
import os
import re
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
DATA = {".data", ".rodata", ".sdata", ".sdata2", ".bss", ".sbss", ".sbss2", ".ctors", ".dtors"}


def kind(section, name):
    if name.startswith("__vt__"):
        return "vtable"
    if name.startswith("__RTTI") or name.startswith("__TI"):
        return "rtti"
    if re.match(r"@\d+$", name) or name.startswith("@stringBase"):
        return "compiler-local"  # string and float literals, switch tables
    if section in (".bss", ".sbss", ".sbss2"):
        return "bss"
    return "named"


def main():
    d = open(os.path.join(ROOT, "orig/GMFE69/Moh2RelGC.elf"), "rb").read()
    shoff = struct.unpack_from(">I", d, 0x20)[0]
    shnum, shstr = struct.unpack_from(">HH", d, 0x30)
    sections = [struct.unpack_from(">10I", d, shoff + 40 * i) for i in range(shnum)]

    def cstr(at):
        return d[at:d.index(b"\0", at)].decode("latin1")

    names = {i: cstr(sections[shstr][4] + s[0]) for i, s in enumerate(sections)}
    owned = collections.defaultdict(list)
    for path in glob.glob(os.path.join(ROOT, "config/GMFE69/*.json")):
        unit = json.load(open(path))
        if not isinstance(unit.get("sections"), list):
            continue
        for s in unit["sections"]:
            if s["name"] in DATA:
                start = int(s["address"], 16)
                owned[s["name"]].append((start, start + s["size"]))
    for ranges in owned.values():
        ranges.sort()

    def is_owned(section, address):
        ranges = owned.get(section, [])
        i = bisect.bisect_right(ranges, (address, 1 << 40)) - 1
        return i >= 0 and ranges[i][0] <= address < ranges[i][1]

    total, left = collections.Counter(), collections.defaultdict(list)
    for s in sections:
        if s[1] != 2:
            continue
        strtab = sections[s[6]]
        for at in range(s[4], s[4] + s[5], 16):
            name_at, value, size, _, _, shndx = struct.unpack_from(">IIIBBH", d, at)
            section = names.get(shndx)
            if not shndx or shndx >= 0xFF00 or section not in DATA:
                continue
            name = cstr(strtab[4] + name_at)
            total[section] += size
            if not is_owned(section, value):
                left[(section, kind(section, name))].append((size, name, value))

    if len(sys.argv) > 2 and sys.argv[1] == "--list":
        for (section, k), rows in sorted(left.items()):
            if k == sys.argv[2]:
                for size, name, value in sorted(rows, reverse=True):
                    print(f"{section}\t{value:#x}\t{size}\t{name}")
        return
    for section in sorted(total):
        rows = {k: v for (s, k), v in left.items() if s == section}
        unowned = sum(z for v in rows.values() for z, _, _ in v)
        print(f"{section}: {total[section]:,} B in symbols, {unowned:,} B unowned")
        for k, v in sorted(rows.items(), key=lambda kv: -sum(z for z, _, _ in kv[1])):
            print(f"  {k}: {sum(z for z, _, _ in v):,} B in {len(v)} symbols")


if __name__ == "__main__":
    main()
