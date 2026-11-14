ADD=('    void Negate(const CVector3& v) {','''    void Sub(const CVector3& a, const CVector3& b) { d.c.x = a.d.c.x - b.d.c.x; d.c.y = a.d.c.y - b.d.c.y; d.c.z = a.d.c.z - b.d.c.z; }
    void Add(const CVector3& o) { d.c.x += o.d.c.x; d.c.y += o.d.c.y; d.c.z += o.d.c.z; }
    void Negate(const CVector3& v) {''')
DIFF=('''    diff.d.c.x = start.d.c.x - m_center.d.c.x;
    diff.d.c.y = start.d.c.y - m_center.d.c.y;
    diff.d.c.z = start.d.c.z - m_center.d.c.z;''','    diff.Sub(start, m_center);')
EV=('''    float sx = t * d.d.c.x;
    float sy = t * d.d.c.y;
    float sz = t * d.d.c.z;
    x = s.d.c.x + sx;
    y = s.d.c.y + sy;
    z = s.d.c.z + sz;''','''    CVector3 sd;
    sd.Scale(d, t);
    s.Add(sd);
    x = s.d.c.x;
    y = s.d.c.y;
    z = s.d.c.z;''')
BLK_OLD='''                float cx, cy, cz;
                Evaluate(line, t, cx, cy, cz);
                float bx, by, bz;
                BoxPoint(b0, b1, b2, bx, by, bz);
                normal.d.c.x = cx - bx;
                normal.d.c.y = cy - by;
                normal.d.c.z = cz - bz;
                normal.Normalize();
                SClsnContact contact;
                contact.m_separation = separation;
                float k = -capsule.m_radius;
                float ox = k * normal.d.c.x;
                float oy = k * normal.d.c.y;
                float oz = k * normal.d.c.z;
                tmp.d.c.x = cx + ox;
                tmp.d.c.y = cy + oy;
                tmp.d.c.z = cz + oz;
'''
BLK_NEW='''                CVector3 c;
                Evaluate(line, t, c.d.c.x, c.d.c.y, c.d.c.z);
                CVector3 b;
                BoxPoint(b0, b1, b2, b.d.c.x, b.d.c.y, b.d.c.z);
                normal.Sub(c, b);
                normal.Normalize();
                SClsnContact contact;
                contact.m_separation = separation;
                CVector3 o;
                o.Scale(normal, -capsule.m_radius);
                tmp = c;
                tmp.Add(o);
'''
TMP_OLD='''                float k = -capsule.m_radius;
                float ox = k * normal.d.c.x;
                float oy = k * normal.d.c.y;
                float oz = k * normal.d.c.z;
                tmp.d.c.x = cx + ox;
                tmp.d.c.y = cy + oy;
                tmp.d.c.z = cz + oz;
'''
V={'base':[],'diff':[ADD,DIFF],'ev':[ADD,EV],'blk':[ADD,(BLK_OLD,BLK_NEW)],
'norm':[ADD,('''                normal.d.c.x = cx - bx;
                normal.d.c.y = cy - by;
                normal.d.c.z = cz - bz;''','''                normal.Sub(CVector3(cx, cy, cz), CVector3(bx, by, bz));''')],
'tmp':[ADD,(TMP_OLD,'''                CVector3 o;
                o.Scale(normal, -capsule.m_radius);
                tmp.d.c.x = cx + o.d.c.x;
                tmp.d.c.y = cy + o.d.c.y;
                tmp.d.c.z = cz + o.d.c.z;
''')],
}
import re
K=[ADD,DIFF,V['tmp'][1]]
NOLD='''        float nx = p.d.c.x - m_center.d.c.x;
        float ny = p.d.c.y - m_center.d.c.y;
        float nz = p.d.c.z - m_center.d.c.z;
        if (nx * nx + ny * ny + nz * nz > r * r) {'''
NNEW='''        CVector3 n;
        n.Sub(p, m_center);
        if (n.LengthSq() > r * r) {'''
REST=[('            lenSq = nx * nx + ny * ny + nz * nz;','            lenSq = n.LengthSq();'),
('''            nx *= inv;
            ny *= inv;
            nz *= inv;''','''            n.Scale(n, inv);''')]
AB=[(f'h{i} * m_axes[{i}].d.c.x * nx + h{i} * m_axes[{i}].d.c.y * ny + h{i} * m_axes[{i}].d.c.z * nz',f'h{i} * m_axes[{i}].d.c.x * n.d.c.x + h{i} * m_axes[{i}].d.c.y * n.d.c.y + h{i} * m_axes[{i}].d.c.z * n.d.c.z') for i in (2,1,0)]
NDOT=('    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }','    float Dot(const CVector3& o) const { return d.c.x * o.d.c.x + d.c.y * o.d.c.y + d.c.z * o.d.c.z; }')
V={'keep':K,'n':K+[(NOLD,NNEW)]+REST+AB,
'n_dotlen':K+[(NOLD,NNEW.replace('n.LengthSq()','n.Dot(n)'))]+[(REST[0][0],'            lenSq = n.Dot(n);'),REST[1]]+AB,
'n_mulinv':K+[(NOLD,NNEW)]+[REST[0],(REST[1][0],'            n.d.c.x *= inv;\n            n.d.c.y *= inv;\n            n.d.c.z *= inv;')]+AB,
}
