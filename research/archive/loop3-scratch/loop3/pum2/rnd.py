import itertools,sys,json,subprocess,tempfile,difflib,random
sys.path.insert(0,'scratch/game');import fast
t=json.load(open('scratch/loop3/pum2/t.json'));F='__ct__19PopUpMessageHandlerFPcPc'
s=open('scratch/loop3/pum2/v1.cpp').read()
a0=s.index('inline void CUserMessageTable::CreateUMStructFromFile() {'); a1=s.index('void CUserMessageTable::Load(')
decl=['    unsigned char* entry;\n','    char* end;\n','    unsigned short i;\n','    unsigned char extra;\n','    unsigned char length;\n','    unsigned char* index = m_lengths;\n','    int pos = 2;\n','    char* text = m_strings;\n']
body=['        entry = index + pos;\n','        extra = entry[0];\n','        m_table[i].m_length = extra;\n','        length = entry[1];\n','        m_table[i].m_text = text;\n','        end = text + length;\n','        text = end + 1 + extra;\n','        m_table[i].m_padding = length;\n','        *end = 0;\n']
dep=[(0,1),(0,3),(1,2),(1,6),(3,5),(3,7),(5,6),(5,8),(4,6),(6,8)]
orders=[p for p in itertools.permutations(range(9)) if all(p.index(x)<p.index(y) for x,y in dep)]
def D(P,fn):
    a=[x if isinstance(x,str) else x[0] for x in fast.target(fn)]
    b=[x if isinstance(x,str) else x[0] for x in P.get(fn,[])]
    return sum(1 for l in difflib.unified_diff(a,b,n=0,lineterm='') if l[:1] in '+-' and not l.startswith(('+++','---')))
random.seed(int(sys.argv[1]))
best=99
for it in range(int(sys.argv[2])):
    d=decl[:]; random.shuffle(d)
    et=random.choice(['unsigned char','int','unsigned int','char']); lt=random.choice(['unsigned char','int','unsigned int'])
    d=[x.replace('unsigned char extra',et+' extra').replace('unsigned char length',lt+' length') for x in d]
    o=random.choice(orders) if random.random()<0.5 else tuple(range(9))
    b=[body[i] for i in o]
    te=random.choice(['end + 1 + extra','1 + end + extra','extra + end + 1','end + extra + 1'])
    b=[x.replace('end + 1 + extra',te) for x in b]
    inc=random.choice(['i++, pos += 2','++i, pos += 2','pos += 2, i++'])
    helper='''inline void CUserMessageTable::CreateUMStructFromFile() {
%s    unsigned short count = *(unsigned short*)index;
    ChangeEndian(count);
    m_count = count;
    if (m_table)
        delete[] m_table;
    m_table = new UserMessage[m_count];
    for (i = 0; i < m_count; %s) {
%s    }
}

'''%(''.join(d),inc,''.join(b))
    src=s[:a0]+helper+s[a1:]
    with tempfile.TemporaryDirectory() as w:
        ob,log=fast.compile_object(t,src,w)
        if not ob: continue
        P=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',ob],capture_output=True,text=True).stdout)
    n=D(P,F); m=D(P,'Load__17CUserMessageTableFPcPc')
    if n<16 or (n<=16 and m==0 and n<best):
        print(it,n,m,flush=True); open('scratch/loop3/pum2/perm/rnd_%s_%d_%d_%d.cpp'%(sys.argv[1],it,n,m),'w').write(src)
    best=min(best,n)
print('done')
