"""scan2.py: broader select census: arms = addi/mr/fmr/lwz pairs into the same register after a compare branch."""
import csv,re,sys,collections
sys.path.insert(0,'scratch/loop3/minmax1'); from scan import dis,CB,tgt
c=collections.Counter(); ex=collections.defaultdict(list)
for row in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    ins=dis(row['address'],row['size']); idx={a:i for i,(a,_,_) in enumerate(ins)}
    for i,(a,op,ops) in enumerate(ins):
        if not CB.match(op): continue
        L=tgt(ops)
        if L not in idx: continue
        j=idx[L]
        for k in range(i+1,min(i+5,len(ins))):
            if ins[k][1]=='b':
                if j!=k+1: break
                M=tgt(ins[k][2]); tail=[]; jj=j
                while jj<len(ins) and ins[jj][0]<(M or 0): tail.append(ins[jj]); jj+=1
                if len(tail)>3 or M is None: break
                body=ins[i+1:k]
                sig=' '.join(o for _,o,_ in body)+' | '+' '.join(o for _,o,_ in tail)
                if (body and tail and body[-1][2].split(',')[0]==tail[-1][2].split(',')[0] and body[-1][1] in('addi','mr','fmr')) or (not body and tail and tail[-1][1] in ('fmr','mr','addi')):
                    key=sig+('  base='+body[-1][2].split(',')[1] if body and body[-1][1]=='addi' else '')
                    c[(row['function'],key)]+=1
                break
for (f,k),n in sorted(c.items()): print(n,f[:60],k,sep='\t')
