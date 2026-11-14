import sys,json,tempfile
sys.path.insert(0,'scratch/loop3/big1'); sys.path.insert(0,'scratch/loop3/big2'); sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import sc,fast,varmap,who2
def asm(text,flags):
    cfg=json.load(open(sc.T)); fl=cfg['compiler_flags']
    for k,v in flags.items(): fl[fl.index(k)+1]=v
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(cfg,text,w)
        if not obj: print(log[:2000]); return None
        return varmap.rows(['--disassemble='+sc.F,obj])
t=open(sys.argv[1]).read()
for fl in [{}, {'-inline':'deferred,auto'}, {'-inline':'deferred,auto','-RTTI':'on'}]:
    m=asm(t,fl)
    if m: tb=who2.tab(m); print(fl,*sc.regions(m),tb[-1] if tb else '')
