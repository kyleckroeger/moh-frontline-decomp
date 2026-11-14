A='inline CVector3 operator*(float k, const CVector3& v) { return CVector3(k * v.x, k * v.y, k * v.z); }\n'
O='                        flat += (flat.Dot(front) * -2.0f) * front;\n'
V=[
('plus',[(A,A+'inline CVector3 operator+(const CVector3& a, const CVector3& b) { return CVector3(a.x + b.x, a.y + b.y, a.z + b.z); }\n'),(O,'                        flat = flat + (flat.Dot(front) * -2.0f) * front;\n')]),
('addscaled',[(A,A+'inline void AddScaled(CVector3& a, const CVector3& b, float k) { a.x += k * b.x; a.y += k * b.y; a.z += k * b.z; }\n'),(O,'                        AddScaled(flat, front, flat.Dot(front) * -2.0f);\n')]),
('addscaled2',[(A,A+'inline void AddScaled(CVector3& a, float k, const CVector3& b) { a.x = a.x + k * b.x; a.y = a.y + k * b.y; a.z = a.z + k * b.z; }\n'),(O,'                        AddScaled(flat, flat.Dot(front) * -2.0f, front);\n')]),
]
