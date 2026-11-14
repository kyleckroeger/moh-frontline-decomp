import sys,itertools
from s import run
base=open('ubb0.cpp').read()
i0=base.index('    if (IsMoving()) {')
i1=base.index('        CVector3 lo = start;')
tail=base[i1:].replace('        ez += az;\n','').replace('        ey = ay + ey;\n','')
pre=base[:i0]
HEAD='''        float t;
        float age;
        float t2;
        float a2;
        float k;
        float vx, vy, vz, ex, ey, ez;
        CVector3 sv;
        GetSystemInitialVelocity(sv);
        CVector3 sa;
        GetSystemAcceleration(sa);
        t = (m_updateTicks - m_startTicks) / 60.0f;
        age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;
        t2 = t * t;
        a2 = age * age;
        k = 2.0f * t;
        CVector3 start;
'''
HELPS={
'P1':'static inline float Pos(float t, float t2, float v, float a) { float r = t * v; r += Mul2(t2, a); return r; }\n',
'P2':'static inline float Pos(float t, float t2, float v, float a) { return Mul2(t, v) + Mul2(t2, a); }\n',
'P3':'static inline float Pos(float t, float t2, float v, float a) { float p = Mul2(t, v); float q = Mul2(t2, a); return p + q; }\n',
}
VEL={'V1':'static inline float Vel(float k, float a, float v) { return Mul2(k, a) + v; }\n',
     'V0':''}
def body(vel,order):
    s=''
    for i in order:
        s+='        start.d.v[%d] = Pos(t, t2, sv.d.v[%d], sa.d.v[%d]);\n'%(i,i,i)
    n='xyz'
    for i in order:
        s+='        v%s = %s;\n'%(n[i],'Vel(k, sa.d.v[%d], sv.d.v[%d])'%(i,i) if vel=='V1' else 'Mul2(k, sa.d.v[%d]) + sv.d.v[%d]'%(i,i))
    for i in order:
        s+='        e%s = Mul2(age, v%s) + start.d.v[%d];\n        e%s += Mul2(a2, sa.d.v[%d]);\n'%(n[i],n[i],i,n[i],i)
    return s
for h,hs in HELPS.items():
  for v,vs in VEL.items():
    for order in [(0,1,2),(2,1,0)]:
      src=(pre+'    if (IsMoving()) {\n'+HEAD+body(v,order)+tail).replace('void CParticleSystem::UpdateBoundingBox() {',hs+vs+'void CParticleSystem::UpdateBoundingBox() {')
      print(run('ubb',src),h,v,order,flush=True)
