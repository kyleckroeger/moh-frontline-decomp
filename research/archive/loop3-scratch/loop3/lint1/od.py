"""od.py <hexaddr> <len> [grep-regex]: original disassembly"""
import sys,re
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast,varmap
a=int(sys.argv[1],16); n=int(sys.argv[2],0)
rows=varmap.rows(['--start-address=%#x'%a,'--stop-address=%#x'%(a+n),fast.ELF])
pat=re.compile(sys.argv[3]) if len(sys.argv)>3 else None
for i,r in enumerate(rows):
    if not pat or pat.search(r): print('%08x %s'%(a+4*i,r))
