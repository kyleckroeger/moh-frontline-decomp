import sys,json,subprocess,difflib
sys.path.insert(0,'scratch/game');import fast
f=sys.argv[1];o=sys.argv[2]
a=[x if isinstance(x,str) else x[0] for x in fast.target(f)]
b=[x if isinstance(x,str) else x[0] for x in fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout).get(f,[])]
for l in difflib.unified_diff(a,b,n=1,lineterm=''): print(l)
