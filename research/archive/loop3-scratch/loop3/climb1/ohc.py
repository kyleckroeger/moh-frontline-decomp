# ohc.py KEY base.cpp spec.py seed N : hill-climb statement order. spec defines BLOCK (exact text), S (list of stmt strings, no indent), DEP {i:[j..]} (i after j), IND
import sys,csv,random
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import score
key,base,spec,seed,N=sys.argv[1],sys.argv[2],sys.argv[3],int(sys.argv[4]),int(sys.argv[5])
row=[r for r in csv.DictReader(open(R+'research/drafts/index.tsv'),delimiter='\t') if key in r['function']][0]
text=open(base).read(); ns={}; exec(open(spec).read(),ns)
BLOCK,S,DEP,IND=ns['BLOCK'],ns['S'],ns['DEP'],ns.get('IND','    ')
if ns.get("SPLIT",1): pass
PRE=ns.get('PRE',[])
for o,n in PRE: assert o in text,o[:50]; text=text.replace(o,n,1)
assert BLOCK in text
def valid(o):
    pos={x:k for k,x in enumerate(o)}
    if not all(pos[i]>pos[j] for i,js in DEP.items() for j in js): return False
    if REG:
        # a moved decl must sit right after a line of its own region (or region opener)
        for k,x in enumerate(o):
            if x in DEC and REG[x]!=(AFT[o[k-1]] if k>0 else 0): return False
    return True
def build(o): return text.replace(BLOCK,''.join(IND+S[i]+'\n' for i in o),1)
REG=ns.get('REG'); DEC=ns.get('dec',{}); AFT=ns.get('AFT',{})
def sc(o):
    r=score.real(R+row['compile_settings_from'],build(o),row['function'],row['address'],row['size']); return r[0] if r else 999
rng=random.Random(seed); cur=list(range(len(S))); assert valid(cur); best=sc(cur); print('start',best,flush=True)
out=spec.replace('.py','_best%d'%seed+ns.get('EXT','.cpp')); seen={tuple(cur):best}
for k in range(N):
    o=list(cur); i,j=rng.sample(range(len(o)),2); o.insert(j,o.pop(i))
    if not valid(o) or tuple(o) in seen: continue
    v=sc(o); seen[tuple(o)]=v
    if v<=best:
        if v<best: print(k,v,[S[x][:30] for x in o],flush=True); open(out,'w').write(build(o))
        best=v; cur=o
print("end",best,"scored",len(seen),sorted(set(seen.values()))[:10])
