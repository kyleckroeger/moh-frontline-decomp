"""sim2.py json rec K: exact simplify replica of 0x507b50 (degree counter = len(list), decremented via
removed nodes' lists -> asymmetric lists after coalescing keep survivors' degrees high). Prints sweep of
callee nodes, stale-edge counts, and whether push order == real."""
import json,sys
d=json.load(open(sys.argv[1])); r=d[int(sys.argv[2])] if sys.argv[2]!='max' else max(d,key=lambda r:len(r['stack'])); K=int(sys.argv[3])
nm={x[1]:x[2] for x in r['names']}
nb={e[0]:e[3] for e in r['stack']}; real={e[0]:e[4] for e in r['stack']}
realpush=[e[0] for e in r['stack']][::-1]
deg={v:len(nb[v]) for v in nb}; rem=set(nb); push=[]; sw={}; sweep=0
def remove(v):
    rem.discard(v); push.append(v)
    for u in nb[v]:
        if u in deg: deg[u]-=1
while rem:
    sweep+=1; got=False
    print('  sweep',sweep,' '.join('%s:%d'%(nm.get(v,v),deg[v]) for v in sorted(rem) if real[v]>=26))
    for v in sorted(rem):
        if deg[v]<K: remove(v); got=True; sw[v]=sweep
    if not got:
        v=min(rem,key=lambda v:1/deg[v]); remove(v); sw[v]=-sweep
print('K',K,'match real push:',push==realpush)
for v in push[::-1]:
    if real[v]>=13:
        back=sum(1 for u in nb if v in nb[u]); vir=sum(1 for x in nb[v] if x>=32)
        print(' %d %-10s sweep %d r%d stale %d'%(v,nm.get(v,''),sw[v],real[v],vir-back))
