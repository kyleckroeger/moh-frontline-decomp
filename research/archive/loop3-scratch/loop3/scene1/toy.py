import sys,json,tempfile,subprocess
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast,varmap
t='/home/kyle/workspace/moh-frontline/config/GMFE69/scene.json'
src=open(sys.argv[1]).read()
with tempfile.TemporaryDirectory() as w:
    obj,log=fast.compile_object(json.load(open(t)),src,w)
    if not obj: print(log); sys.exit()
    for f in sys.argv[2:]:
        print('==',f); print('\n'.join(varmap.rows(['--disassemble='+f,obj])))
