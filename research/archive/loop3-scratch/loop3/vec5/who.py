import sys,re
sys.path.insert(0,'.')
import sc
S=sc.UBB()
def tag(m):
    d={}
    seq=m[452:463]
    for x in seq:
        op,args=x.split()[0],x.split()[1]
        r=args.split(',')[0]
        mm=re.match(r'lfs f\d+,(\d+)\(r1\)',x)
        if mm: d[{'144':'sax','148':'say','152':'saz','160':'svx','164':'svy','168':'svz'}[mm.group(1)]]=r
    fd=[x.split()[1].split(',')[0] for x in seq if x.startswith('fdivs')]
    fm=[x.split()[1].split(',')[0] for x in seq if x.startswith('fmuls')]
    d['t']=fd[0] if fd else '?'
    return ' '.join('%s=%s'%(k,d.get(k,'?')) for k in ['t','sax','say','saz','svx','svy','svz'])+' fm='+','.join(fm)
for f in sys.argv[1:]:
    s=open(f).read(); m=S.asm(s)
    print('%-14s'%f,S(s),tag(m))
print('%-14s'%'orig',tag(S.o))
