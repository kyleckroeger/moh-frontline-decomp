import sys,os,csv,json,glob,re,tempfile
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/research/tools'); sys.path.insert(0,'/home/kyle/workspace/moh-frontline/scratch/loop3/hdrdraft')
os.chdir('/home/kyle/workspace/moh-frontline')
import drive, fast
rows={Path:r for Path,r in ((r['draft'].split('/')[-1],r) for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'))}
for f in sorted(glob.glob('scratch/loop3/hdrdraft/out/*.nc.cpp')):
    if sys.argv[1:] and not any(k in f for k in sys.argv[1:]): continue
    b=os.path.basename(f); d=b.split('.cpp.')[0]+'.cpp'; d=d if d in rows else b.split('.c.')[0]+'.c'
    r=rows[d]; st=drive.settings(r['compile_settings_from'])
    with tempfile.TemporaryDirectory() as w:
        log=fast.compile_object(json.load(open(st)),open(f).read(),w)[1]
    errs=[]
    lines=log.splitlines()
    for i,l in enumerate(lines):
        if l.strip().startswith('#   Error'):
            src=lines[i-1].strip()[:90]; msg=lines[i+1].strip()[2:][:70]
            errs.append(f'{msg} @ {src}')
    print('==',b, len(errs)); [print('   ',e) for e in errs[:4]]
