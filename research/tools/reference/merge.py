"""merge.py: true best draft per open function: table3 pick vs old real.tsv vs known hand drafts; drops discovery variants."""
import csv,sys,os,subprocess,re,json
sys.path.insert(0,'scratch/solo/ret'); import score
from concurrent.futures import ThreadPoolExecutor as TP
t=[x for x in csv.reader(open('scratch/solo/sweep/table3.tsv'),delimiter='\t') if x]
old={x[4]:x[6] for x in csv.reader(open('scratch/solo/gb/real.tsv.bak-1108'),delimiter='\t') if x and x[0].isdigit()}
oldT={x[2]:x[6] for x in csv.reader(open('scratch/solo/sweep/table3.tsv.bak-1108'),delimiter='\t') if x}
bad=('solo/ret/o','solo/rs/o')
def go(x):
    a,sz,f,tm,d=x[2],x[3],x[4],x[5],x[6]
    cands={c for c in (d,old.get(a),oldT.get(a)) if c and c!='-' and os.path.isfile(c) and not any(b in c for b in bad) and c.endswith('.cpp')}
    if not os.path.isfile(tm): return None
    best=None
    for c in sorted(cands):
        try:r=score.real(tm,open(c,errors='replace').read(),f,a,sz)
        except Exception: r=None
        if r and (best is None or r[0]<best[0]): best=(r[0],r[1],c)
    return (best[0],best[1],sz,a,f,tm,best[2]) if best else None
with TP(4) as ex: R=[r for r in ex.map(go,t) if r]
R.sort(key=lambda r:r[0])
with open('scratch/solo/ret/rank.tsv','w') as o:
    for r in R: o.write('\t'.join(map(str,r))+'\n')
print(len(R),'ranked',sum(int(r[2]) for r in R),'bytes')
for lim in (0,3,5,10,20,30,60):
    s=[r for r in R if r[0]<=lim]; print('<=',lim,len(s),sum(int(r[2]) for r in s))
