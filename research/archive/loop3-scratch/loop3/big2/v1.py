A='inline CVector3 operator*(float k, const CVector3& v)'
RS='''static inline void Cross(CVector3 &out, const CVector3 &a, const CVector3 &b) {
    CVector3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    out.x = result.x;
    out.y = result.y;
    out.z = result.z;
}
'''
RSN='''static inline void Normalize(CVector3 &v) {
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length != 0.0f) {
        float inverse = 1.0f / length;
        v.x *= inverse;
        v.y *= inverse;
        v.z *= inverse;
    }
}
'''
C1='                right.Cross(dir, up);\n'
C2='                up.Cross(right, dir);\n'
MT='''    void CrossT(const CVector3& a, const CVector3& b) {
        CVector3 result;
        result.x = a.y * b.z - a.z * b.y;
        result.y = a.z * b.x - a.x * b.z;
        result.z = a.x * b.y - a.y * b.x;
        x = result.x;
        y = result.y;
        z = result.z;
    }
'''
MO='    float Length() const {'
V=[
('rs_both',[(A,RS+A),(C1,'                Cross(right, dir, up);\n'),(C2,'                Cross(up, right, dir);\n')]),
('rs_2',[(A,RS+A),(C2,'                Cross(up, right, dir);\n')]),
('rs_1',[(A,RS+A),(C1,'                Cross(right, dir, up);\n')]),
('mt_2',[(MO,MT+MO),(C2,'                up.CrossT(right, dir);\n')]),
('mt_both',[(MO,MT+MO),(C2,'                up.CrossT(right, dir);\n'),(C1,'                right.CrossT(dir, up);\n')]),
('rs_both_n',[(A,RS+RSN+A),(C1,'                Cross(right, dir, up);\n'),(C2,'                Cross(up, right, dir);\n'),('                right.Normalize();\n','                Normalize(right);\n'),('                up.Normalize();\n                result','                Normalize(up);\n                result')]),
]
