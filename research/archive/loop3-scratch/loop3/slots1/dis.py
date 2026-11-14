import sys,json,tempfile
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast,varmap
t,f,p=sys.argv[1:4]
with tempfile.TemporaryDirectory() as w:
    obj,log=fast.compile_object(json.load(open(t)),open(p).read(),w)
    if not obj: print(log); sys.exit()
    print(';'.join(varmap.rows(['--disassemble='+f,obj])))
