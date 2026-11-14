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
    return out
def build(ls): return pre+'\n'.join(ls)+'\n'+post
best=S(build(lines)); print('base',best,flush=True)
for it in range(int(passes)):
    imp=False
    moves=[('s',i) for i in range(len(lines)-1)]+[('m',(i,j)) for i in range(len(lines)) for j in range(len(lines)) if abs(i-j)>1]
    if forms: moves+=[('f',i) for i in range(len(lines))]
    random.shuffle(moves)
    for kind,i in moves:
        cands=[]
        if kind=='s':
            if not ok(lines[i],lines[i+1]): continue
            ls=lines[:]; ls[i],ls[i+1]=ls[i+1],ls[i]; cands=[ls]
        elif kind=='m':
            i,j=i
            ls=lines[:]; x=ls.pop(i)
            rng=range(j,i) if j<i else range(i,j)
            if all(ok(x,ls[k]) if j<i else ok(ls[k],x) for k in rng):
                ls.insert(j,x); cands=[ls]
        else:
            for v in variants(lines[i]):
                ls=lines[:]; ls[i]=v; cands.append(ls)
        for ls in cands:
            r=S(build(ls))
            if r<best:
                best,lines=r,ls; imp=True; print('it',it,kind,i,best,flush=True)
                open(outf,'w').write(build(lines))
                break
    if not imp: break
print('final',best)
