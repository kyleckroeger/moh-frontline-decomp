import sys,itertools,re
sys.path.insert(0,'.'); import sc
S=sc.UBB()
s=open(sys.argv[1]).read()
a=s.index('        float t = (m_upd'); b=s.index('        CVector3 lo = start;')
E={'t2':'t * t','k':'2.0f * t','a2':'age * age'}
for c in itertools.product([0,1],repeat=3):
    mid=s[a:b]
    for on,v in zip(c,['t2','k','a2']):
        if on:
            mid=re.sub(r'        float %s = [^;]*;\n'%v,'',mid)
            mid=re.sub(r'\b%s\b'%v,'(%s)'%E[v],mid)
    src=s[:a]+mid+s[b:]
    open('cse_%d%d%d.cpp'%c,'w').write(src)
