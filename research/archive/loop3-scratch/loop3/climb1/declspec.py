# decl-only climb: BLOCKFILE; lines that are pure declarations 'T name;' (indent kept) move to any line position at the SAME indent
# with the line before it at >= indent... simplified: decl may only move within its original brace depth region.
import re
raw=[l for l in open(BLOCKFILE).read().rstrip('\n').split('\n')]
BLOCK='\n'.join(raw)+'\n'; IND=''
S=raw[:]   # keep full lines incl indent
dec={}
for i,s in enumerate(S):
    m=re.match(r'^\s*(?:const\s+|unsigned\s+)?[A-Za-z_][\w:<>]*(?:\s*\*+\s*|\s+)(\w+);$',s)
    if m and not re.match(r'^\s*(return|break|continue|delete|goto)\b',s): dec[i]=m.group(1)
# depth region id per line
reg=[]; aft=[]; rid=0; stack=[0]
for s in S:
    reg.append(stack[-1])
    for ch in s:
        if ch=='{': rid+=1; stack.append(rid)
        elif ch=='}': stack.pop()
    aft.append(stack[-1])
AFT=dict(enumerate(aft))
DEP={}; prev=None; REG=dict(enumerate(reg))
for i,s in enumerate(S):
    if i in dec: DEP[i]=[]; continue
    d=[j for j,n in dec.items() if j<i and re.search(r'\b%s\b'%n,s)]
    if prev is not None: d.append(prev)
    DEP[i]=d; prev=i
