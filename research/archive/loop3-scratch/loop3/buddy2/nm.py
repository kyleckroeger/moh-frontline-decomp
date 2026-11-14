import sys,json,tempfile,subprocess
sys.path.insert(0,'scratch/game'); import fast
o,log=fast.compile_object(json.load(open(sys.argv[1])),open(sys.argv[2]).read(),tempfile.mkdtemp())
if not o: print(log); sys.exit()
print(subprocess.run(['build/tools/powerpc-eabi-nm','-n','-S',o],capture_output=True,text=True).stdout)
