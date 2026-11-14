"""scan3.py: census of two-arm selects (cond-branch; X; b; L: Y; M:) with X,Y writing one register, any opcodes."""
import csv,re,sys,collections
sys.path.insert(0,'scratch/loop3/minmax1'); from scan import dis,CB,tgt
for row in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
    ins=dis(row['address'],row['size']); idx={a:i for i,(a,_,_) in enumerate(ins)}
    c=collections.Counter()
    for i,(a,op,ops) in enumerate(ins[:-3]):
        if not CB.match(op) or ins[i+2][1]!='b': continue
        L=tgt(ops); M=tgt(ins[i+2][2])
        if L!=ins[i+3][0] or M!=ins[i+3][0]+4: continue
        x,y=ins[i+1],ins[i+3]
        if x[2].split(',')[0]!=y[2].split(',')[0]: continue
        k='%s/%s'%(x[1],y[1])
        if k in('li/li',): continue
        if x[1]=='addi' and x[2].split(',')[1]!=y[2].split(',')[1]: k+='(mixed base)'
        c[k]+=1
    if c: print(row['real_lines'],row['function'][:60],dict(c),sep='\t')
