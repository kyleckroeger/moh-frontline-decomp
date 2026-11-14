# rm.py file func: score + register substitution map (orig->ours) from aligned diff lines
import sys,json,tempfile,subprocess,difflib,re,os,collections
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open(os.environ.get('T','/home/kyle/workspace/moh-frontline/scratch/loop3/systems5/t.json')))
f=sys.argv[2] if len(sys.argv)>2 else 'CreateDestructable__FP20TriggerObject_structi'
for fn in sys.argv[1:2]:
  src=open(fn).read(); d=tempfile.mkdtemp(); obj,log=fast.compile_object(t,src,d)
  if not obj: sys.exit(log[:3000])
  mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout)[f]
  a=fast.target(f)
  sm=difflib.SequenceMatcher(None,a,mine,autojunk=False)
  m=collections.Counter(); n=0
  for op,i1,i2,j1,j2 in sm.get_opcodes():
    if op=='equal': continue
    n+=(i2-i1)+(j2-j1)
    if op=='replace' and i2-i1==j2-j1:
      for x,y in zip(a[i1:i2],mine[j1:j2]):
        rx=re.findall(r'\br\d+',x); ry=re.findall(r'\br\d+',y)
        if re.sub(r'\br\d+','R',x)==re.sub(r'\br\d+','R',y):
          for p,q in zip(rx,ry):
            if p!=q: m[(p,q)]+=1
  print(n,fn,' '.join(f'{p}->{q}:{c}' for (p,q),c in sorted(m.items())))
