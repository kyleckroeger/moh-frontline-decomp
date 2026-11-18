"""nmap.py <src> <func> [F|R] : exact node -> instruction map via the compiler itself (tr.py MARK recolouring), then
each node's ORIGINAL register (operand-signature match against the original asm), in colour (pop) order.
env: REC=n overall record index (default: auto), UNIT/ADR/SIZE when the function has no index.tsv row, TAG temp name.
Prints a solver result: whether some pop order gives the original colours on our graph, and our-graph edges whose
two ends share an original colour (edges the original graph cannot have)."""
import sys,os,json,subprocess,re,collections,csv,difflib
ROOT='/home/kyle/workspace/moh-frontline'; HERE=ROOT+'/scratch/loop4/tr2'; TMP=ROOT+'/scratch/loop4/tr4/tmp'
sys.path.insert(0,ROOT+'/research/tools'); import fast
src,func=sys.argv[1],sys.argv[2]; CL=sys.argv[3] if len(sys.argv)>3 else 'F'
pre='f' if CL=='F' else 'r'; CLS=3 if CL=='F' else 4
row=[r for r in csv.DictReader(open(ROOT+'/research/drafts/index.tsv'),delimiter='\t') if r['function']==func]
if row: unit=os.path.basename(row[0]['compile_settings_from'])[:-5]; adr=int(row[0]['address'],16); size=int(row[0]['size'])
else: unit,adr,size=None,0,0
unit=os.environ.get('UNIT',unit); adr=int(os.environ.get('ADR',hex(adr)),16); size=int(os.environ.get('SIZE',size))
tag=os.environ.get('TAG','n')
def trace(mark,name):
    js=f'{TMP}/{tag}_{name}.json'; obj=f'{TMP}/{tag}_{name}.o'
    env=dict(os.environ,TRO=obj,MARK=mark)
    subprocess.run(['bash','-c',f'ulimit -v 3000000; timeout 600 python3 {HERE}/tr.py {src} {unit} {js}'],check=True,env=env)
    return json.load(open(js)),obj
RX=r'\b([rf]\d+)\b'
def dis(args):
    out=subprocess.run([fast.OBJDUMP,'-d']+args,capture_output=True,text=True).stdout; ins=[]
    for l in out.splitlines():
        m=re.match(r'^\s*([0-9a-f]+):\t[0-9a-f ]+\t(\S+)\s*(.*)$',l)
        if m: ins.append((int(m.group(1),16),m.group(2),re.sub(r'-?\d+\(r2\)','0(0)',m.group(3).split('<')[0].strip())))
    return ins
d0,o0=trace('','u')
ours=dis(['--disassemble='+func,o0])
orig=dis([f'--start-address={adr:#x}',f'--stop-address={adr+size:#x}',fast.ELF]) if adr else ours
used=collections.Counter(int(t[1:]) for _,op,a in ours for t in re.findall(RX,a) if t[0]==pre)
# record choice
cands=[i for i,r in enumerate(d0) if r['cls']==CLS]
def fit(i):
    cs=collections.Counter(e[4] for e in d0[i]['stack'])
    return len(set(cs)-set(used))*100+len(set(used)-set(cs)-set(range(0,14) if CL=='F' else {0,1,2,3,4,5,6,7,8,9,10,11,12,13}))
R=int(os.environ['REC']) if os.environ.get('REC') else min(cands,key=fit)
rec=d0[R]; st=rec['stack']; nm={x[1]:x[2] for x in rec['names']}
col={e[0]:e[4] for e in st}; nb={e[0]:set(e[3]) for e in st}
excl={1,2,13} if CL=='R' else set()
markers=[x for x in range(31,-1,-1) if x not in used and x not in col.values() and x not in excl and x not in (0,)]
DIFF=len(markers)<6 or os.environ.get('DIFFMARK')
if DIFF: markers=[x for x in range(31,0,-1) if x not in excl]
idxs=[e[0] for e in st]
occ=collections.defaultdict(list)  # node -> [(ins index, operand pos)]
at={}
todo=list(idxs)
while todo:
    mk={}; usedm=set()
    for i in list(todo):
        m=next((x for x in markers if x not in usedm and x!=col[i]),None)
        if m is None: continue
        mk[i]=m; usedm.add(m)
        if len(mk)>=len(markers)-1: break
    for i in mk: todo.remove(i)
    _,ob=trace(f'{R}@'+','.join(f'{i}:{m}' for i,m in mk.items()),'m')
    mi=dis(['--disassemble='+func,ob]); back={m:i for i,m in mk.items()}
    sm=difflib.SequenceMatcher(None,[x[1] for x in ours],[x[1] for x in mi],autojunk=False)
    for tg,i1,i2,j1,j2 in sm.get_opcodes():
        if tg!='equal': continue
        for k in range(i2-i1):
            ta=re.findall(RX,mi[j1+k][2]); to=re.findall(RX,ours[i1+k][2])
            for p,t in enumerate(ta):
                if t[0]==pre and int(t[1:]) in back and (not DIFF or (p<len(to) and to[p]!=t)):
                    occ[back[int(t[1:])]].append((i1+k,p)); at[(i1+k,p)]=back[int(t[1:])]
