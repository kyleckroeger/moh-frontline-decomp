import sys,itertools,re
from s import run
base=open('cv0.cpp').read()
i0=base.index('            if (solved) {\n')+len('            if (solved) {\n')
i1=base.index('            for (it = begin; found')
pre,post=base[:i0],base[i1:]
HELP='''inline float MulV(float a, float b) { return a * b; }
inline float Add(float a, float b) { return a + b; }
inline float AddR(float a, float b) { return b + a; }
inline float AddC(float a, float b) { float r = a; r += b; return r; }
inline float Lin(float s, float u, float t, float v) { float p = s; p *= u; float q = t; q *= v; return p + q; }
inline float LinR(float s, float u, float t, float v) { float q = t; q *= v; float p = s; p *= u; return p + q; }
inline float LinV(float s, float u, float t, float v) { return Mul(s, u) + Mul(t, v); }
inline float Dot2(float s, float u, float t, float v) { return s * u + t * v; }
'''
MD='inline float Mul(float a, float b) { float r = a; r *= b; return r; }'
pre=pre.replace(MD,MD+'\n'+HELP)
def body(yexpr):
    # keep x and z as in draft; replace the y computation
    return '''                float sx = Mul(b, bv.x);
                float tx = Mul(px, a);
                float ux = sx + tx;
                float dx = Mul(ux, 1.05f);
                m_velocity.x = velocity.x - dx;
                %s
                float dy = Mul(uy, 1.05f);
                float sz = Mul(b, bv.z);
                float tz = Mul(pz, a);
                float uz = sz + tz;
                float dz = Mul(uz, 1.05f);
                m_velocity.y = velocity.y - dy;
                m_velocity.z = velocity.z - dz;
            }
''' % yexpr
ys=[]
for p1 in ['Mul(b, bv.y)','Mul(bv.y, b)','MulV(b, bv.y)','b * bv.y']:
  for p2 in ['Mul(py, a)','Mul(a, py)','MulV(py, a)','py * a']:
    for f in ['Add(%s, %s)','AddR(%s, %s)','AddC(%s, %s)','%s + %s']:
      ys.append('float uy = '+f%(p1,p2)+';')
      ys.append('float uy = '+f%(p2,p1)+';')
for L in ['Lin','LinR','LinV','Dot2']:
  for args in ['b, bv.y, py, a','bv.y, b, py, a','b, bv.y, a, py','py, a, b, bv.y','a, py, b, bv.y']:
    ys.append('float uy = %s(%s);'%(L,args))
for y in ys:
    r=run('cv',pre+body(y)+post)
    print(r[0] if r else None, y, flush=True)
