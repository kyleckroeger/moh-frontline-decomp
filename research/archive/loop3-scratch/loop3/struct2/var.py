# var.py base.cpp out.cpp 'old' 'new' ['old' 'new' ...] -> writes out.cpp and prints score
import sys,subprocess
src,dst=sys.argv[1],sys.argv[2]; s=open(src).read()
a=sys.argv[3:]
for i in range(0,len(a),2):
    if a[i] not in s: print('MISSING',a[i][:60]); sys.exit(1)
    s=s.replace(a[i],a[i+1])
open(dst,'w').write(s)
r=subprocess.run(['sh','sc.sh','scratch/loop3/struct2/t_rs.json','scratch/loop3/struct2/'+dst,'RenderSystem__15CParticleSystemFR10CDmaPacket','0x8007d704','4532'],capture_output=True,text=True,cwd='/home/kyle/workspace/moh-frontline/scratch/loop3/struct2')
print(dst,' '.join(r.stdout.split()))
