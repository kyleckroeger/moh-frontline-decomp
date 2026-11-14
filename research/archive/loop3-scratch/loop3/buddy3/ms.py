# ms.py src... : per-function line-diff scores; aliases NAME_<tag>__F... scored against NAME__F...
import sys, json, tempfile, subprocess, difflib, re
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/research/tools'); import fast
t = json.load(open('/home/kyle/workspace/moh-frontline/scratch/loop3/buddy2/tmpl_pf.json'))
ADDR = {'BIFunc_UpdateBuddyCheckpoints__FPPiPv': (0x800567e8, 900), 'BIFunc_UpdateBuddyProgress__FPPiPv': (0x80056bd4, 1424),
        'BIFunc_BuddyDied__FPPiPv': (0x80057164, 1244), 'BIFunc_GetInitialBuddyOrders__FPPiPv': (0x80057640, 1264),
        'BIFunc_AddBuddy__FPPiPv': (0x80057b30, 892), 'AddBuddy__F9BuddyInfob': (0x80057eac, 912)}
def score(path):
    w = tempfile.mkdtemp(); obj, log = fast.compile_object(t, open(path).read(), w)
    if not obj: return None
    mine = fast.parse(subprocess.run([fast.OBJDUMP, '-dr', obj], capture_output=True, text=True).stdout)
    out = {}
    for n, b in mine.items():
        base = re.sub(r'_ZZ\w*?(__F)', r'\1', n)
        if base not in ADDR: continue
        a, s = ADDR[base]; tg = fast.target(base, a, a + s)
        out[n] = sum(1 for l in difflib.unified_diff(tg, b, n=0, lineterm='') if l[:1] in '+-' and not l.startswith(('+++', '---')))
    return out
if __name__ == '__main__':
    for p in sys.argv[1:]:
        r = score(p)
        short = lambda n: n.replace('BIFunc_AddBuddy', 'BA').replace('BIFunc_', '').replace('__FPPiPv', '').replace('__F9BuddyInfob', '').replace('Buddy', '')
        print(p, 'FAIL' if r is None else ' '.join(f'{short(k)}={v}' for k, v in r.items()), flush=True)
