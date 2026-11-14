import csv,re,os,glob
rows=list(csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'))
files=set(f for f in glob.glob('scratch/loop3/**/*',recursive=True) if 'best' in os.path.basename(f).lower())
for n in glob.glob('scratch/loop3/*/*.md')+glob.glob('scratch/loop3/*.md')+['scratch/loop3/CLAIMS.txt','research/plans/three-day-loop.md']+glob.glob('research/log/*.md'):
    t=open(n,errors='replace').read()
    d=os.path.dirname(n)
    for m in re.finditer(r'[\w./-]+\.(?:cpp|c)\b',t):
        p=m.group(0)
        for c in (p,os.path.join(d,p),'scratch/loop3/'+p,'scratch/'+p):
            if os.path.isfile(c) and c.startswith('scratch'): files.add(os.path.normpath(c)); break
files={f for f in files if f.endswith(('.cpp','.c'))}
out=[]
for r in rows:
    base=os.path.basename(r['draft']).split('.')[0]
    # defining pattern: base( preceded by :: or space at line start-ish with { following later
    pat=re.compile(r'(::|^|\s|\*)'+re.escape(base)+r'\s*\([^;]*?\)\s*(const\s*)?\{',re.M|re.S)
    for f in sorted(files):
        t=open(f,errors='replace').read()
        if base in t and pat.search(t): out.append((r['function'],r['real_lines'],f))
with open('scratch/loop3/harvest1/cands.tsv','w') as o:
    for x in out: o.write('\t'.join(x)+'\n')
print(len(files),len(out),len({x[0] for x in out}))
