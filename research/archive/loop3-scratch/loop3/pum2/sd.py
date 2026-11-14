import sys,json,subprocess,tempfile,difflib
sys.path.insert(0,'scratch/game');import fast
t=json.load(open(sys.argv[1]));f=sys.argv[2];src=sys.argv[3]
with tempfile.TemporaryDirectory() as w:
    o,log=fast.compile_object(t,open(src).read(),w)
    if not o: print('ERR',log[:1500]);sys.exit()
    a=[x if isinstance(x,str) else x[0] for x in fast.target(f)]
    b=[x if isinstance(x,str) else x[0] for x in fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout).get(f,[])]
for i,(x,y) in enumerate(zip(a,b)):
    print('%3d %s %-40s %s'%(i,' ' if x==y else '*',x,y))
