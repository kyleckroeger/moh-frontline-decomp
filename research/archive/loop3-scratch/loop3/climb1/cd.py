import sys,csv
R='/home/kyle/workspace/moh-frontline/'
sys.path.insert(0,R+'research/tools'); import os; os.chdir(R)
import check_draft as c
key,f=sys.argv[1:3]
row=[r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t') if key in r['function']][0]
r=c.compare(open(f).read(),row['compile_settings_from'],row['function'],row['address'],row['size'])
print(key,r['status'],r.get('lines'),r.get('similarity'))
