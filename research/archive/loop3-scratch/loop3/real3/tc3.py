exec(open('tc.py').read().split('V={')[0])
R='            CVector3 rv;\n            rv.Scale(dir, fR);\n'
SVc='''            CVector3 sv;
            sv.d.c.x = fS * e0.d.c.x;
            sv.d.c.y = fS * e0.d.c.y;
            sv.d.c.z = fS * e0.d.c.z;
            SetSum(point, m_vertices[0], sv);
'''
TVc='''            CVector3 tv;
            tv.d.c.x = fT * e1.d.c.x;
            tv.d.c.y = fT * e1.d.c.y;
            tv.d.c.z = fT * e1.d.c.z;
            point.Add(tv);
'''
TVs='            CVector3 tv;\n            tv.Scale(e1, fT);\n            point.Add(tv);\n'
V={
'h':[(OLD,R+'            CVector3 point;\n'+SVc+TVc)],
'i':[(OLD,R+'            CVector3 point;\n'+SVc+TVs)],
'j':[(OLD,'            CVector3 point;\n'+R+SVc+TVc)],
'k':[(OLD,'            CVector3 point;\n'+R+SVc+TVs)],
}
