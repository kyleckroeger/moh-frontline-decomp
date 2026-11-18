import json,sys,subprocess
j='/tmp/claude-1000/-home-kyle-workspace-moh-frontline/e1f4a78d-97e2-4c48-9222-8d4f5cde9463/scratchpad/p.json'
subprocess.run(['python3','tr.py',sys.argv[1],'buddy_head',j],check=True)
for r in json.load(open(j)):
    if r['cls']!=4: continue
    reg={e[0]:e[4] for e in r['stack']}
    print(' '.join(f"{x[1]}{x[2]}:r{reg.get(x[1],'-')}" for x in r['names']))
