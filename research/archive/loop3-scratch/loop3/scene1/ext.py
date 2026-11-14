import sys,json,tempfile,subprocess
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast
t=sys.argv[2] if len(sys.argv)>2 else "/home/kyle/workspace/moh-frontline/config/GMFE69/scene_dtor.json"
with tempfile.TemporaryDirectory() as w:
    obj,log=fast.compile_object(json.load(open(t)),open(sys.argv[1]).read(),w)
    print(subprocess.run([fast.OBJDUMP,'-s','-j','extab','-j','extabindex',obj],capture_output=True,text=True).stdout[-600:])
    print(subprocess.run([fast.OBJDUMP,'-r','-j','extabindex',obj],capture_output=True,text=True).stdout[-600:])
