D='''                CVector3 dir;
                CVector3 right;
                CVector3 up;
'''
V=[
('arr',[(D,'''                CVector3 axes[3];
                CVector3& up = axes[0];
                CVector3& right = axes[1];
                CVector3& dir = axes[2];
''')]),
('struct',[(D,'''                struct { CVector3 up; CVector3 right; CVector3 dir; } b;
                CVector3& up = b.up;
                CVector3& right = b.right;
                CVector3& dir = b.dir;
''')]),
('arr_rev',[(D,'''                CVector3 axes[3];
                CVector3& dir = axes[2];
                CVector3& right = axes[1];
                CVector3& up = axes[0];
''')]),
]
