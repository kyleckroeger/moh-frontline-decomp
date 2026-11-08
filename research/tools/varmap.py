"""varmap.py <tmpl> <draft> <func> <start> <end>: which NAMED variables sit in the wrong register. Compiles with -sym on (same code), aligns against the original, prints var: ours -> orig votes, and a suggested declaration order for callee-saved locals (earlier declared = higher register, 85-91% on landed code) (research helper)."""
import sys,json,re,struct,subprocess,tempfile,difflib,collections
import fast,dw
def rows(args):
    r=[]
    for l in subprocess.run([fast.OBJDUMP,'-d']+args,capture_output=True,text=True).stdout.splitlines():
        m=re.match(r'^\s*([0-9a-f]+):\t[0-9a-f ]+\t(.*)$',l)
        if m:
            s=re.sub(r'\s+',' ',re.sub(r'<.*>','',m.group(2))).strip()
            s=re.sub(r'-?\d+\(r2\)','0(0)',s); s=re.sub(r'\b(b\w*) ([\w,]*?)[0-9a-f]+$',r'\1 \2T',s)
            r.append(s)
    return r
def analyse(tmpl,src,func,start,end):
    t=json.load(open(tmpl)) if isinstance(tmpl,str) else tmpl
    obj,es=dw.dump(t,src)
    m=rows(['--disassemble='+func,obj]); o=rows(['--start-address='+start,'--stop-address='+end,fast.ELF])
    blind=lambda r:[re.sub(r'\b[fr]\d+\b','R',x) for x in r]
    a,b=blind(m),blind(o); votes=collections.defaultdict(collections.Counter)
    for tag,i1,i2,j1,j2 in difflib.SequenceMatcher(None,a,b,autojunk=False).get_opcodes():
        if tag!='equal': continue
        for x,y in zip(m[i1:i2],o[j1:j2]):
            for p,q in zip(re.findall(r'\b[fr]\d+\b',x),re.findall(r'\b[fr]\d+\b',y)): votes[p][q]+=1
    vs=[];cur=None
    for e in es or []:
        if e is None: continue
        tag,at=e
        if tag in(6,0x2e) and 0x2008 in at: cur=at[0x2008]; continue
        if tag in(5,0xc) and at.get(0x38) and cur==func:
            loc=at.get(0x23); r=None
            if isinstance(loc,bytes) and len(loc)==5 and loc[0]==1:
                n=struct.unpack('>I',loc[1:5])[0]; r=('r%d'%n if n<32 else 'f%d'%(n-32))
            vs.append(('param' if tag==5 else 'local',at[0x38],r))
    return vs,votes
def pick(votes,r):
    c=votes.get(r)
    return c.most_common(1)[0][0] if c else None
if __name__=='__main__':
    tmpl,draft,func,start,end=sys.argv[1:6]
    vs,votes=analyse(tmpl,open(draft,errors='replace').read(),func,start,end)
    for k,n,r in vs:
        c=votes.get(r,{})
        print(f'{k:5} {n:18} {str(r):5} -> {dict(c) if c else "-"}{"" if not c or list(c)==[r] else "   <<"}')
    print('wrong registers:',{p:dict(c) for p,c in sorted(votes.items()) if set(c)!={p}})
    locs=[(n,pick(votes,r)) for k,n,r in vs if k=='local' and r and r[0]=='r' and int(r[1:])>=14 and pick(votes,r)]
    print('suggest GPR decl order:',[n for n,q in sorted(locs,key=lambda x:-int(x[1][1:]))])
