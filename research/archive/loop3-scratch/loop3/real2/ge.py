B=open('/home/kyle/workspace/moh-frontline/scratch/loop3/real2/ge_base.cpp').read()
s=B.index('void CVolBox::GetExtents'); e=B.index('}\n',s)+2
OLD=B[s:e]
ADD=('    float& operator[](int i) { return d.v[i]; }','''    void Scale(const CVector3& o, float k) { d.c.x = k * o.d.c.x; d.c.y = k * o.d.c.y; d.c.z = k * o.d.c.z; }
    void Sub(const CVector3& a, const CVector3& b) { d.c.x = a.d.c.x - b.d.c.x; d.c.y = a.d.c.y - b.d.c.y; d.c.z = a.d.c.z - b.d.c.z; }
    void Add(const CVector3& o) { d.c.x += o.d.c.x; d.c.y += o.d.c.y; d.c.z += o.d.c.z; }
    void Abs(const CVector3& o) { d.c.x = fabs(o.d.c.x); d.c.y = fabs(o.d.c.y); d.c.z = fabs(o.d.c.z); }
    float& operator[](int i) { return d.v[i]; }''')
def F(body): return [ADD,(OLD,'void CVolBox::GetExtents(CVector3& min, CVector3& max) const {\n'+body+'}\n')]
SC='''    CVector3 a;
    CVector3 b;
    CVector3 c;
    a.Scale(m_basis[0], m_halfWidth);
    b.Scale(m_basis[1], m_halfDepth);
    c.Scale(m_basis[2], m_halfHeight);
    a.Abs(a);
    b.Abs(b);
    c.Abs(c);
'''
SC2='''    CVector3 a;
    CVector3 b;
    CVector3 c;
    a.Scale(m_basis[0], m_halfWidth);
    a.Abs(a);
    b.Scale(m_basis[1], m_halfDepth);
    b.Abs(b);
    c.Scale(m_basis[2], m_halfHeight);
    c.Abs(c);
'''
M1='''    min.Sub(m_center, a);
    min.Sub(min, b);
    min.Sub(min, c);
    max = m_center;
    max.Add(a);
    max.Add(b);
    max.Add(c);
'''
M2='''    CVector3 e = a;
    e.Add(b);
    e.Add(c);
    min.Sub(m_center, e);
    max = m_center;
    max.Add(e);
'''
M3='''    min.d.c.x = m_center.d.c.x - a.d.c.x - b.d.c.x - c.d.c.x;
    min.d.c.y = m_center.d.c.y - a.d.c.y - b.d.c.y - c.d.c.y;
    min.d.c.z = m_center.d.c.z - a.d.c.z - b.d.c.z - c.d.c.z;
    max.d.c.x = m_center.d.c.x + a.d.c.x + b.d.c.x + c.d.c.x;
    max.d.c.y = m_center.d.c.y + a.d.c.y + b.d.c.y + c.d.c.y;
    max.d.c.z = m_center.d.c.z + a.d.c.z + b.d.c.z + c.d.c.z;
'''
V={'base':[]}
for i,sc in enumerate((SC,SC2)):
    for j,m in enumerate((M1,M2,M3)):
        V[f's{i}m{j}']=F(sc+m)
