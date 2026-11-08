"""addunit.py <id>...: append unit manifests to config/GMFE69/project.json (research helper)."""
import json, sys
p = 'config/GMFE69/project.json'
m = json.load(open(p))
key = next(k for k, v in m.items() if isinstance(v, list) and 'bstimer.json' in v)
for u in sys.argv[1:]:
    if f'{u}.json' not in m[key]:
        m[key].append(f'{u}.json')
open(p, 'w').write(json.dumps(m, indent=2) + '\n')
print(key, len(m[key]))
