"""pairs.py func: list li/mr pairs in orig or ours with the other side's lines"""
import sys,re,json,tempfile,csv
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); sys.path.insert(0,R+'scratch/game')
import fast,varmap,difflib
rows=list(csv.reader(open(R+'research/drafts/index.tsv'),delimiter='\t'))[1:]
for real,size,addr,func,tm,draft in rows:
    if sys.argv[1] not in func: continue
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(json.load(open(R+tm)),open(R+draft,errors='replace').read(),w)
        m=varmap.rows(['--disassemble='+func,obj])
    o=varmap.rows(['--start-address=0x'+addr,'--stop-address='+hex(int(addr,16)+int(size)),fast.ELF])
    for name,A,B in (('ours',m,o),('orig',o,m)):
        for k,x in enumerate(A):
            mm=re.match(r'li (r\d+),0$',x)
            if not mm: continue
            for j in range(k+1,min(k+12,len(A))):
                if re.match(r'mr (r\d+),'+mm.group(1)+'$',A[j]):
                    print(func[:30],name,k,A[k:j+1],'other:',B[k:j+1] if k<len(B) else None)
