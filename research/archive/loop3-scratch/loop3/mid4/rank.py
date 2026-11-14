import csv,glob,re,os
rows=list(csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'))
q={r['function']:r for r in csv.DictReader(open('research/plans/loop_queue.tsv'),delimiter='\t')}
claims=open('scratch/loop3/CLAIMS.txt').read().splitlines()
notes=[(p,open(p,errors='replace').read()) for p in glob.glob('scratch/loop3/*/notes.md')]
state=open('research/plans/three-day-loop.md').read()
state=state[state.index('Waiting on the maintainer'):state.index('## Hard rules')]
pend=os.listdir('research/drafts/pending')
shorts={}
for r in rows:
    s=os.path.basename(r['draft']).split('.')[0]; r['short']=s; shorts[s]=shorts.get(s,0)+1
out=[]
for r in rows:
    f=r['function']; s=r['short']; size=int(r['size'])
    qr=q.get(f,{}); area=qr.get('area','?'); note=qr.get('note','')
    mp = area=='Multiplayer' or 'Multiplayer' in f
    keys=[f]
    if shorts[s]==1 and len(s)>5: keys.append(s)
    m=re.match(r'(\w+?)__(\d+)(\w+)',f)
    if m:
        n=int(m.group(2)); cls=m.group(3)[:n]; keys.append(cls+'::'+m.group(1))
    def hit(t): return any(k in t for k in keys)
    nclaim=sum(1 for c in claims if hit(c) and ' released ' not in c and ' done' not in c)
    nnote=sum(1 for p,t in notes for line in t.splitlines() if hit(line))
    why=[]
    if mp: why.append('multiplayer')
    if note: why.append('queue-note')
    if any(p.startswith(s+'.') for p in pend): why.append('pending')
    if any(k in state for k in keys if k!=s) or (len(s)>8 and re.search(r'\b'+re.escape(s)+r'\b',state)): why.append('state-q')
    if f.startswith('GetExtents__7CVolBox') and why==['state-q']: why=[]
    if f in ('BSInitBIFuncs__Fv','__ct__19PopUpMessageHandlerFPcPc'): why.append('state-q9')
    if f=='PopUpMessageHandler_Loop__19PopUpMessageHandlerFv': why.append('code-exact-needs-identity(mid3 q)')
    att=nclaim+nnote
    out.append((size/(1+att),f,size,r['real_lines'],nclaim,nnote,';'.join(why),r['draft']))
out.sort(key=lambda x:-x[0])
with open('scratch/loop3/mid4/rank.tsv','w') as fo:
    fo.write('score\tfunction\tsize\treal_lines\tclaims\tnote_mentions\texcluded\tdraft\n')
    for o in out:
        if o[6]: continue
        fo.write('%.0f\t%s\t%d\t%s\t%d\t%d\t%s\t%s\n'%o)
    fo.write('# excluded\n')
    for o in out:
        if o[6]: fo.write('%.0f\t%s\t%d\t%s\t%d\t%d\t%s\t%s\n'%o)
