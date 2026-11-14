# hc.py which src out startmark endmark passes [forms]
# hill climb on a block: adjacent swaps of independent statements, decl swaps, per-line form toggles
import re,sys,random
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/loop3/vec3')
import sc
which,srcf,outf,sm,em,passes=sys.argv[1:7]
forms=len(sys.argv)>7 and sys.argv[7]=='1'
S={'ubb':sc.UBB,'rs':sc.RS}[which]()
src=open(srcf).read()
a=src.index(sm); b=src.index(em,a)
a=src.rindex('\n',0,a)+1
pre,blk,post=src[:a],src[a:b],src[b:]
lines=blk.rstrip('\n').split('\n')
post='\n'+post if not blk.endswith('\n') else post
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
    if 'Random' in x and 'Random' in y: return False
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
    for mm in re.finditer(r'\b(\w+)(\[([0-3])\]|\.([xyzw])\b)',s):
        if mm.group(1) in ('d','v','m','w','t') or mm.group(1).startswith('c'): continue
        rep=mm.group(1)+('.'+'xyzw'[int(mm.group(3))] if mm.group(3) else '[%d]'%'xyzw'.index(mm.group(4)))
        out.append(s[:mm.start()]+rep+s[mm.end():])
    return out
def build(ls): return pre+'\n'.join(ls)+'\n'+post

import random
best=S(build(lines)); cur=best; print('base',best,flush=True)
N=int(passes)
for it in range(N):
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
    sc_=S(build(ls))
    if sc_<=cur:
        cur=sc_; lines=ls
        if sc_<best:
            best=sc_; print('it',it,best,flush=True); open(outf,'w').write(build(lines))
print('final',best)
