import re,subprocess,collections,sys
names=[l.split('\t')[0] for l in open('syms.tsv') if 'NOT-IN' not in l]
names=list(dict.fromkeys(names))
for n in names:
    base=n.split('__')[0] if '__' in n[2:] else n
    pat=base
    r=subprocess.run(['grep','-rhE',r'\b'+re.escape(pat)+r'\b','src','include','research/drafts','research/data','--include=*.cpp','--include=*.c','--include=*.h'],capture_output=True,text=True,cwd='/home/kyle/workspace/moh-frontline')
    c=collections.Counter()
    for l in r.stdout.splitlines():
        l=l.strip()
        if re.search(r'^(extern|static|const|volatile)\b|^\w[\w\s\*<>:,]*\b'+re.escape(pat)+r'\s*(\[[^\]]*\])*\s*(;|=)',l) and '(' not in l.split(pat)[0]:
            c[re.sub(r'\s+',' ',l)[:150]]+=1
    print('==',n)
    for k,v in c.most_common(4): print('   %3d %s'%(v,k))
