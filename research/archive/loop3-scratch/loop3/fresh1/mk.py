import csv,glob,re,subprocess,os
q=list(csv.DictReader(open('research/plans/loop_queue.tsv'),delimiter='\t'))
idx={r['function']:r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t')}
claims=open('scratch/loop3/CLAIMS.txt').read()
notes=''.join(open(f).read() for f in glob.glob('scratch/loop3/*/notes.md'))
logs=''.join(open(f).read() for f in glob.glob('research/log/*'))
# log since 2026-11-11 for autopilot: take text after first '2026-11-11'
ap=open('research/log/autopilot.md').read(); i=ap.find('2026-11-11'); aprun=ap[i:] if i>=0 else ''
def demangle(m):
    mm=re.match(r'(\w+?)__(\d+)(\w+)',m)
    base=re.sub(r'<.*','',m.split('__')[0] if '__' in m[1:] else m)
    if m.startswith('__'): base=m
    cls=None
    if mm:
        n=int(mm.group(2)); cls=mm.group(3)[:n]
    return base,cls
out=[]
for r in q:
    f=r['function']
    if f not in idx: continue
    if r['area']=='Multiplayer': continue
    base,cls=demangle(f)
    keys=[f]
    if cls: keys.append(f'{cls}::{base}')
    if len(base)>=10 and base not in ('Init','Draw','Update'): keys.append(base)
    def cnt(t): return max(t.count(k) for k in keys)
    out.append((int(idx[f]['real_lines'] or 0),int(r['size']),cnt(claims),cnt(notes),cnt(aprun),cnt(logs),r['area'],f,r['note'],idx[f]['draft']))
out.sort(key=lambda x:(x[2]+x[3],-x[1]/(x[0]+1)))
with open('scratch/loop3/fresh1/untouched.tsv','w') as o:
    o.write('real_lines\tsize\tclaims\tnotes\tautopilot_run\tlogs_all\tarea\tfunction\tnote\tdraft\n')
    for x in out: o.write('\t'.join(map(str,x))+'\n')
print(len(out))
