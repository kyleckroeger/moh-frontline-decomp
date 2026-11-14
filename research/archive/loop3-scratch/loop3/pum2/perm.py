import itertools,sys,json,subprocess,tempfile,difflib
sys.path.insert(0,'scratch/game');import fast
t=json.load(open('scratch/loop3/pum2/t.json'));f='__ct__19PopUpMessageHandlerFPcPc'
s=open('scratch/loop3/pum2/v1.cpp').read()
blk='''    unsigned char* entry;
    char* end;
    unsigned short i;
    unsigned char extra;
    unsigned char length;
    unsigned char* index = m_lengths;
    int pos = 2;
    char* text = m_strings;
'''
assert blk in s
L=blk.splitlines(True)
a=[x if isinstance(x,str) else x[0] for x in fast.target(f)]
best=99
for p in itertools.permutations(range(5)):
    body=''.join(L[i] for i in p)+''.join(L[5:])
    src=s.replace(blk,body)
    with tempfile.TemporaryDirectory() as w:
        o,log=fast.compile_object(t,src,w)
        b=[x if isinstance(x,str) else x[0] for x in fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout).get(f,[])]
    n=sum(1 for l in difflib.unified_diff(a,b,n=0,lineterm='') if l[:1] in '+-' and not l.startswith(('+++','---')))
    if n<=best: best=n; print(n,p,flush=True)
    if n==0: open('scratch/loop3/pum2/v_exact.cpp','w').write(src); break
