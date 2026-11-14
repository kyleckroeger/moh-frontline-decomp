B=open('cap_base.cpp').read()
s=B.index('void CVolCapsule::GetExtents'); e=B.index('bool CVolCapsule::TestVisibility')
OLD=B[s:e]
def G(body): return [(OLD,'void CVolCapsule::GetExtents(CVector3& min, CVector3& max) const {\n'+body+'}\n\n')]
SEL='''    min.x = lo.x < hi.x ? lo.x : hi.x;
    min.y = lo.y < hi.y ? lo.y : hi.y;
    min.z = lo.z < hi.z ? lo.z : hi.z;
'''
SEL2='''    max.x = lo.x > hi.x ? lo.x : hi.x;
    max.y = lo.y > hi.y ? lo.y : hi.y;
    max.z = lo.z > hi.z ? lo.z : hi.z;
'''
V={
'base':[],
'sub_rv':G('''    float radius = m_radius + CONTACT_EPSILON;
    CVector3 r(radius, radius, radius);
    {
    CVector3 lo; lo.Sub(m_axis.GetStart(), r);
    CVector3 hi; hi.Sub(m_axis.GetEnd(), r);
'''+SEL+'''    }
    {
    CVector3 lo = m_axis.GetStart(); lo.Add(r);
    CVector3 hi = m_axis.GetEnd(); hi.Add(r);
'''+SEL2+'''    }
'''),
'sub_rv2':G('''    float radius = m_radius + CONTACT_EPSILON;
    CVector3 r(radius, radius, radius);
    {
    CVector3 lo; lo.Sub(m_axis.GetStart(), r);
    CVector3 hi; hi.Sub(m_axis.GetEnd(), r);
'''+SEL+'''    }
    {
    CVector3 lo = m_axis.GetStart(); lo += r;
    CVector3 hi = m_axis.GetEnd(); hi += r;
'''+SEL2+'''    }
'''),
'minus_eq':G('''    float radius = m_radius + CONTACT_EPSILON;
    CVector3 r(radius, radius, radius);
    {
    CVector3 lo = m_axis.GetStart(); lo -= r;
    CVector3 hi = m_axis.GetEnd(); hi -= r;
'''+SEL+'''    }
    {
    CVector3 lo = m_axis.GetStart(); lo += r;
    CVector3 hi = m_axis.GetEnd(); hi += r;
'''+SEL2+'''    }
'''),
}
MINB='''    float radius = m_radius + CONTACT_EPSILON;
    CVector3 r(radius, radius, radius);
    {
    CVector3 lo; lo.Sub(m_axis.GetStart(), r);
    CVector3 hi; hi.Sub(m_axis.GetEnd(), r);
'''+SEL+'''    }
'''
V={}
for n,mx in {
 'assign':'''    CVector3 lo; lo = m_axis.GetStart(); lo.Add(r);
    CVector3 hi; hi = m_axis.GetEnd(); hi.Add(r);
''',
 'member':'''    CVector3 lo = m_axis.m_start; lo.Add(r);
    CVector3 hi = m_axis.m_end; hi.Add(r);
''',
 'ctor':'''    CVector3 lo(m_axis.GetStart()); lo.Add(r);
    CVector3 hi(m_axis.GetEnd()); hi.Add(r);
''',
 'scale1':'''    CVector3 lo; lo.Scale(m_axis.GetStart(), 1.0f); lo.Add(r);
    CVector3 hi; hi.Scale(m_axis.GetEnd(), 1.0f); hi.Add(r);
''',
 'subneg':'''    CVector3 nr(-radius, -radius, -radius);
    CVector3 lo; lo.Sub(m_axis.GetStart(), nr);
    CVector3 hi; hi.Sub(m_axis.GetEnd(), nr);
''',
}.items():
    V[n]=G(MINB+'    {\n'+mx+SEL2+'    }\n')
V={}
MAXV='''    {
        CVector3 a = m_axis.m_start;
        float ax = AddV(a.x, radius);
        float ay = AddV(a.y, radius);
        float az = AddV(a.z, radius);
        CVector3 b = m_axis.m_end;
        float bx = AddV(b.x, radius);
        float by = AddV(b.y, radius);
        float bz = AddV(b.z, radius);
        ax = ax > bx ? ax : bx;
        max.x = ax;
        ay = ay > by ? ay : by;
        max.y = ay;
        az = az > bz ? az : bz;
        max.z = az;
    }
'''
MAXP=MAXV.replace('AddV(a.x, radius)','a.x + radius').replace('AddV(a.y, radius)','a.y + radius').replace('AddV(a.z, radius)','a.z + radius').replace('AddV(b.x, radius)','b.x + radius').replace('AddV(b.y, radius)','b.y + radius').replace('AddV(b.z, radius)','b.z + radius')
V['submin_addvmax']=G(MINB+MAXV)
V['submin_plainmax']=G(MINB+MAXP)
MAXS='''    {
    CVector3 nr(-radius, -radius, -radius);
    CVector3 lo; lo.Sub(m_axis.GetStart(), nr);
    CVector3 hi; hi.Sub(m_axis.GetEnd(), nr);
'''+SEL2+'''    }
'''
