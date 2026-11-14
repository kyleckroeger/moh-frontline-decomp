"""cand.py <function substring> [candidate files...]: compare draft vs candidates (lines, check_draft status, branch diffs)."""
import csv,json,os,re,subprocess,sys,tempfile
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import check_draft, fast
O='build/tools/powerpc-eabi-objdump'
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
def br(r,text):
    t=json.load(open(r['compile_settings_from'])); t['include_dirs']=t.get('include_dirs',[])+['src/game','src/ea','include']
    a=int(r['address'],16); n=int(r['size'])
    with tempfile.TemporaryDirectory() as w:
        os.environ['SUFFIX']='cpp'
        o,log=fast.compile_object(t,text,w)
        if not o: return 'CF'
        d=subprocess.run([O,'-d',o],capture_output=True,text=True).stdout
        m=re.search(r'<'+re.escape(r['function'])+r'>:\n(.*?)(?:\n\n|\Z)',d,re.S)
        if not m: return 'NF'
        A=load(m.group(1).split('\n'))
    B=load(subprocess.run([O,'-d','--start-address=%#x'%a,'--stop-address=%#x'%(a+n),'orig/GMFE69/Moh2RelGC.elf'],capture_output=True,text=True).stdout.split('\n'))
    bd=[(i,x,y) for i,(x,y) in enumerate(zip(A,B)) if x!=y and x and y and not (x.startswith('bl') and y.startswith('bl'))]
    return '%d len%d/%d %s'%(len(bd),len(A),len(B),' '.join('%d:%s|%s'%z for z in bd[:4]))
rows=[r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t') if sys.argv[1] in r['function']]
for r in rows:
    print('##',r['function'],r['real_lines'],r['draft'],r['compile_settings_from'])
    for f in [r['draft']]+sys.argv[2:]:
        text=open(f,errors='replace').read()
        c=check_draft.compare(text,r['compile_settings_from'],r['function'],r['address'],r['size'])
        extra='' if c['status'] in('consistent','code_exact') else ' missing=%s extra=%s'%(dict(c.get('missing',{})),dict(c.get('extra',{})))
        print('  %-55s lines=%s status=%s br=%s%s'%(f,c.get('lines'),c['status'],br(r,text),extra[:300]),flush=True)
