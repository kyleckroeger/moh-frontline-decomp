import sys,json,tempfile
sys.path.insert(0,'scratch/game'); import fast
rows=[l.rstrip('\n').split('\t') for l in open('research/plans/loop_queue.tsv')][1:]
idx={l.split('\t')[3]:l.split('\t')[4] for l in open('research/drafts/index.tsv') if l.count('\t')>=5}
for r in rows:
    import re
    if not re.search('Audio|Shell|System|Video|Other',r[2]) or not r[9] or r[10]: continue
    tmpl=idx.get(r[4])
    if not tmpl: continue
    t=json.load(open(tmpl)); src=open(r[9]).read()
    out=[]
    for flip in (False,True):
        tt=json.loads(json.dumps(t))
        if flip:
            tt['compiler_flags']=[('auto' if 'deferred' in x else 'auto,deferred') if x.startswith(('auto','deferred')) else x for x in tt['compiler_flags']]
        with tempfile.TemporaryDirectory() as d:
            out.append(fast.score(tt,src,[(r[4],)],d)[r[4]])
    print(r[4][:40], tmpl.split('/')[-1], out, flush=True)
