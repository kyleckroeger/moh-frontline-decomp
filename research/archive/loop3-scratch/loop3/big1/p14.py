O1='                        scale = dir.x * dir.x + dir.y * dir.y;\n'
A='    float Length() const { return sqrtf(x * x + y * y + z * z); }\n'
V=[
('inl',[(A,A+'    float LengthXYSq() const { return x * x + y * y; }\n'),(O1,'                        scale = dir.LengthXYSq();\n')]),
('loc',[(O1,'                        float hx = dir.x;\n                        float hy = dir.y;\n                        scale = hx * hx + hy * hy;\n')]),
('sq',[(A,A+'    \n'),(O1,'                        scale = Sq(dir.x) + Sq(dir.y);\n'),('#define X_ZERO','static inline float Sq(float v) { return v * v; }\n#define X_ZERO')]),
]
