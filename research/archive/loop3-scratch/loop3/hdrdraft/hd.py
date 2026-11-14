"""hd.py KEY FIXFILE: run the greedy header move on a draft with pre/post fixes from FIXFILE (defines pre(text), post(text, cls))."""
import sys, os, csv, json
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/scratch/loop3/hdrdraft')
import drive
key, fix = sys.argv[1], sys.argv[2]
ns = {}
exec(open('/home/kyle/workspace/moh-frontline/scratch/loop3/hdrdraft/' + fix).read(), ns)
r = next(r for r in csv.DictReader(open('research/drafts/index.tsv'), delimiter='\t') if key in r['draft'])
res = drive.work(r, ns.get('pre'), ns.get('post'))
print(json.dumps(res))
with open(drive.OUT / 'results_hand.jsonl', 'a') as f: f.write(json.dumps(res) + '\n')
