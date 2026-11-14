# d.py KEY [draft]: real diff lines (score.eq) with context
import sys,csv,json,tempfile
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import score,fast,varmap
key=sys.argv[1]
row=[r for r in csv.DictReader(open(R+'research/drafts/index.tsv'),delimiter='\t') if key in r['function']][0]
dr=sys.argv[2] if len(sys.argv)>2 and sys.argv[2] else R+row['draft']
a=row['address']; e=hex(int(a,16)+int(row['size']))
w=tempfile.mkdtemp()
obj,log=fast.compile_object(json.load(open(R+row['compile_settings_from'])),open(dr,errors='replace').read(),w)
if not obj: sys.exit(log)
m=varmap.rows(['--disassemble='+row['function'],obj]); o=varmap.rows(['--start-address=0x'+a,'--stop-address='+e,fast.ELF])
idx=[i for i,(x,y) in enumerate(zip(o,m)) if not score.eq(x,y)]
show=set()
C=int(sys.argv[3]) if len(sys.argv)>3 else 1
for i in idx: show|=set(range(i-C,i+C+1))
for i in sorted(show):
    if 0<=i<min(len(o),len(m)): print(f'{i:4} {o[i]:30} {m[i]:30} {"<<" if i in idx else ""}')
print(len(idx),'diff lines; len',len(o),len(m))
