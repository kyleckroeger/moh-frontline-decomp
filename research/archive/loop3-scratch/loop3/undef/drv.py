import sys,glob
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/tools')
from pathlib import Path
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/loop3/undef')
import source_link as sl
sl.ROOT=Path('/home/kyle/workspace/moh-frontline')
units=sorted(glob.glob(str(sl.ROOT/'build/reconstruction/units/*/compiled.o')))
drafts=sorted(glob.glob(str(sl.ROOT/'build/source_link/drafts/*.o')))
data=sorted(glob.glob(str(sl.ROOT/'build/source_link/data/*.o')))
f=sl.resolve(units,drafts,data)
for k in ("undefined","fragment_local","duplicate","small_data","stripped"):
    print(k,len(f[k]))
print('stripped:',sorted(f['stripped']))
print('undef non-vt:',sorted(x for x in f['undefined'] if not x.startswith('__vt__')))