# signatures
INTERN={}
def dus(op,toks):
    if op.startswith(('st','psq_st','cmp','fcmp','tw','mt')) or op.startswith('b'): return False
    return True
COMM=('fadds','fadd','fmuls','fmul','add','and','or','xor','mullw')
def sigs(L):
    last={}; out=[]; ncall=0
    for i,(a,op,args) in enumerate(L):
        toks=re.findall(RX,args)
        if op in('bl','bctrl'):
            ncall+=1; s=INTERN.setdefault(('call',ncall),len(INTERN))
            for r in ['f%d'%k for k in range(14)]+['r0']+['r%d'%k for k in range(3,13)]: last[r]=('callret',r,s)
            out.append((s,[])); continue
        pat=re.sub(RX,'R',args)
        if op.startswith('b'): pat=re.sub(r'[0-9a-f]+$','T',pat)
        dfirst=bool(toks) and dus(op,toks)
        ids=['D' if (k==0 and dfirst) else last.get(t,('in',t)) for k,t in enumerate(toks)]
        key=list(ids)
        if op in COMM and len(ids)==3: key=[ids[0]]+sorted(ids[1:],key=repr)
        if op in('fmadds','fmsubs','fnmadds','fnmsubs','fmadd','fmsub') and len(ids)==4: key=[ids[0]]+sorted(ids[1:3],key=repr)+[ids[3]]
        s=INTERN.setdefault((op,pat,tuple(key)),len(INTERN))
        if dfirst: last[toks[0]]=('def',s,0)
        if op.startswith(('lwzu','lfsu','stwu','stfsu','lbzu','lhzu','lwzux','stwux')) and len(toks)>1: last[toks[1]]=('def',s,1)
        out.append((s,ids))
    return out
S1=sigs(ours); S2=sigs(orig)
occ2=collections.defaultdict(list)
for j,(s,_) in enumerate(S2): occ2[s].append(j)
seen=collections.Counter(); imap={}
for i,(s,_) in enumerate(S1):
    k=seen[s]; seen[s]+=1
    if k<len(occ2[s]): imap[i]=occ2[s][k]
def origreg(i,p):
    j=imap.get(i)
    if j is None: return None
    ta=re.findall(RX,ours[i][2]); tb=re.findall(RX,orig[j][2])
    if len(ta)!=len(tb): return None
    ida=S1[i][1]; idb=S2[j][1]
    if ida[p]=='D' or ida.count(ida[p])>1 or idb[p]==ida[p]: return tb[p]
    for q in range(len(idb)):
        if idb[q]==ida[p]: return tb[q]
    return tb[p]
def oreg(n):
    rs=collections.Counter(origreg(i,p) for i,p in occ[n]); rs.pop(None,None)
    return rs
def isvol(x): return x<14 if CL=='F' else (x==0 or 3<=x<=12)
print(f'{func} {unit} record {R}: {len(st)} nodes, {len(markers)} markers; pop order (first popped first)')
tgt={}; mism=0
for e in st:
    n=e[0]; rs=oreg(n); ours_r=f'{pre}{col[n]}'
    o='/'.join(sorted(rs)) if rs else ('-' if not occ[n] else '?')
    if len(rs)==1: tgt[n]=int(next(iter(rs))[1:])
    flag='!' if rs and o!=ours_r else ' '; mism+=flag=='!'
    first=min(occ[n])[0] if occ[n] else None
    w=f'{ours[first][0]:x} {ours[first][1]} {ours[first][2]}' if first is not None else '(no operand)'
    print(f'{flag} {n:4} {nm.get(n,""):10} ours {ours_r:4} orig {o:8} k{e[2]:<3} uses{len(occ[n]):<3} {w}')
print('mismatching nodes:',mism)
nodes=[e[0] for e in st if e[0] in tgt]
def need(v,ps):
    c=tgt[v]; held={tgt[x] for x in nb[v] if x in ps}|{x for x in nb[v] if x<32}
    if c in held: return None
    if isvol(c): lower=[x for x in range(c) if isvol(x) and x not in excl]
    else:
        obt={tgt[x] for x in ps if not isvol(tgt[x])}
        lower=[x for x in range(32) if isvol(x) and x not in excl]+[x for x in obt if x<c]
    return [x for x in lower if x not in held]
placed=[]; ps=set(); left=list(nodes)
while left:
    ok=[v for v in left if need(v,ps)==[]]
    if not ok: print('STUCK; remaining:',' '.join(f'{v}{nm.get(v,"")}={pre}{tgt[v]}:{"CONFLICT" if need(v,ps) is None else need(v,ps)}' for v in left)); break
    v=max(ok); placed.append(v); ps.add(v); left.remove(v)
