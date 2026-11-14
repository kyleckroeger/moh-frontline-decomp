import sys,itertools
from s import run
base=open('ubb0.cpp').read()
i0=base.index('    if (IsMoving()) {')
i1=base.index('        CVector3 lo = start;')
# tail after: needs ex,ey,ez,vx,vy,vz,start,sa,age
tail=base[i1:]
tail=tail.replace('        ez += az;\n','').replace('        ey = ay + ey;\n','')
pre=base[:i0]
def mk(body):
    return pre+'    if (IsMoving()) {\n'+body+tail
HEAD='''        CVector3 sv;
        GetSystemInitialVelocity(sv);
        CVector3 sa;
        GetSystemAcceleration(sa);
        float t = (m_updateTicks - m_startTicks) / 60.0f;
        float age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
'''
A='''        float t2 = t * t;
        float a2 = age * age;
        float k = 2.0f * t;
        CVector3 start;
        start.d.v[0] = t * sv.d.v[0];
        start.d.v[1] = t * sv.d.v[1];
        start.d.v[2] = t * sv.d.v[2];
        start.d.v[0] += Mul2(t2, sa.d.v[0]);
        start.d.v[1] += Mul2(t2, sa.d.v[1]);
        start.d.v[2] += Mul2(t2, sa.d.v[2]);
        float vx = Mul2(k, sa.d.v[0]) + sv.d.v[0];
        float vy = Mul2(k, sa.d.v[1]) + sv.d.v[1];
        float vz = Mul2(k, sa.d.v[2]) + sv.d.v[2];
        float ex = Mul2(age, vx) + start.d.v[0];
        float ey = Mul2(age, vy) + start.d.v[1];
        float ez = Mul2(age, vz) + start.d.v[2];
        ex += Mul2(a2, sa.d.v[0]);
        ey += Mul2(a2, sa.d.v[1]);
        ez += Mul2(a2, sa.d.v[2]);
'''
B='''        float t2 = t * t;
        float a2 = age * age;
        float k = 2.0f * t;
        CVector3 start;
        float vx, vy, vz, ex, ey, ez;
        start.d.v[0] = t * sv.d.v[0];
        start.d.v[0] += Mul2(t2, sa.d.v[0]);
        vx = Mul2(k, sa.d.v[0]) + sv.d.v[0];
        start.d.v[1] = t * sv.d.v[1];
        start.d.v[1] += Mul2(t2, sa.d.v[1]);
        vy = Mul2(k, sa.d.v[1]) + sv.d.v[1];
        start.d.v[2] = t * sv.d.v[2];
        start.d.v[2] += Mul2(t2, sa.d.v[2]);
        vz = Mul2(k, sa.d.v[2]) + sv.d.v[2];
        ex = Mul2(age, vx) + start.d.v[0];
        ex += Mul2(a2, sa.d.v[0]);
        ey = Mul2(age, vy) + start.d.v[1];
        ey += Mul2(a2, sa.d.v[1]);
        ez = Mul2(age, vz) + start.d.v[2];
        ez += Mul2(a2, sa.d.v[2]);
'''
for n,b in (('A',A),('B',B)):
    t=mk(HEAD+b); open('ubb_%s.cpp'%n,'w').write(t); print(n,run('ubb',t))
