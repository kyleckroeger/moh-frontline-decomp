import sys,json,subprocess,tempfile,difflib,re,os
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game');import fast
os.chdir('/home/kyle/workspace/moh-frontline')
tm,of,uf=sys.argv[1:4]
t=json.load(open(tm))
a=[x if isinstance(x,str) else x[0] for x in fast.target(of)]
for src in sys.argv[4:]:
    with tempfile.TemporaryDirectory() as w:
        o,log=fast.compile_object(t,open(src).read(),w)
        if not o: print('ERR',src,flush=True);continue
        P=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout)
    k=[x for x in P if x.startswith(uf)]
    if not k: print('NOFUNC',src);continue
    b=[x if isinstance(x,str) else x[0] for x in P[k[0]]]
    n=sum(1 for l in difflib.unified_diff(a,b,n=0,lineterm='') if l[:1] in '+-' and not l.startswith(('+++','---')))
    st=[x.split(',')[1] for x in b if re.match(r'st[wb] r\d+,\d+\(r1\)$',x) and not re.match(r'stw r(0,52|31,44|30,40|29,36)',x)]
    print(n,len(b)-len(a),' '.join(st),os.path.basename(src),flush=True)
