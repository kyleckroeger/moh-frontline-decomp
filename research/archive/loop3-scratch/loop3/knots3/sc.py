# sc.py KEY file...: real score; KEY looks up index.tsv row by function substring
import sys,csv
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import score
row=[r for r in csv.DictReader(open(R+'research/drafts/index.tsv'),delimiter='\t') if sys.argv[1] in r['function']][0]
for f in sys.argv[2:]:
    print(f, score.real(R+row['compile_settings_from'],open(f).read(),row['function'],row['address'],row['size']),flush=True)
