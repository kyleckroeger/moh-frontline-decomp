"""sct.py <tmpl> <function-substring> <draft>...: score with explicit template"""
import csv,sys
sys.path.insert(0,'research/tools'); import score
row=[r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t') if sys.argv[2] in r['function']][0]
for d in sys.argv[3:]:
    print(d, score.real(sys.argv[1],open(d,errors='replace').read(),row['function'],row['address'],row['size']),flush=True)
