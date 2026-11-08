"""hold.py: RESEARCH ONLY, never landed. Prepend a holder emitting the block's exact bytes to each ShellMenu draft; score each draft with true string offsets."""
import subprocess,re,sys,json,tempfile
sys.path.insert(0,'scratch/solo/ret'); import score,fast
ELF='orig/GMFE69/Moh2RelGC.elf'; B=0x801783c8; SZ=0xaf8
nm=[l.split(None,2) for l in subprocess.run(['build/tools/powerpc-eabi-nm','-n',ELF],capture_output=True,text=True).stdout.splitlines()]
L=sorted((int(p[0],16),p[2]) for p in nm if len(p)==3 and B<=int(p[0],16)<B+SZ and p[2].startswith('@'))
secs=subprocess.run(['build/tools/powerpc-eabi-readelf','-SW',ELF],capture_output=True,text=True).stdout
m=re.search(r'\.rodata\s+PROGBITS\s+(\S+)\s+(\S+)',secs); va,off=int(m.group(1),16),int(m.group(2),16)
D=open(ELF,'rb').read(); get=lambda a,n:D[a-va+off:a-va+off+n]
def lit(b):
    b=b.rstrip(b'\0') if b.rstrip(b'\0')!=b'' and b.split(b'\0')[0]==b.rstrip(b'\0') else b[:-1]
    return '"'+''.join(chr(c) if 32<=c<127 and c not in(34,92,63) else f'\\{c:03o}' for c in b)+'"'
items=[]
TABLES={'@2136','@2146','@2182','@2183','@2203','@2586'}
for i,(a,n) in enumerate(L):
    e=L[i+1][0] if i+1<len(L) else B+SZ
    raw=get(a,e-a)
    if n in TABLES:
        w=[int.from_bytes(raw[k:k+4],'big') for k in range(0,len(raw),4)]
        items.append('{ unsigned long t[%d] = {%s}; HOLD_P(t); }'%(len(w),','.join(f'{x}u' for x in w)))
    else: items.append('HOLD_F(%s);'%lit(raw))
holder='void HOLD_F(const char*);\nvoid HOLD_P(unsigned long*);\nvoid HOLD_ALL() {\n'+''.join(f'    {x}\n' for x in items)+'}\n'
PR=get(B,108)
target=get(B,SZ)
prime='namespace dwi {\nstatic const unsigned long dwi_prime_list[27] = {'+','.join(str(int.from_bytes(PR[i:i+4],'big'))+'u' for i in range(0,108,4))+'};\n}\n'
PL=re.compile(r'namespace dwi \{\s*static const unsigned long dwi_prime_list\[27\]\s*=\s*\{.*?\};\s*\}\s*',re.S)
T='config/GMFE69/ShellMenu_ctor.json'
J=[('MemoryCardBootUpCheck__10CShellMenuFv','800e3aac',624,'scratch/solo/sm/buc_a.cpp'),
('MemoryCardAutoLoad__10CShellMenuFi','800e42e8',1104,'scratch/solo/sm/al3.cpp'),
('MemoryCardSaveGame__10CShellMenuFiiPc','800e4c9c',1484,'scratch/solo/sm/sg.cpp'),
('PlayGalleryMovie__10CShellMenuFiiii','800e1eb0',152,'scratch/solo/sm/pg.cpp'),
('PlayBriefingMovie__10CShellMenuFi','800e0770',1804,'scratch/solo/sm/pb.cpp'),
('PlayCreditsSlideShow__10CShellMenuFv','800e0e7c',1660,'scratch/solo/sm/pc.cpp'),
('PassMP_PrefWeaponNameToIStudio__10CShellMenuFiPc','800de6b0',596,'scratch/par/bigfiles/pmw/ShellMenu_sku.cpp'),
('ShellInit__10CShellMenuFUl','800e752c',1232,'scratch/solo/sm/si_best.cpp'),
('MemoryCardBootUpPortCheck__10CShellMenuFi','800e270c',5024,'scratch/solo/sm/pcheck.cpp')]
for f,a,sz,d in J:
    s=PL.sub('',open(d,errors='replace').read())
    src=prime+holder+s
    open('scratch/solo/sm/h/'+d.split('/')[-1],'w').write(src)
    t=json.load(open(T))
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(t,src,w)
        if not obj: print(f[:30],'COMPILE',log[-300:]); continue
        o=subprocess.run([fast.OBJDUMP,'-h',obj],capture_output=True,text=True).stdout
        m=re.search(r'\.rodata\s+([0-9a-f]+)\s',o); rsz=int(m.group(1),16)
        dump=subprocess.run([fast.OBJDUMP,'-s','-j','.rodata',obj],capture_output=True,text=True).stdout
        by=b''
        for l in dump.splitlines():
            q=re.match(r'^ [0-9a-f]+ ((?:[0-9a-f]+ ?){1,4})',l)
            if q: by+=bytes.fromhex(q.group(1).replace(' ',''))
    same=by[:SZ]==target
    r=score.real(T,src,f,a,sz)
    print(f[:34],'rodata',hex(rsz),'prefix-exact',same,(next((hex(i) for i in range(min(len(by),SZ)) if by[i]!=target[i]),None) if not same else ''),'real',r)
