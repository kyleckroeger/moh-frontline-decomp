import itertools
s=open('pc.c').read()
a=s.index('static inline void pan('); b=s.index('void SNDI_precalcaztospkrvol')
pre,post=s[:a],s[b:]
B1='pan(&sndaztospkr[i][0], &sndaztospkr[i][sndgs.speakers - 1], azimuth + (0x10000 - az[sndgs.speakers - 1]), az[0] - azimuth);'
B2='pan2(&sndaztospkr[i][sndgs.speakers - 1], &sndaztospkr[i][0], 0x10000 - azimuth + az[0], azimuth - az[sndgs.speakers - 1]);'
assert B1 in post and B2 in post
X1='azimuth + (0x10000 - az[sndgs.speakers - 1])'; Y1='az[0] - azimuth'
X2='0x10000 - azimuth + az[0]'; Y2='azimuth - az[sndgs.speakers - 1]'
P0='&sndaztospkr[i][0]'; PN='&sndaztospkr[i][sndgs.speakers - 1]'
# helper: H(p,q,x,y): stores p=shape(x*127/S), q=shape(y*127/S)
bodies={
 'A':'    *p = shape(x * 127 / (y + x));\n    *q = shape(y * 127 / (y + x));\n',
 'B':'    *p = shape(x * 127 / (x + y));\n    *q = shape(y * 127 / (x + y));\n',
 'C':'    int a = x * 127 / (x + y);\n    int b = y * 127 / (x + y);\n    *p = shape(a);\n    *q = shape(b);\n',
 'D':'    int a = x * 127 / (y + x);\n    int b = y * 127 / (y + x);\n    *p = shape(a);\n    *q = shape(b);\n',
 'E':'    int b = y * 127 / (x + y);\n    int a = x * 127 / (x + y);\n    *p = shape(a);\n    *q = shape(b);\n',
 'F':'    int s = x + y;\n    *p = shape(x * 127 / s);\n    *q = shape(y * 127 / s);\n',
}
sigs={'n':('signed char* p, signed char* q, int x, int y',lambda p,q,x,y:'pan(%s, %s, %s, %s);'%(p,q,x,y)),
      'r':('int x, int y, signed char* p, signed char* q',lambda p,q,x,y:'pan(%s, %s, %s, %s);'%(x,y,p,q)),
      's':('signed char* p, int x, signed char* q, int y',lambda p,q,x,y:'pan(%s, %s, %s, %s);'%(p,x,q,y))}
out=[]
for bk,body in bodies.items():
  for sk,(sig,call) in sigs.items():
    # branch1: [0]=f(x1), [n-1]=f(y1) store [0] first (pan). branch2 orig pan2: stores [0] first=shape(Y2/..), then [n-1]=shape(X2)
    for o1 in (0,1):
      for o2 in (0,1):
        c1=call(P0,PN,X1,Y1) if o1==0 else call(PN,P0,Y1,X1)
        c2=call(P0,PN,Y2,X2) if o2==0 else call(PN,P0,X2,Y2)
        h='static inline void pan(%s) {\n%s}\n\n'%(sig,body)
        t=pre+h+post.replace(B1,c1).replace(B2,c2)
        k='p/pc_%s%s%d%d.c'%(bk,sk,o1,o2); open(k,'w').write(t); out.append(k)
print(' '.join(out))
