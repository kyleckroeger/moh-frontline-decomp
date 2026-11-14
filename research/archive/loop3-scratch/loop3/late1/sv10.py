from sv import *
import itertools
B=open('sx_inl_hl.cpp').read()
s0=B.index('static void SweepExtents('); s1=B.index('\n}\n',s0)+3
fn=B[s0:s1]
hdr=fn[:fn.index('    active.UseBuffer')]
body=fn[len(hdr):]
lines=hdr.split('\n'); sig='\n'.join(lines[:2])+'\n'; decls=[l+'\n' for l in lines[2:] if l.strip()]
print(decls,flush=True)
for perm in itertools.permutations(range(7)):
    s=B[:s0]+sig+''.join(decls[k] for k in perm)+body+B[s1:]
    r=score(s,'s10'); print(perm,r[:70],flush=True)
    if r.startswith('0 '): open('sx_zero.cpp','w').write(s); break
