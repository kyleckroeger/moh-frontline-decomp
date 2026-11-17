#!/usr/bin/env python3
"""entry_trace.py ELF OUT [--units-from PATTERN]: log the first entry of every game function.

For an image linked with `pure_link.py --probe-space N`. Each traced function's first
instruction becomes a branch to a stub in the reserved space. The first time the stub runs,
it reads the embedded frame buffer at 0xC8A00000 + 4*k (the Z+Color range, which Dolphin logs) for the function's index k. Dolphin logs
that read ("... EFB read @ 0x084.....") at any point of the run, even before OSInit. Reading
changes nothing. An earlier version wrote to 0xCC01xxxx, which Dolphin decodes onto real hardware
registers, and that changed the run. The stub then executes the moved instruction and branches back.
The stub uses r11, r12 and cr0, which are free at a function's entry, plus one flag byte
per function.

Traced: the functions of game and EA units (src/game, src/ea) and drafts, except those
whose first instruction is a relative branch. Writes OUT and OUT.map (k -> name), then:

    entry_trace.py --decode OUT.map dolphin.log   print the functions in first-entry order
"""
import argparse, re, struct, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from formats import Elf32  # noqa: E402

STUB = 40  # bytes per stub: 10 instructions


def ha(x):
    return ((x >> 16) + (1 if x & 0x8000 else 0)) & 0xFFFF


def lo(x):
    return x & 0xFFFF


def branch(frm, to, link=False):
    d = (to - frm) & 0x3FFFFFC
    return 0x48000000 | d | (1 if link else 0)


def game_functions(elf):
    """Function symbols that come from game code: those defined in .text whose names a
    game/EA unit config or a draft lists."""
    import json, csv
    names = set()
    for cfg in (ROOT / "config/GMFE69").glob("*.json"):
        try:
            c = json.load(open(cfg))
        except Exception:
            continue
        if isinstance(c, dict) and str(c.get("source", "")).startswith(("src/game", "src/ea")):
            names |= {f["name"] for f in c.get("functions", [])}
    for row in csv.DictReader(open(ROOT / "research/drafts/index.tsv"), delimiter="\t"):
        names.add(row["function"])
    text = next(s for s in elf.sections if s["name"] == ".text")
    seen, out = set(), []
    for s in elf.symbols():
        if s["type"] == 2 and s["section"] == text["index"] and s["size"] >= 8 and s["name"] in names \
                and s["address"] not in seen:
            seen.add(s["address"])
            out.append(s)
    return sorted(out, key=lambda s: s["address"])


def build(src, dst):
    data = bytearray(Path(src).read_bytes())
    elf = Elf32(bytes(data))
    syms = {s["name"]: s for s in elf.symbols() if s["name"]}
    space = syms["_probe_space"]["address"]
    text = next(s for s in elf.sections if s["name"] == ".text")
    room = text["address"] + text["size"] - space
    funcs = game_functions(elf)
    capacity = (room - 0x4000) // STUB
    funcs = funcs[:capacity]
    flags = space + len(funcs) * STUB
    flags = (flags + 31) & ~31
    off = lambda a: text["offset"] + a - text["address"]
    names = []
    for k, f in enumerate(funcs):
        entry = f["address"]
        first = struct.unpack_from(">I", data, off(entry))[0]
        if first >> 26 in (16, 18):   # bc / b: relative, can't be moved
            continue
        stub = space + len(names) * STUB
        flag = flags + len(names)
        probe = 0xC8A00000 + 4 * len(names)
        code = [
            0x3D800000 | ha(flag),                  # lis r12, flag@ha
            0x896C0000 | lo(flag),                  # lbz r11, flag@l(r12)
            0x2C0B0000,                             # cmpwi r11, 0
            0x40820014,                             # bne +20 (to the moved instruction)
            0x39600001,                             # li r11, 1
            0x996C0000 | lo(flag),                  # stb r11, flag@l(r12)
            0x3D800000 | ha(probe),                 # lis r12, probe@ha
            0x818C0000 | lo(probe),                 # lwz r12, probe@l(r12)
            first,                                  # the moved instruction
        ]
        code.append(branch(stub + 4 * len(code), entry + 4))
        for i, w in enumerate(code):
            struct.pack_into(">I", data, off(stub + 4 * i), w)
        struct.pack_into(">I", data, off(entry), branch(entry, stub))
        names.append(f["name"])
    Path(dst).write_bytes(data)
    Path(str(dst) + ".map").write_text("".join(f"{k}\t{n}\n" for k, n in enumerate(names)))
    print(f"traced {len(names)} functions (room for {capacity})")


def decode(mapfile, log):
    names = dict(l.rstrip("\n").split("\t", 1) for l in open(mapfile))
    for line in open(log, errors="replace"):
        m = re.search(r"EFB read @ 0x(08[a-b][0-9a-f]{5})", line)
        if m:
            k = (int(m.group(1), 16) - 0x08A00000) // 4
            print(names.get(str(k), f"?{k}"))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("args", nargs="+")
    ap.add_argument("--decode", action="store_true")
    a = ap.parse_args()
    if a.decode:
        decode(*a.args)
    else:
        build(*a.args)


if __name__ == "__main__":
    main()
