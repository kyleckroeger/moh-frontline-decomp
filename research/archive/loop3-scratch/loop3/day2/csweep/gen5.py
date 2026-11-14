import os,itertools
B=open('scratch/loop3/day2/csweep/base.cpp').read()
out='scratch/loop3/day2/csweep/v5'; os.makedirs(out,exist_ok=True)
T=['if (skinned)','if (skinned == true)','if (skinned != false)','if ((int)skinned)','if (skinned != 0)','if ((char)skinned)','if (skinned == 1)','if ((int)skinned != 0)']
parts=B.split('if (skinned)')
assert len(parts)==4
n=0
for combo in itertools.product(range(len(T)),repeat=3):
  if sum(1 for c in combo if c)>1 and len(set(combo))>1: continue
  s=parts[0]
  for k,c in enumerate(combo): s+=T[c]+parts[k+1]
  open('%s/t%d%d%d.cpp'%(out,*combo),'w').write(s); n+=1
print(n)
