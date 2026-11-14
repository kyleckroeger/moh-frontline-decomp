import sys; sys.path.insert(0,'research/tools'); import score
t,f,a,n=sys.argv[1:5]
for src in sys.argv[5:]:
    print(src.split('/')[-1], score.real(t, open(src).read(), f, a, n))
