import sys,re
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/loop3/hdr6')
from survey import ns_blocks
t=open(sys.argv[1]).read()
for a,b in ns_blocks(t): print(t[a:b]); print('-----')
