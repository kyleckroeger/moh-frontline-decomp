import sys,json,tempfile,subprocess
sys.path.insert(0,'scratch/game'); import fast
o,log=fast.compile_object(json.load(open(sys.argv[1])),open(sys.argv[2]).read(),tempfile.mkdtemp())
if not o: print(log); sys.exit()
for fn in sys.argv[3:]:
    out=subprocess.run(['build/tools/powerpc-eabi-objdump','-dr','--disassemble='+fn,o],capture_output=True,text=True).stdout
    seq=[l.split()[-1] for l in out.split('\n') if 'ADDR16_HA' in l and ('...bss' in l or '...rodata' in l)]
    print(fn[:30],seq[:3])
