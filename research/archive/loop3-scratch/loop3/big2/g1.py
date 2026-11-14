import sys,itertools
sys.path.insert(0,'scratch/loop3/big1'); import sc
base=open('scratch/loop3/big2/base.cpp').read()
A='inline CVector3 operator*(float k, const CVector3& v)'
S=['%s.x = a.y * b.z - a.z * b.y;','%s.y = a.z * b.x - a.x * b.z;','%s.z = a.x * b.y - a.y * b.x;']
D='''                CVector3 dir;
                CVector3 right;
                CVector3 up;
'''
N=['dir','right','up']
res={}
for order,form,dord in itertools.product(itertools.permutations(range(3)),range(3),itertools.permutations(range(3))):
    if form==0: body='inline void Cross2(CVector3& o, const CVector3& a, const CVector3& b) { %s }\n'%' '.join(S[i]%'o' for i in order)
    elif form==1: body='inline void Cross2(CVector3& o, const CVector3& a, const CVector3& b) { CVector3 r; %s o.x = r.x; o.y = r.y; o.z = r.z; }\n'%' '.join(S[i]%'r' for i in order)
    else: body='inline void Cross2(CVector3& o, const CVector3& a, const CVector3& b) { float rx, ry, rz; %s o.x = rx; o.y = ry; o.z = rz; }\n'%' '.join((S[i]%'r').replace('r.','r') for i in order)
    t=base.replace(A,body+A).replace('                up.Cross(right, dir);\n','                Cross2(up, right, dir);\n')
    t=t.replace(D,''.join('                CVector3 %s;\n'%N[i] for i in dord))
    m=sc.asm(t)
    if not m: print('FAIL',order,form,dord); continue
    r=sc.regions(m); print(order,form,dord,*r,flush=True)
