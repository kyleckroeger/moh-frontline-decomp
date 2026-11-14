"""toy.py <settings.json> <src.cpp>: compile and objdump (no symbol filter)."""
import sys,json,tempfile,subprocess
sys.path.insert(0,'scratch/game'); import fast
o,log=fast.compile_object(json.load(open(sys.argv[1])),open(sys.argv[2]).read(),tempfile.mkdtemp())
if not o: sys.exit(log)
print(subprocess.run(['build/tools/powerpc-eabi-objdump','-dr',o],capture_output=True,text=True).stdout)
