# ours.py src -> dump our asm of RenderSystem to src.s
import sys,json,os
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game')
import fast,varmap,tempfile
src=sys.argv[1]
w=tempfile.mkdtemp()
obj,log=fast.compile_object(json.load(open('/home/kyle/workspace/moh-frontline/scratch/loop3/struct2/t_rs.json')),open(src).read(),w)
if not obj: print(log[-2000:]); sys.exit(1)
m=varmap.rows(['--disassemble=RenderSystem__15CParticleSystemFR10CDmaPacket',obj])
open(src+'.s','w').write('\n'.join(m)+'\n')
print(len(m))
