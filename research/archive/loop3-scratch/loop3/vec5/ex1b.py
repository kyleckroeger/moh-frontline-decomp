import sys
sys.path.insert(0,'.')
import sc
S=sc.UBB()
src=open('UBB_BEST58.cpp').read()
s=src.replace('        GetSystemAcceleration(sa);\n','        GetSystemAcceleration(sa);\n        float svz = sv.d.v[2];\n        float saz = sa.d.v[2];\n')
b=s.index('t = (m_upd'); e=s.index('CVector3 tlo')
s=s[:b]+s[b:e].replace('sv.d.v[2]','svz').replace('sa.d.v[2]','saz')+s[e:]
open('z1.cpp','w').write(s)
m=S.asm(s)
for i in range(450,500): print(i,S.o[i][:30],'|',m[i][:30])
