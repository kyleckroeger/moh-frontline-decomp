"""vr.py KEY base.cpp variants.py: V = {name: [(old,new),...]} applied to base; prints real score per variant (KEY = index.tsv function substring)."""
import sys,csv
from pathlib import Path
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import score
key,base,vf=sys.argv[1:4]
if key.count(':')==3:
    u,f,a,z=key.split(':'); row={'compile_settings_from':f'config/GMFE69/{u}.json','function':f,'address':a,'size':z}
else:
    row=[r for r in csv.DictReader(open(R+'research/drafts/index.tsv'),delimiter='\t') if key in r['function']][0]
text=open(base).read(); ns={}; exec(open(vf).read(),ns)
out=Path(vf).with_suffix(''); out.mkdir(exist_ok=True)
for name,pairs in ns['V'].items():
    s=text; bad=None
    for o,n in pairs:
        if o not in s: bad=o; break
        s=s.replace(o,n,1)
    if bad: print('MISSING',name,repr(bad[:60])); continue
    (out/f'{name}.cpp').write_text(s)
    print(name, score.real(R+row['compile_settings_from'],s,row['function'],row['address'],row['size']),flush=True)
