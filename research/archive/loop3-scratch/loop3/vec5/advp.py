import sys,itertools,re
sys.path.insert(0,'.'); import sc
S=sc.UBB()
s=open(sys.argv[1]).read()
names=['p','v','a','t','t2']
args={'p':'start.d.v[%d]','v':'v%s','a':'sa.d.v[%d]','t':'age','t2':'a2'}
res=[]
for perm in itertools.permutations(names):
    x=s.replace('static inline float Adv(float p, float v, float a, float t, float t2)','static inline float Adv(%s)'%', '.join('float '+n for n in perm))
    for i,c in enumerate('xyz'):
        old='Adv(start.d.v[%d], v%s, sa.d.v[%d], age, a2)'%(i,c,i)
        new='Adv(%s)'%', '.join({'p':'start.d.v[%d]'%i,'v':'v'+c,'a':'sa.d.v[%d]'%i,'t':'age','t2':'a2'}[n] for n in perm)
        x=x.replace(old,new)
    r=S(x); res.append((r,perm))
res.sort(); print(res[:6])
open('advbest.cpp','w').write(x)
