import sys,os,itertools
os.chdir('/home/kyle/workspace/moh-frontline')
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
base=open('scratch/loop3/late2c/dx0.cpp').read()
old="""        filter = &xafilterf[*packet->in >> 4];
        packet->count -= 28;
        c0 = filter[0];
        table = &SND::xatablef[(*packet->in++ & 0xF) * 16];
        c1 = filter[4];
"""
assert old in base
T=["table = &SND::xatablef[(%s & 0xF) * 16];","table = SND::xatablef + (%s & 0xF) * 16;","table = &SND::xatablef[(%s & 0xF) << 4];","table = SND::xatablef + ((%s & 0xF) << 4);"]
res=[]
for ti,form,order in itertools.product(range(4),range(4),range(6)):
    if form==0: tl=[T[ti]%"*packet->in++"]
    elif form==1: tl=[T[ti]%"*packet->in","packet->in++;"]
    elif form==2: tl=[T[ti]%"*packet->in","packet->in += 1;"]
    else: tl=[T[ti]%"packet->in[0]","packet->in++;"]
    st=["packet->count -= 28;","c0 = filter[0];","c1 = filter[4];"]
    perms=list(itertools.permutations(range(3)))
    p=perms[order]
    # filter first, then order of three + table inserted after c0 by default (keep table between c0 and c1 if possible)
    seq=["filter = &xafilterf[*packet->in >> 4];"]
    rest=[st[i] for i in p]
    # insert table after c0
    k=rest.index("c0 = filter[0];")
    seq+=rest[:k+1]+tl+rest[k+1:]
    t=base.replace(old,''.join('        %s\n'%z for z in seq))
    r=score.real('config/GMFE69/'+'coda.json' if False else open('research/drafts/index.tsv').read() and [l.split('\t')[4] for l in open('research/drafts/index.tsv') if 'decodexac' in l][0],t,'decodexac__3SNDFPQ23SND10MXAPACKETF','80168828','400')
    s=9999 if r is None else r[0]+abs(r[1])*5
    fn='scratch/loop3/late2c/v/dx_%d%d%d.c'%(ti,form,order)
    if s<28: open(fn,'w').write(t)
    res.append((s,fn))
res.sort(); print(res[:8])
