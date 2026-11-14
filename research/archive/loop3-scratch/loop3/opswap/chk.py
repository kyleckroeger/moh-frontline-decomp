# chk.py FUNC FILE...: check_draft.compare status for variant files
import sys, csv, os
os.chdir('/home/kyle/workspace/moh-frontline'); sys.path.insert(0, 'research/tools')
import check_draft
r = {x['function']: x for x in csv.DictReader(open('research/drafts/index.tsv'), delimiter='\t')}[sys.argv[1]]
for fn in sys.argv[2:]:
    c = check_draft.compare(open(fn, errors='replace').read(), r['compile_settings_from'], r['function'], r['address'], r['size'])
    print(None if c is None else (c['status'], round(c['similarity'], 3), c['calls_same'], dict(c['missing']), dict(c['extra'])), fn)
