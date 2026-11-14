"""sc.py <function-substring> <draft>...: score drafts against index row"""
import csv,sys
sys.path.insert(0,'research/tools'); import score
row=[r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t') if sys.argv[1] in r['function']][0]
for d in sys.argv[2:]:
    print(d, score.real(row['compile_settings_from'],open(d,errors='replace').read(),row['function'],row['address'],row['size']),flush=True)
