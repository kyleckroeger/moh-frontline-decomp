"""unit.py <config.json> <src.cpp>: score every function of a unit config against the original."""
import sys,json
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
c=json.load(open(sys.argv[1])); t=open(sys.argv[2]).read()
tot=0
for f in c['functions']:
    s=score.real(sys.argv[1],t,f['name'],f['address'],str(f['size']))
    print(f['name'][:70],s); tot+= (s[0] if s else 999)
print('TOTAL',tot)
