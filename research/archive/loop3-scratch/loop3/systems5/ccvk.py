import sys,random,tempfile,subprocess,difflib,json,itertools
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
T=json.load(open('/home/kyle/workspace/moh-frontline/scratch/loop3/systems5/t.json'))
F='CreateCollisionVolume__FP20TriggerObject_structi'; A=fast.target(F)
s=open('ccv_best.cpp').read()
i=s.index('BSGO_Destructable* CreateCollisionVolume('); head,body=s[:i],s[i:]
R=[('    TriggerCoreView* core = trigger->GetCore();','    core = c_trigger->GetCore();'),
   ('    CCollisionVolume* node = (CCollisionVolume*)','    node = (CCollisionVolume*)'),
   ('    IVolume* volume = 0;','    volume = 0;'),
   ('        int z, y, x;\n',''),
   ('CreateBSObject(core->scriptName, &object->m_object, object, trigger,','CreateBSObject(core->scriptName, &object->m_object, object, c_trigger,'),
   ('(void*)param','(void*)c_param')]
for a,b in R: assert a in body,a; body=body.replace(a,b)
H=['    TriggerObject_struct* const c_trigger = trigger;','    int const c_param = param;','    int z;','    int y;','    int x;','    TriggerCoreView* core;','    CCollisionVolume* node;','    IVolume* volume;']
L0='BSGO_Destructable* CreateCollisionVolume(TriggerObject_struct* trigger, int param) {\n'
def gen(p): return head+body.replace(L0,L0+'\n'.join(H[k] for k in p)+'\n')
def score(src):
    d=tempfile.mkdtemp(); obj,log=fast.compile_object(T,src,d)
    if not obj: subprocess.run(['rm','-rf',d]); return 9999
    mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout).get(F); subprocess.run(['rm','-rf',d])
    sm=difflib.SequenceMatcher(None,A,mine,autojunk=False)
    return sum((i2-i1)+(j2-j1) for op,i1,i2,j1,j2 in sm.get_opcodes() if op!='equal')
p=list(range(len(H))); best=score(gen(p)); print('start',best,flush=True)
open('ccvk_start.cpp','w').write(gen(p))
rng=random.Random(int(sys.argv[2]))
for it in range(int(sys.argv[1])):
    q=p[:]
    for _ in range(rng.choice([1,1,2])):
        a,b=rng.randrange(len(q)),rng.randrange(len(q)); q.insert(b,q.pop(a))
    sc=score(gen(q))
    if sc<=best:
        if sc<best: print(it,sc,q,flush=True); open('ccvk_best.cpp','w').write(gen(q))
        best,p=sc,q
print('best',best,p)
