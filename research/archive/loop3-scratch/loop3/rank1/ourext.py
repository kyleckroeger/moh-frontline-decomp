"""ourext.py json draft: compile and dump extab/extabindex of our object."""
import sys,json,tempfile,subprocess
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
d=tempfile.mkdtemp(); o,log=fast.compile_object(json.load(open(sys.argv[1])),open(sys.argv[2]).read(),d)
if not o: sys.exit(log)
print(subprocess.run([fast.OBJDUMP,'-s','-j','extab','-j','extabindex','-r',o],capture_output=True,text=True).stdout[-3000:])
