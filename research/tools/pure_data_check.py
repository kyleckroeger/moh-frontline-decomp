#!/usr/bin/env python3
"""pure_data_check.py [ELF]: compare the pure-source image's initialised data with the original's.

Every named data object in .data, .sdata, .rodata and .sdata2 of the original is looked up in the
pure image by name (file statics by name and size), and its bytes are compared with the words the
original relocates masked out (pointers differ with the layout). A difference means the object the
pure image uses comes from a view whose initial values aren't the original's: a unit fragment's
compiled copy (in the verified build its data is stripped, so nothing checked it), a draft's, or a
data file's. Prints one line per differing object, most bytes first.

    pure_data_check.py                       build/pure/Moh2RelGC_pure.elf
    pure_data_check.py build/pure/pure_l12.elf
"""
import collections, struct, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT / "tools"), str(ROOT / "research/tools")]
from formats import Elf32  # noqa: E402
from check_data import Image, ORIGINAL  # noqa: E402

SECTIONS = {".data", ".sdata", ".rodata", ".sdata2"}


def main():
    pure = Image(sys.argv[1] if len(sys.argv) > 1 else ROOT / "build/pure/Moh2RelGC_pure.elf")
    orig = Image(ORIGINAL)
    found = collections.defaultdict(list)
    for s in pure.symbols:
        if s["type"] == 1 and s["name"] and 0 < s["section"] < 0xFF00:
            base = s["name"].split(".")[0] if not s["name"].startswith("@") else s["name"]
            found[base].append(s)
    results, missing = [], 0
    for o in orig.symbols:
        if o["type"] != 1 or not o["name"] or o["name"].startswith(("@", "...")) or "$" in o["name"]:
            continue
        if not (0 < o["section"] < 0xFF00) or orig.section_name(o["section"]) not in SECTIONS or not o["size"]:
            continue
        cands = [c for c in found.get(o["name"], []) if c["size"] == o["size"]]
        if not cands:
            missing += 1
            continue
        ob = bytearray(orig.read(o["section"], o["address"], o["size"]))
        for a in orig.relocs[o["section"]]:
            if o["address"] <= a < o["address"] + o["size"]:
                ob[a - o["address"]:a - o["address"] + 4] = bytes(4)
        best = None
        for c in cands:
            pb = bytearray(pure.read(c["section"], c["address"], c["size"]))
            for a in orig.relocs[o["section"]]:
                if o["address"] <= a < o["address"] + o["size"]:
                    pb[a - o["address"]:a - o["address"] + 4] = bytes(4)
            diff = sum(1 for x, y in zip(ob, pb) if x != y)
            best = diff if best is None else min(best, diff)
        if best:
            results.append((best, o["name"], o["size"], orig.section_name(o["section"])))
    results.sort(reverse=True)
    print(f"{len(results)} objects differ, {missing} original objects not found by name and size")
    for diff, name, size, sec in results:
        print(f"{diff:6} of {size:6} bytes  {sec:8} {name}")


if __name__ == "__main__":
    main()
