import sys,itertools
sys.path.insert(0,'scratch/loop3/big1'); sys.path.insert(0,'scratch/loop3/big2'); import sc,who2
base=open('scratch/loop3/big2/base.cpp').read()
TOP='    CMatrix boneWorld;\n'
def go(t,name):
    m=sc.asm(t)
    if not m: print(name,'FAIL'); return
    tb=who2.tab(m); print(name,sc.regions(m)[0],sc.regions(m)[2][:40],tb[-1] if tb else '',flush=True)
for bp,tp,sp,ep in itertools.product(range(3),range(3),range(2),range(2)):
    t=base
    pre=[]
    if bp:
        t=t.replace('    float blink = 0.0f;\n','    blink = 0.0f;\n'); pre.append('float blink;')
    if tp:
        t=t.replace('                float t = (float)','                t = (float)')
        if tp==1: pre.append('float t;')
        else: t=t.replace('                CMatrix result;\n','                float t;\n                CMatrix result;\n')
    if sp:
        t=t.replace('                    float scale = -1.0f;\n','                    scale = -1.0f;\n'); pre.append('float scale;')
    if ep:
        t=t.replace('                    float e = front','                    e = front'); pre.append('float e;')
    for perm in (itertools.permutations(pre) if len(pre)<=3 else [pre]):
        tt=t.replace(TOP,''.join('    %s\n'%p for p in perm)+TOP) if bp!=2 else t.replace(TOP,TOP+''.join('    %s\n'%p for p in perm))
        go(tt,'%d%d%d%d %s'%(bp,tp,sp,ep,'/'.join(perm)))
