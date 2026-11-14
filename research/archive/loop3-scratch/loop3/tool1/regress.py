"""regress.py [--tools DIR] unit...: redraft each unit with port_unit (rebuilt args from its
committed manifest) into a scratch config and diff extab/extabindex/stripped fields."""
import argparse, json, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[3]
p = argparse.ArgumentParser(); p.add_argument("--tools", default=str(ROOT / "tools")); p.add_argument("units", nargs="+")
a = p.parse_args()
sys.path.insert(0, str(ROOT / "tools")); sys.path.insert(0, a.tools)
import port_unit
print("port_unit from", port_unit.__file__)
here = Path(__file__).resolve().parent
out = here / "config"; out.mkdir(exist_ok=True)
port_unit.CONFIG = out
port_unit.ROOT = here  # work dir -> scratch/loop3/tool1/scratch/port/<id>
FIELDS = ("address", "size", "linked_offset", "linked_size", "stripped_objects")
bad = 0
for uid in a.units:
    path = ROOT / "config/GMFE69" / f"{uid}.json"
    m = json.loads(path.read_text())
    ls = m.get("linker_stripped", {})
    ns = argparse.Namespace(id=uid, source=m["source"], like=str(path), original_file=m.get("original_file"),
        file_index=m.get("original_file_index"), no_file_record="original_file" not in m, compiler=None,
        define=None, category=m["category"], weak_duplicate=m.get("weak_duplicates", []),
        file_duplicate=m.get("file_duplicates", []), pool=m.get("pooled_sections", []),
        linker_stripped=ls.get("functions", []) + ls.get("objects", []),
        leading_filler=m.get("leading_filler"), upstream_path=None)
    try:
        port_unit.draft(ns)
    except Exception as e:
        print(f"{uid}: DRAFT FAILED {e!r}"); bad += 1; continue
    d = json.loads((out / f"{uid}.json").read_text())
    diffs = []
    secs = lambda u: {s["name"]: s for s in u["sections"]}
    old, new = secs(m), secs(d)
    for name in sorted(set(old) | set(new)):
        o, n = old.get(name, {}), new.get(name, {})
        for f in FIELDS:
            if o.get(f) != n.get(f) and (name in ("extab", "extabindex") or f == "stripped_objects"):
                diffs.append(f"{name}.{f}: committed {o.get(f)} drafted {n.get(f)}")
    if m.get("stripped_sections") != d.get("stripped_sections"):
        diffs.append(f"stripped_sections: committed {m.get('stripped_sections')} drafted {d.get('stripped_sections')}")
    print(f"{uid}: {'OK' if not diffs else 'DIFF'}"); [print("   ", x) for x in diffs]
    bad += bool(diffs)
print("FAILED UNITS:", bad)
sys.exit(bool(bad))
