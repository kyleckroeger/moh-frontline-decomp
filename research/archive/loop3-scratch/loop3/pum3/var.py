# var.py base.cpp  then stdin: variants separated by '====' lines, each with 'OLD>>>NEW' pairs separated by '----'
import sys,json,subprocess,tempfile,difflib
sys.path.insert(0,'scratch/game');import fast
t=json.load(open('scratch/loop3/pum3/t.json'));f='__ct__19PopUpMessageHandlerFPcPc'
s=open(sys.argv[1]).read()
a=[x if isinstance(x,str) else x[0] for x in fast.target(f)]
for k,v in enumerate(sys.stdin.read().split('\n====\n')):
    src=s
    for pr in v.split('\n----\n'):
        if '>>>' not in pr: continue
        o,n=pr.split('>>>');
        if o not in src: print(k,'MISSING',o[:60]); break
        src=src.replace(o,n,1)
    with tempfile.TemporaryDirectory() as w:
        ob,log=fast.compile_object(t,src,w)
        if not ob: print(k,'ERR',log[:400]); continue
        P=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',ob],capture_output=True,text=True).stdout)
    def D(fn):
        a=[x if isinstance(x,str) else x[0] for x in fast.target(fn)]
        b=[x if isinstance(x,str) else x[0] for x in P.get(fn,[])]
        return sum(1 for l in difflib.unified_diff(a,b,n=0,lineterm='') if l[:1] in '+-' and not l.startswith(('+++','---')))
    n=D(f); m=D('Load__17CUserMessageTableFPcPc')
    print(k,n,'Load',m,v.replace('\n',' ')[:110]); open('scratch/loop3/pum3/perm/var%d.cpp'%k,'w').write(src)
