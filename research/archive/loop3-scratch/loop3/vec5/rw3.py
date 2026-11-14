# hc.py which src out startmark endmark passes [forms]
# hill climb on a block: adjacent swaps of independent statements, decl swaps, per-line form toggles
import re,sys,random
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/loop3/vec5')
import sc
which,srcf,outf,sm,em,passes=sys.argv[1:7]
forms=len(sys.argv)>7 and sys.argv[7]=='1'
S={'ubb':sc.UBB,'rs':sc.RS}[which]()
src=open(srcf).read()
MARKS=[(sm,em)]+[tuple(x.split('|||')) for x in sys.argv[8:]]
def split(src):
    parts=[];pos=0;blocks=[]
    for sm_,em_ in MARKS:
        a=src.index(sm_,pos); b=src.index(em_,a); a=src.rindex('\n',0,a)+1
        while src[b-1]!='\n': b=src.rindex('\n',0,b)+1
        parts.append(src[pos:a]); blocks.append(src[a:b].rstrip('\n').split('\n')); pos=b
    parts.append(src[pos:])
    return parts,blocks
parts,BL=split(src)
TOK=re.compile(r'[A-Za-z_]\w*(?:\.d\.v\[\d\]|\[\d\]|\.[xyzwrgb]\b)?')
KW={'float','int','CVector3','Mul','Mul2','SubV','AddV','if','fabs','__fabs','Min','Max','d','v','f'}
def nt(t):
    for a,b in (('.x','[0]'),('.y','[1]'),('.z','[2]'),('.w','[3]'),('.d.v[','[')): 
        if a in t: t=t.replace(a,b)
    return t
def toks(s): return {nt(t) for t in TOK.findall(s) if t not in KW and not re.match(r'\d',t)}
def conf(A,B):
    for x in A:
        for y in B:
            if x==y or x.startswith(y+'.') or y.startswith(x+'.') or x.startswith(y+'[') or y.startswith(x+'['): return True
    return False
def du(l):
    s=l.strip()
    if not s.endswith(';') or s.startswith('if') or 'Turn' in s or s.startswith('}'): return None
    if re.match(r'(float|CVector3)\s+\w+;$',s): return 'decl'
    mm=re.match(r'(\w+)\.(Sub|Scale|Add|Set)\((.*)\);$',s)
    if mm: return ({mm.group(1)},toks(mm.group(3)))
    m=re.match(r'(?:(?:float|CVector3|int)\s+)?([\w.\[\]]+)\s*([*+-]?=)\s*(.*);$',s)
    if not m: return None
    d=toks(m.group(1)); u=toks(m.group(3))
    if m.group(2)!='=': u|=d
    if re.match(r'(float|CVector3|int)\s',s): return ('vdecl',d,u)
    return (d,u)
def ok(x,y):
    p,q=du(x),du(y)
    if p is None or q is None: return False
    if p=='decl' and q=='decl': return True
    if p=='decl' or q=='decl': return False
    if p[0]=='vdecl' or q[0]=='vdecl':
        p=p[1:] if p[0]=='vdecl' else p; q=q[1:] if q[0]=='vdecl' else q
        # a declaration with init can move but not past uses of its name
    dx,ux=p; dy,uy=q
    return not (conf(dx,uy) or conf(dy,ux) or conf(dx,dy))
