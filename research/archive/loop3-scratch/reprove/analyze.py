# analyze.py TEST CTL SKIP IGNORE: like dolphin_boot prove, but only report (no marking)
import sys
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import dolphin_boot as d
t,c,skip,ign=sys.argv[1],sys.argv[2],int(sys.argv[3]),{int(x) for x in sys.argv[4].split(',') if x}
A=d.traces(t,skip,True,ign); B=d.traces(c,skip,True,ign)
x=d.first_difference(A,B)
print(len(A),len(B),'diff:',x)
same = x[0] if x else len(A)
ok = x is None or (x[0]==min(len(A),len(B)) and x[0]>=100)
print('PROVEN' if ok else 'NOT PROVEN', same)
for n in d.covered(t, same if ok else None, skip): print('COVER',n)
