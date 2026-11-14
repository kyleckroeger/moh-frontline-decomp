ADD=('    float Length() const {','''    void Sub(const CVector3& a, const CVector3& b) { d.v[0] = a.d.v[0] - b.d.v[0]; d.v[1] = a.d.v[1] - b.d.v[1]; d.v[2] = a.d.v[2] - b.d.v[2]; }
    float Length() const {''')
F=('''    front.d.v[0] = position.d.v[0] - center.d.v[0];
    front.d.v[1] = position.d.v[1] - center.d.v[1];
    front.d.v[2] = position.d.v[2] - center.d.v[2];''','    front.Sub(position, center);')
L=('''        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];''','        m_dir.Sub(m_end, m_start);')
V={'base':[],'f':[ADD,F],'l':[ADD,L],'fl':[ADD,F,L]}
