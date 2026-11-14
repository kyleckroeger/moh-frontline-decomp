import sys
exec(open('cvv.py').read().split('ys=[]')[0])
y=sys.argv[1]
t=pre+body(y)+post
open('last.cpp','w').write(t)
print(run('cv',t,True))
