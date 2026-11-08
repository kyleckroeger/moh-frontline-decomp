import subprocess,sys,json,os
# hill.py <src> <tmpl> <fn> <start> <end> <declblockfile> [SUFFIX]
src,tmpl,fn,st,en,blk=sys.argv[1:7]
suffix=sys.argv[7] if len(sys.argv)>7 else 'cpp'
s=open(src).read(); old=open(blk).read()
assert old in s
d=old.strip('\n').split('\n')
def score(order):
    open('<temp>'%os.getpid()+suffix,'w').write(s.replace(old,'\n'.join(order)+'\n'))
    r=subprocess.run(['python3','scratch/par/parks/ssr.py',tmpl,'<temp>'%os.getpid()+suffix,fn,st,en],capture_output=True,text=True,cwd='<repo>',env={'SUFFIX':suffix,'PATH':'/usr/bin:/bin'}).stdout.split()
    try: return int(r[1])
    except Exception: return 9999
cur=list(d); best=score(cur); print('start',best,flush=True)
improved=True
while improved and best>0:
    improved=False
    for i in range(len(cur)):
        for j in range(len(cur)):
            if i==j: continue
            c=cur[:]; x=c.pop(i); c.insert(j,x)
            sc=score(c)
            if sc<best:
                best=sc; cur=c; improved=True; print(best,[y.split()[-1] for y in cur],flush=True)
                break
        if improved: break
    if not improved:
        import itertools
        for i,j in itertools.combinations(range(len(cur)),2):
            c=cur[:]; c[i],c[j]=c[j],c[i]
            sc=score(c)
            if sc<best:
                best=sc; cur=c; improved=True; print('swap',best,[y.split()[-1] for y in cur],flush=True); break
print('final',best); print('\n'.join(cur))
