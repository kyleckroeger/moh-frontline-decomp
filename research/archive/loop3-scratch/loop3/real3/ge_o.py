B=open('ge_base.cpp').read()
s=B.index('void CVolBox::GetExtents'); e=B.index('}\n',s)+2
OLD=B[s:e]
PRE=[('    float& operator[](int i) { return d.v[i]; }','''    void Scale(const CVector3& o, float k) { d.c.x = k * o.d.c.x; d.c.y = k * o.d.c.y; d.c.z = k * o.d.c.z; }
    void Sub(const CVector3& a, const CVector3& b) { d.c.x = a.d.c.x - b.d.c.x; d.c.y = a.d.c.y - b.d.c.y; d.c.z = a.d.c.z - b.d.c.z; }
    void Add(const CVector3& o) { d.c.x += o.d.c.x; d.c.y += o.d.c.y; d.c.z += o.d.c.z; }
    void Abs(const CVector3& o) { d.c.x = __fabs(o.d.c.x); d.c.y = __fabs(o.d.c.y); d.c.z = __fabs(o.d.c.z); }
    float& operator[](int i) { return d.v[i]; }''')]
BODY='''    CVector3 a;
    CVector3 b;
    CVector3 c;
    a.Scale(m_basis[0], m_halfWidth);
    b.Scale(m_basis[1], m_halfDepth);
    c.Scale(m_basis[2], m_halfHeight);
    a.Abs(a);
    b.Abs(b);
    c.Abs(c);
    min.d.c.x = m_center.d.c.x - a.d.c.x - b.d.c.x - c.d.c.x;
    min.d.c.y = m_center.d.c.y - a.d.c.y - b.d.c.y - c.d.c.y;
    min.d.c.z = m_center.d.c.z - a.d.c.z - b.d.c.z - c.d.c.z;
    max.d.c.x = m_center.d.c.x + a.d.c.x + b.d.c.x + c.d.c.x;
    max.d.c.y = m_center.d.c.y + a.d.c.y + b.d.c.y + c.d.c.y;
    max.d.c.z = m_center.d.c.z + a.d.c.z + b.d.c.z + c.d.c.z;
'''
PRE.append((OLD,'void CVolBox::GetExtents(CVector3& min, CVector3& max) const {\n'+BODY+'}\n'))
BLOCK=BODY
S=[l.strip() for l in BODY.strip('\n').split('\n')]
DEP={3:[0],4:[1],5:[2],6:[3],7:[4],8:[5]}
for k in range(9,15): DEP[k]=[6,7,8]
