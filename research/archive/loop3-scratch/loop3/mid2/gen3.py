"""gen3.py <src> <names,comma> [maxn]: permute top declarations of CreateRootHierObject (core assigned after decls)."""
import itertools, sys, os
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/scratch/game')
import fast, json
ROOT = '/home/kyle/workspace/moh-frontline/'
T = json.load(open(ROOT + 'config/GMFE69/objcreate_swap.json'))
src = open(sys.argv[2 - 1]).read()
hdr_old = """    CVolBox* box;
    TriggerCoreView* core;
    BSGO_Tank* go;
    CTankObject* tank;
    CHierObject* object;
    CVolBox* world;
    core = trigger->core;"""
assert hdr_old in src
D = {'go': 'BSGO_Tank* go;', 'object': 'CHierObject* object;', 'tank': 'CTankObject* tank;', 'box': 'CVolBox* box;',
     'world': 'CVolBox* world;', 'core': 'TriggerCoreView* core;'}
names = sys.argv[2].split(',')
F = 'CreateRootHierObject__FP20TriggerObject_structi'
W = '/tmp/claude-1000/-home-kyle-workspace-moh-frontline/a5d18f30-70be-41af-9a00-608205f1aeba/scratchpad/m2w' + str(os.getpid())
os.makedirs(W, exist_ok=True)
res = []
best = None
for perm in itertools.permutations(names):
    h = ''.join('    ' + D[n] + '\n' for n in perm) + '    core = trigger->core;'
    s = src.replace(hdr_old, h)
    n = fast.score(T, s, [(F,)], W)[F]
    res.append((n, perm))
    if best is None or n < best[0]:
        best = (n, perm); print(n, perm, flush=True)
        open(ROOT + 'scratch/loop3/mid2/g1/best_%s' % os.path.basename(sys.argv[1]), 'w').write(s)
res.sort(); print(res[:5])
