# usage: python3 sc.py UNIT SRC [FUNCSUBSTR]  -> per-function differing lines
import sys,json
sys.path.insert(0,'research/tools')
import score
uid,src=sys.argv[1],sys.argv[2]
u=json.load(open(f'config/GMFE69/{uid}.json'))
t=open(src,errors='replace').read()
for f in u['functions']:
    if len(sys.argv)>3 and sys.argv[3] not in f['name']: continue
    print(f['name'], score.real(f'config/GMFE69/{uid}.json',t,f['name'],f['address'],str(f['size'])))
