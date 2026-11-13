#!/usr/bin/env python3
"""fprclob.py: find drafts that keep a value in f14-f19 across a call to CMatrix::Multiply.

CMatrix::Multiply (0x8008fc50) calls the hand-written paired-single PSMultMatrix
(0x8008e9ec), which uses f0-f19 and saves none of them. f14-f19 are callee-saved in the
EABI, so compiled code may keep values there across a call; the original callers happen
not to across Multiply, but a draft's register allocation may (RenderSystem, 2026-11-13).

Every original function that calls Multiply, directly or through other calls, without
saving f14-f19 itself passes the clobber on (118 functions); calls to any of them count.

  fprclob.py [NAME ...]       drafts in research/drafts/index.tsv (default: all) whose
                              compiled code (link_drafts.py's flags) has f14-f19 live
                              after a call to one of those functions ("CLOBBERED"), or
                              after a virtual call ("virtual": a risk only if the callee
                              can reach Multiply; the originals do this too)
  fprclob.py --original ADDR SIZE   the same check on an original function

Liveness is a backward data-flow over the function's control-flow graph built from
objdump's disassembly; calls define f0-f13 (and f14-f19 for the clobbering callees).
"""
import csv, os, re, subprocess, sys, tempfile
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT / "research/tools"), str(ROOT / "tools")]
import fast
import link_drafts

CLOBBER = set()  # filled by clobbering_functions()
BAD = set(range(14, 20))
FREG = re.compile(r"\bf(\d+)\b")


