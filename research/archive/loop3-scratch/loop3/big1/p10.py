O='                up.Cross(right, dir);\n'
V=[
('expl',[(O,'''                up.x = right.y * dir.z - right.z * dir.y;
                up.y = right.z * dir.x - right.x * dir.z;
                up.z = right.x * dir.y - right.y * dir.x;
''')]),
('expl_yzx',[(O,'''                up.y = right.z * dir.x - right.x * dir.z;
                up.z = right.x * dir.y - right.y * dir.x;
                up.x = right.y * dir.z - right.z * dir.y;
''')]),
('expl_tmp',[(O,'''                float ux = right.y * dir.z - right.z * dir.y;
                float uy = right.z * dir.x - right.x * dir.z;
                float uz = right.x * dir.y - right.y * dir.x;
                up.x = ux;
                up.y = uy;
                up.z = uz;
''')]),
('copies',[(O,'''                CVector3 r = right;
                CVector3 f = dir;
                up.Cross(r, f);
''')]),
]
