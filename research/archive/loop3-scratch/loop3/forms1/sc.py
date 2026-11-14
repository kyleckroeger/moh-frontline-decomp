# sc.py <function substring> <src>... : real score of src(s) with the draft's index settings
import sys, csv
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); import score
key=sys.argv[1]
for row in csv.DictReader(open('research/drafts/index.tsv'), delimiter='\t'):
    if key in row['function']:
        for s in sys.argv[2:]:
            print(s, score.real(row['compile_settings_from'], open(s,errors='replace').read(), row['function'], row['address'], row['size']), flush=True)
        break
