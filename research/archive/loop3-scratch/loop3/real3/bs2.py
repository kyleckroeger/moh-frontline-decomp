exec(open('bs.py').read())
SET=('    float Dot(const CVector3& o) const','''    void Set(float ax, float ay, float az) { d.c.x = ax; d.c.y = ay; d.c.z = az; }
    float Dot(const CVector3& o) const''')
D3='m_axes[0].Dot(diff), m_axes[1].Dot(diff), m_axes[2].Dot(diff)'
V={}
V['set']=[SET,(OLD,'    local.Set('+D3+');\n')]
V['ctor']=[('    CVector3 local;\n'+OLD,'    CVector3 local('+D3+');\n')]
