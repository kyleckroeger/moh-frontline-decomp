"""scan.py: for every research/drafts row, count shared-zero pairs (li rA,0 then mr rB,rA within 4 lines) orig vs ours."""
import sys,re,json,tempfile,csv
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); sys.path.insert(0,R+'scratch/game')
import fast,varmap
def pairs(rows):
    out=[]
    for k,x in enumerate(rows):
        m=re.match(r'li (r\d+),0$',x)
        if not m: continue
        a=m.group(1)
        for y in rows[k+1:k+12]:
            m2=re.match(r'mr (r\d+),(r\d+)$',y)
            if m2 and m2.group(2)==a: out.append((k,x,y)); break
    return out
rows=list(csv.reader(open(R+'research/drafts/index.tsv'),delimiter='\t'))[1:]
only=sys.argv[1:]
for real,size,addr,func,tm,draft in rows:
    if only and not any(o in func for o in only): continue
    try:
        with tempfile.TemporaryDirectory() as w:
            obj,log=fast.compile_object(json.load(open(R+tm)),open(R+draft,errors='replace').read(),w)
            if not obj: print('ERR',func); continue
            m=varmap.rows(['--disassemble='+func,obj])
        o=varmap.rows(['--start-address=0x'+addr,'--stop-address='+hex(int(addr,16)+int(size)),fast.ELF])
    except Exception as e:
        print('EXC',func,e); continue
    po,pm=pairs(o),pairs(m)
    flag='**' if len(po)>len(pm) else ('<>' if len(po)!=len(pm) else '  ')
    print(flag,real,len(po),len(pm),func,draft,flush=True)
