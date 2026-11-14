import sys,itertools,re
from s import run
exec(open('cvv.py').read().split('def body')[0])
def body3(tmpl, layout):
    out=[]
    for ax,p in (('x','px'),('y','py'),('z','pz')):
        out.append('float u%s = %s;'%(ax, tmpl.format(B='bv.'+ax,P=p)))
        out.append('float d%s = Mul(u%s, 1.05f);'%(ax,ax))
        if layout==0: out.append('m_velocity.%s = velocity.%s - d%s;'%(ax,ax,ax))
    if layout==1:
        for ax in 'xyz': out.append('m_velocity.%s = velocity.%s - d%s;'%(ax,ax,ax))
    if layout==2:
        out=[l for l in out if not l.startswith('float d')]
        for ax in 'xyz': out.append('m_velocity.%s = velocity.%s - Mul(u%s, 1.05f);'%(ax,ax,ax))
    return '\n'.join('                '+l for l in out)+'\n            }\n'
T=[]
for L in ['Lin','LinR','LinV','Dot2']:
  for args in ['b, {B}, {P}, a','{B}, b, {P}, a','b, {B}, a, {P}','{P}, a, b, {B}','a, {P}, b, {B}','{B}, b, a, {P}']:
    T.append('%s(%s)'%(L,args))
for A in ['Add','AddR','AddC']:
  for p1 in ['Mul(b, {B})','Mul({B}, b)','MulV(b, {B})']:
    for p2 in ['Mul({P}, a)','Mul(a, {P})','MulV({P}, a)']:
      T.append('%s(%s, %s)'%(A,p1,p2)); T.append('%s(%s, %s)'%(A,p2,p1))
for t in T:
  for lay in (0,1,2):
    r=run('cv',pre+body3(t,lay)+post)
    print(r[0] if r else None, lay, t, flush=True)