def variants(l):
    s=l.rstrip(); out=[]
    m=re.match(r'(\s*(?:float\s+)?[\w.\[\]]+ = )([\w.\[\]]+) \* ([\w.\[\]]+);$',s)
    if m: out+=[m.group(1)+'Mul2(%s, %s);'%(m.group(2),m.group(3)), m.group(1)+'%s * %s;'%(m.group(3),m.group(2))]
    m=re.match(r'(\s*(?:float\s+)?[\w.\[\]]+ = )Mul2\(([\w.\[\]]+), ([\w.\[\]]+)\);$',s)
    if m: out+=[m.group(1)+'%s * %s;'%(m.group(2),m.group(3))]
    m=re.match(r'(\s*)([\w.\[\]]+) \+= ([\w.\[\]]+);$',s)
    if m: out+=[m.group(1)+'%s = %s + %s;'%(m.group(2),m.group(2),m.group(3)),m.group(1)+'%s = %s + %s;'%(m.group(2),m.group(3),m.group(2))]
    m=re.match(r'(\s*)([\w.\[\]]+) = ([\w.\[\]]+) \+ ([\w.\[\]]+);$',s)
    if m:
        out+=[m.group(1)+'%s = %s + %s;'%(m.group(2),m.group(4),m.group(3))]
        if m.group(2) in (m.group(3),m.group(4)):
            o=m.group(4) if m.group(2)==m.group(3) else m.group(3)
            out+=[m.group(1)+'%s += %s;'%(m.group(2),o)]
    m=re.match(r'(\s*)([\w.\[\]]+) = ([\w.\[\]]+) - ([\w.\[\]]+);$',s)
    if m: out+=[m.group(1)+'%s = SubV(%s, %s);'%(m.group(2),m.group(3),m.group(4))]
    m=re.match(r'(\s*)([\w.\[\]]+) = SubV\(([\w.\[\]]+), ([\w.\[\]]+)\);$',s)
    if m: out+=[m.group(1)+'%s = %s - %s;'%(m.group(2),m.group(3),m.group(4))]
    m=re.match(r'(\s*)([\w.\[\]]+) = (Min|Max)\(([\w.\[\]]+), ([\w.\[\]]+)\);$',s)
    if m: out+=[m.group(1)+'%s = %s %s %s ? %s : %s;'%(m.group(2),m.group(4),'<' if m.group(3)=='Min' else '>',m.group(5),m.group(4),m.group(5))]
    m=re.match(r'(\s*)([\w.\[\]]+) = ([\w.\[\]]+) ([<>]) ([\w.\[\]]+) \? \3 : \5;$',s)
    if m: out+=[m.group(1)+'%s = %s(%s, %s);'%(m.group(2),'Min' if m.group(4)=='<' else 'Max',m.group(3),m.group(5))]
    for a_,b_ in (('sa.d.v[2]','saz'),('sv.d.v[2]','svz'),('Mul(t2, sa.d.v, 2)','Mul2(t2, saz)'),('Mul(t2, sa.d.v, 0)','Mul2(t2, sa.d.v[0])'),('Mul(t2, sa.d.v, 1)','Mul2(t2, sa.d.v[1])')):
        if a_ in s and not s.strip().startswith(('saz =','svz =')): out.append(s.replace(a_,b_))
        if b_ in s and not s.strip().startswith(('saz =','svz =')) and not s.strip().startswith('float'): out.append(s.replace(b_,a_))
    return out
def build(BL):
    o=''
    for p,b in zip(parts,BL): o+=p+'\n'.join(b)+'\n'
    return o+parts[-1]

import random
best=S(build(BL)); cur=best; print('base',best,flush=True)
N=int(passes)
for it in range(N):
    bi=random.randrange(len(BL)); lines=BL[bi]
    r=random.random()
    if r<0.5:
        i=random.randrange(len(lines)-1)
        if not ok(lines[i],lines[i+1]): continue
        ls=lines[:]; ls[i],ls[i+1]=ls[i+1],ls[i]
    elif r<0.8:
        i=random.randrange(len(lines)); j=random.randrange(len(lines))
        if abs(i-j)<2: continue
        ls=lines[:]; x=ls.pop(i)
        rng=range(j,i) if j<i else range(i,j)
        if not all(ok(x,ls[k]) if j<i else ok(ls[k],x) for k in rng): continue
        ls.insert(j,x)
    else:
        if not forms: continue
        i=random.randrange(len(lines)); v=variants(lines[i])
        if not v: continue
        ls=lines[:]; ls[i]=random.choice(v)
    NB=BL[:]; NB[bi]=ls
    sc_=S(build(NB))
    if sc_<=cur:
        cur=sc_; BL=NB
        if sc_<best:
            best=sc_; print('it',it,best,flush=True); open(outf,'w').write(build(BL))
print('final',best)
