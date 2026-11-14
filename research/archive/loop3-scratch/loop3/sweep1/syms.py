import sys,json,tempfile,subprocess
sys.path.insert(0,'research/tools')
import fast
t,d=sys.argv[1:3]
obj,log=fast.compile_object(json.load(open(t)),open(d).read(),tempfile.mkdtemp())
print(subprocess.run(['build/tools/powerpc-eabi-nm','-nS',obj],capture_output=True,text=True).stdout)
