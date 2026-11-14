MEM=('    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; }','''    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; }
    void Add(const CVector3& o) { x += o.x; y += o.y; z += o.z; }
    float Dot(const CVector3& o) const { return x * o.x + y * o.y + z * o.z; }
    CVector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }''')
PX='''            px = d2 * n2.x;
            py = d2 * n2.y;
            pz = d2 * n2.z;
            e = px * n1.x + py * n1.y + pz * n1.z;'''
PXV='''            p.Scale(n2, d2);
            e = p.Dot(n1);'''
SOL='''                float sx = Mul(b, bv.x);
                float tx = Mul(px, a);
                float ux = sx + tx;
                float dx = Mul(ux, 1.05f);
                float sy = Mul(b, bv.y);
                float ty = Mul(py, a);
                float uy = sy + ty;
                float dy = Mul(uy, 1.05f);
                float sz = Mul(b, bv.z);
                float tz = Mul(pz, a);
                float uz = sz + tz;
                float dz = Mul(uz, 1.05f);
                m_velocity.x = velocity.x - dx;
                m_velocity.y = velocity.y - dy;
                m_velocity.z = velocity.z - dz;'''
DECL=('            float px;\n','')
DECL2=('            float pz;\n','')
DECL3=('            float py;\n','            CVector3 p;\n')
sols={
'A':'''                CVector3 s;
                s.Scale(bv, b);
                p *= a;
                s.Add(p);
                s *= 1.05f;
                m_velocity.Sub(velocity, s);''',
'B':'''                bv.Scale(bv, b);
                p *= a;
                bv.Add(p);
                bv *= 1.05f;
                m_velocity.Sub(velocity, bv);''',
'C':'''                CVector3 s;
                s.Scale(bv, b);
                CVector3 t;
                t.Scale(p, a);
                s.Add(t);
                s.Scale(s, 1.05f);
                m_velocity.Sub(velocity, s);''',
'D':'''                p *= a;
                CVector3 s;
                s.Scale(bv, b);
                s.Add(p);
                s *= 1.05f;
                m_velocity.Sub(velocity, s);''',
}
V={}
for k,s in sols.items():
    V[k]=[MEM,DECL,DECL2,DECL3,(PX,PXV),(SOL,s)]
V['ponly']=[MEM,(PX,'''            px = d2 * n2.x;
            py = d2 * n2.y;
            pz = d2 * n2.z;
            e = px * n1.x + py * n1.y + pz * n1.z;''')]
