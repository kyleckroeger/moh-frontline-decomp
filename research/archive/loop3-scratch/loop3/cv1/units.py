import json,glob,sys
units=[]
for p in glob.glob('config/GMFE69/*.json'):
    try: d=json.load(open(p))
    except Exception: continue
    if not isinstance(d,dict) or 'sections' not in d: continue
    for s in d['sections']:
        if not isinstance(s,dict): continue
        if s['name']=='.text':
            a=int(s['address'],16); units.append((a,a+s['size'],d['id'],d.get('original_file',''),d.get('source',''),d.get('category','')))
units.sort()
def find(addr):
    out=[]
    for i,u in enumerate(units):
        if u[0]<=addr<u[1]: return ('in',u,units[i-1] if i else None,units[i+1] if i+1<len(units) else None)
    prev=max([u for u in units if u[1]<=addr],default=None)
    nxt=min([u for u in units if u[0]>addr],default=None)
    return ('gap',None,prev,nxt)
for a in sys.argv[1:]:
    r=find(int(a,16)); print(a,r[0]); 
    for u in r[1:]:
        if u: print('   %08x-%08x %s %s %s'%(u[0],u[1],u[2],u[3],u[5]))
def unit_of(addr):
    for u in units:
        if u[0]<=addr<u[1]: return u[2]
    return '?'
