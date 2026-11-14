"""fd.py OBJ FUNC [target-name]: unified diff of FUNC (normalised) vs original."""
import sys,subprocess,difflib
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); import fast
mine=fast.parse(subprocess.run([fast.OBJDUMP,'-dr',sys.argv[1]],capture_output=True,text=True).stdout)
f=sys.argv[2]; t=fast.target(sys.argv[3] if len(sys.argv)>3 else f)
for l in difflib.unified_diff(t,mine[f],n=2,lineterm=''): print(l)
