OLD='''            CVector3 away;
            away.Sub(point, position);
            away.Normalize();
            CVector3 motion;
            motion.c.x = point.c.x;
            motion.c.y = point.c.y;
            motion.c.z = point.c.z;
            motion.c.x -= local.c.x;
            motion.c.y -= local.c.y;
            motion.c.z -= local.c.z;
            motion.Normalize();'''
MEM=('    float Dot(const CVector3& v) const','''    void Add(const CVector3& o) { c.x += o.c.x; c.y += o.c.y; c.z += o.c.z; }
    CVector3& operator-=(const CVector3& o) { c.x -= o.c.x; c.y -= o.c.y; c.z -= o.c.z; return *this; }
    void Set(float ax, float ay, float az) { c.x = ax; c.y = ay; c.z = az; }
    void Scale(const CVector3& o, float k) { c.x = k * o.c.x; c.y = k * o.c.y; c.z = k * o.c.z; }
    float Dot(const CVector3& v) const''')
A='''            CVector3 away;
            away.Sub(point, position);
            away.Normalize();
'''
def M(body): return '            CVector3 motion;\n'+body+'            motion.Normalize();\n'
copy='''            motion.c.x = point.c.x;
            motion.c.y = point.c.y;
            motion.c.z = point.c.z;
'''
bodies={
 'sub':'            motion.Sub(point, local);\n',
 'copysubself':copy+'            motion.Sub(motion, local);\n',
 'copyminus':copy+'            motion -= local;\n',
 'setminus':'            motion.Set(point.c.x, point.c.y, point.c.z);\n            motion -= local;\n',
 'setsubself':'            motion.Set(point.c.x, point.c.y, point.c.z);\n            motion.Sub(motion, local);\n',
}
V={'base':[]}
for k,b in bodies.items():
    V[k+'_after']=[MEM,(OLD,A+M(b))]
    V[k+'_before']=[MEM,(OLD,M(b)+A)]
# copy before away, subtract after
V['copy_split']=[MEM,(OLD,'            CVector3 motion;\n'+copy+A+'''            motion.c.x -= local.c.x;
            motion.c.y -= local.c.y;
            motion.c.z -= local.c.z;
            motion.Normalize();''')]
V['copy_split_m']=[MEM,(OLD,'            CVector3 motion;\n'+copy+A+'            motion -= local;\n            motion.Normalize();')]
CC=('    float Dot(const CVector3& v) const','''    CVector3() {}
    CVector3(const CVector3& o) { pair[0] = o.pair[0]; pair[1] = o.pair[1]; }
    float Dot(const CVector3& v) const''')
V={}
V['ccminus']=[MEM,CC,(OLD,A+'            CVector3 motion = point;\n            motion -= local;\n            motion.Normalize();')]
V['ccsub']=[MEM,CC,(OLD,A+'            CVector3 motion = point;\n            motion.Sub(motion, local);\n            motion.Normalize();')]
NF=('''    void Normalize() {
        float length = sqrtf(c.x * c.x + c.y * c.y + c.z * c.z);
        if (length != 0.0f) {
            float scale = 1.0f / length;
            c.x *= scale;
            c.y *= scale;
            c.z *= scale;
        }
    }''','''    float Normalize() {
        float length = sqrtf(c.x * c.x + c.y * c.y + c.z * c.z);
        if (length != 0.0f) {
            float scale = 1.0f / length;
            c.x *= scale;
            c.y *= scale;
            c.z *= scale;
        }
        return length;
    }''')
V['nf']=[NF]
V['nf_sub']=[NF,MEM,(OLD,A+M(bodies['sub']))]
V['nf_copyminus']=[NF,MEM,(OLD,A+M(bodies['copyminus']))]
