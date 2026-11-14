import json,sys,os,tempfile
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); os.chdir('/home/kyle/workspace/moh-frontline')
import fast
obj,log=fast.compile_object(json.load(open(sys.argv[1])),open(sys.argv[2]).read(),tempfile.mkdtemp(dir='/tmp/claude-1000'))
print(obj); print(log[:int(sys.argv[3]) if len(sys.argv)>3 else 2500])
