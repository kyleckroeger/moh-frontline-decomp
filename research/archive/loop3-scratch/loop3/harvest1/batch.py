import csv,sys,os
sys.argv=[sys.argv[0],'x']+sys.argv[1:]
src=open('scratch/loop3/harvest1/cand.py').read().split("rows=[r for r")[0]
exec(src)
part,nparts=int(sys.argv[2]),int(sys.argv[3])
rows={r['function']:r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t')}
C=[l.rstrip('\n').split('\t') for l in open('scratch/loop3/harvest1/cands.tsv')]
# add the draft itself for each function
todo=[]
for fn in sorted({c[0] for c in C}): todo.append((fn,rows[fn]['draft']))
todo+= [(c[0],c[2]) for c in C]
out=open('scratch/loop3/harvest1/res.%d.tsv'%part,'w')
for i,(fn,f) in enumerate(todo):
    if i%nparts!=part: continue
    r=rows[fn]
    try:
        text=open(f,errors='replace').read()
        c=check_draft.compare(text,r['compile_settings_from'],r['function'],r['address'],r['size'])
        b=br(r,text) if c['status']!='no_compile' else '-'
        out.write('\t'.join(map(str,[fn,r['real_lines'],f,c.get('lines'),c['status'],b.split(' ')[0],b]))+'\n'); out.flush()
    except Exception as e:
        out.write('\t'.join([fn,r['real_lines'],f,'ERR',repr(e)[:100],'-','-'])+'\n'); out.flush()
