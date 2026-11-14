# sc.py: scoring helper. import sc; s=sc.Scorer(tmpl,func,addr,size); s(src)->(structural,real)
# CLI: python3 sc.py ubb|rs src.cpp [diff]
import re,sys,json,tempfile,difflib
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast,varmap,score
R='/home/kyle/workspace/moh-frontline/'
norm=lambda x:re.sub(r'\b[rf]\d+\b','R',re.sub(r'-?\d+\(r13\)|0\(0\)','SD',x))
norm2=lambda x:re.sub(r'lis R,-?\d+|addi R,R,-?\d+$',lambda m:m.group(0).split(',')[0],norm(x))
class Scorer:
    def __init__(s,tmpl,F,a,size):
        s.T=json.load(open(R+tmpl)); s.F=F
        s.o=varmap.rows(['--start-address=0x'+a,'--stop-address='+hex(int(a,16)+size),fast.ELF])
        s.no=[norm2(x) for x in s.o]
    def asm(s,src):
        with tempfile.TemporaryDirectory() as w:
            obj,log=fast.compile_object(s.T,src,w)
            if not obj: return None
            return varmap.rows(['--disassemble='+s.F,obj])
    def __call__(s,src):
        m=s.asm(src)
        if m is None: return (99999,99999)
        real=sum(1 for x,y in zip(s.o,m) if not score.eq(x,y))+abs(len(s.o)-len(m))
        sm=difflib.SequenceMatcher(None,s.no,[norm2(x) for x in m],autojunk=False)
        n=sum(max(i2-i1,j2-j1) for tag,i1,i2,j1,j2 in sm.get_opcodes() if tag!='equal')
        return (n,real)
UBB=lambda:Scorer('config/GMFE69/particlesystem_visible.json','UpdateBoundingBox__15CParticleSystemFv','8007fbe8',2948)
RS=lambda:Scorer('scratch/loop3/struct2/t_rs.json','RenderSystem__15CParticleSystemFR10CDmaPacket','8007d704',4532)
if __name__=='__main__':
    s={'ubb':UBB,'rs':RS}[sys.argv[1]]()
    src=open(sys.argv[2]).read()
    print(s(src))
    if len(sys.argv)>3:
        m=s.asm(src)
        for i,(x,y) in enumerate(zip(s.o,m)):
            print('%4d %s %-40s %s'%(i,' ' if score.eq(x,y) else '*',x[:40],y[:40]))

def hunks(S,src,ctx=0):
    m=S.asm(src); nm=[norm2(x) for x in m]
    sm=difflib.SequenceMatcher(None,S.no,nm,autojunk=False)
    for tag,i1,i2,j1,j2 in sm.get_opcodes():
        if tag=='equal': continue
        print('== %s o[%d:%d] m[%d:%d]'%(tag,i1,i2,j1,j2))
        for k in range(max(i2-i1,j2-j1)):
            x=S.o[i1+k] if i1+k<i2 else ''; y=m[j1+k] if j1+k<j2 else ''
            print('   %-38s | %s'%(x[:38],y[:40]))
