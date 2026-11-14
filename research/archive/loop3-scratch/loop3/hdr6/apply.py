"""apply.py plain|rd|relabel UNIT...: copy try_dwi's moved source over the real one; plain adds include
to include_dirs, rd/relabel take the unit.json redraft.py/relabel.py wrote (scratch include dir dropped)."""
import json, sys, shutil
from pathlib import Path
R = Path("/home/kyle/workspace/moh-frontline"); H = R / "scratch/loop3/hdr6"
mode = sys.argv[1]
for u in sys.argv[2:]:
    p = R / f"config/GMFE69/{u}.json"
    cfg = json.loads(p.read_text())
    src = R / cfg["source"]
    d = H / ("rd" if mode == "rd" else "work") / u
    shutil.copy(d / "src" / src.name, src)
    if mode != "plain":
        new = json.loads((d / "unit.json").read_text())
        new["source"] = cfg["source"]
        new["include_dirs"] = [x for x in new["include_dirs"] if "hdr6" not in x]
        if "include" not in new["include_dirs"]:
            new["include_dirs"].append("include")
        cfg = new
    elif "include" not in cfg["include_dirs"]:
        cfg["include_dirs"].append("include")
    p.write_text(json.dumps(cfg, indent=2) + "\n")
    print(u)
