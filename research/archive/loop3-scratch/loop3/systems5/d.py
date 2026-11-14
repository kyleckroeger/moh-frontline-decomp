import sys,json,tempfile,subprocess,difflib
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
import os; t=json.load(open(os.environ.get('T','/home/kyle/workspace/moh-frontline/scratch/loop3/systems5/t.json')))
src=open(sys.argv[1]).read(); f=sys.argv[2] if len(sys.argv)>2 else 'CreateGerman__FP20TriggerObject_structib'
d=tempfile.mkdtemp(); obj,log=fast.compile_object(t,src,d)
if not obj: sys.exit(log[:3000])
mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout)[f]
a=fast.target(f)
n=0
for l in difflib.unified_diff(a,mine,n=int(sys.argv[3]) if len(sys.argv)>3 else 1,lineterm=''):
    if l.startswith(('+++','---')): continue
    print(l)
