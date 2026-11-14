"""v.py KEY base.cpp variants.py : variants.py defines V = {name: [(old,new),...]} ; scores each (score.real)."""
import sys,csv,os
sys.path.insert(0,'research/tools'); import score
key,base,vf=sys.argv[1:4]
row=next(r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t') if key in r['function'])
b=open(base).read(); g={}; exec(open(vf).read(),g)
outd=os.path.dirname(vf)
for k,reps in g['V'].items():
    s=b; ok=True
    for o,n in reps:
        if o not in s: print(k,'MISSING',o[:50]); ok=False; break
        s=s.replace(o,n,1)
    if not ok: continue
    open(os.path.join(outd,'v_'+k+'.cpp'),'w').write(s)
    import json,tempfile
    d=json.load(open(row['compile_settings_from']));d['include_dirs']=d.get('include_dirs',[])+['src/game','src/ea','include'];tp=tempfile.mktemp(suffix='.json');json.dump(d,open(tp,'w'))
    print(k, score.real(tp,s,row['function'],row['address'],row['size']), flush=True)
