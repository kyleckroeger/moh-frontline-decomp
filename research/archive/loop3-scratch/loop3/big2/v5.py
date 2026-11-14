U='''                up.x = 0.0f;
                up.y = 0.0f;
                up.z = 1.0f;
                right.Cross(dir, up);
'''
V=[
('tmp',[(U,'                right.Cross(dir, CVector3(0.0f, 0.0f, 1.0f));\n')]),
('tmpd',[(U,'                right.Cross(dir, CVector3(0.0f, 0.0f, 1.0f));\n'),('                CVector3 up;\n',''),('                up.Cross(right, dir);\n','                CVector3 up;\n                up.Cross(right, dir);\n')]),
]
