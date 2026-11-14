from v import *
import itertools,sys
NB="    CVolBox* const box = new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;\n"
COREI="    TriggerCoreView* core = c_trigger->core;\n"
OBJI="    CStaticObject* object = CStaticObjectFactory::AllocateStaticObject(core->objectType, -1);\n"
items={'p':"    int const c_param = param;\n",'t':"    TriggerObject_struct* const c_trigger = trigger;\n",
 'xyz':"    int y;\n    int x;\n    int z;\n",'c':"    TriggerCoreView* core;\n",'o':"    CStaticObject* object;\n"}
res=[]
for perm in itertools.permutations('ptxco'):
    key={'x':'xyz'}
    if perm.index('t')>perm.index('c') and False: pass
    h=''.join(items[key.get(k,k)] for k in perm)+"    core = c_trigger->core;\n"
    s=base.replace(H_OLD,h).replace(COREI,"").replace(OBJI,"    object = CStaticObjectFactory::AllocateStaticObject(core->objectType, -1);\n").replace(B_OLD,NB)
    r=score(s,'p8'); n=r.split()[0]
    res.append((int(n) if n.isdigit() else 9999,''.join(perm),r[:150])); 
res.sort()
for r in res[:12]: print(r)
