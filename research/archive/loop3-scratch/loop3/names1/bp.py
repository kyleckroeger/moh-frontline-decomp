B=open('/home/kyle/workspace/moh-frontline/scratch/loop3/names1/bp_base.cpp').read()
s=B.index('    CVector3 a;\n    CVector3 b;\n    CVector3 c;\n'); e=B.index('    {\n        float distance = plane.Project2(corner, point);')
OLD=B[s:e]
ADD=('    void Negate(const CVector3& v) {','''    void Sub(const CVector3& a, const CVector3& b) { d.c.x = a.d.c.x - b.d.c.x; d.c.y = a.d.c.y - b.d.c.y; d.c.z = a.d.c.z - b.d.c.z; }
    void Add(const CVector3& o) { d.c.x += o.d.c.x; d.c.y += o.d.c.y; d.c.z += o.d.c.z; }
    void Negate(const CVector3& v) {''')
N1='''    SClsnContact contact;
    CVector3 point;
    CVector3 a;
    CVector3 b;
    CVector3 c;
    a.Scale(m_axes[0], m_half[0]);
    b.Scale(m_axes[1], m_half[1]);
    c.Scale(m_axes[2], m_half[2]);
    CVector3 corner = m_center;
    corner -= a;
    corner -= b;
    corner -= c;
    contact.m_vector0.Negate(plane.m_normal);
    contact.m_separation = plane.Project2(corner, point);
    contact.m_vector1 = point;
    corner += a;
    corner += a;
'''
V={'base':[], 'nat1':[ADD,(OLD,N1)],
'nat2':[ADD,(OLD,N1.replace('corner -= a;','corner.Sub(corner, a);').replace('corner -= b;','corner.Sub(corner, b);').replace('corner -= c;','corner.Sub(corner, c);').replace('corner += a;','corner.Add(a);'))],
'nat3':[ADD,(OLD,N1.replace('''    SClsnContact contact;
    CVector3 point;
''','').replace('    CVector3 c;\n','    CVector3 c;\n    SClsnContact contact;\n    CVector3 point;\n'))],
}
