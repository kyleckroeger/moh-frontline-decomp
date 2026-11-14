import re,sys
s=open('aim_cb.8005e9ec.cpp').read()
i=s.index('static inline void PitchRotate'); j=s.index('int CAnimCallbacks::aim_cb')
V={}
V['nat']='''static inline void PitchRotate(CVector3& forward, CVector3& up, float c, float s, float t) {
    CVector3 cf = c * forward;
    CVector3 su = s * up;
    forward.x = cf.x + su.x;
    forward.y = cf.y + su.y;
    forward.z = cf.z + su.z;
    float bx = forward.x - su.x;
    float by = forward.y - su.y;
    float bz = forward.z - su.z;
    up *= c;
    bx *= t;
    by *= t;
    bz *= t;
    up.x -= bx;
    up.y -= by;
    up.z -= bz;
}
'''
V['nat2']=V['nat'].replace('''    float bx = forward.x - su.x;
    float by = forward.y - su.y;
    float bz = forward.z - su.z;
    up *= c;''','''    up *= c;
    float bx = forward.x - su.x;
    float by = forward.y - su.y;
    float bz = forward.z - su.z;''')
V['vec']='''static inline void PitchRotate(CVector3& forward, CVector3& up, float c, float s, float t) {
    CVector3 cf = c * forward;
    CVector3 su = s * up;
    forward = cf + su;
    CVector3 b = forward - su;
    up *= c;
    b *= t;
    up -= b;
}
'''
V['vec2']='''static inline void PitchRotate(CVector3& forward, CVector3& up, float c, float s, float t) {
    CVector3 su = s * up;
    forward = c * forward + su;
    CVector3 b = forward - su;
    up *= c;
    up -= t * b;
}
'''
V['vec3']='''static inline void PitchRotate(CVector3& forward, CVector3& up, float c, float s, float t) {
    CVector3 su = s * up;
    forward *= c;
    forward += su;
    CVector3 b = forward - su;
    up *= c;
    b *= t;
    up -= b;
}
'''
for k,v in V.items(): open('aim_%s.cpp'%k,'w').write(s[:i]+v+'\n'+s[j:])
