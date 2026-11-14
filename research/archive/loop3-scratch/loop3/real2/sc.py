import sys
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import score
cfg,src=sys.argv[1:3]
for spec in sys.argv[3:]:
    f,a,z=spec.split(':')
    print(f, score.real(R+cfg,open(src).read(),f,a,z))
