"""orig.py <file.cpp> [nth]: the original's symbols of one file record: locals, then globals inside its data sections."""
import sys,re
L=open('/home/kyle/workspace/moh-frontline/scratch/loop3/merge1/orig_syms.txt').read().splitlines()
starts=[i for i,l in enumerate(L) if l.endswith('\t00000000 '+sys.argv[1]) and ' df ' in l]
i=starts[int(sys.argv[2]) if len(sys.argv)>2 else 0]
j=i+1
while j<len(L) and ' df ' not in L[j]: j+=1
blk=L[i:j]
secs={}
for l in blk:
    p=l.split()
    if p[2]=='d' and p[0]!='00000000': secs[p[3]]=int(p[0],16)
for l in blk[1:]:
    p=l.split()
    if p[2]!='d': print(l)
# globals: sections end at next file's start of the same section; approximate with the next section symbol of same name
nexts={}
for l in L[j:]:
    p=l.split()
    if len(p)>3 and p[2]=='d' and p[3] in secs and p[3] not in nexts and int(p[0],16)>secs[p[3]]: nexts[p[3]]=int(p[0],16)
    if len(nexts)==len(secs): break
print('--- sections',{k:(hex(v),hex(nexts.get(k,0)-v)) for k,v in secs.items()})
for l in L:
    p=l.split()
    if len(p)>=5 and p[1] in ('g','w') and p[0]!='00000000':
        a=int(p[0],16); s=p[-3] if p[2] in 'OF' else p[2]
        for k,v in secs.items():
            if k in ('.text',): continue
            if v<=a<nexts.get(k,v): print(l)
