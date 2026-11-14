import itertools
L=['    local.d.c.z = m_axes[2].Dot(diff);\n','    local.d.c.y = m_axes[1].Dot(diff);\n','    local.d.c.x = m_axes[0].Dot(diff);\n']
OLD=''.join(L)
DDD=('    float Dot(const CVector3& o) const','''    void DotDotDot(const CVector3& a, const CVector3& b, const CVector3& c, const CVector3& v) {
        d.c.x = a.d.c.x * v.d.c.x + a.d.c.y * v.d.c.y + a.d.c.z * v.d.c.z;
        d.c.y = b.d.c.x * v.d.c.x + b.d.c.y * v.d.c.y + b.d.c.z * v.d.c.z;
        d.c.z = c.d.c.x * v.d.c.x + c.d.c.y * v.d.c.y + c.d.c.z * v.d.c.z;
    }
    float Dot(const CVector3& o) const''')
V={}
for p in itertools.permutations(range(3)):
    V['o'+''.join(map(str,p))]=[(OLD,''.join(L[i] for i in p))]
V['ddd']=[DDD,(OLD,'    local.DotDotDot(m_axes[0], m_axes[1], m_axes[2], diff);\n')]
AB='    CVector3 abs;\n    abs.Abs(local);\n'
V['ddd_absup']=[DDD,(OLD,'    local.DotDotDot(m_axes[0], m_axes[1], m_axes[2], diff);\n'+AB),(AB,'')]
