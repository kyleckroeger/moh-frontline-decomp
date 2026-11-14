import json,sys
rows=[]
for u in sys.argv[1:]:
    c=json.load(open('/home/kyle/workspace/moh-frontline/config/GMFE69/%s.json'%u))
    for f in c['functions']+c.get('label_functions',[]):
        rows.append((int(f['address'],16),f['size'],f['binding'],u,f['name']))
    for s in c['sections']:
        print('  sec',u,s['name'],s['address'],s['size'],s.get('linked_size',''),s.get('stripped_objects',''))
for r in sorted(rows): print('%08x %5d %d %-28s %s'%r)
