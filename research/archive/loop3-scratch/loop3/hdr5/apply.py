"""apply.py TRYDIR UNIT...: copy try_header's moved source over the real one, add include to include_dirs."""
import json, sys, shutil
from pathlib import Path
R = Path("/home/kyle/workspace/moh-frontline")
d = Path(sys.argv[1])
for u in sys.argv[2:]:
    p = R / f"config/GMFE69/{u}.json"
    raw = p.read_text(); cfg = json.loads(raw)
    src = R / cfg["source"]
    shutil.copy(d / u / "src" / src.name, src)
    if "include" not in cfg["include_dirs"]:
        cfg["include_dirs"].append("include")
        p.write_text(json.dumps(cfg, indent=2) + "\n")
    print(u)
