import sys,json,tempfile
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game')
import fast,varmap
obj,log=fast.compile_object(json.load(open('/home/kyle/workspace/moh-frontline/scratch/loop3/struct2/t_rs.json')),open(sys.argv[1]).read(),tempfile.mkdtemp())
if not obj: print(log); sys.exit()
print('\n'.join(varmap.rows(['--disassemble='+sys.argv[2],obj])))
