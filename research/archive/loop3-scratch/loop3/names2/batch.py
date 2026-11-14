"""batch.py: compile each unit with '#pragma dont_inline on' so its inline helpers are emitted out of line;
match each emitted weak function (by very-loose instruction key) against every original function."""
import sys,json,subprocess,tempfile,os,re,pickle,csv,collections
from concurrent.futures import ThreadPoolExecutor
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import fast
sys.path.insert(0,R+'scratch/loop3/names2'); import norm
vidx=pickle.load(open(R+'scratch/loop3/names2/orig_vidx.pkl','rb'))
units=sorted(set(r[0] for r in csv.reader(open(R+'scratch/loop3/names2/inlines.tsv'),delimiter='\t') if r[0]!='unit'))
if len(sys.argv)>1: units=[u for u in units if any(a in u for a in sys.argv[1:])]
def run(unit):
    base=os.path.splitext(os.path.basename(unit))[0]
    try: t=json.load(open(R+f'config/GMFE69/{base}.json'))
    except Exception: return unit,'NOCONFIG',[]
    src='#pragma dont_inline on\n'+open(R+'src/'+unit,errors='replace').read()
    env_suffix='c' if unit.endswith('.c') else 'cpp'
    with tempfile.TemporaryDirectory() as w:
        from setup_compiler import setup_compiler
        compiler, wrapper = setup_compiler(t['compiler_version'], 'GC')
        p=os.path.join(w,'probe.'+env_suffix); open(p,'w').write(src)
        cmd=[str(wrapper),str(compiler/'mwcceppc.exe'),'-c',*t['compiler_flags'],'-I-',*[f'-I{R+p2}' for p2 in t.get('include_dirs',[])],p,'-o','probe.o']
        r=subprocess.run(cmd,cwd=w,capture_output=True,text=True)
        obj=os.path.join(w,'probe.o')
        if r.returncode or not os.path.exists(obj): return unit,'FAIL '+(r.stdout+r.stderr)[-200:].replace('\n',' '),[]
        syms=subprocess.run([fast.OBJDUMP,'-t',obj],capture_output=True,text=True).stdout
        weak=set(l.split()[-1] for l in syms.splitlines() if ' w ' in l and ' F ' in l or (' w ' in l and '.text' in l))
        dis=subprocess.run([fast.OBJDUMP,'-d',obj],capture_output=True,text=True).stdout
    f=norm.parse_dis(dis); hits=[]
    for (n,a),ins in f.items():
        if n not in weak or not ins: continue
        k=norm.vloose(ins)
        if k in vidx:
            hits.append((n,len(ins),[(o[0],'%x'%o[1]) for o in vidx[k]][:6]))
        else: hits.append((n,len(ins),[]))
    return unit,'OK',hits
with ThreadPoolExecutor(3) as ex, open(R+'scratch/loop3/names2/batch_out.tsv','a') as out:
    for unit,st,hits in ex.map(run,units):
        if st!='OK': out.write(f'{unit}\t{st}\n'); print(unit,st[:100]); continue
        for n,L,m in hits:
            out.write(f'{unit}\t{n}\t{L}\t'+' '.join(a+'@'+b for a,b in m)+'\n')
        out.flush()
