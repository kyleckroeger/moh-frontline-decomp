O='''                    CVector3 side;
                    CVector3 flat;
                    CVector3 front;
'''
V=[
('onedecl',[(O,'                    CVector3 side, flat, front;\n')]),
]
O2='''                CVector3 dir;
                CVector3 right;
                CVector3 up;
'''
V.append(('onedecl2',[(O2,'                CVector3 dir, right, up;\n')]))
