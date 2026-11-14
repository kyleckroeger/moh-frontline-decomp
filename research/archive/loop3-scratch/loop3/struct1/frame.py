# frame.py tmpl src func -> frame size line
import sys,json,os,subprocess,re
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t,src,f=sys.argv[1:4]
w=os.path.abspath(os.path.dirname(src)+'/w'); os.makedirs(w,exist_ok=True)
obj,log=fast.compile_object(json.load(open(t)),open(src).read(),w)
if not obj: print('ERR',log[-400:]); sys.exit()
out=subprocess.run(['/home/kyle/workspace/moh-frontline/build/tools/powerpc-eabi-objdump','-d','--disassemble='+f,obj],capture_output=True,text=True).stdout
print([l.split('\t')[-1] for l in out.splitlines() if 'subfic' in l or 'stwu' in l][:1])
