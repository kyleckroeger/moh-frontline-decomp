OLD='''            CVector3 point;
            float svx = fS * e0.d.c.x;
            float svy = fS * e0.d.c.y;
            float svz = fS * e0.d.c.z;
            CVector3 sv;
            sv.d.c.x = svx;
            sv.d.c.y = svy;
            sv.d.c.z = svz;
            SetSum(point, m_vertices[0], sv);
            float tvx = fT * e1.d.c.x;
            float tvy = fT * e1.d.c.y;
            float tvz = fT * e1.d.c.z;
            CVector3 tv;
            tv.d.c.x = tvx;
            tv.d.c.y = tvy;
            tv.d.c.z = tvz;
            point.Add(tv);
            CVector3 rv;
            rv.Scale(dir, fR);
'''
V={
'a':[(OLD,'''            CVector3 point;
            CVector3 sv;
            sv.Scale(e0, fS);
            SetSum(point, m_vertices[0], sv);
            CVector3 tv;
            tv.Scale(e1, fT);
            point.Add(tv);
            CVector3 rv;
            rv.Scale(dir, fR);
''')],
'b':[(OLD,'''            CVector3 point;
            point.Scale(e0, fS);
            point.Add(m_vertices[0]);
            CVector3 tv;
            tv.Scale(e1, fT);
            point.Add(tv);
            CVector3 rv;
            rv.Scale(dir, fR);
''')],
'c':[(OLD,'''            CVector3 point = m_vertices[0];
            CVector3 sv;
            sv.Scale(e0, fS);
            point.Add(sv);
            CVector3 tv;
            tv.Scale(e1, fT);
            point.Add(tv);
            CVector3 rv;
            rv.Scale(dir, fR);
''')],
'd':[(OLD,'''            CVector3 point;
            CVector3 tv;
            tv.Scale(e0, fS);
            SetSum(point, m_vertices[0], tv);
            tv.Scale(e1, fT);
            point.Add(tv);
            CVector3 rv;
            rv.Scale(dir, fR);
''')],
'e':[(OLD,'''            CVector3 point;
            CVector3 sv;
            CVector3 tv;
            CVector3 rv;
            sv.Scale(e0, fS);
            SetSum(point, m_vertices[0], sv);
            tv.Scale(e1, fT);
            point.Add(tv);
            rv.Scale(dir, fR);
''')],
}
