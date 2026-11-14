# sbsfull.py json func draft lo hi: full side-by-side incl. equal lines for orig index range
import sys,json,tempfile,subprocess,difflib
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
t=json.load(open(sys.argv[1])); t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
f=sys.argv[2]; o,log=fast.compile_object(t,open(sys.argv[3]).read(),tempfile.mkdtemp())
m=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout)[f]
a=fast.target(f); lo,hi=int(sys.argv[4]),int(sys.argv[5])
for i in range(lo,min(hi,len(a))): print('%4d %-34s %s %s'%(i,a[i],'|' if a[i]!=(m[i] if i<len(m) else '') else ' ',m[i] if i<len(m) else ''))
