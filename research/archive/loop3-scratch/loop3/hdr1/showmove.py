import sys,os,difflib
os.environ["TRY_HEADER_DIR"]="/home/kyle/workspace/moh-frontline/scratch/loop3/hdr1/include"
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
import try_header as th
for f in sys.argv[2:]:
    t=open(f).read(); n=th.moved(t,sys.argv[1])
    sys.stdout.writelines(difflib.unified_diff(t.splitlines(1),n.splitlines(1),f,f,n=0))
