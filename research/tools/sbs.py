"""sbs.py <tmpl> <draft> <func> <start> <end>: side-by-side orig | ours with << on register differences (research helper)."""
import sys,json,re
import fast,varmap,tempfile
tm,dr,f,a,e=sys.argv[1:6]
obj,log=fast.compile_object(json.load(open(tm)),open(dr,errors='replace').read(),tempfile.mkdtemp())
if not obj: sys.exit(log)
m=varmap.rows(['--disassemble='+f,obj]); o=varmap.rows(['--start-address='+a,'--stop-address='+e,fast.ELF])
for i,(x,y) in enumerate(zip(o,m)): print(f'{i:4} {x:30} {y:30} {"<<" if x!=y else ""}')
