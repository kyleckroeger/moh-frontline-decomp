O1='''                        flat += (flat.Dot(front) * -2.0f) * front;
'''
A='inline CVector3 operator*(float k, const CVector3& v) { return CVector3(k * v.x, k * v.y, k * v.z); }\n'
R=[
'inline void Reflect(CVector3& v, const CVector3& n) { v += (v.Dot(n) * -2.0f) * n; }\n',
'inline void Reflect(CVector3& v, const CVector3& n) { float k = v.Dot(n) * -2.0f; v += k * n; }\n',
'inline void Reflect(CVector3& v, const CVector3& n) { v += (-2.0f * v.Dot(n)) * n; }\n',
'inline void Reflect(CVector3& v, const CVector3& n) { v -= (2.0f * v.Dot(n)) * n; }\n',
]
V=[('R%d'%i,[(A,A+r),(O1,'                        Reflect(flat, front);\n')]) for i,r in enumerate(R)]
