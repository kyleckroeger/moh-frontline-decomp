import sys,itertools
sys.path.insert(0,'scratch/loop3/big1'); import sc
base=open('scratch/loop3/big1/h1.cpp').read()
OLD=base[base.index('                short* angles = m_boneAngles[bone];'):base.index('                if (m_value43c8 < m_value43c4)')]
TG=['(int)(2670176.8f * (ax - 0.34906584f)) >> 8','(int)(2670176.8f * az) >> 8','(int)(2670176.8f * ay) >> 8']
C='xyz'
def gen(Tt,Xt,diff,prod,angpos,xpos,inter):
    L=[]; ang='short* angles = m_boneAngles[bone];'
    if angpos==0: L.append(ang)
    tg=['%s d%s = %s;'%(Tt,C[i],('(short)(%s)'%TG[i]) if Tt=='int' else TG[i]) for i in range(3)]
    xs=['%s %s = angles[%d];'%(Xt,C[i],i) for i in range(3)]
    if xpos==0: 
        if angpos==1: L.append(ang)
        L+=xs; L+=tg
    else:
        L+=tg
        if angpos==1: L.append(ang)
        L+=xs
    def dst(i):
        c=C[i]
        if diff==0: d=['d%s = (short)(d%s - %s);'%(c,c,c) if Tt=='int' else 'd%s = d%s - %s;'%(c,c,c)]; dv='d'+c
        elif diff==1: d=['short e%s = d%s - %s;'%(c,c,c)]; dv='e'+c
        else: d=[]; dv='(short)(d%s - %s)'%(c,c)
        if prod==0: p=['short s%s = %s * t;'%(c,dv),'angles[%d] = s%s + %s;'%(i,c,c)]
        elif prod==1: p=['angles[%d] = (short)(%s * t) + %s;'%(i,dv,c)]
        else: p=['int s%s = (short)(%s * t);'%(c,dv),'angles[%d] = s%s + %s;'%(i,c,c)]
        return d,p
    parts=[dst(i) for i in range(3)]
    if inter==0:
        for d,p in parts: L+=d
        for d,p in parts: L+=p[:-1]
        for d,p in parts: L+=p[-1:]
    else:
        for d,p in parts: L+=d+p
    return ''.join('                '+l+'\n' for l in L)
best=[]
for k in itertools.product(['int','short'],['int','short'],[0,1,2],[0,1,2],[0,1],[0,1],[0,1]):
    t=base.replace(OLD,gen(*k)); m=sc.asm(t)
    if not m: print(k,'FAIL'); continue
    r=sc.regions(m); print(k,*r,flush=True)
