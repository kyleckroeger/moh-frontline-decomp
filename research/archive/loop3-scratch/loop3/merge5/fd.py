"""fd.py <obj> <func>: diff original vs object's function."""
import sys,os,subprocess,difflib
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); os.chdir('/home/kyle/workspace/moh-frontline')
import fast
mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',sys.argv[1]],capture_output=True,text=True).stdout)
for l in difflib.unified_diff(fast.target(sys.argv[2]),mine[sys.argv[2]],n=1,lineterm=''): print(l)
