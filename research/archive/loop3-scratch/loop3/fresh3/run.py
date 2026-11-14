import sys,glob,json,tempfile,os
sys.path.insert(0,'scratch/game'); import fast
t=json.load(open(sys.argv[1])); fn=sys.argv[2]
res=[]
for f in sorted(glob.glob(sys.argv[3])):
    r=fast.score(t,open(f).read(),[(fn,)],tempfile.mkdtemp())
    res.append((list(r.values())[0],f))
res.sort(key=lambda x:(x[0] if isinstance(x[0],int) else 9999))
for x in res[:int(sys.argv[4]) if len(sys.argv)>4 else 8]: print(*x)
