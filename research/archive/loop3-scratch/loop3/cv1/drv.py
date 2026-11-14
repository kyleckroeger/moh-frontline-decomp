# usage: TRY_HEADER_DIR=... TRY_HEADER_WORK=... python3 drv.py JOBS unit...
import os,sys,json,re
from pathlib import Path
sys.path.insert(0,'research/tools')
import try_header as th
from concurrent.futures import ProcessPoolExecutor
pat=re.compile(r"^(?:class|struct)\s+CVector3\b[^;{]*\{", re.M)
def job(uid):
    unit=json.loads((th.CONFIG/f"{uid}.json").read_text())
    src=(th.ROOT/unit["source"]).read_text(errors="replace")
    if pat.search(src):
        r=th.attempt((uid,"CVector3")); return (uid,"own",r[2],r[3][:120])
    unit=dict(unit, include_dirs=[str(th.HEADER_DIR.relative_to(th.ROOT))]+unit["include_dirs"])
    st,d=th.verify(unit, th.WORK/uid/"onhdr"); return (uid,"hdr",st,d[:120])
if __name__=="__main__":
    n=int(sys.argv[1])
    with ProcessPoolExecutor(n) as p:
        for r in p.map(job, sys.argv[2:]): print("\t".join(r),flush=True)
