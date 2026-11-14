import csv,sys,collections,syms
fl=syms.load('fl_nm.txt'); rs=syms.load('rs_nm.txt')
def idx(L):
    byq=collections.defaultdict(list); byn=collections.defaultdict(list)
    for r in L:
        byq[(r[3],r[4])].append(r); byn[r[4]].append(r)
    return byq,byn
flq,fln=idx(fl); rsq,rsn=idx(rs)
rows=list(csv.reader(open('inlines.tsv'),delimiter='\t'))[1:]
out=collections.Counter()
res=[]
for u,line,q,params,const,pre,body in rows:
    if '::' in q: cls,name=q.rsplit('::',1)
    else: cls,name='',q
    name=name.split('<')[0].strip()
    if name.startswith('operator') or name in ('new','delete') or 'operator' in name: st='operator'
    elif cls and (name==cls.split('::')[-1] or name=='~'+cls.split('::')[-1]): st='ctor/dtor'
    elif (cls,name) in flq: st='FL:'+flq[(cls,name)][0][1]
    elif (cls,name) in rsq: st='RS'
    elif not cls and name in fln: st='FLmember:'+','.join(sorted(set(r[3]+':'+r[1] for r in fln[name])))[:80]
    elif not cls and name in rsn: st='RSmember:'+','.join(sorted(set(r[3] for r in rsn[name])))[:80]
    elif cls and name in fln: st='FLother:'+','.join(sorted(set(r[3]+':'+r[1] for r in fln[name])))[:80]
    elif cls and name in rsn: st='RSother:'+','.join(sorted(set(r[3] for r in rsn[name])))[:80]
    else: st='unknown'
    res.append((u,line,cls,name,st,params,body))
    out[st.split(':')[0]]+=1
print(out)
with open('classified.tsv','w') as f:
    for r in res: f.write('\t'.join(r)+'\n')
