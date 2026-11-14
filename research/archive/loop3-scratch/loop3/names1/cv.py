HDR=('''    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));''','''    float Dot(const CVector3& o) const { return x * o.x + y * o.y + z * o.z; }
    void Scale(const CVector3& o, float k) { x = k * o.x; y = k * o.y; z = k * o.z; }
    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; }
    void Add(const CVector3& o) { x += o.x; y += o.y; z += o.z; }
    CVector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));''')
OLDS='''                float sx = Mul(b, bx);
                float tx = Mul(px, a);
                float ux = sx + tx;
                float dx = Mul(ux, 1.05f);
                float sy = Mul(b, by);
                float ty = Mul(py, a);
                float uy = sy + ty;
                float dy = Mul(uy, 1.05f);
                float sz = Mul(b, bz);
                float tz = Mul(pz, a);
                float uz = sz + tz;
                float dz = Mul(uz, 1.05f);
                m_velocity.x = velocity.x - dx;
                m_velocity.y = velocity.y - dy;
                m_velocity.z = velocity.z - dz;'''
def S(new): return [HDR,(OLDS,new)]
V={
'base':[],
'nomul':S('''                float ux = b * bx + px * a;
                float uy = b * by + py * a;
                float uz = b * bz + pz * a;
                m_velocity.x = velocity.x - ux * 1.05f;
                m_velocity.y = velocity.y - uy * 1.05f;
                m_velocity.z = velocity.z - uz * 1.05f;'''),
'vec1':S('''                CVector3 bv; bv.x = bx; bv.y = by; bv.z = bz;
                CVector3 pv; pv.x = px; pv.y = py; pv.z = pz;
                CVector3 s; s.Scale(bv, b);
                CVector3 t; t.Scale(pv, a);
                s.Add(t);
                s *= 1.05f;
                m_velocity.Sub(velocity, s);'''),
'vec2':S('''                CVector3 bv; bv.x = bx; bv.y = by; bv.z = bz;
                CVector3 pv; pv.x = px; pv.y = py; pv.z = pz;
                CVector3 s; s.Scale(bv, b);
                pv *= a;
                s.Add(pv);
                s *= 1.05f;
                m_velocity.Sub(velocity, s);'''),
}
B1_OLD='''            bx = best * first->m_normal.x;
            by = best * first->m_normal.y;
            bz = best * first->m_normal.z;
            m_velocity.x -= bx;
            m_velocity.y -= by;
            m_velocity.z -= bz;'''
B1_NEW='''            bv.Scale(first->m_normal, best);
            m_velocity.Sub(m_velocity, bv);'''
B1_NEWb='''            bv.Scale(first->m_normal, best);
            m_velocity -= bv;'''
DECL=('''        float bx;
        float by;
        float bz;
''','''        CVector3 bv;
''')
SOLV_OLD = OLDS
P_OLD='''            px = d2 * n2.x;
            py = d2 * n2.y;
            pz = d2 * n2.z;
            e = px * n1.x + py * n1.y + pz * n1.z;'''
P_NEW='''            p.Scale(n2, d2);
            e = p.Dot(n1);'''
PDECL=('''            float px;
            float pz;
''','''            CVector3 p;
''')
PDECL2=('''            float py;
''','')
def full(tail, b1=B1_NEW, extra=[]):
    return [HDR, DECL, (B1_OLD,b1), PDECL, PDECL2, (P_OLD,P_NEW), (OLDS, tail)] + extra
T1='''                bv.Scale(bv, b);
                p *= a;
                bv.Add(p);
                bv *= 1.05f;
                m_velocity.Sub(velocity, bv);'''
T2='''                CVector3 s;
                s.Scale(bv, b);
                p *= a;
                s.Add(p);
                s *= 1.05f;
                m_velocity.Sub(velocity, s);'''
T3='''                CVector3 s;
                CVector3 t;
                s.Scale(bv, b);
                t.Scale(p, a);
                s.Add(t);
                s *= 1.05f;
                m_velocity.Sub(velocity, s);'''
V0=1
DC_OLD='            dc = best * (n1.x * n2.x + n1.y * n2.y + n1.z * n2.z);'
V={}
for tn,T in (('T1',T1),('T2',T2)):
    for dn,dcn in (('dc0',DC_OLD),('dc1','            dc = best * n1.Dot(n2);'),('dc2','            dc = best * n2.Dot(n1);'),('dc3','            dc = n1.Dot(n2) * best;')):
        for en,en_ in (('e0',P_NEW),('e1',P_NEW.replace('p.Dot(n1)','n1.Dot(p)'))):
            V[f'{tn}_{dn}_{en}']=[HDR, DECL, (B1_OLD,B1_NEW), PDECL, PDECL2, (P_OLD,en_), (OLDS, T)] + ([(DC_OLD,dcn)] if dcn!=DC_OLD else [])
# scalar p, vector b-block, draft tail
V['bvec_only']=[HDR,DECL,(B1_OLD,B1_NEW),(OLDS,OLDS.replace('Mul(b, bx)','Mul(b, bv.x)').replace('Mul(b, by)','Mul(b, bv.y)').replace('Mul(b, bz)','Mul(b, bv.z)'))]
