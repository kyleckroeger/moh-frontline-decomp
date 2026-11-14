# cd.py <function substring> <src>: check_draft compare() on a scratch file
import sys,csv; sys.path.insert(0,'research/tools'); import check_draft as C
for row in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    if sys.argv[1] in row['function']:
        r=C.compare(open(sys.argv[2],errors='replace').read(),row['compile_settings_from'],row['function'],row['address'],row['size'])
        print(r['status'],r.get('lines'),r.get('similarity'),{k:r[k] for k in ('missing','extra','widths') if k in r}); break
