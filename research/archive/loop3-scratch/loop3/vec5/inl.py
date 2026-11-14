import sys
sys.path.insert(0,'.'); import sc
S=sc.UBB()
s=open('cmb_0000.cpp').read()
a=s.index('        float t = (m_upd'); b=s.index('        CVector3 lo = start;')
fa=s.index('void CParticleSystem::UpdateBoundingBox')
V={}
V['A']=('''static inline void Pos(CVector3& p, const CVector3& v, const CVector3& a, float t) {
    float t2 = t * t;
    p.d.v[0] = t * v.d.v[0];
    p.d.v[1] = t * v.d.v[1];
    p.d.v[2] = t * v.d.v[2];
    p.d.v[0] += Mul2(t2, a.d.v[0]);
    p.d.v[1] += Mul2(t2, a.d.v[1]);
    p.d.v[2] += Mul2(t2, a.d.v[2]);
}
''','''        float t = (m_updateTicks - m_startTicks) / 60.0f;
        float age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
        CVector3 start;
        Pos(start, sv, sa, t);
        float k = 2.0f * t;
        float a2 = age * age;
        float vx = k * sa.d.v[0];
        float vy = k * sa.d.v[1];
        float vz = k * sa.d.v[2];
        vx += sv.d.v[0];
        vy += sv.d.v[1];
        vz += sv.d.v[2];
        float ex = age * vx;
        float ey = age * vy;
        float ez = age * vz;
        ex += start.d.v[0];
        ey += start.d.v[1];
        ez += start.d.v[2];
        ex += Mul2(a2, sa.d.v[0]);
        ey += Mul2(a2, sa.d.v[1]);
        ez += Mul2(a2, sa.d.v[2]);
''')
V['B']=('''static inline float Vel(float v, float a, float t) { float r = 2.0f * t * a; r += v; return r; }
''','''        float t = (m_updateTicks - m_startTicks) / 60.0f;
        float age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
        float t2 = t * t;
        float a2 = age * age;
        CVector3 start;
        start.d.v[0] = t * sv.d.v[0];
        start.d.v[1] = t * sv.d.v[1];
        start.d.v[2] = t * sv.d.v[2];
        start.d.v[0] += Mul2(t2, sa.d.v[0]);
        start.d.v[1] += Mul2(t2, sa.d.v[1]);
        start.d.v[2] += Mul2(t2, sa.d.v[2]);
        float vx = Vel(sv.d.v[0], sa.d.v[0], t);
        float vy = Vel(sv.d.v[1], sa.d.v[1], t);
        float vz = Vel(sv.d.v[2], sa.d.v[2], t);
        float ex = age * vx;
        float ey = age * vy;
        float ez = age * vz;
        ex += start.d.v[0];
        ey += start.d.v[1];
        ez += start.d.v[2];
        ex += Mul2(a2, sa.d.v[0]);
        ey += Mul2(a2, sa.d.v[1]);
        ez += Mul2(a2, sa.d.v[2]);
''')
V['C']=(V['A'][0],'''        float t = (m_updateTicks - m_startTicks) / 60.0f;
        float age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
        CVector3 start;
        Pos(start, sv, sa, t);
        float k = 2.0f * t;
        CVector3 vel;
        vel.d.v[0] = k * sa.d.v[0];
        vel.d.v[1] = k * sa.d.v[1];
        vel.d.v[2] = k * sa.d.v[2];
        float vx = vel.d.v[0] + sv.d.v[0];
        float vy = vel.d.v[1] + sv.d.v[1];
        float vz = vel.d.v[2] + sv.d.v[2];
        CVector3 end;
        Pos(end, CVector3(vx, vy, vz), sa, age);
        float ex = end.d.v[0] + start.d.v[0];
        float ey = end.d.v[1] + start.d.v[1];
        float ez = end.d.v[2] + start.d.v[2];
''')
for k,(h,blk) in V.items():
    src=s[:fa]+h+s[fa:a]+blk+s[b:]
    open('inl_%s.cpp'%k,'w').write(src)
