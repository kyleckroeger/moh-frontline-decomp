import subprocess,os,itertools
os.environ['T']='/home/kyle/workspace/moh-frontline/config/GMFE69/proxtrig_leafs.json'
F='UpdateProximityTriggerStatus__Fv'
base=open('up.cpp').read()
def score(src,tag):
    open(f'u_{tag}.cpp','w').write(src)
    return subprocess.run(['python3','rm.py',f'u_{tag}.cpp',F],capture_output=True,text=True).stdout.strip()
TOP="void UpdateProximityTriggerStatus() {\n    int i;\n    BSGO_Basic* user;\n"
GL="    for (user = g_pGermanList; user; user = user->m_next)\n        UpdateStatus(user, true, false);\n"
assert TOP in base and GL in base
parts={'i':"    int i;\n",'u':"    BSGO_Basic* user;\n",'g':"    BSGO_Basic* german;\n"}
res=[]
for perm in itertools.permutations('iug'):
  for gl in ['for','while','const']:
    h="void UpdateProximityTriggerStatus() {\n"+''.join(parts[p] for p in perm)
    if gl=='for': g="    for (german = g_pGermanList; german; german = german->m_next)\n        UpdateStatus(german, true, false);\n"
    elif gl=='while': g="    german = g_pGermanList;\n    while (german) {\n        UpdateStatus(german, true, false);\n        german = german->m_next;\n    }\n"
    else: g="    for (german = g_pGermanList; german; german = german->m_next) {\n        BSGO_Basic* const g = german;\n        UpdateStatus(g, true, false);\n    }\n"
    s=base.replace(TOP,h).replace(GL,g)
    r=score(s,'x'); res.append((int(r.split()[0]),''.join(perm)+gl,r[:120]))
for r in sorted(res)[:8]: print(r)