def clobbering_functions():
    """Original functions that can return with f14-f19 changed: PSMultMatrix, and every
    function that calls one of them directly without saving those registers itself."""
    import struct
    from formats import Elf32
    import dolphin_boot
    d = Path(fast.ELF).read_bytes()
    elf, segs = Elf32(d), dolphin_boot.segments(d)
    syms = {s["name"]: s for s in elf.symbols()}
    lo = syms["__save_fpr"]["address"]
    funcs = {s["address"]: (s["name"], s["size"]) for s in elf.symbols() if s["type"] == 2 and s["size"]}
    info = {}
    for a, (name, size) in funcs.items():
        fo = dolphin_boot.file_offset(segs, a)
        if fo is None:
            continue
        saved, calls = set(), set()
        for x in range(0, size, 4):
            i = struct.unpack_from(">I", d, fo + x)[0]
            op, rd, ra = i >> 26, (i >> 21) & 31, (i >> 16) & 31
            if op == 18 and i & 1:
                li = i & 0x03fffffc
                t = (a + x + li - (0x04000000 if li & 0x02000000 else 0)) & 0xffffffff
                if lo <= t < lo + 0x48:
                    saved |= set(range(14 + (t - lo) // 4, 32))
                elif t in funcs:
                    calls.add(t)
            if op in (54, 55, 60, 61) and ra == 1:  # stfd, stfdu, psq_st, psq_stu off r1
                saved.add(rd)
        info[a] = (name, saved, calls)
    root = syms["PSMultMatrix__FPfPCfPCf"]["address"]
    clob = {a: set() for a in info}
    clob[root] = set(BAD)
    changed = True
    while changed:
        changed = False
        for a, (name, saved, calls) in info.items():
            if a == root:
                continue
            c = set().union(*(clob.get(t, set()) for t in calls)) - saved if calls else set()
            if c - clob[a]:
                clob[a] |= c
                changed = True
    return {info[a][0] for a, c in clob.items() if c}


def parse(obj):
    """{func: [(addr, mnemonic, operands, call_target)]} from objdump -dr."""
    out = subprocess.run([fast.OBJDUMP, "-dr", obj], capture_output=True, text=True).stdout
    funcs, cur = {}, None
    for line in out.splitlines():
        m = re.match(r"^[0-9a-f]+ <(.+)>:$", line)
        if m:
            cur = funcs.setdefault(m[1], [])
            continue
        m = re.match(r"^\s*([0-9a-f]+):\s+(?:[0-9a-f]{2} ){4}\s*(\S+)\s*([^<]*)(?:<([^>+]+)>)?", line)
        if m and cur is not None:
            cur.append([int(m[1], 16), m[2], m[3].strip(), m[4] if m[2] == "bl" else None])
            continue
        m = re.match(r"^\s+[0-9a-f]+: R_PPC_REL24\s+(\S+)", line)
        if m and cur:
            cur[-1][3] = m[1]
    return funcs


def defs_uses(mn, ops):
    regs = [int(r) for r in FREG.findall(ops)]
    if not regs:
        return set(), set()
    if mn.startswith(("stf", "psq_st")) or mn.startswith("fcmp") or mn == "mtfsf":
        return set(), set(regs)
    if mn.startswith(("lf", "psq_l")):
        return {regs[0]}, set()
    return {regs[0]}, set(regs[1:])


def check(insns):
    n = len(insns)
    index = {a: i for i, (a, *_ ) in enumerate(insns)}
    succ = []
    for i, (a, mn, ops, call) in enumerate(insns):
        s = []
        base = mn.rstrip("+-")
        target = re.search(r"([0-9a-f]+)$", ops)
        if base in ("blr", "bctr") or mn.startswith("blr"):
            s = [] if base in ("blr", "bctr") else [i + 1]
        elif base == "b":
            s = [index[int(target[1], 16)]] if target and int(target[1], 16) in index else []
        elif base.startswith("b") and not base.startswith("bl") and target and int(target[1], 16) in index:
            s = [index[int(target[1], 16)], i + 1]
        else:
            s = [i + 1]
        succ.append([j for j in s if j < n])
    live_in = [set() for _ in range(n)]
    changed = True
    while changed:
        changed = False
        for i in range(n - 1, -1, -1):
            a, mn, ops, call = insns[i]
            out = set().union(*(live_in[j] for j in succ[i])) if succ[i] else set()
            if mn in ("bl", "bctrl") or call:
                d, u = set(range(0, 14)) | (BAD if call in CLOBBER else set()), set(range(1, 9))
            else:
                d, u = defs_uses(mn, ops)
            new = (out - d) | u
            if new != live_in[i]:
                live_in[i] = new
                changed = True
    hits = []
    for i, (a, mn, ops, call) in enumerate(insns):
        if call in CLOBBER or mn == "bctrl":
            out = set().union(*(live_in[j] for j in succ[i])) if succ[i] else set()
            if out & BAD:
                hits.append((a, call or "bctrl", sorted(out & BAD)))
    return hits


def original(name, start, size):
    out = subprocess.run([fast.OBJDUMP, "-d", f"--start-address={start:#x}", f"--stop-address={start + size:#x}",
                          fast.ELF], capture_output=True, text=True).stdout
    tmp = Path(tempfile.mkdtemp()) / "o.txt"
    insns = []
    for line in out.splitlines():
        m = re.match(r"^\s*([0-9a-f]+):\s+(?:[0-9a-f]{2} ){4}\s*(\S+)\s*([^<]*)(?:<([^>+]+)>)?", line)
        if m:
            insns.append([int(m[1], 16), m[2], m[3].strip(), m[4] if m[2] == "bl" else None])
    return check(insns)


def main():
    CLOBBER.update(clobbering_functions())
    if sys.argv[1:2] == ["--original"]:
        print(original("", int(sys.argv[2], 16), int(sys.argv[3])))
        return
    names = sys.argv[1:]
    os.chdir(ROOT)
    rows = list(csv.DictReader(open("research/drafts/index.tsv"), delimiter="\t"))
    groups = {}
    for r in rows:
        if names and not any(k in r["function"] for k in names):
            continue
        groups.setdefault((r["draft"], r["compile_settings_from"]), []).append(r["function"])
    for (draft, settings), fns in sorted(groups.items()):
        with tempfile.TemporaryDirectory() as work:
            obj, log = link_drafts.compile_draft(draft, settings, work)
            if not obj:
                continue
            funcs = parse(obj)
        for f in fns:
            if f not in funcs:
                continue
            hits = check(funcs[f])
            if hits:
                direct = [h for h in hits if h[1] != "bctrl"]
                print(("CLOBBERED " if direct else "virtual   ") + f, draft,
                      [(hex(a), c, r) for a, c, r in (direct or hits)[:4]])


if __name__ == "__main__":
    main()
