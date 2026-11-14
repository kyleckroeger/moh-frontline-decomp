HDR = ('    CVector3Data d;\n} __attribute__((aligned(8)));',
'''    void Scale(const CVector3& o, float k) { d.c.x = k * o.d.c.x; d.c.y = k * o.d.c.y; d.c.z = k * o.d.c.z; }
    void Sub(const CVector3& a, const CVector3& b) { d.c.x = a.d.c.x - b.d.c.x; d.c.y = a.d.c.y - b.d.c.y; d.c.z = a.d.c.z - b.d.c.z; }
    CVector3& operator-=(const CVector3& o) { d.c.x -= o.d.c.x; d.c.y -= o.d.c.y; d.c.z -= o.d.c.z; return *this; }
    CVector3Data d;
} __attribute__((aligned(8)));''')
import re
B0 = open('/home/kyle/workspace/moh-frontline/research/drafts/Orthonormalize.8008f1e8.cpp').read()
body_start = B0.index('void CMatrix::Orthonormalize() {')
body_end = B0.index('void CMatrix::Transpose()')
OLD = B0[body_start:body_end]
def V_(body): return [HDR, (OLD, 'void CMatrix::Orthonormalize() {\n' + body + '\n}\n\n')]
STORE = '''    m[0][0] = right.d.c.x; m[0][1] = right.d.c.y; m[0][2] = right.d.c.z;
    m[1][0] = f.d.c.x; m[1][1] = f.d.c.y; m[1][2] = f.d.c.z;
    m[2][0] = u.d.c.x; m[2][1] = u.d.c.y; m[2][2] = u.d.c.z;'''
V = {
'inplace_subsub': None,
'scale_sub': V_('''    CVector3 forward = Row(1);
    CVector3 up = Row(2);
    CVector3 right = Row(0);
    right.Normalize();
    CVector3 t;
    t.Scale(right, forward.Dot(right));
    CVector3 f;
    f.Sub(forward, t);
    f.Normalize();
    CVector3 t1;
    t1.Scale(right, up.Dot(right));
    CVector3 t2;
    t2.Scale(f, up.Dot(f));
    CVector3 u;
    u.Sub(up, t1);
    u -= t2;
    u.Normalize();
''' + STORE),
'scale_sub_b': V_('''    CVector3 forward = Row(1);
    CVector3 up = Row(2);
    CVector3 right = Row(0);
    right.Normalize();
    CVector3 t;
    CVector3 f;
    t.Scale(right, forward.Dot(right));
    f.Sub(forward, t);
    f.Normalize();
    CVector3 u;
    CVector3 t1;
    CVector3 t2;
    t1.Scale(right, up.Dot(right));
    t2.Scale(f, up.Dot(f));
    u.Sub(up, t1);
    u -= t2;
    u.Normalize();
''' + STORE),
'scale_inplace': V_('''    CVector3 forward = Row(1);
    CVector3 up = Row(2);
    CVector3 right = Row(0);
    right.Normalize();
    CVector3 t;
    t.Scale(right, forward.Dot(right));
    CVector3 f;
    f.Sub(forward, t);
    f.Normalize();
    CVector3 u;
    t.Scale(right, up.Dot(right));
    u.Sub(up, t);
    t.Scale(f, up.Dot(f));
    u -= t;
    u.Normalize();
''' + STORE),
}
V['inplace_subsub'] = [(a, b.replace('    u -= t;\n', '    u.Sub(u, t);\n')) for a, b in V['scale_inplace']]
V['inplace_noop'] = [(HDR[0], HDR[1].replace('    CVector3& operator-=(const CVector3& o) { d.c.x -= o.d.c.x; d.c.y -= o.d.c.y; d.c.z -= o.d.c.z; return *this; }\n','')), V['inplace_subsub'][1]]
V['inplace_t2'] = V_('''    CVector3 forward = Row(1);
    CVector3 up = Row(2);
    CVector3 right = Row(0);
    right.Normalize();
    CVector3 t;
    t.Scale(right, forward.Dot(right));
    CVector3 f;
    f.Sub(forward, t);
    f.Normalize();
    CVector3 u;
    CVector3 t2;
    t.Scale(right, up.Dot(right));
    u.Sub(up, t);
    t2.Scale(f, up.Dot(f));
    u.Sub(u, t2);
    u.Normalize();
''' + STORE)
