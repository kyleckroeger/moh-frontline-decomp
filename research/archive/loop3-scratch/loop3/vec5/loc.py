import sys,itertools
sys.path.insert(0,'.'); import sc
S=sc.UBB()
s=open(sys.argv[1]).read()
anc='        GetSystemAcceleration(sa);\n'
a=s.index(anc)+len(anc); b=s.index('        CVector3 lo = start;')
names=[('sv',0),('sv',1),('sv',2),('sa',0),('sa',1),('sa',2)]
res=[]
for c in itertools.product([0,1],repeat=6):
    decl=''; mid=s[a:b]
    for (v,i),on in zip(names,c):
        if on:
            n='%s%s'%(v,'xyz'[i]); decl+='        float %s = %s.d.v[%d];\n'%(n,v,i); mid=mid.replace('%s.d.v[%d]'%(v,i),n)
    src=s[:a]+decl+mid+s[b:]
    r=S(src); res.append((r,c)); 
    if r[1]<int(sys.argv[2]): print(c,r,flush=True)
res.sort(); print(res[:5])
