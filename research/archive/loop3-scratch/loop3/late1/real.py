import sys
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); import score
t,f,a,n=sys.argv[1:5]
for fn in sys.argv[5:]: print(score.real(t,open(fn).read(),f,a,n),fn)
