"""sc.py <tmpl.json> <func> <addr> <size> files... : real-line score per file"""
import sys
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import score
t,f,a,s=sys.argv[1:5]
for p in sys.argv[5:]:
    print(score.real(t,open(p,errors='replace').read(),f,a,s),p)
