import sys,json,tempfile,subprocess
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast
t=json.load(open('/home/kyle/workspace/moh-frontline/scratch/loop3/struct2/t_rs.json'))
with tempfile.TemporaryDirectory() as w:
    obj,log=fast.compile_object(t,open(sys.argv[1]).read(),w)
    if not obj: print(log)
    out=subprocess.run(['/home/kyle/workspace/moh-frontline/build/tools/powerpc-eabi-objdump','-d',obj],capture_output=True,text=True).stdout
    print('\n'.join(l.split("\t")[-1] if l.startswith(' ') else l for l in out.splitlines()[6:]))
