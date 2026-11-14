# flagx.py json func draft [suffix]: score draft under inline-flag variants
import sys,json,tempfile,subprocess,difflib,os,copy
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t0=json.load(open(sys.argv[1])); f=sys.argv[2]; src=open(sys.argv[3],errors='replace').read()
if len(sys.argv)>4: os.environ['SUFFIX']=sys.argv[4]
t0['include_dirs']=t0.get('include_dirs',[])+['src/game','src/ea','include']
a=fast.target(f); w=tempfile.mkdtemp()
fl=t0['compiler_flags']; i=fl.index('-inline') if '-inline' in fl else None
cur=fl[i+1] if i is not None else None
out=[]
for v in ['auto','deferred','deferred,auto','on','off','noauto','all','auto,level=1']:
    t=copy.deepcopy(t0)
    if i is None: t['compiler_flags']+=['-inline',v]
    else: t['compiler_flags'][i+1]=v
    obj,log=fast.compile_object(t,src,w)
    if not obj: out.append((v,'FAIL')); continue
    m=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout).get(f)
    if m is None: out.append((v,'NOFUNC')); continue
    sm=difflib.SequenceMatcher(None,a,m,autojunk=False)
    out.append((v,sum((i2-i1)+(j2-j1) for op,i1,i2,j1,j2 in sm.get_opcodes() if op!='equal')))
print('cur=%s'%cur,' '.join('%s:%s'%x for x in out))
