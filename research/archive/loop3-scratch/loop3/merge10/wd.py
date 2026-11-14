"""wd.py OBJ IDX: compiled weak symbols missing from the original file record -> --weak-duplicate args."""
import sys,subprocess
L=open('scratch/loop3/merge1/orig_syms.txt').read().split('\n')
i=int(sys.argv[2])+3
assert ' df ' in L[i], L[i]
names=set(); j=i+1
while j<len(L) and ' df ' not in L[j]: names.add(L[j].split()[-1] if L[j].split() else ''); j+=1
# globals of this file are listed elsewhere in orig_syms by address; collect addresses range
allnames=set(l.split()[-1] for l in L if l.split())
nm=subprocess.run(['build/tools/powerpc-eabi-nm',sys.argv[1]],capture_output=True,text=True).stdout.split('\n')
out=[]
for l in nm:
    p=l.split()
    if len(p)==3 and p[1] in 'WwVv':
        out.append(p[2])
print(' '.join(out))
