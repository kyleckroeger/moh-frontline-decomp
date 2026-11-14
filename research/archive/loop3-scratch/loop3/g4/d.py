"""d.py <func-substring> [draftpath] [ctx]: sbs diff lines (only <<) for an index.tsv row, compiled from draftpath (default the row's draft)."""
import csv,sys,json,tempfile,os
sys.path.insert(0,'scratch/game'); sys.path.insert(0,'research/tools')
import fast,varmap
key=sys.argv[1]; dr=sys.argv[2] if len(sys.argv)>2 and sys.argv[2]!='-' else None; ctx=int(sys.argv[3]) if len(sys.argv)>3 else 0
for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    if key in r['function']: break
else: sys.exit('no row')
t=json.load(open(r['compile_settings_from'])); t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
os.environ['SUFFIX']='cpp'
obj,log=fast.compile_object(t,open(dr or r['draft'],errors='replace').read(),tempfile.mkdtemp())
if not obj: sys.exit(log[-3000:])
a=int(r['address'],16); n=int(r['size'])
m=varmap.rows(['--disassemble='+r['function'],obj]); o=varmap.rows(['--start-address=%#x'%a,'--stop-address=%#x'%(a+n),fast.ELF])
bad=[i for i,(x,y) in enumerate(zip(o,m)) if x!=y]
show=set(j for i in bad for j in range(i-ctx,i+ctx+1))
print('len',len(o),len(m),'diff',len(bad))
for i,(x,y) in enumerate(zip(o,m)):
    if i in show: print(f'{i:4} {x:34} {y:34} {"<<" if x!=y else ""}')
