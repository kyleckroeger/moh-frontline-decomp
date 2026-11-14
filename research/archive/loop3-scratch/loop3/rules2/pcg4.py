import itertools
s=open('pc.c').read()
a=s.index('static inline void pan('); b=s.index('void SNDI_precalcaztospkrvol')
pre,post=s[:a],s[b:]
B1='pan(&sndaztospkr[i][0], &sndaztospkr[i][sndgs.speakers - 1], azimuth + (0x10000 - az[sndgs.speakers - 1]), az[0] - azimuth);'
B2='pan2(&sndaztospkr[i][sndgs.speakers - 1], &sndaztospkr[i][0], 0x10000 - azimuth + az[0], azimuth - az[sndgs.speakers - 1]);'
X1='azimuth + (0x10000 - az[sndgs.speakers - 1])'; Y1='az[0] - azimuth'
X2='0x10000 - azimuth + az[0]'; Y2='azimuth - az[sndgs.speakers - 1]'
P0='&sndaztospkr[i][0]'; PN='&sndaztospkr[i][sndgs.speakers - 1]'
bodies={'a':'    *p = shape(x * 127 / s);\n    *q = shape(y * 127 / s);\n',
        'b':'    int a = x * 127 / s;\n    int b = y * 127 / s;\n    *p = shape(a);\n    *q = shape(b);\n'}
decl={'p':'signed char* p','q':'signed char* q','x':'int x','y':'int y','s':'int s'}
out=[]
for bk,body in bodies.items():
  for sig in ('pqxys','pqyxs','spqxy','pqsxy','pqsyx'):
    for sv in (0,1):
        def call(p,q,x,y,ss): d={'p':p,'q':q,'x':x,'y':y,'s':ss}; return 'pan(%s);'%', '.join(d[c] for c in sig)
        s1='(%s) + (%s)'%(Y1,X1) if sv==0 else '(%s) + (%s)'%(X1,Y1)
        s2='(%s) + (%s)'%(X2,Y2) if sv==0 else '(%s) + (%s)'%(Y2,X2)
        h='static inline void pan(%s) {\n%s}\n\n'%(', '.join(decl[c] for c in sig),body)
        t=pre+h+post.replace(B1,call(P0,PN,X1,Y1,s1)).replace(B2,call(P0,PN,Y2,X2,s2))
        k='p/pc4_%s%s%d.c'%(bk,sig,sv); open(k,'w').write(t); out.append(k)
print(' '.join(out))
