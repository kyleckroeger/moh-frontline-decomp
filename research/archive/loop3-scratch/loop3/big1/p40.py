O='''                up.x = 0.0f;
                up.y = 0.0f;
                up.z = 1.0f;
                right.Cross(dir, up);
                right.Normalize();
                up.Cross(right, dir);
                up.Normalize();
'''
A='#define X_ZERO'
V=[
('basis',[(A,'static inline void MakeBasis(const CVector3& dir, CVector3& right, CVector3& up) {\n    up.x = 0.0f;\n    up.y = 0.0f;\n    up.z = 1.0f;\n    right.Cross(dir, up);\n    right.Normalize();\n    up.Cross(right, dir);\n    up.Normalize();\n}\n'+A),(O,'                MakeBasis(dir, right, up);\n')]),
('basis2',[(A,'static inline void MakeUp(const CVector3& dir, const CVector3& right, CVector3& up) {\n    up.Cross(right, dir);\n    up.Normalize();\n}\n'+A),('                up.Cross(right, dir);\n                up.Normalize();\n','                MakeUp(dir, right, up);\n')]),
('basis3',[(A,'static inline void Orthogonal(CVector3& up, const CVector3& right, const CVector3& dir) {\n    right.Normalize();\n}\n'+A)]),
]
V=V[:2]
