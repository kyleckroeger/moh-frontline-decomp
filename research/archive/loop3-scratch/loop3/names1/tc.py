B=open('/home/kyle/workspace/moh-frontline/scratch/loop3/names1/tc_sub.cpp').read()
OLDB=B[B.index('            CVector3 point;\n            float svx'):B.index('            CVector3 rv;\n')]
def blk(new): return [(OLDB,new)]
V={
'base':[],
'add_tvAdd':blk(OLDB.replace('point += tv;','point.Add(tv);')),
'scale_both':blk('''            CVector3 point;
            CVector3 sv;
            sv.Scale(e0, fS);
            SetSum(point, m_vertices[0], sv);
            CVector3 tv;
            tv.Scale(e1, fT);
            point.Add(tv);
'''),
'scale_copyadd':blk('''            CVector3 point = m_vertices[0];
            CVector3 sv;
            sv.Scale(e0, fS);
            point.Add(sv);
            CVector3 tv;
            tv.Scale(e1, fT);
            point.Add(tv);
'''),
'scale_copyadd2':blk('''            CVector3 sv;
            CVector3 tv;
            sv.Scale(e0, fS);
            tv.Scale(e1, fT);
            CVector3 point = m_vertices[0];
            point.Add(sv);
            point.Add(tv);
'''),
'sv_scale_tv_locals':blk('''            CVector3 point;
            CVector3 sv;
            sv.Scale(e0, fS);
            SetSum(point, m_vertices[0], sv);
            float tvx = fT * e1.d.c.x;
            float tvy = fT * e1.d.c.y;
            float tvz = fT * e1.d.c.z;
            CVector3 tv;
            tv.d.c.x = tvx;
            tv.d.c.y = tvy;
            tv.d.c.z = tvz;
            point.Add(tv);
'''),
}
