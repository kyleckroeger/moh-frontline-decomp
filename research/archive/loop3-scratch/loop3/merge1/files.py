import json,collections,os
os.chdir('/home/kyle/workspace/moh-frontline')
p=json.load(open('config/GMFE69/project.json'))
snap=json.load(open('progress/GMFE69.snapshot.json'))
unmatched=collections.defaultdict(list)
for u in snap['code_map']['units']:
    fn=u.get('file_name')
    for r in u['ranges']:
        unmatched[fn].append(r['name'])
by=collections.defaultdict(list)
for f in p['units']:
    c=json.load(open('config/GMFE69/'+f))
    of=c.get('original_file'); 
    by[(of,c.get('original_file_index'))].append(c['id'])
multi={k:v for k,v in by.items() if len(v)>1 and k[0]}
for k,v in sorted(multi.items(),key=lambda x:str(x[0])):
    print(k[0],k[1],len(v),'UNMATCHED:%d'%len(unmatched.get(k[0],[])) if k[0] in unmatched else 'complete',' '.join(sorted(v)))

unmatched_idx=collections.defaultdict(list)
for u in snap['code_map']['units']:
    if 'file_record' in u: unmatched_idx[u['file_record']]+= [r['name'] for r in u['ranges']]
u2f={}
for k,v in by.items():
    for x in v: u2f[x]=k
vt=collections.defaultdict(list)
for line in open('scratch/loop3/vt1/vt_keys.tsv'):
    a=line.rstrip('\n').split('\t')
    if a[3].startswith('unit:') and a[1]=='global':
        vt[u2f.get(a[3][5:])].append(a[0])
fl=collections.defaultdict(list)
for line in open('research/layout/source_link.tsv'):
    a=line.rstrip('\n').split('\t')
    if a[0]=='fragment_local':
        fs={u2f.get(o[5:]) for o in a[2].split() if o.startswith('unit:')}
        for f in fs: fl[f].append(a[1])
import sys
out=open('scratch/loop3/merge1/complete_files.tsv','w')
out.write('original_file\tfile_index\tn_fragments\tfragments\tvtables\tfragment_local\n')
rows=[]
for k,v in multi.items():
    if k[0] in unmatched: continue
    rows.append((0 if vt.get(k) else (1 if fl.get(k) else 2), k[0], k[1], v))
rows.sort()
for pr,f,i,v in rows:
    out.write('%s\t%s\t%d\t%s\t%s\t%s\n'%(f,i,len(v),' '.join(sorted(v)),' '.join(vt.get((f,i),[])) or '-',' '.join(fl.get((f,i),[])) or '-'))
out.close()
print(sum(1 for r in rows if r[0]==0), sum(1 for r in rows if r[0]==1), len(rows))
nv=[ (a,u2f.get(a)) for a in []]
