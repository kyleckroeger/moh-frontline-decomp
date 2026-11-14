import itertools, sys, os
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/scratch/game')
import fast, json
ROOT='/home/kyle/workspace/moh-frontline/'
T=json.load(open(ROOT+'config/GMFE69/objcreate_swap.json'))
src=open(ROOT+'scratch/loop3/mid2/crh0.cpp').read()
hdr_old="""    CVolBox* box;
    TriggerCoreView* core;
    BSGO_Tank* go;
    CTankObject* tank;
    CHierObject* object;
    CVolBox* world;
    core = trigger->core;"""
D={'go':'BSGO_Tank* go;','object':'CHierObject* object;','tank':'CTankObject* tank;','box':'CVolBox* box;','world':'CVolBox* world;',
   'f':'int const f = flag;','t':'TriggerObject_struct* const t = trigger;'}
mode=sys.argv[1]
names=['go','object','tank','box','world']+(['f','t'] if mode=='p' else [])
best=None
F='CreateRootHierObject__FP20TriggerObject_structi'
res=[]
bases=[('go','object','tank','box','world'),('box','go','tank','object','world'),('world','box','tank','object','go'),('go','tank','object','box','world')]
def perms():
    for b in bases:
        for i in range(6):
            for j in range(7):
                l=list(b); l.insert(i,'f'); l.insert(j,'t'); yield tuple(l)
                l=list(b); l.insert(i,'t'); l.insert(j,'f'); yield tuple(l)
for perm in perms():
    h='    TriggerCoreView* const core = trigger->core;\n'+''.join('    '+D[n]+'\n' for n in perm)
    s=src.replace(hdr_old,h.rstrip('\n'))
    if mode=='p': s=s.replace('AttachScript(trigger, flag,','AttachScript(t, f,')
    r=fast.score(T,s,[(F,)],'/tmp/claude-1000/-home-kyle-workspace-moh-frontline/a5d18f30-70be-41af-9a00-608205f1aeba/scratchpad/m2w')
    n=r[F]
    res.append((n,perm))
    if best is None or n<best[0]:
        best=(n,perm); print(n,perm,flush=True); open(ROOT+'scratch/loop3/mid2/g1/best_%s.cpp'%mode,'w').write(s)
res.sort(); print(res[:5])
