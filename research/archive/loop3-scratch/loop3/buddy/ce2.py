import sys,json,tempfile,subprocess
sys.path.insert(0,'scratch/game'); import fast
o,log=fast.compile_object(json.load(open(sys.argv[1])),open(sys.argv[2]).read(),tempfile.mkdtemp())
print(o); print(log[:1500] if log else '')
if o: print(subprocess.run(['build/tools/powerpc-eabi-nm',o],capture_output=True,text=True).stdout)
