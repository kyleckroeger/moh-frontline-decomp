A='inline CVector3 operator*(float k, const CVector3& v) { return CVector3(k * v.x, k * v.y, k * v.z); }\n'
M='inline float MulV(float a, float b) { return a * b; }\n'
V=[
('mulv',[(A,M+'inline CVector3 operator*(float k, const CVector3& v) { return CVector3(MulV(k, v.x), MulV(k, v.y), MulV(k, v.z)); }\n')]),
('mulvx',[(A,M+'inline CVector3 operator*(float k, const CVector3& v) { return CVector3(MulV(k, v.x), k * v.y, k * v.z); }\n')]),
('dotmulv',[(A,M+A),('    float Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }\n','    float Dot(const CVector3& v) const;\n'),('#define X_ZERO','inline float CVector3::Dot(const CVector3& v) const { return MulV(x, v.x) + MulV(y, v.y) + MulV(z, v.z); }\n#define X_ZERO')]),
]
