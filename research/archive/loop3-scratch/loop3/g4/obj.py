"""obj.py <func-substring> [draft]: objdump our compiled function with branch targets (offsets)."""
import csv,sys,json,tempfile,os,subprocess
sys.path.insert(0,'scratch/game'); import fast
key=sys.argv[1]; dr=sys.argv[2] if len(sys.argv)>2 else None
for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    if key in r['function']: break
t=json.load(open(r['compile_settings_from'])); t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
os.environ['SUFFIX']='cpp'
obj,log=fast.compile_object(t,open(dr or r['draft'],errors='replace').read(),tempfile.mkdtemp())
if not obj: sys.exit(log[-3000:])
print(subprocess.run(['build/tools/powerpc-eabi-objdump','-dr','--disassemble='+r['function'],obj],capture_output=True,text=True).stdout)
