# opswap.py maxreal: list drafts whose diff contains commutative ops with the same operands in swapped order
import sys,json,tempfile,subprocess,difflib,csv,re,os
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/game'); import fast
C={'fmuls','fadds','fmul','fadd','add','mullw','or','and','xor'}
mx=int(sys.argv[1])
for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    try: rl=int(r['real_lines'])
    except: continue
    if rl>mx or rl==0: continue
    t=json.load(open(r['compile_settings_from'])); t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
    o,log=fast.compile_object(t,open(r['draft'],errors='replace').read(),tempfile.mkdtemp())
    if not o: continue
    m=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout).get(r['function'])
    if not m: continue
    a=fast.target(r['function']); n=0
    for x,y in zip(a,m):
        if x==y: continue
        px=re.split('[ ,]+',x); py=re.split('[ ,]+',y)
        if px[0]==py[0] and px[0] in C and len(px)==4 and px[1]==py[1] and px[2]==py[3] and px[3]==py[2]: n+=1
        if px[0]==py[0] and px[0] in ('fmadds','fmsubs','fnmsubs') and len(px)==5 and px[2]==py[3] and px[3]==py[2] and px[1]==py[1] and px[4]==py[4]: n+=1
    if n: print(n,rl,r['function'][:60],r['draft'],flush=True)
