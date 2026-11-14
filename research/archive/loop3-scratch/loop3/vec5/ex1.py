import sys,re
sys.path.insert(0,'.')
import sc
S=sc.UBB()
src=open('UBB_BEST58.cpp').read()
def T(name,s): print(name,S(s),flush=True)
T('base',src)
# named z locals declared right after GetSystemAcceleration
for nm,decl in [('svz+saz','        float svz = sv.d.v[2];\n        float saz = sa.d.v[2];\n'),('saz+svz','        float saz = sa.d.v[2];\n        float svz = sv.d.v[2];\n')]:
    s=src.replace('        GetSystemAcceleration(sa);\n','        GetSystemAcceleration(sa);\n'+decl)
    b=s.index('GetSystemAcceleration(sa);'); e=s.index('CVector3 tlo')
    mid=s[b:e]; head=mid.split('\n',3); 
    rest=mid[mid.index('t = (m_upd'):]
    rest2=rest.replace('sv.d.v[2]','svz').replace('sa.d.v[2]','saz')
    T(nm,s[:b]+mid[:mid.index('t = (m_upd')]+rest2+s[e:])
