# var2.py tmpl func addr size base.cpp out.cpp 'old' 'new' ...
import sys,subprocess
t,f,a,n,src,dst=sys.argv[1:7]; s=open(src).read(); r=sys.argv[7:]
for i in range(0,len(r),2):
    if r[i] not in s: print('MISSING',r[i][:60]); sys.exit(1)
    s=s.replace(r[i],r[i+1])
open(dst,'w').write(s)
o=subprocess.run(['sh','sc.sh','scratch/loop3/struct2/'+t,'scratch/loop3/struct2/'+dst,f,a,n],capture_output=True,text=True,cwd='.').stdout
print(dst,' '.join(o.split()))
