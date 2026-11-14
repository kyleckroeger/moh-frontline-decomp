# usage: python3 ver.py UNIT SRCFILE [HEADERDIR]  -> verifies SRCFILE as UNIT's source
import os,sys,json
from pathlib import Path
sys.path.insert(0,'research/tools')
import try_header as th
uid,src=sys.argv[1],sys.argv[2]
unit=json.loads((th.CONFIG/f"{uid}.json").read_text())
inc=[sys.argv[3]] if len(sys.argv)>3 else []
unit=dict(unit, source=str(Path(src).resolve().relative_to(th.ROOT)), include_dirs=inc+unit["include_dirs"])
w=Path(src).resolve().parent/("b_"+Path(src).stem)
print(uid, *th.verify(unit, w))
