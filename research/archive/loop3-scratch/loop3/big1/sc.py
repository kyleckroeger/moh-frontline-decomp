"""sc.py src... : real score + per-region diff counts for UpdateAnimation (big1)."""
import sys,json,re,tempfile
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import fast,varmap,score
T='config/GMFE69/Soldier_object_lit.json'; F='UpdateAnimation__14CSoldierObjectFf'
A='0x800acfb8'; E='0x800adeac'
_o=None
def orig():
    global _o
    if _o is None: _o=varmap.rows(['--start-address='+A,'--stop-address='+E,fast.ELF])
    return _o
def asm(text):
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(json.load(open(T)),text,w)
        if not obj: return None
        return varmap.rows(['--disassemble='+F,obj])
R=[('d<0',480,520),('cross',600,640),('conv',700,780)]
def regions(m):
    o=orig(); bad=[i for i,(x,y) in enumerate(zip(o,m)) if not score.eq(x,y)]
    out=[]
    for n,a,b in R: out.append('%s=%d'%(n,sum(1 for i in bad if a<=i<b)))
    other=[i for i in bad if not any(a<=i<b for n,a,b in R)]
    out.append('other=%d%s'%(len(other),(' '+','.join(map(str,other[:8]))) if other else ''))
    return len(bad)+abs(len(o)-len(m)),len(m)-len(o),' '.join(out)
if __name__=='__main__':
    for s in sys.argv[1:]:
        m=asm(open(s).read())
        if not m: print(s,'COMPILEFAIL'); continue
        print('%-30s'%s.split('/')[-1],*regions(m))
