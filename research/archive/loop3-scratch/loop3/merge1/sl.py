import sys, json, glob as _g
from pathlib import Path
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools')
import source_link as sl
R=Path('/home/kyle/workspace/moh-frontline')
sl.OUT=R/'scratch/loop3/merge1/source_link.tsv'
sl.WORK=R/'scratch/loop3/merge1/sl_work'
ids={u[:-5] for u in json.load(open(R/'config/GMFE69/project.json'))['units']}
real=sl.glob.glob
def g(p,*a,**k):
    r=real(p,*a,**k)
    if p.endswith('units/*/compiled.o'):
        r=[x for x in r if Path(x).parent.name in ids]
    return r
sl.glob.glob=g
sys.argv=['source_link.py','--jobs','4']
sl.main()
