"""region.py <tmpl> <func> <addr> <size> <lo> <hi> drafts...: print our instructions lo..hi per draft, grouped."""
import sys,json,tempfile,collections
sys.path.insert(0,'research/tools')
import fast,varmap
t,f,a,s,lo,hi=sys.argv[1:7]; lo=int(lo);hi=int(hi)
g=collections.defaultdict(list)
for d in sys.argv[7:]:
    obj,log=fast.compile_object(json.load(open(t)),open(d).read(),tempfile.mkdtemp())
    if not obj: g['FAIL'].append(d);continue
    m=varmap.rows(['--disassemble='+f,obj])
    g[' | '.join(m[lo:hi])].append(d.split('/')[-1])
for k,v in g.items(): print(len(v),v[:6],'\n   ',k)
