import itertools
O='    float Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }\n'
T=[('x','v.x'),('y','v.y'),('z','v.z')]
V=[]
for p in itertools.permutations(range(3)):
  for bits in itertools.product([0,1],repeat=3):
    terms=['%s * %s'%(T[i] if bits[i]==0 else T[i][::-1]) for i in p]
    V.append(('D'+''.join(map(str,p))+''.join(map(str,bits)),[(O,'    float Dot(const CVector3& v) const { return %s; }\n'%' + '.join(terms))]))
