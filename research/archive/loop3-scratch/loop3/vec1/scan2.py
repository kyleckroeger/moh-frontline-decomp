import sys,json,tempfile,re,csv
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast,varmap,score
R='/home/kyle/workspace/moh-frontline/'
sq=re.compile(r"\bf(1[4-9]|2[0-9]|3[01])\b")
for row in csv.reader(open(R+'research/drafts/index.tsv'),delimiter='\t'):
    if len(row)<6 or not row[2] or not row[0]: continue
    sc,size,a,f,t,src=row[:6]
    try:
        o=varmap.rows(['--start-address=0x'+a,'--stop-address='+hex(int(a,16)+int(size)),fast.ELF])
        osq=len(set(m2 for x in o for m2 in sq.findall(x)))
        if not osq: continue
        with tempfile.TemporaryDirectory() as w:
            obj,log=fast.compile_object(json.load(open(R+t)),open(R+src,errors='replace').read(),w)
            if not obj: print(f,'FAIL'); continue
            m=varmap.rows(['--disassemble='+f,obj])
        msq=len(set(m2 for x in m for m2 in sq.findall(x)))
        real=sum(1 for x,y in zip(o,m) if not score.eq(x,y))+abs(len(o)-len(m))
        print('%-60s real %4d calleeFPR orig %d ours %d'%(f[:60],real,osq,msq),flush=True)
    except Exception as e: print(f,'ERR',e)
