import sys,json,subprocess,tempfile,difflib
sys.path.insert(0,'scratch/game');import fast
# sc.py TMPL ORIGFUNC OURFUNC [-v] src...
tm,of,uf=sys.argv[1:4]
t=json.load(open(tm))
show='-v' in sys.argv
for src in [a for a in sys.argv[4:] if a!='-v']:
    with tempfile.TemporaryDirectory() as w:
        import re,os
        txt=re.sub(r'#include "(\w+\.h)"',lambda m:open(os.path.join('scratch/loop3/sinit2',m.group(1))).read(),open(src).read())
        o,log=fast.compile_object(t,txt,w)
        if not o: print(src,'ERR',log[:600]);continue
        a=[x if isinstance(x,str) else x[0] for x in fast.target(of)]
        P=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout)
        k=[x for x in P if x.startswith(uf)]
        if not k: print(src,'NOFUNC',list(P)[:10]);continue
        b=[x if isinstance(x,str) else x[0] for x in P[k[0]]]
        n=sum(1 for l in difflib.unified_diff(a,b,n=0,lineterm='') if l[:1] in '+-' and not l.startswith(('+++','---')))
        print(n,len(a),len(b),src)
        if show:
            for i in range(max(len(a),len(b))):
                x=a[i] if i<len(a) else '';y=b[i] if i<len(b) else ''
                print('%3d %s %-34s %s'%(i,' ' if x==y else '*',x,y))
