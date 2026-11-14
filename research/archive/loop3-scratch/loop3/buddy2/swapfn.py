import sys,re
# swapfn.py target.cpp source.cpp signature-prefix  -> replace function in target with the one in source
t=open(sys.argv[1]).read(); s=open(sys.argv[2]).read(); sig=sys.argv[3]
def grab(x):
    i=x.index(sig); j=x.index('\n}\n',i)+3; return i,j
i,j=grab(t); a,b=grab(s)
open(sys.argv[1],'w').write(t[:i]+s[a:b]+t[j:])
