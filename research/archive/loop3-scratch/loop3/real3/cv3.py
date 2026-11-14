SOL=open('cv.py').read().split("SOL='''")[1].split("'''")[0]
MEM=('    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; }','''    void Sub(const CVector3& a, const CVector3& b) { x = a.x - b.x; y = a.y - b.y; z = a.z - b.z; }
    void Add(const CVector3& o) { x += o.x; y += o.y; z += o.z; }
    float Dot(const CVector3& o) const { return x * o.x + y * o.y + z * o.z; }
    CVector3& operator*=(float f) { x *= f; y *= f; z *= f; return *this; }''')
OPS=('class CAnimObject;','''inline CVector3 operator*(const CVector3& v, float f) { CVector3 r; r.x = v.x * f; r.y = v.y * f; r.z = v.z * f; return r; }
inline CVector3 operator*(float f, const CVector3& v) { CVector3 r; r.x = f * v.x; r.y = f * v.y; r.z = f * v.z; return r; }
inline CVector3 operator+(const CVector3& a, const CVector3& b) { CVector3 r; r.x = a.x + b.x; r.y = a.y + b.y; r.z = a.z + b.z; return r; }
inline CVector3 operator-(const CVector3& a, const CVector3& b) { CVector3 r; r.x = a.x - b.x; r.y = a.y - b.y; r.z = a.z - b.z; return r; }
class CAnimObject;''')
P1=[('            float py;\n            float px;\n','            CVector3 p;\n'),('            float pz;\n',''),
    ('''            px = d2 * n2.x;
            py = d2 * n2.y;
            pz = d2 * n2.z;
            e = px * n1.x + py * n1.y + pz * n1.z;''','''            p.Scale(n2, d2);
            e = p.Dot(n1);''')]
P1b=[('            float py;\n            float px;\n','            CVector3 p;\n'),('            float pz;\n',''),
    ('''            px = d2 * n2.x;
            py = d2 * n2.y;
            pz = d2 * n2.z;
            e = px * n1.x + py * n1.y + pz * n1.z;''','''            p.Scale(n2, d2);
            e = p.x * n1.x + p.y * n1.y + p.z * n1.z;''')]
sols={
 'op1':'                m_velocity = velocity - (b * bv + p * a) * 1.05f;',
 'op2':'                m_velocity.Sub(velocity, (b * bv + p * a) * 1.05f);',
 'op3':'                CVector3 c = (b * bv + p * a) * 1.05f;\n                m_velocity.Sub(velocity, c);',
 'mem':'''                CVector3 s;
                s.Scale(bv, b);
                p *= a;
                s.Add(p);
                s *= 1.05f;
                m_velocity.Sub(velocity, s);''',
 'mem2':'''                bv.Scale(bv, b);
                p *= a;
                bv.Add(p);
                bv *= 1.05f;
                m_velocity.Sub(velocity, bv);''',
}
V={}
for k,s in sols.items():
    V[k]=[MEM,OPS]+P1+[(SOL,s)]
    V[k+'b']=[MEM,OPS]+P1b+[(SOL,s)]
