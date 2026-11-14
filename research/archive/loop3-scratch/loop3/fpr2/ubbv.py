import sys
from s import run
base=open('ubb0.cpp').read()
HELP='''static inline float Ticks(float a, float b) { return (a - b) / 60.0f; }
static inline float TicksC(float a, float b) { float r = a; r -= b; r /= 60.0f; return r; }
static inline float Sq(float a) { return a * a; }
static inline float SqC(float a) { float r = a; r *= a; return r; }
static inline float Twice(float a) { return 2.0f * a; }
'''
base=base.replace('void CParticleSystem::UpdateBoundingBox() {',HELP+'void CParticleSystem::UpdateBoundingBox() {')
L={'t':'        t = (m_updateTicks - m_startTicks) / 60.0f;\n',
   'age':'        age = (g_ParticleSystemManager.GetCurrentTicks() - m_updateTicks) / 60.0f;\n',
   't2':'        t2 = t * t;\n','a2':'        a2 = age * age;\n','k':'        k = 2.0f * t;\n'}
OPT={'t':[L['t'],'        t = Ticks(m_updateTicks, m_startTicks);\n','        t = TicksC(m_updateTicks, m_startTicks);\n'],
 'age':[L['age'],'        age = Ticks(g_ParticleSystemManager.GetCurrentTicks(), m_updateTicks);\n','        age = TicksC(g_ParticleSystemManager.GetCurrentTicks(), m_updateTicks);\n'],
 't2':[L['t2'],'        t2 = Sq(t);\n','        t2 = SqC(t);\n','        t2 = Mul2(t, t);\n'],
 'a2':[L['a2'],'        a2 = Sq(age);\n','        a2 = SqC(age);\n','        a2 = Mul2(age, age);\n'],
 'k':[L['k'],'        k = Twice(t);\n','        k = Mul2(2.0f, t);\n','        k = Mul2(t, 2.0f);\n']}
import itertools
keys=list(OPT)
res=[]
for combo in itertools.product(*[range(len(OPT[k])) for k in keys]):
    t=base
    for k,c in zip(keys,combo): t=t.replace(L[k],OPT[k][c])
    r=run('ubb',t); print(r[0] if r[0] is not None else None,combo,flush=True)
