import json, sys, os
sys.argv=[sys.argv[0]]+sys.argv[1:]
sys.path.insert(0,'scratch/loop3/hdr2')
import redraft as r
for u in sys.argv[1:]:
    old=json.load(open(f'config/GMFE69/{u}.json'))
    r.compile_only(old, r.WORK/u/'control')
