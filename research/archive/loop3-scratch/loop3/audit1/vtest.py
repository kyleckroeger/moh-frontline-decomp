"""vtest.py UID SRC [--cvec]: verify unit UID with source SRC (or SRC with its CVector3 view moved to the shared header)."""
import json, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "research/tools"))
import try_header as th
uid, src = sys.argv[1], Path(sys.argv[2]).resolve()
unit = json.loads((th.CONFIG / f"{uid}.json").read_text())
if "--cvec" in sys.argv:
    out = ROOT / "scratch/loop3/audit1/var" / (uid + "_cvec" + src.suffix)
    out.write_text(th.moved(src.read_text(errors="replace"), "CVector3"))
    src = out
unit = dict(unit, source=str(src.relative_to(ROOT)), include_dirs=unit["include_dirs"] + ["include"])
tag = src.parent.name + "_" + src.stem
print(uid, src.name, *th.verify(unit, ROOT / "scratch/loop3/audit1/work" / tag))
