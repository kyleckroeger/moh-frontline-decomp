import sys,json,tempfile,subprocess
sys.path.insert(0,'scratch/game'); import fast
o,log=fast.compile_object(json.load(open(sys.argv[1])),open(sys.argv[2]).read(),tempfile.mkdtemp())
out=subprocess.run(['build/tools/powerpc-eabi-objdump','-dr','--disassemble='+sys.argv[3],o],capture_output=True,text=True).stdout
print('\n'.join(out.split('\n')[:int(sys.argv[4])]))
