import itertools,sys,json,subprocess,tempfile,difflib
sys.path.insert(0,'scratch/game');import fast
t=json.load(open('scratch/loop3/pum2/t.json'));f='__ct__19PopUpMessageHandlerFPcPc'
s=open('scratch/loop3/pum2/v1.cpp').read()
blk='''        entry = index + pos;
        extra = entry[0];
        m_table[i].m_length = extra;
        length = entry[1];
        m_table[i].m_text = text;
        end = text + length;
        text = end + 1 + extra;
        m_table[i].m_padding = length;
        *end = 0;
'''
assert blk in s
L=blk.splitlines(True)
# deps: (before, after)
dep=[(0,1),(0,3),(1,2),(1,6),(3,5),(3,7),(5,6),(5,8),(4,6),(6,8)]
a=[x if isinstance(x,str) else x[0] for x in fast.target(f)]
best=99;seen=0
for p in itertools.permutations(range(9)):
    pos={v:i for i,v in enumerate(p)}
    if any(pos[x]>pos[y] for x,y in dep): continue
    seen+=1
    src=s.replace(blk,''.join(L[i] for i in p))
    with tempfile.TemporaryDirectory() as w:
        o,log=fast.compile_object(t,src,w)
        b=[x if isinstance(x,str) else x[0] for x in fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout).get(f,[])]
    n=sum(1 for l in difflib.unified_diff(a,b,n=0,lineterm='') if l[:1] in '+-' and not l.startswith(('+++','---')))
    if n<best: best=n; print(n,p,flush=True); open('scratch/loop3/pum2/v_best2.cpp','w').write(src)
    if n==0: break
print('tried',seen)
