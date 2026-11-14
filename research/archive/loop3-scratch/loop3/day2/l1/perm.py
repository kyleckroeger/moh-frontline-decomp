# perm.py json func base.cpp declblock_file : permute declaration lines (block given in file) and score; prints best
import sys,itertools,json,tempfile,subprocess,difflib,os
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open(sys.argv[1])); f=sys.argv[2]; src=open(sys.argv[3]).read(); decl=open(sys.argv[4]).read()
t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
assert decl in src
lines=decl.splitlines(True); a=fast.target(f); res=[]
w=tempfile.mkdtemp()
for p in itertools.permutations(lines):
    s=src.replace(decl,''.join(p)); obj,log=fast.compile_object(t,s,w)
    if not obj: continue
    m=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout).get(f)
    sm=difflib.SequenceMatcher(None,a,m,autojunk=False)
    n=sum((i2-i1)+(j2-j1) for op,i1,i2,j1,j2 in sm.get_opcodes() if op!='equal')
    res.append((n,''.join(p)))
res.sort(key=lambda x:x[0])
print('counts',sorted(set(r[0] for r in res)))
print(res[0][0]); print(res[0][1])
if res[0][0]==0: open(sys.argv[3]+'.best','w').write(src.replace(decl,res[0][1]))
