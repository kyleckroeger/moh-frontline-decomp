import sys, re
FAM = ['UpdateCheckpoints', 'UpdateProgress', 'Died', 'GetInitialOrders', 'BA', 'Add']
for l in open(sys.argv[1]):
    p, *kv = l.split()
    if not kv: print(p, 'FAIL'); continue
    d = {k: int(v) for k, v in (x.split('=') for x in kv)}
    best = {}
    for k, v in d.items():
        f = k.split('_ZZ')[0]
        if f not in best or v < best[f][0]: best[f] = (v, k)
    tot = sum(best[f][0] for f in FAM if f != 'BA')
    print(tot, p.split('/')[-1], ' '.join(f'{f}={best[f][0]}' for f in FAM), ' '.join(best[f][1] for f in FAM if best[f][0] == 0 or True) if len(sys.argv) > 2 else '')
