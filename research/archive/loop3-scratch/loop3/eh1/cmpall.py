import sys, csv, difflib
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/scratch/loop3/eh1')
import eh
R = eh.R
for r in csv.DictReader(open(R + 'research/drafts/index.tsv'), delimiter='\t'):
    if not any(k in r['function'] for k in sys.argv[1:]): continue
    o = eh.fmt(eh.orig_record(int(r['address'], 16))).splitlines()
    m = eh.fmt(eh.obj_record(R + r['compile_settings_from'], open(R + r['draft']).read(), r['function'])).splitlines()
    print('==', r['function'], r['draft'])
    for l in difflib.unified_diff(o, m, 'orig', 'ours', n=0, lineterm=''): print(l[:400])
