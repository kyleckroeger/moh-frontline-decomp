import re
plain=open('cv_sol.txt').read()
BODY=plain+'\n'
BLOCK=BODY; IND='                '
S0=[l.strip() for l in BODY.strip('\n').split('\n')]
# split: decls first
S=[]
for s in S0:
    m=re.match(r'float (\w+) = (.*)$',s)
    if m: S.append('float %s;'%m.group(1)); S.append('%s = %s'%(m.group(1),m.group(2)))
    else: S.append(s)
# canonical start order: all as originally (decl immediately before assignment)
defs={}; asg={}
for i,s in enumerate(S):
    m=re.match(r'float (\w+);$',s)
    if m: defs[m.group(1)]=i
    m=re.match(r'(\w+) = ',s)
    if m: asg[m.group(1)]=i
DEP={}
for i,s in enumerate(S):
    m=re.match(r'([\w.]+) = (.*)$',s)
    if not m: continue
    lhs,rhs=m.groups(); d=[]
    if lhs in defs: d.append(defs[lhs])
    for n in asg:
        if re.search(r'\b%s\b'%n,rhs): d.append(asg[n])
    DEP[i]=d
