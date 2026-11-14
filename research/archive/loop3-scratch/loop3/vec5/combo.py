import sys,itertools
sys.path.insert(0,'.'); import sc
S=sc.UBB()
s=open('n1.cpp').read()
a=s.index('        CVector3 start;\n'); b=s.index('        CVector3 lo = start;')
X='xyz'
def g(i,vec):
    if i==0:
        return ('        CVector3 start;\n        start.Scale(sv, t);\n        CVector3 sacc;\n        sacc.Scale(sa, t2);\n        start += sacc;\n' if vec else
            '        CVector3 start;\n'+''.join('        start.d.v[%d] = t * sv.d.v[%d];\n'%(j,j) for j in range(3))+''.join('        start.d.v[%d] += Mul2(t2, sa.d.v[%d]);\n'%(j,j) for j in range(3)))
    if i==1:
        return ('        CVector3 vel;\n        vel.Scale(sa, k);\n        vel += sv;\n'+''.join('        float v%s = vel.d.v[%d];\n'%(X[j],j) for j in range(3)) if vec else
            ''.join('        float v%s = k * sa.d.v[%d];\n'%(X[j],j) for j in range(3))+''.join('        v%s += sv.d.v[%d];\n'%(X[j],j) for j in range(3)))
    if i==2:
        return ('        CVector3 end;\n        end.Scale(CVector3(vx, vy, vz), age);\n        end += start;\n' if vec else
            ''.join('        float e%s = age * v%s;\n'%(X[j],X[j]) for j in range(3))+''.join('        e%s += start.d.v[%d];\n'%(X[j],j) for j in range(3)))
    if i==3:
        return ('        CVector3 eacc;\n        eacc.Scale(sa, a2);\n'+(' end += eacc;\n'+''.join('        float e%s = end.d.v[%d];\n'%(X[j],j) for j in range(3)) if E else ''.join('        e%s += eacc.d.v[%d];\n'%(X[j],j) for j in range(3))) if vec else
            (''.join('        float e%s = end.d.v[%d];\n'%(X[j],j) for j in range(3)) if E else '')+''.join('        e%s += Mul2(a2, sa.d.v[%d]);\n'%(X[j],j) for j in range(3)))
for c in itertools.product([0,1],repeat=4):
    E=c[2]
    blk=''.join(g(i,c[i]) for i in range(4))
    src=s[:a]+blk+s[b:]
    open('cmb_%d%d%d%d.cpp'%c,'w').write(src)
    print(c,S(src),flush=True)
