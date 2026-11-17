#!/usr/bin/env python3
"""pure_align_check.py: data objects whose original address was 32-byte aligned but whose
address in a pure-source image isn't (endgame W1).

DMA (ARAM, DSP, GX, DVD) ignores the low 5 bits of a main-memory address, so a buffer that the
original linker happened to, or was told to, align on 32 bytes and that the pure link places at
16 mod 32 receives its data 16 bytes early. That broke route 1_2 (g_frame_cache, 2026-11-17).
Not every hit is a DMA target; the list is for review.

    pure_align_check.py [ELF]   (default build/pure/Moh2RelGC_pure.elf)

Original addresses come from the unit configs (config/GMFE69/*.json); a name with more than one
address there (file statics) is skipped.
"""
import glob, json, re, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]


def original_addresses():
    found = {}

    def walk(o):
        if isinstance(o, dict):
            for k, v in o.items():
                if isinstance(v, str) and re.fullmatch(r"0x8[0-9a-f]{7}", v):
                    found.setdefault(k, set()).add(int(v, 16))
                else:
                    walk(v)
        elif isinstance(o, list):
            for x in o:
                walk(x)

    for f in glob.glob(str(ROOT / "config/GMFE69/*.json")):
        try:
            walk(json.load(open(f)))
        except ValueError:
            pass
    return {k: v.pop() for k, v in found.items() if len(v) == 1}


def main():
    elf = sys.argv[1] if len(sys.argv) > 1 else str(ROOT / "build/pure/Moh2RelGC_pure.elf")
    orig = original_addresses()
    rows = []
    out = subprocess.run(["nm", "-S", "-n", elf], capture_output=True, text=True, check=True).stdout
    for line in out.splitlines():
        p = line.split()
        if len(p) != 4 or p[2] not in "bBdDsSgGrR":
            continue
        addr, size, name = int(p[0], 16), int(p[1], 16), p[3]
        o = orig.get(name)
        if o is not None and o % 32 == 0 and addr % 32 and size >= 32:
            rows.append((size, name, o, addr))
    for size, name, o, addr in sorted(rows, reverse=True):
        print(f"{size:8d} {name:40s} original {o:08x} pure {addr:08x}")
    print(f"{len(rows)} objects lost 32-byte alignment")


if __name__ == "__main__":
    main()
