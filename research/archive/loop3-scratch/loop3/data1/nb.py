import sys, json
sys.path.insert(0,'/home/kyle/workspace/moh-frontline/tools')
exec(open('dump.py').read().split("names=")[0])
q=[l.split('\t') for l in open('syms.tsv') if '\t?\t' in l]
for sec in secs: pass
for n,sn,a,sz,f,*_ in q:
    a=int(a,16)
    same=sorted([s for s in syms if s['name'] and 0<s['section']<0xff00 and secname(s['section'])==sn and abs(s['address']-a)<0x400],key=lambda s:s['address'])
    before=[s for s in same if s['address']<a][-3:]; after=[s for s in same if s['address']>a][:3]
    def fmt(s): return f"{s['name'][:30]}({file_of(sn,s['address'])})"
    print(n, sn, '|', ' '.join(fmt(s) for s in before), '<>', ' '.join(fmt(s) for s in after))
