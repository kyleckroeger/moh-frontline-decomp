#!/usr/bin/env python3
"""relabel.py CLASS UNIT...: after try_header (TRY_HEADER_WORK=hdr3/try), rename the unit's
stripped_objects labels from the control compile to the moved compile's, matching objects by
section, size and relocation targets (labels in targets normalised, in order), then verify.
Writes try/<unit>/unit.json; prints unit status detail mapping."""
import json, os, re, struct, sys, traceback
from pathlib import Path
ROOT = Path("/home/kyle/workspace/moh-frontline")
WORK = Path(os.environ.get("RELABEL_WORK", ROOT / "scratch/loop3/hdr3/try"))
os.environ["TRY_HEADER_WORK"] = str(WORK)
sys.path.insert(0, str(ROOT / "research/tools")); sys.path.insert(0, str(ROOT / "tools"))
os.chdir(ROOT)
import try_header as th
from formats import Elf32

def sigs(path):
    obj = Elf32(path.read_bytes()); syms = list(obj.symbols())
    def at(sec, addr):
        for s in syms:
            if s["section"] == sec and s["type"] in (1, 2) and s["address"] <= addr < s["address"] + max(s["size"], 1):
                return f"{s['name']}+{addr - s['address']:#x}"
        return f"{obj.sections[sec]['name']}+{addr:#x}"
    out = {}
    for s in syms:
        if not (s["name"].startswith("@") and s["section"] and s["section"] < 0xff00):
            continue
        rel = []
        for rs in obj.sections:
            if rs["type"] != 4 or rs["info"] != s["section"]:
                continue
            data = obj.contents(rs)
            for o in range(0, rs["size"], 12):
                loc, info, add = struct.unpack_from(">IIi", data, o)
                if s["address"] <= loc < s["address"] + s["size"]:
                    t = syms[info >> 8]
                    name = t["name"] if t["name"] and t["type"] != 3 else None
                    tgt = f"{name}+{add:#x}" if name else at(t["section"], t["address"] + add)
                    rel.append(f"{loc - s['address']:#x}:{info & 255}:{re.sub(r'@\d+', '@', tgt)}")
        sec = obj.sections[s["section"]]["name"]
        data = obj.contents(obj.sections[s["section"]]) if obj.sections[s["section"]]["type"] != 8 else b""
        body = data[s["address"]:s["address"] + s["size"]].hex() if data else ""
        out[s["name"]] = (sec, s["size"], tuple(rel), body)
    return out

def labels(o, acc):
    if isinstance(o, dict):
        for k, v in o.items():
            if k == "stripped_objects": acc.extend(x for x in v if x.startswith("@"))
            else: labels(v, acc)
    elif isinstance(o, list):
        for v in o: labels(v, acc)
    return acc

def rename(o, m):
    if isinstance(o, dict):
        return {k: ([m.get(x, x) for x in v] if k == "stripped_objects" else rename(v, m)) for k, v in o.items()}
    if isinstance(o, list): return [rename(v, m) for v in o]
    return o

def one(uid):
    unit = json.loads((ROOT / f"config/GMFE69/{uid}.json").read_text())
    base = WORK / uid
    a, b = sigs(base / "control/compiled.o"), sigs(base / "moved/compiled.o")
    old = labels(unit, [])
    used, m = set(), {}
    for l in old:
        if l not in a: return uid, "nolabel", l, ""
        cands = [n for n in sorted(b, key=lambda n: int(n[1:])) if b[n] == a[l] and n not in used]
        # keep relative order: pick the candidate whose rank among equal signatures matches
        same = [n for n in sorted(a, key=lambda n: int(n[1:])) if a[n] == a[l]]
        idx = same.index(l)
        allb = [n for n in sorted(b, key=lambda n: int(n[1:])) if b[n] == a[l]]
        if len(allb) != len(same): return uid, "nomatch", f"{l}: {len(same)} vs {len(allb)}", ""
        m[l] = allb[idx]; used.add(m[l])
    new = rename(unit, m)
    src = next((base / "src").iterdir())
    new_t = dict(new, source=str(src.relative_to(ROOT)), include_dirs=new["include_dirs"] + ([] if "include" in new["include_dirs"] else ["include"]))
    status, d = th.verify(new_t, base / "relabel")
    real = dict(new, include_dirs=new_t["include_dirs"])
    (base / "unit.json").write_text(json.dumps(real, indent=2) + "\n")
    return uid, status, d, " ".join(f"{k}->{v}" for k, v in m.items() if k != v)

if __name__ == "__main__":
    from concurrent.futures import ProcessPoolExecutor
    def job(u):
        try: return one(u)
        except Exception: return u, "error", traceback.format_exc().splitlines()[-1], ""
    cls = sys.argv[1]
    with ProcessPoolExecutor(4) as p:
        for r in p.map(job, sys.argv[2:]):
            print("\t".join(r), flush=True)
