"""vh.py INCDIR|- units.txt: compile each unit's source (INCDIR prepended to include dirs) and score its functions."""
import sys,json,os,subprocess,tempfile,difflib
R='/home/kyle/workspace/moh-frontline'; os.chdir(R)
sys.path.insert(0,R+'/research/tools'); import fast
inc=sys.argv[1]
for u in open(sys.argv[2]).read().split():
    c=json.load(open('config/GMFE69/%s.json'%u))
    if inc!='-': c['include_dirs']=[inc]+c.get('include_dirs',[])
    wd=tempfile.mkdtemp(dir='/tmp/claude-1000')
    obj,log=fast.compile_object(c,open(c['source']).read(),wd)
    if not obj: print(u,'COMPILE FAIL',log[-300:].replace('\n',' ')); continue
    mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout)
    tot=0;bad=[]
    for f in c['functions']+c.get('label_functions',[]):
        n=f['name']; b=mine.get(n)
        if b is None and n.startswith('__sinit_'): b=mine.get('__sinit_probe_cpp')
        if b is None: bad.append(n+':missing'); continue
        d=sum(1 for l in difflib.unified_diff(fast.target(n),b,n=0,lineterm='') if l[:1] in '+-' and not l.startswith(('+++','---')))
        if d: bad.append('%s:%d'%(n,d))
    print(u,len(bad),' '.join(bad)[:300]); sys.stdout.flush()
    subprocess.run(['rm','-rf',wd])
