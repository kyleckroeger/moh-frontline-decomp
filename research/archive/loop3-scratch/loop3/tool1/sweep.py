"""sweep.py units.txt: draft each unit with tools/port_unit.py and the scratch copy (same compile), report differences."""
import argparse, importlib.util, json, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[3]
here = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT / "tools"))
import port_unit as old
spec = importlib.util.spec_from_file_location("port_unit_new", here / "tools/port_unit.py")
new = importlib.util.module_from_spec(spec); spec.loader.exec_module(new)
for mod, sub in ((old, "cfg_old"), (new, "cfg_new")):
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
    try:
        old.draft(ns)
    except Exception as e:
        print(f"{uid}: OLD FAILED {e!r}", flush=True); continue
    try:
        new.draft(ns)
    except Exception as e:
        print(f"{uid}: NEW FAILED {e!r}", flush=True); continue
    o = json.loads((here / "cfg_old" / f"{uid}.json").read_text()); n = json.loads((here / "cfg_new" / f"{uid}.json").read_text())
    oc = secs(o) == secs(m); nc = secs(n) == secs(m)
    changed = o != n
    print(f"{uid}: {'CHANGED' if changed else 'same'} old==committed:{oc} new==committed:{nc}", flush=True)
