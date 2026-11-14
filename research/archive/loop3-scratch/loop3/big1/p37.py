O='''        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
'''
A='inline CVector3 operator*(float k, const CVector3& v)'
V=[]
D='static inline float Det2(float p, float q, float r, float s) { return p * q - r * s; }\n'
V.append(('det',[(O,'''        x = Det2(a.y, b.z, a.z, b.y);
        y = Det2(a.z, b.x, a.x, b.z);
        z = Det2(a.x, b.y, a.y, b.x);
'''),('class CVector3 {',D+'class CVector3 {')]))
V.append(('cross3',[(O,'''        float ax = a.x, ay = a.y, az = a.z;
        float bx = b.x, by = b.y, bz = b.z;
        x = ay * bz - az * by;
        y = az * bx - ax * bz;
        z = ax * by - ay * bx;
''')]))
V.append(('crossfree',[('                up.Cross(right, dir);\n','                CrossInto(up, right, dir);\n'),(A,'inline void CrossInto(CVector3& r, const CVector3& a, const CVector3& b) { r.x = a.y * b.z - a.z * b.y; r.y = a.z * b.x - a.x * b.z; r.z = a.x * b.y - a.y * b.x; }\n'+A)]))
