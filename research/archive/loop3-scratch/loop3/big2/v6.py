A='inline CVector3 operator*(float k, const CVector3& v)'
C2='                up.Cross(right, dir);\n'
def f(name,body,call='                Cross2(up, right, dir);\n'): return (name,[(A,body+A),(C2,call)])
V=[
f('arr','inline void Cross2(CVector3& o, const CVector3& a, const CVector3& b) { const float* p = &a.x; const float* q = &b.x; o.x = p[1] * q[2] - p[2] * q[1]; o.y = p[2] * q[0] - p[0] * q[2]; o.z = p[0] * q[1] - p[1] * q[0]; }\n'),
f('arra','inline void Cross2(CVector3& o, const CVector3& a, const CVector3& b) { const float* p = &a.x; o.x = p[1] * b.z - p[2] * b.y; o.y = p[2] * b.x - p[0] * b.z; o.z = p[0] * b.y - p[1] * b.x; }\n'),
f('arrb','inline void Cross2(CVector3& o, const CVector3& a, const CVector3& b) { const float* q = &b.x; o.x = a.y * q[2] - a.z * q[1]; o.y = a.z * q[0] - a.x * q[2]; o.z = a.x * q[1] - a.y * q[0]; }\n'),
f('ptr','inline void Cross2(CVector3* o, const CVector3* a, const CVector3* b) { o->x = a->y * b->z - a->z * b->y; o->y = a->z * b->x - a->x * b->z; o->z = a->x * b->y - a->y * b->x; }\n','                Cross2(&up, &right, &dir);\n'),
]
