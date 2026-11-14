A='inline CVector3 operator*(float k, const CVector3& v) { return CVector3(k * v.x, k * v.y, k * v.z); }\n'
R='''                    if (flat.Dot(front) < 0.0f) {
                        flat += (flat.Dot(front) * -2.0f) * front;
                        scale = dir.x * dir.x + dir.y * dir.y;
                    }
'''
H=[
'inline bool ReflectBehind(CVector3& v, const CVector3& n) { float d = v.Dot(n); if (d < 0.0f) { v += (d * -2.0f) * n; return true; } return false; }\n',
'inline bool ReflectBehind(CVector3& v, const CVector3& n) { if (v.Dot(n) < 0.0f) { v += (v.Dot(n) * -2.0f) * n; return true; } return false; }\n',
'inline bool ReflectBehind(CVector3& v, const CVector3& n) { float d = v.Dot(n); if (d < 0.0f) { float k = d * -2.0f; v += k * n; return true; } return false; }\n',
'inline bool ReflectBehind(CVector3& v, const CVector3& n) { bool r = false; if (v.Dot(n) < 0.0f) { v += (v.Dot(n) * -2.0f) * n; r = true; } return r; }\n',
]
V=[]
for i,h in enumerate(H):
    V.append(('rb%d'%i,[(A,A+h),(R,'''                    if (ReflectBehind(flat, front))
                        scale = dir.x * dir.x + dir.y * dir.y;
''')]))
