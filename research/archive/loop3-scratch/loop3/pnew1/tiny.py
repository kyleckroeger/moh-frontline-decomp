import sys,json,tempfile,subprocess
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open('/home/kyle/workspace/moh-frontline/config/GMFE69/objcreate_swap.json'))
src=open(sys.argv[1]).read(); d=tempfile.mkdtemp(); obj,log=fast.compile_object(t,src,d)
if not obj: sys.exit(log[:2000])
print(subprocess.run([fast.OBJDUMP,'-d',obj],capture_output=True,text=True).stdout.split('Disassembly')[-1][:30000])
