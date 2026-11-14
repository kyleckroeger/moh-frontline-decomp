# thc.py KEY file startline endline seed N [split]: hill-climb the order of lines start..end (1-based, inclusive)
# deps by identifiers: a line using X must follow the line defining X (decl or assignment of a float local).
# split=1: 'T x = e;' -> 'T x;' + 'x = e;' tokens (decls float freely before use).
import sys,csv,re,random
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import score
row=[r for r in csv.DictReader(open(R+'research/drafts/index.tsv'),delimiter='\t') if sys.argv[1] in r['function']][0]
L=open(sys.argv[2]).read().split('\n'); s,e=int(sys.argv[3])-1,int(sys.argv[4]); seed=int(sys.argv[5]); N=int(sys.argv[6]); split=len(sys.argv)>7 and sys.argv[7]=='1'
ind=re.match(r'\s*',L[s]).group(0)
toks=[]
for l in L[s:e]:
    m=re.match(r'\s*(float|int|double|CVector3|const CVector3\*|bool) (\w+) = (.*);$',l)
    if split and m: toks.append(('%s %s;'%(m.group(1),m.group(2)))); toks.append('%s = %s;'%(m.group(2),m.group(3)))
    else: toks.append(l.strip())
def defs(t):
    m=re.match(r'(?:float|int|double|CVector3|const CVector3\*|bool) (\w+)',t)
    if m: return m.group(1)
    return None
declared={defs(t) for t in toks if defs(t)}
def ids(t): return set(re.findall(r'\b[A-Za-z_]\w*\b',t))
def assigned(t):
    m=re.match(r'([A-Za-z_]\w*(?:\.\w+)*(?:\.d\.c\.\w|\[\d\])*)\s*[-+*/]?=',t); return m.group(1) if m else None
def valid(o):
    seen=set(); 
    for t in o:
        d=defs(t)
        if d and not t.endswith(');') and ' = ' not in t: seen.add('decl:'+d); continue
        need=ids(t)&declared
        if d: need.discard(d)
        a=assigned(t)
        if a and re.match(r'[\w.\[\]]+\s*=[^=]',t) and a in declared: need.discard(a)
        if a and '.' in a and a.split('.')[0] in declared:
            if 'decl:'+a.split('.')[0] not in seen: return False
            need.discard(a.split('.')[0])
        for v in need:
            if v not in seen: return False
        a=assigned(t)
        if a and a in declared and 'decl:'+a not in seen: return False
        if d: seen.add(d); seen.add('decl:'+d)
        elif a: seen.add(a.split('.')[0])
    return True
def build(o): return '\n'.join(L[:s]+[ind+t for t in o]+L[e:])
def sc(o):
    r=score.real(R+row['compile_settings_from'],build(o),row['function'],row['address'],row['size']); return r[0] if r else 999
rng=random.Random(seed); cur=toks; assert valid(cur); best=sc(cur); print('start',best,flush=True)
out=sys.argv[2].replace('.cpp','_thc%d.cpp'%seed)
for k in range(N):
    o=list(cur); i,j=rng.sample(range(len(o)),2); o.insert(j,o.pop(i))
    if not valid(o): continue
    v=sc(o)
    if v<=best:
        if v<best: print(k,v,flush=True); open(out,'w').write(build(o))
        best=v; cur=o
print('end',best)
