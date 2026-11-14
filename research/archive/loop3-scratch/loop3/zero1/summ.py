"""summ.py src.cpp: per function, the li/mr instructions before the first 'lwz rX,N(r30/r3...)' of the loop; marks SHARED if li rA,0 followed by mr rB,rA."""
import sys,subprocess,re
out=subprocess.run([sys.executable,'/home/kyle/workspace/moh-frontline/scratch/loop3/zero1/pc.py',sys.argv[1]],capture_output=True,text=True).stdout
funcs={};cur=None
for l in out.splitlines():
    m=re.match(r'^[0-9a-f]+ <(.+)>:$',l)
    if m: cur=m.group(1);funcs[cur]=[];continue
    if cur: funcs[cur].append(l.strip())
if not funcs: print(out)
for f,ins in funcs.items():
    zeros=set();shared=False;seq=[]
    for i in ins:
        i=re.sub(r'\s+',' ',i)
        m=re.match(r'li (r\d+),0$',i)
        if m: zeros.add(m.group(1)); seq.append(i)
        m2=re.match(r'mr (r\d+),(r\d+)$',i)
        if m2:
            if m2.group(2) in zeros: shared=True; seq.append(i+'*')
            zeros.discard(m2.group(1))
        if re.match(r'bl\s',i+' ') and not re.match(r'bl[a-z]',i): zeros.clear(); seq.append('|bl|')
    print(('SHARED ' if shared else 'plain  ')+f.split('__')[0].ljust(10),' '.join(seq)[:150])
