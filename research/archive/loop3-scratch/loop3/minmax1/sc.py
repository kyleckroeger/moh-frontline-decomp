import sys; sys.path.insert(0,'research/tools'); import score
t,f,a,z=sys.argv[1:5]
for p in sys.argv[5:]: print(p, score.real(t,open(p).read(),f,a,z))
