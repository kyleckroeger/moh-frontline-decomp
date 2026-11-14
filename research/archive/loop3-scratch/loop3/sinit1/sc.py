import sys,json,subprocess,tempfile,difflib
sys.path.insert(0,'scratch/game');import fast
t=json.load(open('scratch/loop3/sinit1/t.json'))
show='-v' in sys.argv
for src in [a for a in sys.argv[1:] if a!='-v']:
    with tempfile.TemporaryDirectory() as w:
        o,log=fast.compile_object(t,open(src).read(),w)
        if not o: print(src,'ERR',log[:600]);continue
        a=[x if isinstance(x,str) else x[0] for x in fast.target('__sinit_pumhandler_cpp')]
        b=[x if isinstance(x,str) else x[0] for x in fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout).get('__sinit_probe_cpp',[])]
        n=sum(1 for l in difflib.unified_diff(a,b,n=0,lineterm='') if l[:1] in '+-' and not l.startswith(('+++','---')))
        print(n,len(a),len(b),src)
        if show:
            for i in range(max(len(a),len(b))):
                x=a[i] if i<len(a) else '';y=b[i] if i<len(b) else ''
                print('%3d %s %-34s %s'%(i,' ' if x==y else '*',x,y))
        if show: print(subprocess.run([fast.OBJDUMP,'-t',o],capture_output=True,text=True).stdout.split('SYMBOL TABLE:')[1][:1500])
