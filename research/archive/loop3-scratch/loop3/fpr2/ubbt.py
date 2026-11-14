import re,sys
from s import run
base=open('ubb0.cpp').read()
i0=base.index('    if (IsMoving()) {'); pre,blk=base[:i0],base[i0:]
HELP='''static inline float Sq(float a) { return a * a; }
static inline float SqC(float a) { float r = a; r *= a; return r; }
static inline float Tk(float a, float b) { return (a - b) / 60.0f; }
'''
pre=pre.replace('void CParticleSystem::UpdateBoundingBox() {',HELP+'void CParticleSystem::UpdateBoundingBox() {')
def var(tform,t2form,keep_t):
    b=blk
    b=b.replace('        t2 = t * t;\n','')
    b=b.replace('        float t2;\n','')
    if not keep_t:
        b=b.replace('        t = (m_updateTicks - m_startTicks) / 60.0f;\n','').replace('        float t;\n','')
        b=re.sub(r'\bt2\b',t2form.replace('T',tform),b)
        b=re.sub(r'\bt\b',tform,b)
    else:
        b=re.sub(r'\bt2\b',t2form.replace('T','t'),b)
    return pre+b
TF=['Tk(m_updateTicks, m_startTicks)','((m_updateTicks - m_startTicks) / 60.0f)']
T2=['Sq(T)','SqC(T)','Mul2(T, T)','(T * T)']
for kt in (True,False):
  for tf in TF:
    for t2 in T2:
      src=var(tf,t2,kt); r=run('ubb',src); print(r,kt,tf,t2,flush=True)
      if kt: break
