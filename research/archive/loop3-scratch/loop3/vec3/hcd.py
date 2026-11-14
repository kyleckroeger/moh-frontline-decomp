# hcd.py which src out startmark endmark passes : insertion moves of 'float x;' decl lines within the block
import re,sys
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/loop3/vec3')
import sc
which,srcf,outf,sm,em,passes=sys.argv[1:7]
S={'ubb':sc.UBB,'rs':sc.RS}[which]()
src=open(srcf).read()
a=src.index(sm); b=src.index(em,a); a=src.rindex('\n',0,a)+1
pre,blk,post=src[:a],src[a:b],src[b:]
lines=blk.split('\n')
idx=[i for i,l in enumerate(lines) if re.match(r'\s*float \w+;$',l)]
decls=[lines[i] for i in idx]
def build(d):
    ls=lines[:]
    for i,x in zip(idx,d): ls[i]=x
    return pre+'\n'.join(ls)+post
best=S(build(decls)); print('base',best,flush=True)
for it in range(int(passes)):
    imp=False
    for i in range(len(decls)):
        for j in range(len(decls)):
            if i==j: continue
            d=decls[:]; x=d.pop(i); d.insert(j,x)
            r=S(build(d))
            if r<best:
                best,decls=r,d; imp=True; print('it',it,x.strip(),'->',j,best,flush=True)
                open(outf,'w').write(build(decls))
    if not imp: break
print('final',best)
