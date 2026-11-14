"""var2.py base.cpp vars.py [outdir]: like big1 var.py + cross colour table."""
import sys
sys.path.insert(0,'scratch/loop3/big1'); sys.path.insert(0,'scratch/loop3/big2')
import sc,who2
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
    tb=who2.tab(m)
    print('%-16s'%name,*sc.regions(m)[:1],sc.regions(m)[2][:40],tb[-1] if tb else '',flush=True)
    if len(sys.argv)>3: open('%s/out_%s.cpp'%(sys.argv[3],name),'w').write(t)
