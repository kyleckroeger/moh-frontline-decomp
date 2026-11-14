import sys,json,tempfile,subprocess
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open('t.json')); d=tempfile.mkdtemp(); obj,log=fast.compile_object(t,open(sys.argv[1]).read(),d)
out=subprocess.run([fast.OBJDUMP,'-t',obj],capture_output=True,text=True).stdout
for l in out.splitlines():
    if any(s in l for s in ('.data','.sdata','extab')) and '.sdata2' not in l: print(l)
