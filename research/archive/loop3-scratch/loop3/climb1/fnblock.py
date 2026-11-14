# fnblock.py draft.cpp 'signature start' out_block.txt : body lines of the function (between its opening line and the closing '}' at col 0)
import sys
t=open(sys.argv[1]).read().split('\n')
a=[i for i,l in enumerate(t) if l.startswith(sys.argv[2])][0]
b=next(i for i in range(a+1,len(t)) if t[i]=='}')
open(sys.argv[3],'w').write('\n'.join(t[a+1:b])+'\n')
print(b-a-1,'lines')
