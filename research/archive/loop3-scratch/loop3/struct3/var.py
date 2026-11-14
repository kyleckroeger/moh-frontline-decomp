# var.py base.cpp out.cpp 'old' 'new' ... -> writes out.cpp, prints PSS score
import sys,subprocess
src,dst=sys.argv[1],sys.argv[2]; s=open(src).read()
a=sys.argv[3:]
for i in range(0,len(a),2):
    if a[i] not in s: print('MISSING',a[i][:60]); sys.exit(1)
    s=s.replace(a[i],a[i+1],1)
open(dst,'w').write(s)
r=subprocess.run(['sh','s.sh',dst],capture_output=True,text=True,cwd='/home/kyle/workspace/moh-frontline/scratch/loop3/struct3')
print(dst,' '.join(r.stdout.split()[:1]+r.stdout.split()[-3:]))
