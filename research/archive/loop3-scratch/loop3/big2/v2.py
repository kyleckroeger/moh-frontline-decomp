MO='    float Length() const {'
C2='                up.Cross(right, dir);\n'
def m(name,body,call): return (name,[(MO,body+MO),(C2,call)])
V=[
m('rv_ctor','    CVector3 CrossV(const CVector3& b) const { return CVector3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }\n','                up = right.CrossV(dir);\n'),
m('rv_loc','    CVector3 CrossV(const CVector3& b) const { CVector3 r; r.x = y * b.z - z * b.y; r.y = z * b.x - x * b.z; r.z = x * b.y - y * b.x; return r; }\n','                up = right.CrossV(dir);\n'),
m('rv_ctorinit','    CVector3 CrossV(const CVector3& b) const { return CVector3(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }\n','                CVector3 up2 = right.CrossV(dir);\n                up = up2;\n'),
m('out_this','    void CrossTo(const CVector3& b, CVector3& r) const { r.x = y * b.z - z * b.y; r.y = z * b.x - x * b.z; r.z = x * b.y - y * b.x; }\n','                right.CrossTo(dir, up);\n'),
m('out_this2','    void CrossTo(CVector3& r, const CVector3& b) const { r.x = y * b.z - z * b.y; r.y = z * b.x - x * b.z; r.z = x * b.y - y * b.x; }\n','                right.CrossTo(up, dir);\n'),
m('self_cross','    void CrossR(const CVector3& a) { float tx = a.y * z - a.z * y; float ty = a.z * x - a.x * z; float tz = a.x * y - a.y * x; x = tx; y = ty; z = tz; }\n','                up = dir;\n                up.CrossR(right);\n'),
]
