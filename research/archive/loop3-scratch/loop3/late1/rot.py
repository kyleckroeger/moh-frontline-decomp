# print ours->orig register rotation summary for a draft
import sys,json,tempfile,re,collections
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast,varmap
tm='/home/kyle/workspace/moh-frontline/config/GMFE69/staticobjfactory_helmet.json'
o=varmap.rows(['--start-address=0x800b254c','--stop-address=0x800b327c',fast.ELF])
for f in sys.argv[1:]:
    obj,log=fast.compile_object(json.load(open(tm)),open(f).read(),tempfile.mkdtemp())
    if not obj: print(f,'FAIL'); continue
    m=varmap.rows(['--disassemble=Init__20CStaticObjectFactoryFv',obj])
    c=collections.Counter(); bad=0
    for x,y in zip(o,m):
        rx=re.findall(r'\br(\d+)\b',x); ry=re.findall(r'\br(\d+)\b',y)
        if x!=y and not ('0(0)' in y or re.search(r',0$',y) or 'lis' in y): bad+=1
        for a,b in zip(ry,rx):
            if a!=b: c[(int(a),int(b))]+=1
    print(f.split('/')[-1],len(o)-len(m),bad,' '.join('%d>%d:%d'%(a,b,n) for (a,b),n in sorted(c.items())))
