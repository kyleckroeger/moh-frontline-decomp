"""sweep.py units.txt: draft each unit with port_unit before 9bd2207e (oldtools/) and the current
tools/port_unit.py (same compile) into scratch configs; report differing section fields."""
import argparse, importlib.util, json, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[3]
here = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / "tools"))
spec2 = importlib.util.spec_from_file_location("port_unit_fix", Path(__file__).resolve().parent / "fixtools/port_unit.py")
new = importlib.util.module_from_spec(spec2); spec2.loader.exec_module(new)
spec = importlib.util.spec_from_file_location("port_unit_old", Path("/home/kyle/workspace/moh-frontline/tools/port_unit.py"))
old = importlib.util.module_from_spec(spec); spec.loader.exec_module(old)
for mod, sub in ((old, "cfg_cur"), (new, "cfg_fix")):
    (here / sub).mkdir(exist_ok=True); mod.CONFIG = here / sub; mod.ROOT = here
new.compile_sdk = lambda *a, **k: None
def args(uid):
    path = ROOT / "config/GMFE69" / f"{uid}.json"; m = json.loads(path.read_text()); ls = m.get("linker_stripped", {})
    return m, argparse.Namespace(id=uid, source=m["source"], like=str(path), original_file=m.get("original_file"),
        file_index=m.get("original_file_index"), no_file_record="original_file" not in m, compiler=None,
        define=None, category=m["category"], weak_duplicate=m.get("weak_duplicates", []),
        file_duplicate=m.get("file_duplicates", []), pool=m.get("pooled_sections", []),
        linker_stripped=ls.get("functions", []) + ls.get("objects", []), leading_filler=m.get("leading_filler"), upstream_path=None)
def secs(u): return {s["name"]: s for s in u["sections"]}
for uid in Path(sys.argv[1]).read_text().split():
    m, ns = args(uid)
    try: old.draft(ns)
    except Exception as e: print(f"{uid}: OLD FAILED {e!r}", flush=True); continue
    try: new.draft(ns)
    except Exception as e: print(f"{uid}: NEW FAILED {e!r}", flush=True); continue
    o = json.loads((here / "cfg_cur" / f"{uid}.json").read_text()); n = json.loads((here / "cfg_fix" / f"{uid}.json").read_text())
    so, sn, sm = secs(o), secs(n), secs(m)
    diffs = []
    for name in sorted(set(so) | set(sn)):
        a, b = so.get(name, {}), sn.get(name, {})
        for f in sorted(set(a) | set(b)):
            if a.get(f) != b.get(f):
                diffs.append(f"{name}.{f}: old {a.get(f)} new {b.get(f)} committed {sm.get(name, {}).get(f)}")
    rest = sorted(k for k in set(o) | set(n) if k != "sections" and o.get(k) != n.get(k))
    for k in rest: diffs.append(f"{k}: old {o.get(k)} new {n.get(k)} committed {m.get(k)}")
    print(f"{uid}: {'CHANGED' if diffs else 'same'} old==committed:{so == sm} new==committed:{sn == sm}", flush=True)
    for d in diffs: print("   ", d, flush=True)
