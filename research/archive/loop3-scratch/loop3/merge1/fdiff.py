import sys,subprocess,difflib
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import fast
obj,fn=sys.argv[1],sys.argv[2]
mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',obj],capture_output=True,text=True).stdout)[fn]
for l in difflib.unified_diff(fast.target(fn),mine,n=2,lineterm=''): print(l)
