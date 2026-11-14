BODY='''            float det;
            float e;
            float bd;
            CVector3& n2 = second->m_normal;
            float py;
            float px;
            CVector3& n1 = first->m_normal;
            float dc;
            float d2;
            float inv;
            dc = best * (n1.x * n2.x + n1.y * n2.y + n1.z * n2.z);
            d2 = best2 + dc;
            px = d2 * n2.x;
            float pz;
            py = d2 * n2.y;
            pz = d2 * n2.z;
            e = px * n1.x + py * n1.y + pz * n1.z;
            bd = best * d2;
            det = bd - e * dc;
'''
BLOCK=BODY
IND='            '
S=[l.strip() for l in BODY.strip('\n').split('\n')]
import re
names=['py','det','dc','px','pz','e','bd','d2','inv','n1','n2']
DEP={}
defs={}
for i,s in enumerate(S):
    m=re.match(r'(?:float|CVector3&) (\w+)',s)
    if m: defs[m.group(1)]=i
for i,s in enumerate(S):
    if re.match(r'float \w+;$',s): continue
    lhs=re.match(r'(?:CVector3& )?(\w+) =',s).group(1)
    deps=[defs[lhs]] if lhs in defs and defs[lhs]!=i else []
    rhs=s.split('=',1)[1]
    for n in names:
        if re.search(r'\b%s\b'%n,rhs):
            deps.append(defs[n])
            # also after the assignment of n
            for j in range(len(S)):
                if S[j].startswith(n+' ='): deps.append(j)
    DEP[i]=deps
