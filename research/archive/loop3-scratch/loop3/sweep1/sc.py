"""sc.py <tmpl> <func> <addr> <size> <draft>... : real score of each draft. -s: side by side of the first."""
import sys
sys.path.insert(0,'research/tools')
import score
args=sys.argv[1:]
sbs = args[0]=='-s'
if sbs: args=args[1:]
t,f,a,s=args[:4]
for d in args[4:]:
    print(d, score.real(t,open(d,errors='replace').read(),f,a,s))
if sbs:
    import subprocess
    e=hex(int(a,16)+int(s))
    subprocess.run(['python3','research/tools/sbs.py',t,args[4],f,'0x'+a.replace('0x',''),e])
