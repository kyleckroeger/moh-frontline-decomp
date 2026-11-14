"""chk.py <flags-template-unit> <source> <unit> [unit...]: compile merged source, score every function of the units,
show text order vs original and section sizes."""
import json,sys,os,subprocess,tempfile
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
os.chdir('/home/kyle/workspace/moh-frontline')
import fast
tpl=json.load(open('config/GMFE69/%s.json'%sys.argv[1])) if not sys.argv[1].endswith('.json') else json.load(open(sys.argv[1]))
src=open(sys.argv[2]).read()
funcs=[]
for u in sys.argv[3:]:
    c=json.load(open('config/GMFE69/%s.json'%u))
    funcs+= [(f['name'],int(f['address'],16)) for f in c['functions']+c.get('label_functions',[])]
funcs.sort(key=lambda x:x[1])
wd=tempfile.mkdtemp(dir='/tmp/claude-1000') if os.path.isdir('/tmp/claude-1000') else tempfile.mkdtemp()
obj,log=fast.compile_object(tpl,src,wd)
if not obj: print(log[-3000:]); sys.exit(1)
sc=fast.score(tpl,src,[(f,) for f,_ in funcs],wd)
bad=[(f,v) for f,v in sc.items() if v]
print('functions',len(funcs),'nonzero',len(bad))
for f,v in bad: print('  ',v,f)
nm=subprocess.run(['build/tools/powerpc-eabi-nm','-n',obj],capture_output=True,text=True).stdout.split('\n')
tsy=[l.split()[-1] for l in nm if ' T ' in l or ' W ' in l or ' t ' in l or ' w ' in l]
ours=[l for l in tsy if l in dict(funcs)]
print('order ok' if ours==[f for f,_ in funcs if f in ours] else 'ORDER DIFFERS:\n  '+'\n  '.join(ours))
print('extra text syms', [l for l in tsy if l not in dict(funcs)])
print(subprocess.run(['build/tools/powerpc-eabi-objdump','-h',obj],capture_output=True,text=True).stdout.split('Idx')[1][:3000])
print('OBJ',obj)
