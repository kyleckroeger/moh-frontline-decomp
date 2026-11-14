import itertools
A='inline CVector3 operator*(float k, const CVector3& v)'
C2='                up.Cross(right, dir);\n                up.Normalize();\n'
V=[]
T=['x * x','y * y','z * z']
for p in itertools.permutations(range(3)):
    for form in range(2):
        s=' + '.join(T[i] for i in p)
        if form==0: s='%s'%s
        else: s='%s + (%s)'%(T[p[0]],' + '.join(T[i] for i in p[1:]))
        body='inline void Norm2(CVector3& v) { float x = v.x; }\n'
        body='''struct NormHelper {};
inline void Norm2(CVector3& v) {
    float length = sqrtf(%s);
    if (length != 0.0f) { v.x *= 1.0f / length; v.y *= 1.0f / length; v.z *= 1.0f / length; }
}
'''%s.replace('x * x','v.x * v.x').replace('y * y','v.y * v.y').replace('z * z','v.z * v.z')
        V.append(('n'+''.join(map(str,p))+str(form),[(A,body+A),(C2,'                up.Cross(right, dir);\n                Norm2(up);\n')]))
