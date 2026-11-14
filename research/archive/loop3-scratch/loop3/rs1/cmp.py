import sys, re, subprocess
S='/home/kyle/workspace/moh-frontline/scratch/loop3/rs1/'
OD='/home/kyle/workspace/moh-frontline/build/tools/powerpc-eabi-objdump'
def syms(p):
    d={}
    for l in open(S+p):
        f=l.split()
        if len(f)>=5 and ' F ' in l:
            d.setdefault(f[-1],(int(f[0],16),int(f[-2],16)))
    return d
FL=syms('fl_syms.txt'); RS=syms('rs_syms.txt')
def bls(elf,a,n):
    out=subprocess.run([OD,'-d','--start-address=%#x'%a,'--stop-address=%#x'%(a+n),elf],capture_output=True,text=True).stdout
    r=[]
    for l in out.splitlines():
        m=re.search(r'\tbl\s+\S+\s+<([^>]+)>',l)
        if m: r.append(m.group(1))
    return r
def key(m):
    # name + class from either mangling
    m=m.split('<')[0] if m.startswith('TestCDB') else m
    mm=re.match(r'(.+?)__(C?)(\d+)([A-Za-z_]\w*)',m)
    if mm:
        n=int(mm.group(3)); return (mm.group(1), mm.group(4)[:n])
    mm=re.match(r'(.+?)__F',m)
    if mm: return (mm.group(1),'')
    return (m,'')
rsk={}
for k in RS: rsk.setdefault(key(k),[]).append(k)
for name in sys.argv[1:]:
    if name not in FL: print('NOFL',name); continue
    a,n=FL[name]; fb=bls('orig/GMFE69/Moh2RelGC.elf',a,n)
    fk=set(key(x) for x in fb)
    print('==',name,hex(a),n,'FL bl:',sorted(set(fb)))
    for r in rsk.get(key(name),[]):
        ra,rn=RS[r]; rb=bls('../moh-rising-sun/orig/GR8E69/MOH3RDVD.ELF',ra,rn)
        extra=sorted(set(x for x in rb if key(x) not in fk))
        print('  RS',r,hex(ra),rn,'extra:',extra)
