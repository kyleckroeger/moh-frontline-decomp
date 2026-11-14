"""scan.py: every draft in research/drafts/index.tsv whose original has an extab action record; compare with our compile."""
import sys, csv
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/scratch/loop3/eh1')
import eh
R = eh.R
area = {}
for r in csv.DictReader(open(R + 'research/plans/loop_queue.tsv'), delimiter='\t'):
    area[r['function']] = r['area']
rows = list(csv.DictReader(open(R + 'research/drafts/index.tsv'), delimiter='\t'))
out = open(R + 'scratch/loop3/eh1/records.tsv', 'w')
out.write('function\taddress\tsize\treal_lines\tarea\torig_actions\tours_actions\tsame\tdraft\n')
only = sys.argv[1:]
for r in rows:
    f = r['function']; a = int(r['address'], 16)
    if only and not any(o in f for o in only): continue
    o = eh.orig_record(a)
    ar = area.get(f, '?')
    try:
        m = eh.obj_record(R + r['compile_settings_from'], open(R + r['draft']).read(), f)
    except Exception as ex:
        m = 'ERR %s' % ex
    oa = isinstance(o, tuple) and any(o[2].values()); ma = isinstance(m, tuple) and any(m[2].values())
    if not oa and not ma and not str(m).startswith(('ERR', 'COMPILE')): continue
    so, sm = eh.summary(o), eh.summary(m)
    same = 'yes' if isinstance(m, tuple) and eh.fmt(o) == eh.fmt(m) else ('set' if so == sm else 'NO')
    out.write('\t'.join([f, r['address'], r['size'], r['real_lines'], ar, so, sm, same, r['draft']]) + '\n')
    print(f[:60], r['real_lines'], ar, same, '\n   O:', so, '\n   M:', sm, flush=True)
