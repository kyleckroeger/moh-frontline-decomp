"""fit.py json nmap.txt [cands]: exact simplify+colour (K=29) on our graph; score = #nodes whose colour equals the
original's (nmap single-register mappings). Then tries, for each node, (a) moving its index to every gap and
(b) inflating its degree +1..+8, and prints the best single changes."""
import json,sys,re
r=max(json.load(open(sys.argv[1])),key=lambda r:len(r['stack'])); K=29
nb0={e[0]:e[3] for e in r['stack']}; real={e[0]:e[4] for e in r['stack']}
orig={}
for l in open(sys.argv[2]):
    m=re.match(r'[! ]\s+(\d+)\s.*?ours r(\d+)\s+orig r(\d+)\s',l)
    if m: orig[int(m.group(1))]=int(m.group(3))
def run(remap={},add={}):
    f=lambda v: remap.get(v,v)
    nb={f(v):[f(u) for u in l] for v,l in nb0.items()}
    und={v:set(nb[v]) for v in nb}
    for v in nb:
        for u in nb[v]:
            if u in und: und[u].add(v)
    deg={v:len(nb[v])+add.get(v,0) for v in nb}; rem=set(nb); push=[]
    order=sorted(nb)
    while rem:
        got=False
        for v in order:
            if v in rem and deg[v]<K:
                rem.discard(v); push.append(v); got=True
                for u in nb[v]:
                    if u in deg: deg[u]-=1
        if not got:
            v=min(rem,key=lambda v:1/deg[v]); rem.discard(v); push.append(v)
            for u in nb[v]:
                if u in deg: deg[u]-=1
    vol=[0]+list(range(3,13)); obt=[]; col={}
    for v in push[::-1]:
        if real[remap_inv.get(v,v)]==0 and not nb[v]: col[v]=0; continue
        held={col[u] for u in und[v] if u in col}|{u for u in und[v] if u<32}
        c=next((x for x in sorted(vol+obt) if x not in held),None)
        if c is None: c=31-len(obt); obt.append(c)
        col[v]=c
    return sum(1 for v in orig if col.get(f(v))==orig[v])
remap_inv={}
base=run(); print('nodes mapped',len(orig),'base matches',base, 'real-check', sum(1 for v in orig if real[v]==orig[v]))
res=[]
cands=[int(x) for x in sys.argv[3].split(',')] if len(sys.argv)>3 else [v for v in orig if real[v]!=orig[v]]
idxs=sorted(nb0)
for v in cands:
    for t in [32.5]+[x+0.5 for x in idxs if x!=v]:
        remap_inv={t:v}
        s=run({v:t}); res.append((s-base,'idx',v,t))
    remap_inv={}
    for k in list(range(1,9))+[-1,-2,-3,10,15,20,30]:
        s=run({}, {v:k}); res.append((s-base,'add',v,k))
res.sort(reverse=True)
for x in res[:25]: print(x)
print([x for x in res if x[1]=="add"])
