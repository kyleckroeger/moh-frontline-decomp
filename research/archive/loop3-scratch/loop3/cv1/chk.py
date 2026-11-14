import os,sys,json,glob
from pathlib import Path
sys.path.insert(0,'research/tools')
import try_header as th
from concurrent.futures import ProcessPoolExecutor
HD=sys.argv[1]
def job(a):
    uid,src=a
    unit=json.loads((th.CONFIG/f"{uid}.json").read_text())
    if src: unit=dict(unit, source=str(Path(src).resolve().relative_to(th.ROOT)))
    unit=dict(unit, include_dirs=[HD]+unit["include_dirs"])
    return uid,*th.verify(unit, Path('scratch/loop3/cv1/wchk')/uid)
if __name__=="__main__":
    staged={Path(p).stem:p for p in glob.glob('scratch/loop3/cv1/stage/*.cpp')}
    hdr=[u for u in open('scratch/loop3/cv1/hdr.txt').read().split() if u not in staged]
    jobs=[(u,None) for u in hdr]+list(staged.items())
    bad=0
    with ProcessPoolExecutor(4) as p:
        for r in p.map(job,jobs):
            if r[1]!='kept': bad+=1; print(*r,flush=True)
    print(len(jobs),'checked',bad,'bad')
