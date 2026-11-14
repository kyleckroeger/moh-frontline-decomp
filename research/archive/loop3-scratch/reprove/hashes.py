import csv,hashlib,os
os.chdir('/home/kyle/workspace/moh-frontline')
for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    p=r['draft']
    h=hashlib.sha1(open(p,'rb').read()).hexdigest()[:8] if os.path.exists(p) else 'MISSING'
    print(r['function'],h,sep='\t')
