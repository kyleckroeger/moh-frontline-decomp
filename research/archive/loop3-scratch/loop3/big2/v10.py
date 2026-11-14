MO='    float Length() const {'
S='                        scale = dir.x * dir.x + dir.y * dir.y;\n'
A='inline CVector3 operator*(float k, const CVector3& v)'
V=[
('mem',[(MO,'    float LengthXYSq() const { return x * x + y * y; }\n'+MO),(S,'                        scale = dir.LengthXYSq();\n')]),
('free',[(A,'inline float LenXY2(const CVector3& v) { return v.x * v.x + v.y * v.y; }\n'+A),(S,'                        scale = LenXY2(dir);\n')]),
('free2',[(A,'inline float Sq2(float a, float b) { return a * a + b * b; }\n'+A),(S,'                        scale = Sq2(dir.x, dir.y);\n')]),
('freeval',[(A,'inline float LenXY2(CVector3 v) { return v.x * v.x + v.y * v.y; }\n'+A),(S,'                        scale = LenXY2(dir);\n')]),
('memxy',[(MO,'    float Dot2(const CVector3& v) const { return x * v.x + y * v.y; }\n'+MO),(S,'                        scale = dir.Dot2(dir);\n')]),
]
