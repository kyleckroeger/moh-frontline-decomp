"""pr.py NAME BODY [extra]: gen + compile + show the free loop of ~list and reserve."""
import sys,subprocess,re,os
d=os.path.dirname(os.path.abspath(__file__))
os.chdir(d+'/p')
subprocess.run(['python3','gen.py']+sys.argv[1:],check=True)
J=os.environ.get('J','/home/kyle/workspace/moh-frontline/scratch/loop3/merge8/tpl_alarm.json')
out=subprocess.run(['python3',d+'/pc.py',sys.argv[1]+'.cpp','',J],capture_output=True,text=True).stdout
fn=None;buf={};res={}
for l in out.splitlines():
    m=re.match(r'^[0-9a-f]+ <(.+)>:$',l)
    if m: fn=m.group(1); buf[fn]=[]; continue
    if fn: buf[fn].append(l.strip())
if not buf: print(out[-2000:])
for f,ls in buf.items():
    if not ('__dt__' in f or 'reserve' in f): continue
    # loop: from the 'b' before the loop to the bne
    s=' | '.join(ls)
    i=[k for k,l in enumerate(ls) if l.startswith('bne')]
    k=[k for k,l in enumerate(ls) if re.match(r'b\s',l)]
    seg=ls[k[0]-2:i[0]+1] if k and i else ls
    tag='DT' if '__dt__' in f else 'RS'
    m=re.search(r'b \S+ <.*?\+0x[0-9a-f]+> ; lwz r(\d+),4\(r(\d+)\)',' ; '.join(re.sub(r'\s+',' ',x) for x in seg))
    res[tag]=(m.group(2),m.group(1)) if m else ('?','?')
    if os.environ.get('V'): print((tag+' ' if '__dt__' in f else 'RS ')+' ; '.join(re.sub(r'\s+',' ',x.split('#')[0]) for x in seg if not x.startswith(('R_','\t'))))
dt=res.get('DT',('?','?'));rs=res.get('RS',('?','?'))
ok=(dt==('6','5'))+(rs==('5','4'))*2
print('%-6s DT node r%s next r%s %s | RS node r%s next r%s %s %s'%(sys.argv[1],dt[0],dt[1],'ok' if dt==('6','5') else '--',rs[0],rs[1],'ok' if rs==('5','4') else '--','BOTH' if ok==3 else ''))
