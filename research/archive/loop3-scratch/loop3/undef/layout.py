import json, sys, os, collections
os.chdir('/home/kyle/workspace/moh-frontline')
NM='/tmp/claude-1000/-home-kyle-workspace-moh-frontline/a5d18f30-70be-41af-9a00-608205f1aeba/scratchpad/orig.nm'
syms=[]
for l in open(NM):
    a=l.split()
    if len(a)==4: syms.append((int(a[0],16),int(a[1],16),a[2],a[3]))
syms.sort()
fm={f['name']:f for f in json.load(open('build/audit/file_map.json'))['files']}
snap=json.load(open('progress/GMFE69.snapshot.json'))
unm={int(r['address'],16) for u in snap['code_map']['units'] for r in u['ranges']}
owner={}
for p in json.load(open('config/GMFE69/project.json'))['units']:
    u=json.load(open('config/GMFE69/'+p))
    for f in u['functions']: owner[int(f['address'],16)]=u['id']
    for s in u['sections']:
        if s['name'] not in ('.text',):
            a=int(s['address'],16)+s.get('linked_offset',0)
            owner.setdefault(('d',s['name'],a),[]).append((u['id'],s.get('linked_size',s['size'])))
f=fm[sys.argv[1]]
for r in f['ranges']:
    if r['section'] not in sys.argv[2].split(','): continue
    lo,hi=int(r['start'],16),int(r['end'],16)
    print('==',r['section'],hex(lo),hex(hi),hi-lo)
    for a,sz,t,n in syms:
        if lo<=a<hi:
            tag='UNMATCHED' if a in unm else owner.get(a,'')
            print(f'  {a:08x} {sz:6} {t} {n[:70]} {tag}')
# data-owning units in the file
for k,v in owner.items():
    if isinstance(k,tuple):
        for r in f['ranges']:
            if r['section']==k[1] and int(r['start'],16)<=k[2]<int(r['end'],16): print('  dataunit',k[1],hex(k[2]),v)
