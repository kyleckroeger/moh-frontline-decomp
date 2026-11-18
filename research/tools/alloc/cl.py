"""cl.py src... : trace and print BA's callee nodes in colour order: idx:reg:group (T top, A attached, R ready)."""
import json,sys,subprocess,os,collections
for src in sys.argv[1:]:
    j='/tmp/claude-1000/-home-kyle-workspace-moh-frontline/e1f4a78d-97e2-4c48-9222-8d4f5cde9463/scratchpad/t.json'
    os.makedirs(os.path.dirname(j),exist_ok=True)
    subprocess.run(['python3','tr.py',src,'buddy_head',j],check=True)
    d=json.load(open(j))
    cand=[]
    for r in d:
        if r['cls']!=4: continue
        c=collections.Counter(e[4] for e in r['stack'] if e[4]>=14)
        if {'stack','caller','first','index'}<= {x[2] for x in r['names']} and len(r['stack'])>60: cand.append(r)
    r=cand[0] if len(cand)==1 else None
    if r is None: print(src,'ambiguous',len(cand)); continue
    reg={e[0]:e[4] for e in r['stack']}
    cal=[e for e in r['stack'] if e[4]>=14]
    hub={e[0] for e in cal if e[2]>50}
    nb={e[0]:set(x for x in e[3] if reg.get(x,0)>=14)-hub for e in cal}
    # components
    comp={}
    for e in cal:
        if e[0] in hub: comp[e[0]]='*'; continue
    groups=[]
    for v in nb:
        if v in hub or v in comp: continue
        s=[v]; comp[v]=len(groups)
        while s:
            u=s.pop()
            for x in nb[u]:
                if x not in comp: comp[x]=len(groups); s.append(x)
        groups.append(v)
    # label: group containing index-r25 smallest idx with 3 members = top
    mem=collections.defaultdict(list)
    for v,g in comp.items(): mem[g].append(v)
    lab={}
    for g,vs in mem.items():
        if g=='*': continue
        lab[g]='T' if len(vs)==3 else ('1' if len(vs)==1 else ('A' if max(vs)>=80 else 'R'))
    nm={x[1]:x[2] for x in r['names']}
    print(os.path.basename(src),' '.join(f'{e[0]}:{e[4]}{lab.get(comp[e[0]],"*")}{nm.get(e[0],"")}' for e in cal))
    if os.environ.get('ALL'): print('  ',' '.join(f'{x[1]}{x[2]}' for x in r['names']))
