import re,sys,json,tempfile,difflib,random
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/loop3/scripting/tools')
import fast,varmap,score
T=json.load(open('/home/kyle/workspace/moh-frontline/scratch/loop3/struct2/t_rs.json'))
F='RenderSystem__15CParticleSystemFR10CDmaPacket'; a='8007d704'; size=4532
o=varmap.rows(['--start-address=0x'+a,'--stop-address='+hex(int(a,16)+size),fast.ELF])
norm=lambda x:re.sub(r'\b[rf]\d+\b','R',re.sub(r'-?\d+\(r13\)|0\(0\)','SD',x))
norm2=lambda x:re.sub(r'lis R,-?\d+|addi R,R,-?\d+$',lambda m:m.group(0).split(',')[0],norm(x))
no=[norm2(x) for x in o]
def sc(src):
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(T,src,w)
        if not obj: return (99999,99999)
        m=varmap.rows(['--disassemble='+F,obj])
    real=sum(1 for x,y in zip(o,m) if not score.eq(x,y))+abs(len(o)-len(m))
    sm=difflib.SequenceMatcher(None,no,[norm2(x) for x in m],autojunk=False)
    n=sum(max(i2-i1,j2-j1) for tag,i1,i2,j1,j2 in sm.get_opcodes() if tag!='equal')
    return (n,real)
src=open(sys.argv[1]).read()
k=src.index('bool CParticleSystem::RenderSystem')
head,body=src[:k],src[k:]
sites=list(re.finditer(r'\b(\w+)\[([0-3])\]',body))
names='sizeRange spawnRange velRange birth deathBase birthRange deathRange right up offset force spawn vel color size centre a b'.split()
sites=[m for m in sites if m.group(1) in names]
print(len(sites),'sites',flush=True)
state=[1]*len(sites)
def build(st):
    out=[];p=0
    for m,s in zip(sites,st):
        out.append(body[p:m.start()]); p=m.end()
        out.append(m.group(0) if s else '%s.%s'%(m.group(1),'xyzw'[int(m.group(2))]))
    out.append(body[p:]); return head+''.join(out)
best=sc(build(state)); print('base',best,flush=True)
for it in range(int(sys.argv[2])):
    improved=False
    for i in random.sample(range(len(sites)),len(sites)):
        st=state[:]; st[i]^=1
        r=sc(build(st))
        if r<best:
            best,state=r,st; improved=True
            print('it',it,'site',i,sites[i].group(0),best,flush=True)
            open('rs_hc_best.cpp','w').write(build(state))
    if not improved: break
print('final',best)
