import sys,re
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import os; os.chdir('/home/kyle/workspace/moh-frontline'); import score
B=open('scratch/loop3/late1/BEST_w1.cpp').read()
POOL=[("g_pStaticObjectArray","StaticObjectSlotView","NewStaticObjects","g_iStaticObjectPoolSize","CStaticObject"),
("g_pHierObjectArray","HierObjectSlotView","NewHierObjects","g_iHierObjectPoolSize","CHierObject"),
("g_pStaticAIObjectArray","StaticAIObjectSlotView","NewTankObjects","g_iStaticAIObjectPoolSize","CTankObject"),
("g_pAttachableObjectArray","AttachableObjectSlotView","NewAttachableObjects","g_iAttachObjectPoolSize","CAttachableObject"),
("g_pThrownObjectArray","ThrownObjectSlotView","NewThrownObjects","g_iMaxThrownObjects","CThrownObject"),
("g_pRecycledThrownObjectArray","RecycledThrownView","NewThrownObjects","g_iMaxRecycledThrownObjects","CThrownObject")]
def sc(s,tag):
    open(f'scratch/loop3/late1/o_{tag}.cpp','w').write(s)
    return score.real('config/GMFE69/staticobjfactory_helmet.json', s, 'Init__20CStaticObjectFactoryFv','800b254c',3376)
def constlate(s,direct=False,const=True):
    for k,(g,v,f,n,T) in enumerate(POOL):
        old=f"    {g} = ({v}*){f}({n});\n"
        assert old in s,old
        e=f"new {T}[{n}]" if direct else f"{f}({n})"
        s=s.replace(old,f"    {T}*{' const' if const else ''} p{k} = {e};\n    {g} = ({v}*)p{k};\n")
    return s
if __name__=='__main__':
    print('base',sc(B,'base'))
    print('cl',sc(constlate(B),'cl'))
    print('cld',sc(constlate(B,True),'cld'))
    print('ncl',sc(constlate(B,False,False),'ncl'))
