#!/usr/bin/env python3
"""try_dwi.py: move units with their own dwi container views onto game/DwiContainers.h and
game/DwiHashMap.h (copies only), then verify as try_header.py does.

    try_dwi.py [--jobs 4] [--only UNIT...] [--out TSV]
Env: TRY_HEADER_DIR (draft headers), TRY_HEADER_WORK (build dir).
"""
import argparse, json, os, re, sys, traceback
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "research/tools"))
import try_header as th
from try_header import block_end, _arity, verify

CONFIG = ROOT / "config/GMFE69"
HEADER_DIR = th.HEADER_DIR
WORK = th.WORK

CONT = ["list_node", "list_iter", "list", "vec_iter", "vec_const_iter", "vector", "fast_vec"]
HASH = ["hash_map"]


def header_inline(name):
    """{class: {(member, arity)}} of members the header defines (in class or out of class)."""
    text = (HEADER_DIR / f"game/{name}").read_text()
    res = {}
    for m in re.finditer(r"^(?:template\s*<[^>]*>\s*)?(?:class|struct)\s+(\w+)\b[^;{]*\{", text, re.M):
        body = text[m.end():block_end(text, m.end() - 1) - 1]
        s = res.setdefault(m[1], set())
        for f in re.finditer(r"^\s+(?:template\s*<[^>]*>\s*)?(?:static\s+|inline\s+)*[\w:<>&\*\s,]*?(~?\w+|operator\S+?)\s*\(([^;{]*)\)\s*(?:const\s*)?(?::[^{;]*)?\{", body, re.M):
            s.add((f[1], _arity(f[2])))
    for m in re.finditer(r"^template\s*<[^>]*>\s*(?:__declspec\(weak\)\s*)?[\w:<>&\*\s]*?\b(\w+)\s*<[^>]*>::(~?\w+|operator\S+?)\s*\(([^;{]*)\)\s*(?:const\s*)?\{", text, re.M):
        res.setdefault(m[1], set()).add((m[2], _arity(m[3])))
    return res


def ns_blocks(t):
    return [(m.start(), block_end(t, m.end() - 1), m.end()) for m in re.finditer(r"^namespace\s+dwi\s*\{", t, re.M)]


def strip_block(inner, names, inl):
    """Remove covered class templates, their forward declarations and out-of-class
    definitions of members the header defines, min/max and the prime table from a
    namespace body. Returns (new_inner, removed_any, kinds)."""
    kinds = set()
    out = inner
    changed = True
    while changed:
        changed = False
        # class templates
        for m in re.finditer(r"(?:^|\n)[ \t]*(template\s*<[^>]*>\s*)?(class|struct)\s+(\w+)\b[^;{]*\{", out):
            if m[3] in names:
                e = block_end(out, m.end() - 1)
                semi = out.index(";", e)
                out = out[:m.start()] + "\n" + out[semi + 1:]
                kinds.add(m[3]); changed = True
                break
        if changed:
            continue
        # forward declarations
        m = re.search(r"(?:^|\n)[ \t]*template\s*<[^>]*>\s*(?:class|struct)\s+(" + "|".join(names) + r")\s*;", out)
        if m:
            out = out[:m.start()] + "\n" + out[m.end():]; changed = True; continue
        # out-of-class member definitions
        for m in re.finditer(r"(?:^|\n)[ \t]*template\s*<[^>]*>\s*(?:__declspec\(weak\)\s*)?(?:inline\s+)?[\w:<>&\*\s]*?\b(\w+)\s*<[^<>]*>::(~?\w+|operator\S+?)\s*\(([^;{]*)\)\s*(?:const\s*)?(?::[^{;]*)?\{", out):
            if m[1] in names and (m[2], _arity(m[3])) in inl.get(m[1], set()):
                e = block_end(out, m.end() - 1)
                out = out[:m.start()] + "\n" + out[e:]
                kinds.add(m[1]); changed = True
                break
        if changed:
            continue
        # min / max
        m = re.search(r"(?:^|\n)[ \t]*template\s*<class T>\s*inline\s+const\s+T&\s+(min|max)\s*\([^)]*\)\s*\{", out)
        if m and "DwiContainers.h" in names_hdr(names):
            e = block_end(out, m.end() - 1)
            out = out[:m.start()] + "\n" + out[e:]
            kinds.add("minmax"); changed = True
            continue
        # prime table
        m = re.search(r"(?:^|\n)[ \t]*static\s+const\s+unsigned\s+long\s+dwi_prime_list\s*\[\s*27\s*\]\s*=\s*\{", out)
        if m and "DwiHashMap.h" in names_hdr(names):
            e = block_end(out, m.end() - 1)
            semi = out.index(";", e)
            out = out[:m.start()] + "\n" + out[semi + 1:]
            kinds.add("primes"); changed = True
    return out, kinds


def names_hdr(names):
    return "DwiHashMap.h" if names == HASH else "DwiContainers.h"


