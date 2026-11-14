import json,re,os,glob
os.chdir('/home/kyle/workspace/moh-frontline')
hdrs={'DwiContainers.h'}
allh=glob.glob('include/**/*.h',recursive=True)
ch=True
while ch:
    ch=False
    for h in allh:
        b=os.path.basename(h)
        if b in hdrs: continue
        t=open(h,errors='replace').read()
        if any(re.search(r'#include\s+["<][^">]*\b'+re.escape(x)+'[">]',t) for x in hdrs): hdrs.add(b); ch=True
units=[u[:-5] for u in json.load(open('config/GMFE69/project.json'))['units']]
out=[]
for u in units:
    c=json.load(open('config/GMFE69/%s.json'%u)); s=c.get('source')
    if not s or not os.path.exists(s): continue
    t=open(s,errors='replace').read()
    if any(re.search(r'#include\s+["<][^">]*\b'+re.escape(x)+'[">]',t) for x in hdrs): out.append(u)
print(sorted(hdrs),file=__import__('sys').stderr)
print('\n'.join(out))
