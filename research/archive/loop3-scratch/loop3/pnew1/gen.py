"""gen.py src names [hdrkey]: permute top declaration lines (those listed) of CRH; header = current lines up to 'core = trigger->core;'"""
import itertools, sys, os, json
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/scratch/game'); import fast
T = json.load(open('/home/kyle/workspace/moh-frontline/config/GMFE69/objcreate_swap.json'))
src = open(sys.argv[1]).read()
D = {'go': 'BSGO_Tank* go;', 'object': 'CHierObject* object;', 'tank': 'CTankObject* tank;', 'box': 'CVolBox* box;',
     'world': 'CVolBox* world;', 'core': 'TriggerCoreView* core;'}
start = src.index('BSGO_Basic* CreateRootHierObject(TriggerObject_struct* trigger, int flag) {\n') 
start = src.index('\n', start) + 1
end = src.index('    core = trigger->core;', start)
hdr = src[start:end]
present = [n for n in D if '    ' + D[n] + '\n' in hdr]
names = sys.argv[2].split(',')
F = 'CreateRootHierObject__FP20TriggerObject_structi'
W = '/tmp/claude-1000/-home-kyle-workspace-moh-frontline/a5d18f30-70be-41af-9a00-608205f1aeba/scratchpad/p1w' + str(os.getpid())
os.makedirs(W, exist_ok=True)
res = []; best = None
for perm in itertools.permutations(names):
    h = ''.join('    ' + D[n] + '\n' for n in perm)
    s = src[:start] + h + src[end:]
    n = fast.score(T, s, [(F,)], W)[F]
    res.append((n, perm))
    if best is None or n < best[0]:
        best = (n, perm); print(n, perm, flush=True)
        open('g1/best_' + os.path.basename(sys.argv[1]), 'w').write(s)
res.sort(); print(res[:5])
