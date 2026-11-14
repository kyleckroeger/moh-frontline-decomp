"""sc.py <func-substring> <variant.cpp>...: score variants against an index row."""
import sys,csv
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score
rows=[r for r in csv.DictReader(open("research/drafts/index.tsv"),delimiter="\t") if sys.argv[1] in r["function"]]
r=rows[0]
import os
if os.environ.get("TM"): r["compile_settings_from"]=os.environ["TM"]
for f in sys.argv[2:]:
    try: s=score.real(r["compile_settings_from"],open(f).read(),r["function"],r["address"],r["size"])
    except Exception as e: s='ERR %s'%e
    print(f,s)
