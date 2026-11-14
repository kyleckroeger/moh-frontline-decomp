"""dhc.py KEY src 'marker' n [out]: hill-climb order of the n lines right after the line containing marker (move-one), real score."""
import sys,csv
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import score
key,src,marker,n=sys.argv[1],sys.argv[2],sys.argv[3],int(sys.argv[4])
out=sys.argv[5] if len(sys.argv)>5 else src+'.best'
row=[r for r in csv.DictReader(open(R+'research/drafts/index.tsv'),delimiter='\t') if key in r['function']][0]
occ=1
if "@@" in marker: marker,occ=marker.split("@@"); occ=int(occ)
L=open(src).read().split("\n"); i=int(marker[1:]) if marker.startswith("#") else [k for k,l in enumerate(L) if marker in l][occ-1]+1
def sc(ds):
    r=score.real(R+row['compile_settings_from'],'\n'.join(L[:i]+ds+L[i+n:]),row['function'],row['address'],row['size'])
    return (r[0]+100*abs(r[1])) if r else 10**6
cur=L[i:i+n]; best=sc(cur); print('start',best,flush=True)
imp=True
while imp and best>0:
    imp=False
    for a in range(n):
        for b in range(n):
            if a==b: continue
            c=cur[:]; x=c.pop(a); c.insert(b,x); s=sc(c)
            if s<best: best,cur,imp=s,c,True; print(best,[l.strip() for l in cur],flush=True)
open(out,'w').write('\n'.join(L[:i]+cur+L[i+n:])); print('final',best)
