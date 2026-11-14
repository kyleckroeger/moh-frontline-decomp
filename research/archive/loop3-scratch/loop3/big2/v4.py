MO='    float Length() const {'
C2='                up.Cross(right, dir);\n'
C1='                right.Cross(dir, up);\n'
SET='    void Set(float ax, float ay, float az) { x = ax; y = ay; z = az; }\n'
CS='    void CrossS(const CVector3& a, const CVector3& b) { Set(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }\n'
V=[
('set2',[(MO,SET+CS+MO),(C2,'                up.CrossS(right, dir);\n')]),
('set12',[(MO,SET+CS+MO),(C2,'                up.CrossS(right, dir);\n'),(C1,'                right.CrossS(dir, up);\n')]),
('setcall',[(MO,SET+MO),(C2,'                up.Set(right.y * dir.z - right.z * dir.y, right.z * dir.x - right.x * dir.z, right.x * dir.y - right.y * dir.x);\n')]),
]
