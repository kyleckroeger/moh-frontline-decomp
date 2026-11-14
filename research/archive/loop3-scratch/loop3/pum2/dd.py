import sys,json,subprocess,os,tempfile,difflib
sys.path.insert(0,'scratch/game');import fast
t=json.load(open(sys.argv[1]));f=sys.argv[2]
for src in sys.argv[3:]:
    with tempfile.TemporaryDirectory() as w:
        o,log=fast.compile_object(t,open(src).read(),w)
        if not o: print(src,'ERR',log[:300]);continue
        a=[x if isinstance(x,str) else x[0] for x in fast.target(f)]
        b=[x if isinstance(x,str) else x[0] for x in fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout).get(f,[])]
        n=sum(1 for l in difflib.unified_diff(a,b,n=0,lineterm='') if l[:1] in '+-' and not l.startswith(('+++','---')))
        print(n,len(a),len(b),src)
