# sc.py json func file... : difflib diff count + score.real per file; -v prints side-by-side diff for one file
import sys,json,tempfile,subprocess,difflib,re,os
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); import score
args=sys.argv[1:]; v='-v' in args; args=[a for a in args if a!='-v']
t=json.load(open(args[0])); f=args[1]
t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
for fn in args[2:]:
  src=open(fn,errors='replace').read(); d=tempfile.mkdtemp(); obj,log=fast.compile_object(t,src,d)
  if not obj: print('FAIL',fn,log[:1500]); continue
  mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout).get(f)
  if mine is None: print('NOFUNC',fn); continue
  a=fast.target(f)
  sm=difflib.SequenceMatcher(None,a,mine,autojunk=False); n=0
  for op,i1,i2,j1,j2 in sm.get_opcodes():
    if op=='equal': continue
    n+=(i2-i1)+(j2-j1)
    if v:
      for k in range(max(i2-i1,j2-j1)):
        x=a[i1+k] if i1+k<i2 else ''; y=mine[j1+k] if j1+k<j2 else ''
        print(f'{i1+k:4} {x:34} | {y}')
  print(n,len(a),len(mine),fn,flush=True)
