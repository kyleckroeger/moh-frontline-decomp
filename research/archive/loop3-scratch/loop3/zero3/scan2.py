"""scan2.py: drafts where an aligned 'li rA,C' (C in 0,1,-1,2) has a different register than orig."""
import sys,re,json,tempfile,csv,difflib
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); sys.path.insert(0,R+'scratch/game')
import fast,varmap
norm=lambda x: re.sub(r'\b[rf]\d+\b','R',x)
rows=list(csv.reader(open(R+'research/drafts/index.tsv'),delimiter='\t'))[1:]
for real,size,addr,func,tm,draft in rows:
    if not real.isdigit() or int(real)>120: continue
    try:
        with tempfile.TemporaryDirectory() as w:
            obj,log=fast.compile_object(json.load(open(R+tm)),open(R+draft,errors='replace').read(),w)
            if not obj: print('ERR',func); continue
            m=varmap.rows(['--disassemble='+func,obj])
        o=varmap.rows(['--start-address=0x'+addr,'--stop-address='+hex(int(addr,16)+int(size)),fast.ELF])
    except Exception as e:
        print('EXC',func,e); continue
    sm=difflib.SequenceMatcher(None,[norm(x) for x in o],[norm(x) for x in m],autojunk=False)
    hits=[]
    for tag,a1,a2,b1,b2 in sm.get_opcodes():
        if tag!='equal': continue
        for k in range(a2-a1):
            x,y=o[a1+k],m[b1+k]
            mx=re.match(r'li (r\d+),(-?\d+)$',x); my=re.match(r'li (r\d+),(-?\d+)$',y)
            if mx and my and mx.group(2)==my.group(2) and mx.group(2) in ('0','1','-1','2') and mx.group(1)!=my.group(1):
                hits.append('%d:%s/%s'%(a1+k,x,my.group(1)))
    if hits: print(real,func[:60],draft,len(hits),' '.join(hits[:4]),flush=True)
