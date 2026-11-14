"""xform.py UNIT...: free SetDifference(r,a,b) -> r.Sub(a,b), free Dot(a,b) -> a.Dot(b) as CVector3 members. Writes units/UNIT.cpp."""
import re,sys
R='/home/kyle/workspace/moh-frontline/'
def args_at(s,i):
    # s[i]=='(' ; return (list of args, end index after ')')
    depth=0; out=[]; cur=''; j=i
    while True:
        c=s[j]
        if c in '([': depth+=1
        if c in ')]': depth-=1
        if depth==0: out.append(cur.strip()); return out,j+1
        if c==',' and depth==1: out.append(cur.strip()); cur=''
        elif not (j==i): cur+=c
        j+=1
def calls(s,name,fn):
    pat=re.compile(r'(?<![\w.>])(?<!float )(?<!void )'+name+r'\(')
    res=''; pos=0
    while True:
        m=pat.search(s,pos)
        if not m: return res+s[pos:]
        a,end=args_at(s,m.end()-1)
        res+=s[pos:m.start()]+fn(a); pos=end
def simple(e): return e if re.fullmatch(r'[\w.]+',e) else '('+e+')'
for u in sys.argv[1:]:
    s=open(R+'src/game/'+u+'.cpp').read()
    log=[]
    m=re.search(r'inline void SetDifference\(CVector3& r, const CVector3& a, const CVector3& b\) \{\n(.*?)\n\}\n\n?',s,re.S)
    if m:
        body=m.group(1)
        s=s[:m.start()]+s[m.end():]
        has=re.search(r'void Sub\(const CVector3&',s) or 'game/CVector3.h"' in s
        if not has:
            b=' '.join(l.strip() for l in body.split('\n'))
            b=re.sub(r'\br\.','',b)
            mem='    void Sub(const CVector3& a, const CVector3& b) { '+b+' }\n'
            cm=re.search(r'^class CVector3 \{\npublic:\n',s,re.M)
            s=s[:cm.end()]+mem+s[cm.end():]
            log.append('Sub member added')
        s=calls(s,'SetDifference',lambda a: simple(a[0])+'.Sub('+a[1]+', '+a[2]+')')
        log.append('SetDifference')
    m=re.search(r'inline float Dot\(const CVector3& a, const CVector3& b\) \{(.*?)\}\n\n?',s,re.S)
    if m:
        body=m.group(1).strip()
        s=s[:m.start()]+s[m.end():]
        if not (re.search(r'float Dot\(const CVector3&[^)]*\) const',s) or 'game/CVector3.h"' in s):
            b=re.sub(r'\ba\.','',body).replace('b.','o.')
            mem='    float Dot(const CVector3& o) const { '+b+' }\n'
            cm=re.search(r'^class CVector3 \{\npublic:\n',s,re.M)
            s=s[:cm.end()]+mem+s[cm.end():]
            log.append('Dot member added: '+mem.strip())
        s=calls(s,'Dot',lambda a: simple(a[0])+'.Dot('+a[1]+')')
        log.append('Dot')
    open(R+'scratch/loop3/real1/units/'+u+'.cpp','w').write(s)
    print(u,log)
