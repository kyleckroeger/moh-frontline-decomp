"""hv.py UNIT...: verify each unit with scratch/loop3/merge9/incA prepended (moved); on failure
also verify control and relabel stripped_objects by signature (relabel.py logic). Writes
try/<u>/unit.json for relabelled ones."""
import json, os, sys, traceback
from pathlib import Path
ROOT = Path("/home/kyle/workspace/moh-frontline")
WORK = ROOT / "scratch/loop3/merge9/try"
os.environ["RELABEL_WORK"] = str(WORK); os.environ["TRY_HEADER_WORK"] = str(WORK)
sys.path.insert(0, str(ROOT / "scratch/loop3/hdr3"))
import relabel as rl
th = rl.th
INC = "scratch/loop3/merge9/incA"
def job(uid):
    try:
        unit = json.loads((ROOT / f"config/GMFE69/{uid}.json").read_text())
        mv = dict(unit, include_dirs=[INC] + unit["include_dirs"])
        st, d = th.verify(mv, WORK / uid / "moved")
        if st == "kept": return uid, "kept", ""
        st2, d2 = th.verify(unit, WORK / uid / "control")
        if st2 != "kept": return uid, "control_fail", d2
        base = WORK / uid
        a, b = rl.sigs(base / "control/compiled.o"), rl.sigs(base / "moved/compiled.o")
        old = rl.labels(unit, []); m = {}
        for l in old:
            if l not in a: return uid, "nolabel", l
            same = [n for n in sorted(a, key=lambda n: int(n[1:])) if a[n] == a[l]]
            allb = [n for n in sorted(b, key=lambda n: int(n[1:])) if b[n] == a[l]]
            if len(allb) != len(same): return uid, "nomatch", f"{l}: {len(same)} vs {len(allb)}"
            m[l] = allb[same.index(l)]
        new = rl.rename(unit, m)
        st3, d3 = th.verify(dict(new, include_dirs=[INC] + new["include_dirs"]), base / "relabel")
        (base / "unit.json").write_text(json.dumps(new, indent=2) + "\n")
        return uid, "relabel_" + st3, d3 + " " + " ".join(f"{k}->{v}" for k, v in m.items() if k != v) + " | was: " + d[:150]
    except Exception:
        return uid, "error", traceback.format_exc().splitlines()[-1]
if __name__ == "__main__":
    from concurrent.futures import ProcessPoolExecutor
    with ProcessPoolExecutor(4) as p:
        for r in p.map(job, sys.argv[1:]):
            print("\t".join(r), flush=True)
