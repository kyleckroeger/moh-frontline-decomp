import sys,itertools
sys.path.insert(0,'scratch/loop3/big1'); import sc
base=open('scratch/loop3/big2/base.cpp').read()
S=['front = body.GetFront();','flat = dir;','flat.z = 0.0f;','front.z = 0.0f;','front.Normalize();','flat.Normalize();']
dep={2:[1],3:[0],4:[3],5:[2]}
O=''.join('                    %s\n'%s for s in S)
assert base.count(O)==1
seen=0
for p in itertools.permutations(range(6)):
    pos={v:i for i,v in enumerate(p)}
    if any(pos[a]<pos[b] for a,bs in dep.items() for b in bs): continue
    t=base.replace(O,''.join('                    %s\n'%S[i] for i in p))
    m=sc.asm(t)
    if not m: print(p,'FAIL'); continue
    print(p,*sc.regions(m),flush=True)
