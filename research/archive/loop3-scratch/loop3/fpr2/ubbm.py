from s import run
base=open('ubb0.cpp').read()
a=base.index('        svz = sv.d.v[2];\n'); 
lines=base[a:].split('\n')
# first 20 lines after svz
blk=lines[:25]; rest='\n'.join(lines[25:])
pre=base[:a]
res=[]
for name in ['svz','saz']:
    L=[l for l in blk if not l.strip().startswith(name+' =')]
    stmt=[l for l in blk if l.strip().startswith(name+' =')][0]
    for p in range(0,16):
        LL=L[:p]+[stmt]+L[p:]
        # must be before first use
        first=min(i for i,l in enumerate(LL) if name in l)
        if not LL[first].strip().startswith(name+' ='): continue
        r=run('ubb',pre+'\n'.join(LL)+'\n'+rest)
        print(r,name,p,flush=True)
