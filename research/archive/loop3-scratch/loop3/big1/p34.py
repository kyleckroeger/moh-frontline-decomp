O='    float Dot(const CVector3& v) const { return x * v.x + y * v.y + z * v.z; }\n'
A='inline CVector3 operator*(float k, const CVector3& v) { return CVector3(k * v.x, k * v.y, k * v.z); }\n'
R='''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
'''
V=[
('freedot',[(A,A+'inline float Dot(const CVector3& a, const CVector3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }\n'),(R,'''                    if (Dot(flat, front) < 0.0f) {
                        flat += (Dot(flat, front) * -2.0f) * front;
''')]),
('dotvar',[(O,'    float Dot(const CVector3& v) const { float r = x * v.x + y * v.y + z * v.z; return r; }\n')]),
('dotacc',[(O,'    float Dot(const CVector3& v) const { float r = x * v.x; r += y * v.y; r += z * v.z; return r; }\n')]),
]
