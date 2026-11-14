exec(open('tc.py').read().split('V={')[0])
R='            CVector3 rv;\n            rv.Scale(dir, fR);\n'
V={
'a':[(OLD,R+'''            CVector3 point;
            CVector3 sv;
            sv.Scale(e0, fS);
            SetSum(point, m_vertices[0], sv);
            CVector3 tv;
            tv.Scale(e1, fT);
            point.Add(tv);
''')],
'd':[(OLD,R+'''            CVector3 point;
            CVector3 tv;
            tv.Scale(e0, fS);
            SetSum(point, m_vertices[0], tv);
            tv.Scale(e1, fT);
            point.Add(tv);
''')],
'f':[(OLD,R+OLD.replace(R,''))],
'g':[(OLD,'''            CVector3 rv;
            CVector3 point;
            CVector3 sv;
            CVector3 tv;
            rv.Scale(dir, fR);
            sv.Scale(e0, fS);
            SetSum(point, m_vertices[0], sv);
            tv.Scale(e1, fT);
            point.Add(tv);
''')],
}
