ADD=('    CVector3& operator+=(CVector3 o)','''    void Scale(const CVector3& o, float k) { d.v[0] = k * o.d.v[0]; d.v[1] = k * o.d.v[1]; d.v[2] = k * o.d.v[2]; }
    void Sub(const CVector3& a, const CVector3& b) { d.v[0] = a.d.v[0] - b.d.v[0]; d.v[1] = a.d.v[1] - b.d.v[1]; d.v[2] = a.d.v[2] - b.d.v[2]; }
    void Add(const CVector3& o) { d.v[0] += o.d.v[0]; d.v[1] += o.d.v[1]; d.v[2] += o.d.v[2]; }
    CVector3& operator+=(CVector3 o)''')
SE=('''        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];''','        m_dir.Sub(m_end, m_start);')
M='''                CVector3 mid(t, cur.m_line.m_dir);
                mid += cur.m_line.m_start;'''
M2='''                CVector3 mid;
                mid.Scale(cur.m_line.m_dir, t);
                mid.Add(cur.m_line.m_start);'''
V={'base':[],'se':[ADD,SE],'mid':[ADD,(M,M2),(M,M2)],'both':[ADD,SE,(M,M2),(M,M2)]}
V={}
for nm,(a,b) in {'gd':('cur.m_line.GetDirection()','cur.m_line.m_start'),'gdgs':('cur.m_line.GetDirection()','cur.m_line.GetStart()')}.items():
    M3=f'''                CVector3 mid;
                mid.Scale({a}, t);
                mid.Add({b});'''
    V[nm]=[ADD,SE,(M,M3),(M,M3)]
GD=('    CVector3 GetEnd() const { return m_end; }','    CVector3 GetEnd() const { return m_end; }\n    CVector3 GetDirection() const { return m_dir; }')
for k in V: V[k]=V[k]+[GD]
