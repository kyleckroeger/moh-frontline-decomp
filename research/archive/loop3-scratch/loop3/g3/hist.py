import sys,csv,json,tempfile,subprocess,re,collections
sys.path.insert(0,'research/tools'); import fast
O='build/tools/powerpc-eabi-objdump'
def mn(txt):
    c=collections.Counter()
    for l in txt.split('\n'):
        p=l.split('\t')
        if len(p)>2 and re.match(r'^\s*[0-9a-f]+:',p[0]): c[p[2].split()[0]]+=1
    return c
for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    if not any(k in r['function'] for k in sys.argv[1:]): continue
    t=json.load(open(r['compile_settings_from'])); t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
    o,_=fast.compile_object(t,open(r['draft'],errors='replace').read(),tempfile.mkdtemp())
    if not o: print('FAIL',r['function'][:40]); continue
    a=int(r['address'],16); n=int(r['size'])
    ours=mn(subprocess.run([O,'-d','--disassemble='+r['function'],o],capture_output=True,text=True).stdout)
    orig=mn(subprocess.run([O,'-d','--start-address=%#x'%a,'--stop-address=%#x'%(a+n),fast.ELF],capture_output=True,text=True).stdout)
    d={k:(ours[k],orig[k]) for k in set(ours)|set(orig) if ours[k]!=orig[k]}
    print(r['function'][:40],d)
