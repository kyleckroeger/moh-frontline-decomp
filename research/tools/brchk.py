"""brchk.py [maxlines]: (run from the repository root) for each research/drafts/index.tsv draft with real_lines <= max, compile it and compare
branch TARGET offsets (which fast.py/sbs.py normalise to T) against the original. Prints drafts whose branch
offsets differ: a sign the draft's control flow is wrong, not its registers."""
import csv,json,os,re,subprocess,sys,tempfile
sys.path.insert(0,'scratch/game'); import fast
O='build/tools/powerpc-eabi-objdump'
mx=int(sys.argv[1]) if len(sys.argv)>1 else 40
def load(lines):
    L=[l for l in lines if re.match(r'^\s*[0-9a-f]+:\t',l)]
    if not L: return []
    base=int(L[0].split(':')[0],16); out=[]
    for l in L:
        p=l.split('\t'); ins=p[2].split(' <')[0].strip() if len(p)>2 else ''
        m=re.match(r'(b\w*)\s+(?:(cr\d),)?([0-9a-f]+)$',ins)
        if m and not ins.startswith('bl '): out.append('%s +%d'%(m.group(1),int(m.group(3),16)-base))
        elif m: out.append('bl')
        else: out.append(None)
    return out
for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    try: rl=int(r['real_lines'])
    except: continue
    if rl>mx: continue
    t=json.load(open(r['compile_settings_from'])); t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
    a=int(r['address'],16); n=int(r['size'])
    with tempfile.TemporaryDirectory() as w:
        os.environ['SUFFIX']='cpp'  # .c drafts are scored as C++ too (score.py); SUFFIX=c fails to compile them
        o,log=fast.compile_object(t,open(r['draft'],errors='replace').read(),w)
        if not o: print('COMPILEFAIL',r['function'][:60]); continue
        d=subprocess.run([O,'-d',o],capture_output=True,text=True).stdout
        m=re.search(r'<'+re.escape(r['function'])+r'>:\n(.*?)(?:\n\n|\Z)',d,re.S)
        if not m: print('NOFUNC',r['function'][:60]); continue
        A=load(m.group(1).split('\n'))
    B=load(subprocess.run([O,'-d','--start-address=%#x'%a,'--stop-address=%#x'%(a+n),'orig/GMFE69/Moh2RelGC.elf'],capture_output=True,text=True).stdout.split('\n'))
    bd=[(i,x,y) for i,(x,y) in enumerate(zip(A,B)) if x!=y and x and y and not (x=='bl' and y=='bl')]
    bd=[z for z in bd if not (z[1].startswith('bl') and z[2].startswith('bl'))]
    print('%s real=%d len %d/%d branchdiffs %d %s'%(r['function'][:70],rl,len(A),len(B),len(bd),' '.join('%d:%s|%s'%z for z in bd[:3])),flush=True)
