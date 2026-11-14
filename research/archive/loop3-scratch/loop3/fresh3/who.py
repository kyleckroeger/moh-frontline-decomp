# who.py tmpl func addr size file [a b] [regclass f|r]: orig>ours register pairs over lines a..b (opcode-equal lines only)
import sys,json,re,collections,tempfile,os
R='/home/kyle/workspace/moh-frontline'
sys.path.insert(0,R+'/research/tools'); sys.path.insert(0,R+'/scratch/game')
import fast,varmap
T=json.load(open(sys.argv[1])); fn=sys.argv[2]; ad=int(sys.argv[3],16); sz=int(sys.argv[4])
if sys.argv[5].endswith('.c'): os.environ['SUFFIX']='c'
obj,log=fast.compile_object(T,open(sys.argv[5]).read(),tempfile.mkdtemp())
if not obj: print(log); sys.exit(1)
m=varmap.rows(['--disassemble='+fn,obj])
o=varmap.rows(['--start-address=%#x'%ad,'--stop-address=%#x'%(ad+sz),fast.ELF])
a,b=(int(sys.argv[6]),int(sys.argv[7])) if len(sys.argv)>7 else (0,len(o))
rc=sys.argv[8] if len(sys.argv)>8 else 'f'
c=collections.Counter(); bad=0
for i in range(a,min(b,len(m),len(o))):
    x=o[i].split(None,1); y=m[i].split(None,1)
    if x[0]!=y[0]: bad+=1; continue
    rx=re.findall(r'\b%s\d+'%rc,x[1] if len(x)>1 else ''); ry=re.findall(r'\b%s\d+'%rc,y[1] if len(y)>1 else '')
    for p,q in zip(rx,ry):
        if p!=q: c[(p,q)]+=1
print('opcode-mismatch',bad,'len',len(m),len(o))
print(' '.join('%s>%s:%d'%(p,q,n) for (p,q),n in c.most_common()))
