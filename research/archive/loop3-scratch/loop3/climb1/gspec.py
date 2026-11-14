# generic spec: set BLOCKFILE (text file holding the exact block) before exec; splits 'T x = e;' into decl + assign.
import re
BODY=open(BLOCKFILE).read()
BLOCK=BODY
IND=re.match(r'\s*',BODY).group(0)
TY=r"(?:float|double|unsigned char|int|bool|CVector3|CMatrix|CVector3&|const CVector3&)"
S=[]
for l in BODY.strip('\n').split('\n'):
    s=l.strip(); m=re.match(TY+r' (\w+) = (.*)$',s)
    if m and not s.startswith('CVector3&') and not s.startswith('const'):
        S.append(s.split(' = ')[0]+';'); S.append(m.group(1)+' = '+m.group(2))
    else: S.append(s)
decl={}
for i,s in enumerate(S):
    m=re.match(TY+r' (\w+)(;| =)',s)
    if m: decl[m.group(1)]=i
def ids(s): return set(re.findall(r'\b[A-Za-z_]\w*\b',s))&set(decl)
def defs(s):
    m=re.match(TY+r' (\w+) =',s)
    if m: return {m.group(1)}
    m=re.match(r'(\w+)(?:\.[\w.]+)?\s*[-+*/]?=[^=]',s)
    if m and m.group(1) in decl: return {m.group(1)}
    m=re.match(r'(\w+)\.(\w+)\(',s)
    if m and m.group(1) in decl and m.group(2) not in ('Dot',): return {m.group(1)}
    m=re.match(r'\w+\((\w+),',s)   # free fn writing first arg
    if m and m.group(1) in decl: return {m.group(1)}
    return set()
DEP={}
for i,s in enumerate(S):
    d=[decl[n] for n in ids(s) if decl[n]!=i]
    if re.match(TY+r' \w+;$',s): DEP[i]=[]; continue
    for j in range(i):
        if re.match(TY+r' \w+;$',S[j]): continue
        a,b=defs(S[j]),defs(s)
        if (a&ids(s)) or (b&ids(S[j])) or (a&b): d.append(j)
    DEP[i]=d
for i,j in globals().get('EXTRADEP',[]): DEP[i].append(j)
