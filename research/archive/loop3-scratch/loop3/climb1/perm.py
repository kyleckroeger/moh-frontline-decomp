# perm.py KEY file.cpp blockfile [EXT]: exhaustive permutations of the block's lines (decl groups)
import sys,csv,itertools
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import score
key,f,bf=sys.argv[1:4]
row=[r for r in csv.DictReader(open(R+'research/drafts/index.tsv'),delimiter='\t') if key in r['function']][0]
t=open(f).read(); B=open(bf).read(); assert B in t
L=B.rstrip('\n').split('\n'); res={}
for p in itertools.permutations(L):
    s=t.replace(B,'\n'.join(p)+'\n',1)
    r=score.real(R+row['compile_settings_from'],s,row['function'],row['address'],row['size'])
    res[p]=r[0] if r else 999
best=min(res.values()); print('best',best,'of',len(res),sorted(set(res.values()))[:8])
for p,v in res.items():
    if v==best: print([x.strip() for x in p]); break
