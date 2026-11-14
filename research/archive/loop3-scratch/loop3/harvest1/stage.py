import csv,sys,os,textwrap
sys.path.insert(0,'research/tools'); import check_draft
rows={r['function']:r for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t')}
out=open('scratch/loop3/harvest1/stage.tsv','w')
for l in open('scratch/loop3/harvest1/plan.tsv'):
    fn,draft,src,note=l.rstrip('\n').split('\t')
    r=rows[fn]; assert r['draft']==draft,(fn,r['draft'])
    old=check_draft.compare(open(draft,errors='replace').read(),r['compile_settings_from'],fn,r['address'],r['size'])
    text=open(src,errors='replace').read()
    new=check_draft.compare(text,r['compile_settings_from'],fn,r['address'],r['size'])
    head='best WIP (harvest1, 2026-11-12): %d real lines (was %s; index said %s), from %s: %s'%(new['lines'],old['lines'],r['real_lines'],src,note)
    hl=''.join('// '+x+'\n' for x in textwrap.wrap(head,100))
    staged=hl+text
    p='scratch/loop3/harvest1/stage/'+os.path.basename(draft)
    open(p,'w').write(staged)
    chk=check_draft.compare(staged,r['compile_settings_from'],fn,r['address'],r['size'])
    out.write('\t'.join(map(str,[fn,r['real_lines'],old['lines'],chk['lines'],chk['status'],src,draft]))+'\n')
    print(fn[:45],r['real_lines'],old['lines'],'->',chk['lines'],chk['status'],flush=True)
