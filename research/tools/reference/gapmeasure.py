"""gapmeasure.py: READ-ONLY. For each original file with block-base users: bytes that a gap-function rule could unlock vs. bytes left uncovered, using rank.tsv best real lines."""
import csv,json,re,subprocess,collections
from pathlib import Path
rank={x[3]:(int(x[0]),int(x[2]),x[4],x[6]) for x in csv.reader(open('scratch/solo/ret/rank.tsv'),delimiter='\t') if x}
table={x[2]:x for x in csv.reader(open('scratch/solo/sweep/table3.tsv'),delimiter='\t') if x}
ELF='orig/GMFE69/Moh2RelGC.elf'
rel=subprocess.run(['build/tools/powerpc-eabi-objdump','-dr','-j','.text',ELF],capture_output=True,text=True).stdout
blk=collections.defaultdict(set)
for l in rel.splitlines():
    m=re.match(r'^\s*([0-9a-f]{8}): R_PPC_ADDR16_(?:LO|HA)\s+(\.\.\.\S+)',l)
    if m: blk[int(m.group(1),16)].add(m.group(2))
addrs=sorted(blk)
import bisect
file_of={}
for p in Path('config/GMFE69').glob('*.json'):
    if p.name=='project.json': continue
    d=json.loads(p.read_text())
    if 'id' not in d: continue
    ofile=d.get('original_file')
    for s in d['sections']:
        if s['name']=='.text': file_of[(int(s['address'],16),int(s['address'],16)+s['size'])]=ofile
spans=sorted(file_of)
def owner(a):
    best=None;bd=1<<40
    for (st,en) in spans:
        d=0 if st<=a<en else min(abs(a-st),abs(a-en))
        if d<bd: bd=d;best=file_of[(st,en)]
    return best
F=collections.defaultdict(list)
for a,x in table.items():
    ai=int(a,16);sz=int(x[3])
    i=bisect.bisect_left(addrs,ai); uses=False
    syms=set()
    while i<len(addrs) and addrs[i]<ai+sz:
        syms|=blk[addrs[i]]; i+=1
    r=rank.get(a)
    F[owner(ai)].append((a,sz,x[4][:34],r[0] if r else None,bool(syms)))
print('file open_fns open_bytes | block_users bytes | code-close(<=3) bytes | stuck-or-undrafted(>3) bytes')
rows=[]
for f,L in F.items():
    bu=[e for e in L if e[4]]
    if not bu: continue
    close=[e for e in bu if e[3] is not None and e[3]<=3]
    tot=sum(e[1] for e in L)
    rows.append((sum(e[1] for e in bu),f,len(L),tot,len(bu),sum(e[1] for e in close),[(e[2],e[1],e[3]) for e in sorted(L,key=lambda e:-e[1])][:12]))
for r in sorted(rows,key=lambda r:-r[0])[:14]:
    print(r[1],r[2],r[3],'|',r[4],r[0],'| close',r[5]); print('    ',r[6])
