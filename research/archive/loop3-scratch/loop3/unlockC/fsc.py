# fsc.py tmpl src func=addr:size ... : real score of several functions in one source
import sys
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); import score
t,s=sys.argv[1],open(sys.argv[2]).read()
for a in sys.argv[3:]:
    f,r=a.split('='); ad,sz=r.split(':')
    print(f, score.real(t,s,f,ad,sz),flush=True)
