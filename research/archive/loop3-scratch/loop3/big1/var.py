"""var.py base.cpp vars.py : vars.py defines V = [(name, [(old,new),...]), ...]; scores each (big1)."""
import sys
sys.path.insert(0,'scratch/loop3/big1')
import sc
base=open(sys.argv[1]).read()
ns={}; exec(open(sys.argv[2]).read(),ns)
for name,reps in ns['V']:
    t=base; ok=True
    for o,n in reps:
        if t.count(o)!=1: print(name,'NOMATCH',repr(o[:50]),t.count(o)); ok=False; break
        t=t.replace(o,n)
    if not ok: continue
    m=sc.asm(t)
    if not m: print('%-24s COMPILEFAIL'%name); continue
    print('%-24s'%name,*sc.regions(m),flush=True)
    if len(sys.argv)>3: open('scratch/loop3/big1/out_%s.cpp'%name,'w').write(t)
