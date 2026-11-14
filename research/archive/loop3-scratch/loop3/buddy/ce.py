import sys,json,tempfile,csv
sys.path.insert(0,'scratch/game'); import fast
r=[r for r in csv.DictReader(open("research/drafts/index.tsv"),delimiter="\t") if sys.argv[1] in r["function"]][0]
o,log=fast.compile_object(json.load(open(r['compile_settings_from'])),open(sys.argv[2]).read(),tempfile.mkdtemp())
print(o); print(log[:1500] if log else "")
