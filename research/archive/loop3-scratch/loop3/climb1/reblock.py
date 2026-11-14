# reblock.py best.cpp firstline lastline out_block.txt : extract block lines (by exact first/last stripped text) from best file
import sys
t=open(sys.argv[1]).read().split('\n'); a=b=None
for i,l in enumerate(t):
    if a is None and l.strip()==sys.argv[2]: a=i
    if a is not None and l.strip()==sys.argv[3]: b=i; break
open(sys.argv[4],'w').write('\n'.join(t[a:b+1])+'\n')
