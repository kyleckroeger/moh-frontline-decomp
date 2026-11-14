# probe.py json src.cpp [func-substring]: compile and print disasm of functions
import sys,json,tempfile,subprocess
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open(sys.argv[1])); t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
o,log=fast.compile_object(t,open(sys.argv[2]).read(),tempfile.mkdtemp())
if not o: print(log[:2000]); sys.exit()
d=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout)
for k,v in d.items():
    if len(sys.argv)>3 and sys.argv[3] not in k: continue
    print('==',k); print('\n'.join(v))
