"""gamerow.py <file.cpp> <note>: rewrite the Game.md row of a merged file (name cell without '(fragment)')."""
import sys,re
p='/home/kyle/workspace/moh-frontline/docs/Game.md'; t=open(p).read().split('\n')
f,note=sys.argv[1],sys.argv[2]
n=0
for i,l in enumerate(t):
    if l.startswith('| `%s` (fragment)'%f) or l.startswith('| `%s` |'%f):
        cells=l.split(' | ')
        cells[0]='| `%s`'%f
        cells[-1]=note+' |'
        t[i]=' | '.join(cells); n+=1
print('rows',n)
open(p,'w').write('\n'.join(t))
