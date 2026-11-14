"""mktpl.py <base-unit> <out.json> <inline> [incdir...]: copy a unit config, RTTI on, -inline <inline>, extra include dirs."""
import json,sys
c=json.load(open('/home/kyle/workspace/moh-frontline/config/GMFE69/%s.json'%sys.argv[1]))
f=c['compiler_flags']; f[f.index('-RTTI')+1]='on'; f[f.index('-inline')+1]=sys.argv[3]
for d in sys.argv[4:]:
    if d not in c['include_dirs']: c['include_dirs'].append(d)
json.dump(c,open(sys.argv[2],'w'))
