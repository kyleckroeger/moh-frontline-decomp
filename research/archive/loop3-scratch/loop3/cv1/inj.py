# usage: python3 inj.py VARIANT JOBS unit...   (inserts a copy constructor into the unit's own CVector3 view)
import os,sys,json,re
from pathlib import Path
os.environ.setdefault("TRY_HEADER_WORK", str(Path("scratch/loop3/cv1/winj").resolve()))
sys.path.insert(0,'research/tools')
import try_header as th
from concurrent.futures import ProcessPoolExecutor
V={
 "none": "",
 "pair": "CVector3(const CVector3& o) { *(CVector3Pair_*)this = *(const CVector3Pair_*)&o; }",
 "dbl": "CVector3(const CVector3& o) { ((double*)this)[0] = ((const double*)&o)[0]; ((double*)this)[1] = ((const double*)&o)[1]; }",
 "flt": "CVector3(const CVector3& o) { ((float*)this)[0] = ((const float*)&o)[0]; ((float*)this)[1] = ((const float*)&o)[1]; ((float*)this)[2] = ((const float*)&o)[2]; ((float*)this)[3] = ((const float*)&o)[3]; }",
 "flt3": "CVector3(const CVector3& o) { ((float*)this)[0] = ((const float*)&o)[0]; ((float*)this)[1] = ((const float*)&o)[1]; ((float*)this)[2] = ((const float*)&o)[2]; }",
 "dbl2": "CVector3(const CVector3& o) { double a = ((const double*)&o)[0]; double b = ((const double*)&o)[1]; ((double*)this)[0] = a; ((double*)this)[1] = b; }",
 "flt4b": "CVector3(const CVector3& o) { float a = ((const float*)&o)[0], b = ((const float*)&o)[1], c = ((const float*)&o)[2], d = ((const float*)&o)[3]; ((float*)this)[0] = a; ((float*)this)[1] = b; ((float*)this)[2] = c; ((float*)this)[3] = d; }",
 "self": "CVector3(const CVector3& o) { *this = o; }",
 "pod4f": "CVector3(const CVector3& o) { *(F4_*)this = *(const F4_*)&o; }",
 "pod4f8": "CVector3(const CVector3& o) { *(F4a_*)this = *(const F4a_*)&o; }",
 "decl": "CVector3(const CVector3& o);",
 "def": "CVector3() {}",
}
PRE="struct CVector3Pair_ { double a, b; };\nstruct F4_ { float a, b, c, d; };\nstruct F4a_ { float a, b, c, d; } __attribute__((aligned(8)));\n"
def job(a):
    var,uid=a
    unit=json.loads((th.CONFIG/f"{uid}.json").read_text())
    src=(th.ROOT/unit["source"]).read_text(errors="replace")
    m=re.search(r"^(?:class|struct)\s+CVector3\b[^;{]*\{\s*\n(\s*public:\s*\n)?", src, re.M)
    if not m: return (uid,var,"noview","")
    new=src[:m.start()]+PRE+src[m.start():m.end()]+"    "+V[var]+("\n    CVector3() {}" if var not in ("none","def") and "CVector3()" not in src else "")+"\n"+src[m.end():]
    base=th.WORK/uid/var
    copy=base/"src"/Path(unit["source"]).name
    copy.parent.mkdir(parents=True,exist_ok=True); copy.write_text(new)
    unit=dict(unit, source=str(copy.relative_to(th.ROOT)))
    st,d=th.verify(unit, base/"b"); return (uid,var,st,d[:110])
if __name__=="__main__":
    vs=sys.argv[1].split(","); n=int(sys.argv[2])
    with ProcessPoolExecutor(n) as p:
        for r in p.map(job, [(v,u) for u in sys.argv[3:] for v in vs]): print("\t".join(r),flush=True)
