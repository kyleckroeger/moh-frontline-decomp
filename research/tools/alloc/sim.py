"""sim.py key [K] [cls]: replay simplify on the last trace record holding key; compare push order with the real stack."""
import json,sys
key=sys.argv[1]; K=int(sys.argv[2]) if len(sys.argv)>2 else 0; cls=int(sys.argv[3]) if len(sys.argv)>3 else 4
d=json.load(open('/tmp/claude-1000/-home-kyle-workspace-moh-frontline/e1f4a78d-97e2-4c48-9222-8d4f5cde9463/scratchpad/tr1.json'))
for r in d:
    if r['cls']!=cls: continue
    nm={x[1]:x[2] for x in r['names']}
    if key not in nm.values(): continue
    nb={e[0]:set(e[3]) for e in r['stack']}
    real=[e[0] for e in r['stack']][::-1]
    def run(K,trace=False):
        rem=set(nb); push=[]; sweep=0; info={}
        while rem:
            sweep+=1; got=False
            for v in sorted(rem):
                deg=sum(1 for x in nb[v] if x<32 or x in rem)
                if deg<K: rem.discard(v); push.append(v); got=True; info[v]=(sweep,deg)
            if not got:
                v=max(rem); rem.discard(v); push.append(v); info[v]=(sweep,'stuck')
        return push,info
    for k in ([K] if K else range(20,33)):
        p,info=run(k)
        if p==real or K: print('K',k,'match' if p==real else 'nomatch')
        if K:
            for v in p[::-1]:
                if v in nm or len(nb[v])>14: print(v,nm.get(v,'?'),info[v],len(nb[v]))