print('greedy feasible prefix:',' '.join(f'{v}{nm.get(v,"")}:{pre}{tgt[v]}' for v in placed))
print('our-graph edges joining two nodes of one orig colour:')
for v in tgt:
    for u in nb[v]:
        if u in tgt and u<v and tgt[u]==tgt[v]: print(f'   {v}{nm.get(v,"")} -- {u}{nm.get(u,"")} both {pre}{tgt[v]}')
if os.environ.get('WHY'):
    pos={e[0]:k for k,e in enumerate(st)}
    print('requirements of mismatching nodes (colour: neighbours holding it in orig; * = popped before v in OUR order):')
    for e in st:
        v=e[0]
        if v not in tgt or tgt[v]==col[v]: continue
        c=tgt[v]; lows=[x for x in range(c) if isvol(x) and x not in excl] if isvol(c) else [x for x in range(14)]
        parts=[]
        for x in lows:
            hs=[u for u in nb[v] if tgt.get(u)==x]
            parts.append(f'{pre}{x}:'+(','.join(f'{u}{"*" if pos[u]<pos[v] else ""}' for u in hs) or 'NONE'))
        print(f'  {v}{nm.get(v,"")} -> {pre}{c}: '+' '.join(parts))
if os.environ.get('ORDER'):
    pri=[int(x) for x in os.environ['ORDER'].split(',')]
    base=[e[0] for e in st]; it=iter(pri); seq=[next(it) if v in pri else v for v in base]
    got={}
    for v in seq:
        held={got[u] for u in nb[v] if u in got}|{u for u in nb[v] if u<32}
        if v in tgt and not isvol(tgt[v]): got[v]=tgt[v]; continue
        got[v]=min(x for x in range(32) if isvol(x) and x not in excl and x not in held)
    bad=[f'{v}{nm.get(v,"")}:{pre}{got[v]}/{pre}{tgt[v]}' for v in seq if v in tgt and got[v]!=tgt[v]]
    print('ORDER sim mismatches',len(bad),' '.join(bad))
if os.environ.get('PERM'):
    import itertools
    grp=[int(x) for x in os.environ['PERM'].split(',')]; rest=[int(x) for x in os.environ.get('REST','').split(',') if x]
    base=[e[0] for e in st]; okp=[]
    for p in itertools.permutations(grp):
        pri=list(p)+rest; it=iter(pri); seq=[next(it) if v in pri else v for v in base]; got={}; bad=0
        for v in seq:
            held={got[u] for u in nb[v] if u in got}|{u for u in nb[v] if u<32}
            if v in tgt and not isvol(tgt[v]): got[v]=tgt[v]; continue
            got[v]=min(x for x in range(32) if isvol(x) and x not in excl and x not in held)
            if v in tgt and got[v]!=tgt[v]: bad=1; break
        if not bad: okp.append(p)
    print('PERM feasible',len(okp))
    for a in grp:
        for b in grp:
            if a!=b and all(p.index(a)<p.index(b) for p in okp): print(f'  always {a}{nm.get(a,"")} before {b}{nm.get(b,"")}')
if os.environ.get('ISIM'):
    # search index orders (simplify sweeps with K + lowest-free colouring) reproducing the orig colours
    import random
    K=int(os.environ['ISIM']); nodes=[e[0] for e in st]; phys=lambda x:x<32
    def colour(rank):
        rem=set(nodes); push=[]
        while rem:
            got=False
            for v in sorted(rem,key=lambda v:rank[v]):
                deg=sum(1 for x in nb[v] if phys(x) or x in rem)
                if deg<K: rem.discard(v); push.append(v); got=True
            if not got:
                v=max(rem,key=lambda v:rank[v]); rem.discard(v); push.append(v)
        got={}
        for v in push[::-1]:
            if v in tgt and not isvol(tgt[v]): got[v]=tgt[v]; continue
            held={got[u] for u in nb[v] if u in got}|{u for u in nb[v] if u<32}
            fr=[x for x in range(32) if isvol(x) and x not in excl and x not in held]; got[v]=fr[0] if fr else max(x for x in range(32) if x not in held and x not in excl)
        return got
    def cost(rank):
        g=colour(rank); return sum(1 for v in tgt if g[v]!=tgt[v])
    rank={v:v for v in nodes}
    for kv in os.environ.get('SETR','').split(','):
        if kv: a,b=kv.split(':'); rank[int(a)]=float(b)
    c0=cost(rank); print('ISIM K',K,'start cost',c0)
    best=c0; random.seed(int(os.environ.get('SEED','1'))); movable=[int(x) for x in os.environ.get('MOV','').split(',') if x] or nodes
    for it in range(int(os.environ.get('ITER','4000'))):
        r=dict(rank); v=random.choice(movable); r[v]=random.uniform(min(nodes)-1,max(nodes)+1)
        c=cost(r)
        if c<=best:
            if c<best: print(' it',it,'cost',c,flush=True)
            best=c; rank=r
        if best==0: break
    print('ISIM best',best,'order (low index first):',' '.join(f'{v}{nm.get(v,"")}' for v in sorted(nodes,key=lambda v:rank[v]) if v in movable or True))
