import sys,subprocess,os,json,tempfile,difflib,re,collections
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open('/home/kyle/workspace/moh-frontline/config/GMFE69/scene_cmppairs.json'))
F='SweepExtents__FRQ23dwi28list<Q210ISceneNode7SExtent>RQ23dwi29fast_vec<Q26CScene9UNodePair>RQ23dwi36fast_vec<Q26CScene15SExclusionEntry>'
A=fast.target(F)
B=open('/home/kyle/workspace/moh-frontline/scratch/loop3/late2/sx/base.cpp').read()
s0=B.index('static void SweepExtents('); s1=B.index('\n}\n',s0)+3
FN=B[s0:s1]
def score(src,tag=None):
  dd=tempfile.mkdtemp(dir='/tmp/claude-1000'); obj,log=fast.compile_object(t,src,dd)
  if tag: open(f'sx/{tag}.cpp','w').write(src)
  if not obj: subprocess.run(['rm','-rf',dd]); return 'ERR '+log[:400].replace('\n',' ')
  mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout)[F]
  subprocess.run(['rm','-rf',dd])
  m=collections.Counter(); n=0
  for op,i1,i2,j1,j2 in difflib.SequenceMatcher(None,A,mine,autojunk=False).get_opcodes():
    if op=='equal': continue
    n+=(i2-i1)+(j2-j1)
    if op=='replace' and i2-i1==j2-j1:
      for x,y in zip(A[i1:i2],mine[j1:j2]):
        rx=re.findall(r'\br\d+',x); ry=re.findall(r'\br\d+',y)
        if re.sub(r'\br\d+','R',x)==re.sub(r'\br\d+','R',y):
          for p,q in zip(rx,ry):
            if p!=q: m[(p,q)]+=1
  return f'{n} '+' '.join(f'{p}->{q}:{c}' for (p,q),c in sorted(m.items()))
def sub(fn): return B[:s0]+fn+B[s1:]
