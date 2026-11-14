import os
B=open('scratch/loop3/day2/csweep/base.cpp').read().split('\n')
i=B.index('        bool skinned = mesh->m_skin != 0;')
out='scratch/loop3/day2/csweep/v2'; os.makedirs(out,exist_ok=True)
for ty in ['bool','int','unsigned char']:
  L=list(B); L[i]='        skinned = mesh->m_skin != 0;'
  # candidate insertion points: after function opening (line index of 'void CPartBin::Link') up to i
  st=[k for k,l in enumerate(L) if l.startswith('void CPartBin::Link')][0]
  for p in range(st+1,i+1):
    ind='    ' if p<=226 else '        '
    if p-1>=0 and L[p-1].rstrip().endswith(('{',';')) and not L[p-1].strip().startswith(('if','for','else')):
      M=L[:p]+[ind+ty+' skinned;']+L[p:]
      open('%s/%s_%d.cpp'%(out,ty.replace(' ','_'),p+1),'w').write('\n'.join(M))
