"""sc.py <function> <file>...: score variant files against the index row's function."""
import csv,sys
sys.path.insert(0,'research/tools'); import score
f=sys.argv[1]
r=[r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t') if r['function']==f][0]
import os
if os.environ.get('TM'): r['compile_settings_from']=os.environ['TM']
for p in sys.argv[2:]:
    print(p, score.real(r['compile_settings_from'],open(p,errors='replace').read(),f,r['address'],r['size']),flush=True)
