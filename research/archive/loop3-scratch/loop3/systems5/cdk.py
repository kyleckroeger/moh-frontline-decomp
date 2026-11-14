# cdk.py: structured search for CreateDestructable (decl placement/order + knobs)
import sys,json,tempfile,subprocess,difflib,re,os,random,itertools
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
T=json.load(open('/home/kyle/workspace/moh-frontline/scratch/loop3/systems5/t.json'))
F='CreateDestructable__FP20TriggerObject_structi'
A=fast.target(F)
base=open('cd_a1.cpp').read()
HEAD=base[:base.index('BSGO_Destructable* CreateDestructable(')]
MSG=open('cd_a1.cpp').read().split('    } else {\n')[-1].split('    }\n    ForceUpdate')[0]
# variables: name -> (type, init expr)
V={'core':('TriggerCoreView*','TRIG->core'),
   'script':('BSGO_Destructable*','new BSGO_Destructable'),
   'object':('CStaticObject*','CStaticObjectFactory::AllocateStaticObject(core->objectType, -1)'),
   'box':('CVolBox*','new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox'),
   'mesh':('MeshView*','object->m_mesh'),
   'worldBox':('CVolBox*','new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox'),
   'x':('int',None),'y':('int',None),'z':('int',None),
   'ctrig':('TriggerObject_struct* const','trigger'),'cparam':('int const','param')}
def gen(st):
    order,top,k=st['order'],st['top'],st['k']
    TRIG='c_trigger' if k['ctrig'] else 'trigger'
    PAR='c_param' if k['cparam'] else 'param'
    out=['BSGO_Destructable* CreateDestructable(TriggerObject_struct* trigger, int param) {']
    def dl(v,init=True):
        t,e=V[v]; n={'ctrig':'c_trigger','cparam':'c_param'}.get(v,v)
        if init and e: return f'    {t} {n} = {e.replace("TRIG",TRIG)};'
        return f'    {t.replace(" const","")} {n};'
    for v in order:
        if v=='ctrig' and not k['ctrig']: continue
        if v=='cparam' and not k['cparam']: continue
        if v in ('ctrig','cparam'): out.append(dl(v)); continue
        if top.get(v): out.append(dl(v,False))
    def use(v):
        t,e=V[v]
        if top.get(v): return f'    {v} = {e.replace("TRIG",TRIG)};'
        return dl(v)
    out.append(use('core'))
    out.append(use('script'))
    out.append('''    if (g_pBoxList)
        g_pBoxList->m_prev = script;
    script->m_next = g_pBoxList;
    script->m_prev = 0;
    g_pBoxList = script;''')
    out.append(use('object'))
    out.append('''    CMatrix matrix;
    CVector3 position(core->position.x, core->position.y, core->position.z);
    core->rotation.GetMatrix(matrix);
    matrix.SetPos(position);
    object->SetTMLocalToWorld(matrix);''')
    out.append(use('box'))
    out.append(use('mesh'))
    out.append('''    CVector3 right = mesh->m_data->m_box->m_right;
    CVector3 front = mesh->m_data->m_box->m_front;
    CVector3 up = mesh->m_data->m_box->m_up;
    box->SetWidth(2.0f * right.Length());
    box->SetHeight(2.0f * up.Length());
    box->SetDepth(2.0f * front.Length());
    right.Normalize();
    front.Normalize();
    up.Normalize();
    box->SetBasis(right, front, up);
    box->SetCenter(mesh->m_data->m_box->m_center);
    object->SetLocalBoundingVolume((ISceneNode::EVolumeType)3, box);
    object->SetLocalBoundingVolume((ISceneNode::EVolumeType)0, box);''')
    out.append(use('worldBox'))
    out.append('''    *worldBox = *box;
    object->SetWorldBoundingVolume((ISceneNode::EVolumeType)3, worldBox);
    object->SetWorldBoundingVolume((ISceneNode::EVolumeType)0, worldBox);
    object->Function124();
    g_scene.Add(*(ISceneNode*)object);
    script->m_object28 = object;
    script->m_value30 = 0;
    script->m_value2c = 0;
    script->m_value38 = 0;
    script->m_value34 = 0;
    script->m_health = core->health;
    if (core->scriptName) {''')
    out.append(f'''        CreateBSObject(core->scriptName, &script->m_object, script, {TRIG}, &script->m_data);
        object->SetScriptObject(script->m_object);
        BSObjectTriggerEvent(script->m_object, 44, (void*){PAR}, 0, false);
    }} else {{''')
    xs={'x':'        x = position.d.v[0];','y':'        y = position.d.v[1];','z':'        z = position.d.v[2];'}
    for v in ('x','y','z'):
        if not top.get(v): xs[v]='        int '+xs[v][8:]
    out += [xs[v] for v in st['xo']]
    out.append(MSG.rstrip('\n'))
    out.append('''    }
    ForceUpdateProximityTriggerStatus(script, true);
    return script;
}
''')
    return HEAD+'\n'.join(out)
def score(src):
    d=tempfile.mkdtemp(dir='/tmp/claude-1000') if False else tempfile.mkdtemp()
    obj,log=fast.compile_object(T,src,d)
    if not obj: return 9999
    mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout).get(F)
    subprocess.run(['rm','-rf',d])
    if not mine: return 9999
    sm=difflib.SequenceMatcher(None,A,mine,autojunk=False)
    return sum((i2-i1)+(j2-j1) for op,i1,i2,j1,j2 in sm.get_opcodes() if op!='equal')
VARS=['ctrig','cparam','z','y','x','core','script','object','box','mesh','worldBox']
st={'order':VARS[:],'top':{'x':1,'y':1,'z':1},'k':{'ctrig':1,'cparam':1},'xo':['x','y','z']}
if __name__=='__main__':
    open('cdk_start.cpp','w').write(gen(st))
    best=score(gen(st)); print('start',best,flush=True)
    rng=random.Random(int(sys.argv[2]) if len(sys.argv)>2 else 1)
    import copy
    for it in range(int(sys.argv[1])):
        s2=copy.deepcopy(st)
        for _ in range(rng.choice([1,1,2,3])):
            m=rng.randrange(5)
            if m<2:
                i,j=rng.randrange(len(VARS)),rng.randrange(len(VARS)); o=s2['order']; o[i],o[j]=o[j],o[i]
            elif m==2:
                v=rng.choice(['core','script','object','box','mesh','worldBox','x','y','z']); s2['top'][v]=1-s2['top'].get(v,0)
            elif m==3:
                kk=rng.choice(list(s2['k'])); s2['k'][kk]^=1
            else: rng.shuffle(s2['xo'])
        sc=score(gen(s2))
        if sc<=best:
            if sc<best: print(it,sc,s2,flush=True); open('cdk_best.cpp','w').write(gen(s2))
            best=sc; st=s2
    print('best',best,st)
