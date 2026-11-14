"""gen4.py <src> <half 0|1>: direct form, const core first, f/t const param copies, all perms of go,object,tank,box,world,f,t."""
import itertools, sys, os
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/scratch/game')
import fast, json
ROOT = '/home/kyle/workspace/moh-frontline/'
T = json.load(open(ROOT + 'config/GMFE69/objcreate_swap.json'))
src = open(sys.argv[1]).read()
hdr_old = """    CVolBox* box;
    TriggerCoreView* core;
    BSGO_Tank* go;
    CTankObject* tank;
    CHierObject* object;
    CVolBox* world;
    core = trigger->core;"""
assert hdr_old in src
D = {'go': 'BSGO_Tank* go;', 'object': 'CHierObject* object;', 'tank': 'CTankObject* tank;', 'box': 'CVolBox* box;',
     'world': 'CVolBox* world;', 'f': 'int const f = flag;', 't': 'TriggerObject_struct* const t = trigger;'}
F = 'CreateRootHierObject__FP20TriggerObject_structi'
W = '/tmp/claude-1000/-home-kyle-workspace-moh-frontline/a5d18f30-70be-41af-9a00-608205f1aeba/scratchpad/m2w' + str(os.getpid())
os.makedirs(W, exist_ok=True)
half = int(sys.argv[2]); best = None; res = []
for i, perm in enumerate(itertools.permutations(['go', 'object', 'tank', 'box', 'world', 'f', 't'])):
    if i % 2 != half: continue
    h = '    TriggerCoreView* const core = trigger->core;\n' + ''.join('    ' + D[n] + '\n' for n in perm)
    s = src.replace(hdr_old, h.rstrip('\n')).replace('AttachScript(trigger, flag,', 'AttachScript(t, f,')
    n = fast.score(T, s, [(F,)], W)[F]
    res.append((n, perm))
    if best is None or n < best[0]:
        best = (n, perm); print(n, perm, flush=True)
        open(ROOT + 'scratch/loop3/mid2/g1/best4_%d.cpp' % half, 'w').write(s)
res.sort(); print(res[:5])
