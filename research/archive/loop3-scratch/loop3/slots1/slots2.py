"""slots.py <tmpl> <func> <addr> <size> files...: r1 offset sequence ours vs orig + length + real score."""
import sys,json,re,tempfile,difflib
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast,varmap,score
t,f,a,s=sys.argv[1:5]
e=hex(int(a,16)+int(s))
o=varmap.rows(['--start-address=0x'+a,'--stop-address='+e,fast.ELF])
def sl(rows): return [m.group(1) for r in rows for m in [re.search(r'(-?\d+)\(r1\)|r1,(\d+)$',r)] if m for _ in [0]]
def sl(rows):
    out=[]
    for r in rows:
        m=re.search(r'(-?\d+)\(r1\)',r) or re.search(r'r1,(-?\d+)$',r)
        if m and not r.startswith(('stwu','stmw','lmw','stw r0','lwz r0')): out.append(r.split()[0][:3]+m.group(1))
    return out
so=sl(o)
for p in sys.argv[5:]:
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(json.load(open(t)),open(p).read(),w)
        if not obj: print(p,'FAIL'); continue
        m=varmap.rows(['--disassemble='+f,obj])
    sm=sl(m)
    nd=sum(1 for x,y in zip(so,sm) if x!=y)+abs(len(so)-len(sm))
    print(p,'len',len(m)-len(o),'slotdiff',nd,'real',score.real(t,open(p).read(),f,a,s))
    if '-v' in sys.argv[:0] or len(sys.argv)==6:
        print(' orig',' '.join(so)); print(' ours',' '.join(sm))
