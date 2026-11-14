# var.py dir base.cpp out.cpp 'old' 'new' ... -> writes dir/out.cpp, prints score + register diff count
import sys,subprocess
d,src,dst=sys.argv[1:4]; s=open(d+'/'+src).read()
a=sys.argv[4:]
for i in range(0,len(a),2):
    if a[i] not in s: print('MISSING',a[i][:60]); sys.exit(1)
    s=s.replace(a[i],a[i+1],1)
open(d+'/'+dst,'w').write(s)
r=subprocess.run(['sh',d+'/sd.sh',dst],capture_output=True,text=True)
print(dst,' '.join(r.stdout.split()[:1]+r.stdout.split()[-3:]), r.stderr[-300:])
import os
if os.environ.get('RD'):
    r=subprocess.run(['python3','../struct4/tools/rdiff.py',d+'/td.json',d+'/'+dst,'TLT_LoadFromRepository__FPCcPiPb','8001cca8','572'],capture_output=True,text=True)
    print(''.join(l+'\n' for l in r.stdout.splitlines() if l[:4].strip().isdigit()))
