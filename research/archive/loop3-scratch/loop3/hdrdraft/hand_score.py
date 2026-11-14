import sys,os,csv,json
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/loop3/hdrdraft')
os.chdir('/home/kyle/workspace/moh-frontline')
import drive, fast, tempfile
key, path = sys.argv[1], sys.argv[2]
r=next(r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t') if key in r['draft'])
st=drive.settings(r['compile_settings_from'])
text=open(path).read()
s=drive.sc(st,text,r)
if s is None:
    with tempfile.TemporaryDirectory() as w:
        print(fast.compile_object(json.load(open(st)),text,w)[1][:1500])
else:
    c=drive.cd.compare(text,st,r['function'],r['address'],r['size'])
    print('score',s,'old',drive.sc(st,open(r['draft']).read(),r),'check',c['status'],'br',drive.branches(st,text,r), {k:c[k] for k in ('missing','extra','widths') if c.get(k)})
