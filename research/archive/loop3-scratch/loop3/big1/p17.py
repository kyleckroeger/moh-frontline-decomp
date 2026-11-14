O='''                up.x = 0.0f;
                up.y = 0.0f;
                up.z = 1.0f;
'''
V=[
('ctor',[(O,'                up = CVector3(0.0f, 0.0f, 1.0f);\n')]),
('zyx',[(O,'''                up.z = 1.0f;
                up.y = 0.0f;
                up.x = 0.0f;
''')]),
('early',[(O,''),('                CVector3 up;\n','                CVector3 up;\n                up.x = 0.0f;\n                up.y = 0.0f;\n                up.z = 1.0f;\n')]),
('declinit',[(O,''),('                CVector3 up;\n','                CVector3 up(0.0f, 0.0f, 1.0f);\n')]),
('newvar',[(O,'                CVector3 zaxis(0.0f, 0.0f, 1.0f);\n'),('right.Cross(dir, up);','right.Cross(dir, zaxis);')]),
]
