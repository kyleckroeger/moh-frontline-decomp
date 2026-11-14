import itertools
O='inline CVector3 operator*(float k, const CVector3& v) { return CVector3(k * v.x, k * v.y, k * v.z); }\n'
V=[]
C='xyz'
for p in itertools.permutations(range(3)):
    body=' '.join('r.%s = k * v.%s;'%(C[i],C[i]) for i in p)
    V.append(('m'+''.join(map(str,p)),[(O,'inline CVector3 operator*(float k, const CVector3& v) { CVector3 r; %s return r; }\n'%body)]))
V.append(('vk',[(O,'inline CVector3 operator*(float k, const CVector3& v) { return CVector3(v.x * k, v.y * k, v.z * k); }\n')]))
V.append(('copy',[(O,'inline CVector3 operator*(float k, const CVector3& v) { CVector3 r(v); r *= k; return r; }\n')]))
