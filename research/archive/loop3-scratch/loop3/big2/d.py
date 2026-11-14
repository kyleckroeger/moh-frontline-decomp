import sys; sys.path.insert(0,'scratch/loop3/big1'); sys.path.insert(0,'research/tools')
import sc,score
m=sc.asm(open(sys.argv[1]).read()); o=sc.orig()
a,b=int(sys.argv[2]),int(sys.argv[3])
for i in range(a,b):
    x=o[i]; y=m[i] if i<len(m) else ''
    print(i,'  ' if score.eq(x,y) else '**', str(x)[:60].ljust(60), str(y)[:60])
