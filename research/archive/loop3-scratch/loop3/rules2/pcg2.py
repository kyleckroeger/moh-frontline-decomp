import itertools
s=open('pc.c').read()
a=s.index('static inline void pan('); b=s.index('void SNDI_precalcaztospkrvol')
pre,post=s[:a],s[b:]
B1='pan(&sndaztospkr[i][0], &sndaztospkr[i][sndgs.speakers - 1], azimuth + (0x10000 - az[sndgs.speakers - 1]), az[0] - azimuth);'
B2='pan2(&sndaztospkr[i][sndgs.speakers - 1], &sndaztospkr[i][0], 0x10000 - azimuth + az[0], azimuth - az[sndgs.speakers - 1]);'
X1='azimuth + (0x10000 - az[sndgs.speakers - 1])'; Y1='az[0] - azimuth'
X2='0x10000 - azimuth + az[0]'; Y2='azimuth - az[sndgs.speakers - 1]'
P0='&sndaztospkr[i][0]'; PN='&sndaztospkr[i][sndgs.speakers - 1]'
body='    *p = shape(x * 127 / (y + x));\n    *q = shape(y * 127 / (y + x));\n'
decl={'p':'signed char* p','q':'signed char* q','x':'int x','y':'int y'}
out=[]
for perm in itertools.permutations('pqxy'):
    sig=', '.join(decl[c] for c in perm)
    def call(p,q,x,y): d={'p':p,'q':q,'x':x,'y':y}; return 'pan(%s);'%', '.join(d[c] for c in perm)
    h='static inline void pan(%s) {\n%s}\n\n'%(sig,body)
    t=pre+h+post.replace(B1,call(P0,PN,X1,Y1)).replace(B2,call(P0,PN,Y2,X2))
    k='p/pc2_%s.c'%''.join(perm); open(k,'w').write(t); out.append(k)
print(' '.join(out))
