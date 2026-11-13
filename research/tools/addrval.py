#!/usr/bin/env python3
"""addrval.py [NAME ...]: drafts that take the address of a global the original only reads or
writes as a value (or the reverse): `extern char X[]` written for a pointer variable.

For each draft (compiled as link_drafts.py does), every relocation's instruction is classed
as an address computation (addi, or a SDA21 addi) or a memory access (load or store at the
symbol). The original function's relocations (.rela.text) are classed the same way. A
4-byte-or-smaller global that the original only accesses as memory but the draft only takes
the address of, or the reverse, is reported (2026-11-13: SPR_BEGIN in __sinit_compartment_cpp).
"""
import csv, os, struct, sys, tempfile
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT / "research/tools"), str(ROOT / "tools")]
import fast, link_drafts
from formats import Elf32

ADDR16_LO, SDA21 = 4, 109


def kind(word):
    op = word >> 26
    if op == 14:
        return "addr"
    if 32 <= op <= 55:
        return "mem"
    return None


def classify(data_of, relocs, end):
    """An address computed into a register that is then used as a load or store base
    (before the register is redefined) counts as a memory access."""
    out = {}
    for where, typ, name in relocs:
        if typ not in (ADDR16_LO, SDA21) or not name:
            continue
        w = data_of(where)
        k = kind(w)
        if k == "addr":
            rd = (w >> 21) & 31
            for x in range(where + 4, end, 4):
                v = data_of(x)
                op, rt, ra = v >> 26, (v >> 21) & 31, (v >> 16) & 31
                if 32 <= op <= 55 and ra == rd:
                    k = "mem"
                    break
                if rt == rd and op not in (4, 10, 11, 16, 18, 19, 59, 63) and not 36 <= op <= 39 \
                        and not 44 <= op <= 55 and not (op == 31 and ((v >> 1) & 1023) in (0, 32, 151, 215, 150, 662)):
                    break  # redefined (stores don't define rt)
                if v >> 26 == 18 and v & 1 and (rd == 0 or 3 <= rd <= 12):
                    break  # a call clobbers a volatile register
        if k:
            out.setdefault(name, set()).add(k)
    return out


def main():
    os.chdir(ROOT)
    orig_bytes = Path(fast.ELF).read_bytes()
    oe = Elf32(orig_bytes)
    import dolphin_boot
    segs = dolphin_boot.segments(orig_bytes)
    syms = list(oe.symbols())
    sizes = {}
    for s in syms:
        if s["name"]:
            sizes.setdefault(s["name"], s["size"])
    orelocs = []
    for sec in oe.sections:
        if sec["type"] == 4 and sec["name"].startswith((".rela.text", ".rela.init")):
            data = oe.contents(sec)
            for off in range(0, len(data), 12):
                where, info, addend = struct.unpack_from(">IIi", data, off)
                orelocs.append((where - (where & 3), info & 255, syms[info >> 8]["name"]))
    orelocs.sort()

    def oword(a):
        fo = dolphin_boot.file_offset(segs, a)
        return struct.unpack_from(">I", orig_bytes, fo)[0]
    rows = list(csv.DictReader(open("research/drafts/index.tsv"), delimiter="\t"))
    names = sys.argv[1:]
    groups = {}
    for r in rows:
        if names and not any(k in r["function"] for k in names):
            continue
        groups.setdefault((r["draft"], r["compile_settings_from"]), []).append(r)
    for (draft, settings), group in sorted(groups.items()):
        with tempfile.TemporaryDirectory() as work:
            obj, log = link_drafts.compile_draft(draft, settings, work)
            if not obj:
                continue
            u = link_drafts.Unit(obj, 0x80400000)
        for r in group:
            name = "__sinit_probe_cpp" if r["function"].startswith("__sinit_") else r["function"]
            if name not in u.funcs:
                continue
            sec, start, size = u.funcs[name]
            base = u.place[sec] - u.base
            mine = [(w, k, s["name"]) for rs, w, k, s, a in u.relocs if rs == sec and start <= w < start + size]
            ours = classify(lambda w: struct.unpack_from(">I", u.blob, base + (w & ~3))[0], mine, start + size)
            a, n = int(r["address"], 16), int(r["size"])
            theirs = classify(oword, [x for x in orelocs if a <= x[0] < a + n], a + n)
            for sym in sorted(set(ours) & set(theirs)):
                if sizes.get(sym, 99) <= 4 and ours[sym] != theirs[sym] and len(ours[sym]) == len(theirs[sym]) == 1:
                    print(f"{r['function'][:60]}\t{sym}\toriginal {sorted(theirs[sym])}\tdraft {sorted(ours[sym])}\t{draft}")


if __name__ == "__main__":
    main()
