ADD=('''    CVector3& operator+=(const CVector3& v) {''','''    void Add(const CVector3& v) { d.v[0] += v.d.v[0]; d.v[1] += v.d.v[1]; d.v[2] += v.d.v[2]; }
    void Sub(const CVector3& a, const CVector3& b) { d.v[0] = a.d.v[0] - b.d.v[0]; d.v[1] = a.d.v[1] - b.d.v[1]; d.v[2] = a.d.v[2] - b.d.v[2]; }
    CVector3& operator+=(const CVector3& v) {''')
def comp(lhs, rhs_fmt):
    return '\n'.join(f'        {lhs}.d.v[{i}]{rhs_fmt.format(i=i)};' for i in range(3))
ST=(comp('start',' = t * sv.d.v[{i}]'),'        start.Scale(sv, t);')
CS=(comp('m_center',' = m_max.d.v[{i}] - m_min.d.v[{i}]'),'        m_center.Sub(m_max, m_min);')
CA=(comp('m_center',' += m_min.d.v[{i}]'),'        m_center.Add(m_min);')
CH=(comp('m_center',' *= 0.5f'),'        m_center.Scale(m_center, 0.5f);')
MN=(comp('m_min',' = lo.d.v[{i}] + m_localMin.d.v[{i}]'),'        m_min = lo;\n        m_min.Add(m_localMin);')
MX=(comp('m_max',' = hi.d.v[{i}] + m_localMax.d.v[{i}]'),'        m_max = hi;\n        m_max.Add(m_localMax);')
V={'base':[],'st':[ADD,ST],'cs':[ADD,CS],'ca':[ADD,CA],'ch':[ADD,CH],'mn':[ADD,MN],'mx':[ADD,MX],'cs_ca':[ADD,CS,CA]}
K=[ADD,ST,CS,CA]
VEL=('''        vx = k * sa.d.v[0];
        vy = k * sa.d.v[1];
        vz = k * sa.d.v[2];
        vx += sv.d.v[0];
        vy += sv.d.v[1];
        vz += sv.d.v[2];''','''        CVector3 vel;
        vel.Scale(sa, k);
        vel.Add(sv);
        vx = vel.d.v[0];
        vy = vel.d.v[1];
        vz = vel.d.v[2];''')
AX=('''        ax = a2 * sa.d.v[0];
        ay = a2 * sa.d.v[1];
        az = a2 * sa.d.v[2];
        ex += ax;
        ey += ay;
        ez += az;''','''        CVector3 aa;
        aa.Scale(sa, a2);
        ex += aa.d.v[0];
        ey += aa.d.v[1];
        ez += aa.d.v[2];''')
V={'keep':K,'vel':K+[VEL],'ax':K+[AX]}
