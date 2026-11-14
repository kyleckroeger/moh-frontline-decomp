import sys,csv,shutil,os
lo,hi=int(sys.argv[1]),int(sys.argv[2])
S=[l.rstrip('\n').split('\t') for l in open('scratch/loop3/harvest1/stage.tsv')][lo:hi]
new={s[0]:s[3] for s in S}
for s in S: shutil.copy('scratch/loop3/harvest1/stage/'+os.path.basename(s[6]),s[6])
lines=open('research/drafts/index.tsv').read().split('\n')
out=[]
n=0
for l in lines:
    p=l.split('\t')
    if len(p)>3 and p[3] in new: p[0]=new[p[3]]; n+=1; l='\t'.join(p)
    out.append(l)
open('research/drafts/index.tsv','w').write('\n'.join(out))
print('updated',n,'of',len(S))
print('; '.join('%s %s -> %s'%(s[0].split('__')[0],s[2],s[3]) for s in S))
