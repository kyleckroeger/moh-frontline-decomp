# hill.py <src> <tmpl> <fn> <declblockfile>: move/swap hill-climb of decl lines, fast.py score
import sys,os,json,itertools,tempfile
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game')
import fast
src,tmpl,fn,blk=sys.argv[1:5]
s=open(src).read(); old=open(blk).read(); assert old in s
d=old.strip('\n').split('\n'); T=json.load(open(tmpl))
W=tempfile.mkdtemp(dir=os.environ.get('W','/tmp'))
def score(o):
    r=fast.score(T,s.replace(old,"\n".join(o)+"\n"),[(fn,)],W); return r.get(fn,9999) if r else 9999
cur=list(d); best=score(cur); print('start',best,flush=True)
imp=True
while imp and best>0:
    imp=False
    cands=[]
    for i in range(len(cur)):
        for j in range(len(cur)):
            if i!=j: c=cur[:]; x=c.pop(i); c.insert(j,x); cands.append(c)
    for i,j in itertools.combinations(range(len(cur)),2):
        c=cur[:]; c[i],c[j]=c[j],c[i]; cands.append(c)
    for c in cands:
        sc=score(c)
        if sc<best:
            best=sc; cur=c; imp=True; print(best,flush=True); print('\n'.join(cur),flush=True); break
open(src+'.hill','w').write(s.replace(old,'\n'.join(cur)+'\n'))
print('final',best)
