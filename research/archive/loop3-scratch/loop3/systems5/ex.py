import sys,json,tempfile,subprocess
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open('t.json')); d=tempfile.mkdtemp(); obj,log=fast.compile_object(t,open(sys.argv[1]).read(),d)
print(subprocess.run([fast.OBJDUMP,'-s','-j','extab','-j','extabindex',obj],capture_output=True,text=True).stdout[-3000:])
print(subprocess.run([fast.OBJDUMP,'-r','-j','extabindex',obj],capture_output=True,text=True).stdout[-1500:])
