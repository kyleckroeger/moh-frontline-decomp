"""var.py base.cpp func json variants.py : apply each variant (old->new replacement list) and print score + reg map"""
import sys,os,subprocess,importlib.util
base,func,tj,vf=sys.argv[1:5]
spec=importlib.util.spec_from_file_location('vv',vf); vv=importlib.util.module_from_spec(spec); spec.loader.exec_module(vv)
src=open(base).read()
for name,reps in vv.V.items():
    s=src
    ok=True
    for a,b in reps:
        if a not in s: print(name,'MISSING',a[:40]); ok=False; break
        s=s.replace(a,b,1)
    if not ok: continue
    fn='v/%s.cpp'%name; open(fn,'w').write(s)
    r=subprocess.run(['python3','rm.py',fn,func],capture_output=True,text=True,env=dict(os.environ,T=tj))
    print(name,(r.stdout or r.stderr)[:300].strip().replace(fn,''),flush=True)
