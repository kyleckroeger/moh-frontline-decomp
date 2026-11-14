import sys,json,tempfile,subprocess,shutil
sys.path.insert(0,'scratch/game'); import fast
o,log=fast.compile_object(json.load(open(sys.argv[1])),open(sys.argv[2]).read(),tempfile.mkdtemp())
shutil.copy(o,sys.argv[3])
