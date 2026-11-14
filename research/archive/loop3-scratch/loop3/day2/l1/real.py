import sys,csv
sys.path.insert(0,'research/tools'); import score
rows={r['function']:r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t')}
f=sys.argv[1]; r=rows[f]
for fn in [r['draft']]+sys.argv[2:]:
    print(score.real(r['compile_settings_from'],open(fn).read(),f,r['address'],r['size']),fn)
