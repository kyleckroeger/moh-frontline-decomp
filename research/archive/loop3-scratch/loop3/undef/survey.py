import json, csv, collections, bisect, os, sys
os.chdir('/home/kyle/workspace/moh-frontline')
nm = {}
for l in open(sys.argv[1]):
    a = l.split()
    if len(a) == 4: nm.setdefault(a[3], []).append((int(a[0], 16), int(a[1], 16), a[2]))
fm = json.load(open('build/audit/file_map.json'))['files']
def file_of(addr, sec):
    for f in fm:
        for r in f['ranges']:
            if r['section'] == sec and int(r['start'], 16) <= addr < int(r['end'], 16):
                return f
snap = json.load(open('progress/GMFE69.snapshot.json'))
unm = collections.defaultdict(list)
for u in snap['code_map']['units']:
    for r in u['ranges']:
        unm[u.get('file_record')].append((r['name'], r['size'], int(r['address'], 16)))
proj = json.load(open('config/GMFE69/project.json'))['units']
units = [json.load(open('config/GMFE69/' + p)) for p in proj]
queue = {r['function']: r for r in csv.DictReader(open('research/plans/loop_queue.tsv'), delimiter='\t')}
drafts = {r['function']: r for r in csv.DictReader(open('research/drafts/index.tsv'), delimiter='\t')}
vts = [l.split('\t')[1] for l in open('research/layout/source_link.tsv') if l.startswith('undefined\t__vt__')]
out = []
for v in vts:
    addr, size, kind = nm[v][0]
    f = file_of(addr, '.data')
    if not f: out.append((v, hex(addr), '?', '', '', '')); continue
    text = [(int(r['start'], 16), int(r['end'], 16)) for r in f['ranges'] if r['section'] == '.text']
    frags = sorted({u['id'] for u in units for s in u['sections'] if s['name'] == '.text'
                    and any(a <= int(s['address'], 16) < b for a, b in text)})
    um = unm.get(f['ordinal'], [])
    desc = []
    for n, sz, a in sorted(um, key=lambda x: x[2]):
        q = queue.get(n, {})
        d = drafts.get(n)
        desc.append(f"{n.split('__')[0] or n}({sz},{q.get('area','')[:6]},{d['real_lines']+'L' if d else 'nodraft'}{','+q['note'][:40] if q.get('note') else ''})")
    out.append((v, hex(addr), f['name'], f['ordinal'], len(frags), len(um), ' '.join(frags), '; '.join(desc)))
for r in sorted(out, key=lambda r: (str(r[2]), r[0])):
    print('\t'.join(map(str, r)))
