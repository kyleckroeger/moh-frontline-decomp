import sys
sys.path.insert(0,'.'); import sc
S=sc.UBB()
s=open('cmb_0000.cpp').read()
a=s.index('        float t = (m_upd'); b=s.index('        CVector3 lo = start;')
fa=s.index('void CParticleSystem::UpdateBoundingBox')
KIN='''static inline void Kin(CVector3& p, float& vx, float& vy, float& vz, const CVector3& v, const CVector3& a, float t) {
    float t2 = t * t;
    float k = 2.0f * t;
    p.d.v[0] = t * v.d.v[0];
    p.d.v[1] = t * v.d.v[1];
    p.d.v[2] = t * v.d.v[2];
    p.d.v[0] += Mul2(t2, a.d.v[0]);
    p.d.v[1] += Mul2(t2, a.d.v[1]);
    p.d.v[2] += Mul2(t2, a.d.v[2]);
    vx = k * a.d.v[0];
    vy = k * a.d.v[1];
    vz = k * a.d.v[2];
    vx += v.d.v[0];
    vy += v.d.v[1];
    vz += v.d.v[2];
}
'''
ADV='''static inline float Adv(float p, float v, float a, float t, float t2) {
    float e = t * v;
    e += p;
    e += Mul2(t2, a);
    return e;
}
'''
END1='''        float a2 = age * age;
        float ex = age * vx;
        float ey = age * vy;
        float ez = age * vz;
        ex += start.d.v[0];
        ey += start.d.v[1];
        ez += start.d.v[2];
        ex += Mul2(a2, sa.d.v[0]);
        ey += Mul2(a2, sa.d.v[1]);
        ez += Mul2(a2, sa.d.v[2]);
'''
END2='''        float a2 = age * age;
        float ex = Adv(start.d.v[0], vx, sa.d.v[0], age, a2);
        float ey = Adv(start.d.v[1], vy, sa.d.v[1], age, a2);
        float ez = Adv(start.d.v[2], vz, sa.d.v[2], age, a2);
'''
HEAD='''        float t = (m_updateTicks - m_startTicks) / 60.0f;
        float age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
        CVector3 start;
        float vx, vy, vz;
        Kin(start, vx, vy, vz, sv, sa, t);
'''
HEAD2='''        float age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
        CVector3 start;
        float vx, vy, vz;
        Kin(start, vx, vy, vz, sv, sa, (m_updateTicks - m_startTicks) / 60.0f);
'''
HEAD3='''        float t = (m_updateTicks - m_startTicks) / 60.0f;
        CVector3 start;
        float vx, vy, vz;
        Kin(start, vx, vy, vz, sv, sa, t);
        float age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
'''
for nm,blk in [('D',HEAD+END1),('E',HEAD+END2),('F',HEAD2+END1),('G',HEAD3+END1),('H',HEAD3+END2)]:
    src=s[:fa]+KIN+ADV+s[fa:a]+blk+s[b:]
    open('inl_%s.cpp'%nm,'w').write(src)
