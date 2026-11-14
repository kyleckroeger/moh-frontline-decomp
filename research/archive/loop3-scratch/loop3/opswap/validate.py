# validate.py finals.tsv: real (old,new), check_draft status, conversion-count guard
import sys, csv, os, json, tempfile, subprocess
os.chdir('/home/kyle/workspace/moh-frontline'); sys.path.insert(0,'scratch/game'); sys.path.insert(0,'research/tools')
import score, check_draft, fast
rows = {x['function']: x for x in csv.DictReader(open('research/drafts/index.tsv'), delimiter='\t')}
def conv(text, r):
    w = tempfile.mkdtemp(); tj = json.load(open(r['compile_settings_from'])); tj['include_dirs'] = tj.get('include_dirs', []) + ['src/game', 'src/ea', 'include']; o, _ = fast.compile_object(tj, text, w)
    m = fast.parse(subprocess.run([fast.OBJDUMP,'-dr',o],capture_output=True,text=True).stdout).get(r['function'],[])
    return tuple(sum(1 for i in m if i.startswith(p)) for p in ('fctiwz','xoris','frsp','extsb','extsh','clrlwi','rlwinm'))
for line in open(sys.argv[1]):
    f, p = line.rstrip('\n').split('\t'); r = rows[f]
    old = open(r['draft'], errors='replace').read(); new = open(p, errors='replace').read()
    ro = score.real(r['compile_settings_from'], old, f, r['address'], r['size'])
    rn = score.real(r['compile_settings_from'], new, f, r['address'], r['size'])
    c = check_draft.compare(new, r['compile_settings_from'], f, r['address'], r['size'])
    print(f[:45], ro, '->', rn, c['status'] if c else None, 'conv', conv(old, r), conv(new, r), flush=True)
