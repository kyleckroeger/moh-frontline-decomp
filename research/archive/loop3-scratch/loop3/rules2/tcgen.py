import itertools
s=open('tc9.cpp').read()
a=s.index('    float h0;\n'); b=s.index('            float proj = a0 + a1 + a2;\n')+len('            float proj = a0 + a1 + a2;\n')
pre,post=s[:a],s[b:]
def dot(i,h,n): 
    return '(float)fabs(%s * m_axes[%d].d.c.x * %s + %s * m_axes[%d].d.c.y * %s + %s * m_axes[%d].d.c.z * %s)'%(h,i,n[0],h,i,n[1],h,i,n[2])
out=[]
for hd in (0,1):
  for nf in ('e','v'):
    for perm in itertools.permutations((0,1,2)):
      for inl in (0,1):
        L=['            float len = sqrtf(nx * nx + ny * ny + nz * nz);','            float inv = 1.0f / len;']
        if nf=='e': n=['(nx * inv)','(ny * inv)','(nz * inv)']
        else:
            L+=['            nx *= inv;','            ny *= inv;','            nz *= inv;']; n=['nx','ny','nz']
        H=lambda i:('m_half[%d]'%i) if hd else 'h%d'%i
        if not hd: L+=['            float h%d = m_half[%d];'%(i,i) for i in (0,1,2)]
        if inl:
            L+=['            float proj = %s + %s + %s;'%tuple(dot(i,H(i),n) for i in (0,1,2))]
        else:
            L+=['            float a%d = %s;'%(i,dot(i,H(i),n)) for i in perm]+['            float proj = a0 + a1 + a2;']
            
        if inl and perm!=(0,1,2): continue
        k='tg_%d%s%d%d%d%d.cpp'%(hd,nf,inl,*perm)
        open('p/'+k,'w').write(pre+'\n'.join(L)+'\n'+post); out.append('p/'+k)
print(' '.join(out))