TOP_NEW = re.compile(r"^inline\s+void\s*\*\s*operator\s+new\s*\(\s*unsigned\s+long\s*,\s*void\s*\*\s*\w*\s*\)\s*\{[^}]*\}[ \t]*\n?", re.M)
TOP_MINMAX = re.compile(r"^(?:/\*[^\n]*\*/\s*\n)?template\s*<class T>\s*inline\s+const\s+T&\s+(Min|Max)\s*\(const T& a, const T& b\)\s*\{\s*return a < b \? (a : b|b : a);\s*\}[ \t]*\n?", re.M)


def moved(text, which):
    """which: subset of {'cont', 'hash'}. Returns (new_text or None, kinds)."""
    kinds_all = set()
    out = text
    includes = []  # (position, header)
    for key, names in (("hash", HASH), ("cont", CONT)):
        if key not in which:
            continue
        hdr = names_hdr(names)
        inl = header_inline(hdr)
        first = None
        for a, b, body in reversed(ns_blocks(out)):
            inner = out[body:b - 1]
            new, kinds = strip_block(inner, names, inl)
            if not kinds:
                continue
            kinds_all |= kinds
            if new.strip() == "":
                out = out[:a] + "\x00" + out[b:]
                pos = a
            else:
                out = out[:body] + new + out[b - 1:]
                out = out[:a] + "\x00" + out[a:]
                pos = a
            first = pos if first is None else min(first, pos)
        if first is not None:
            # keep only the first marker as the include point
            idx = [i for i, c in enumerate(out) if c == "\x00"]
            keep = idx[0]
            out = "".join(c for i, c in enumerate(out) if c != "\x00" or i == keep)
            out = out.replace("\x00", f'#include "game/{hdr}"\n', 1)
    if "cont" in which and kinds_all & set(CONT + ["minmax"]):
        out = TOP_NEW.sub("", out)
    if "cont" in which:
        n = len(TOP_MINMAX.findall(out))
        if n:
            out = TOP_MINMAX.sub("", out)
            out = re.sub(r"(?<![\w:.>])Min\s*\(", "dwi::min(", out)
            out = re.sub(r"(?<![\w:.>])Max\s*\(", "dwi::max(", out)
            if '#include "game/DwiContainers.h"' not in out:
                m = re.search(r"^(?:#include[^\n]*\n)+", out, re.M)
                pos = m.end() if m else 0
                out = out[:pos] + '#include "game/DwiContainers.h"\n' + out[pos:]
            kinds_all.add("Minmax")
    out = re.sub(r"\n{3,}", "\n\n", out)
    return (out if kinds_all else None), kinds_all


def candidates(only=None):
    units = json.loads((CONFIG / "project.json").read_text())["units"]
    jobs = []
    for name in units:
        uid = name[:-5]
        if only and uid not in only:
            continue
        unit = json.loads((CONFIG / name).read_text())
        src = ROOT / unit["source"]
        if not src.exists():
            continue
        t = src.read_text(errors="replace")
        if "namespace dwi" in t or TOP_MINMAX.search(t):
            jobs.append(uid)
    return jobs


def attempt(uid):
    unit = json.loads((CONFIG / f"{uid}.json").read_text())
    base = WORK / uid
    text = (ROOT / unit["source"]).read_text(errors="replace")
    new, kinds = moved(text, {"cont", "hash"})
    if new is None:
        return uid, unit["source"], "skip", "no covered view"
    status, detail = verify(unit, base / "control")
    if status != "kept":
        return uid, unit["source"], "control_fail", detail
    copy = base / "src" / Path(unit["source"]).name
    copy.parent.mkdir(parents=True, exist_ok=True)
    copy.write_text(new)
    unit = dict(unit, source=str(copy.relative_to(ROOT)),
                include_dirs=unit["include_dirs"] + [str(HEADER_DIR.relative_to(ROOT))] + ["include"])
    try:
        status, detail = verify(unit, base / "moved")
    except Exception:
        status, detail = "error", traceback.format_exc().splitlines()[-1]
    return uid, unit["source"], status, ",".join(sorted(kinds)) + " " + detail


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--only", nargs="*")
    ap.add_argument("--out", default=str(ROOT / "research/layout/header_moves/DwiContainers.tsv"))
    ap.add_argument("--dry", action="store_true")
    a = ap.parse_args()
    jobs = candidates(a.only)
    print(f"{len(jobs)} candidate units", flush=True)
    if a.dry:
        for uid in jobs:
            unit = json.loads((CONFIG / f"{uid}.json").read_text())
            new, kinds = moved((ROOT / unit["source"]).read_text(errors="replace"), {"cont", "hash"})
            print(uid, sorted(kinds))
        return
    rows = []
    with ProcessPoolExecutor(a.jobs) as pool:
        for row in pool.map(attempt, jobs):
            rows.append(row)
            print("\t".join(row[:3]) + "\t" + row[3][:160], flush=True)
    out = Path(a.out)
    if a.only and out.exists():
        old = {r.split("\t")[0]: r for r in out.read_text().splitlines()[1:]}
        for r in rows:
            old[r[0]] = "\t".join(r)
        lines = sorted(old.values())
    else:
        lines = sorted("\t".join(r) for r in rows)
    out.write_text("unit\tsource\tstatus\tdetail\n" + "\n".join(lines) + "\n")
    from collections import Counter
    print(Counter(l.split("\t")[2] for l in lines))


if __name__ == "__main__":
    main()
