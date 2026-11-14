import os,re,sys
src=sys.argv[1]; out=sys.argv[2]; os.makedirs(out,exist_ok=True)
B=open(src).read().split('\n')
st=[k for k,l in enumerate(B) if l.startswith('void CPartBin::Link')][0]
en=st+1
while B[en]!='}': en+=1
wh=[k for k in range(st,en) if B[k].strip()=='while (tag) {'][0]
decl=re.compile(r'^\s+(unsigned |const )?[A-Za-z_]\w*[\s\*]+\(?\*?[A-Za-z_]\w*\)?(\[\d+\])*(\[\d+\])*(\[\d+\])*\s*(=\s*[^;{]+)?;$')
skipw=('return','delete','goto','tag =','mask =','slot =')
cands=[k for k in range(st+1,en) if decl.match(B[k]) and not B[k].strip().startswith(skipw) and '(' not in B[k].split('=')[0].replace('(*','')]
# insertion targets: function start lines st+1..st+6, while body start wh+1
targets=list(range(st+1,st+7))+[wh+1,wh+2,wh+3,wh+4]
n=0
for c in cands:
  line=B[c]; ind=re.match(r'\s*',line).group(0)
  m=re.match(r'^(\s+)(.*?)(\w+)((?:\[\d+\])*)\s*=\s*(.+);$',line)
  for t in targets:
    if t==c or t==c+1: continue
    L=list(B)
    if m and not line.strip().startswith('GXColor material'):
      if t>c: continue
      L[c]=m.group(1)+m.group(3)+' = '+m.group(5)+';'
      newd='    '+m.group(2)+m.group(3)+m.group(4)+';'
    else:
      L[c]=None; newd='    '+line.strip()
      if t>c and t>wh and c<wh: continue
    L.insert(t,newd)
    L=[x for x in L if x is not None]
    name='m%03d_%03d'%(c,t); open('%s/%s.cpp'%(out,name),'w').write('\n'.join(L)); n+=1
    print(name,line.strip(),'->',t)
print(n,file=sys.stderr)
