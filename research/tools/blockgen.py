"""blockgen.py: items of the ShellMenu .rodata block 0x801783c8 from the original ELF (research + generator)."""
import subprocess,re
ELF='orig/GMFE69/Moh2RelGC.elf'; B=0x801783c8; SZ=0xaf8
nm=[l.split(None,2) for l in subprocess.run(['build/tools/powerpc-eabi-nm','-n',ELF],capture_output=True,text=True).stdout.splitlines()]
L=sorted((int(p[0],16),p[2]) for p in nm if len(p)==3 and B<=int(p[0],16)<B+SZ and p[2].startswith('@'))
secs=subprocess.run(['build/tools/powerpc-eabi-readelf','-SW',ELF],capture_output=True,text=True).stdout
m=re.search(r'\.rodata\s+PROGBITS\s+(\S+)\s+(\S+)',secs); va,off=int(m.group(1),16),int(m.group(2),16)
D=open(ELF,'rb').read(); get=lambda a,n:D[a-va+off:a-va+off+n]
TABLES={'@2136','@2146','@2182','@2183','@2203','@2586'}
def lit(b):
    b=b.rstrip(b'\0')
    return '"'+''.join(chr(c) if 32<=c<127 and c not in(34,92,63) else f'\\{c:03o}' for c in b)+'"'
ITEMS=[]
for i,(a,n) in enumerate(L):
    e=L[i+1][0] if i+1<len(L) else B+SZ
    ITEMS.append((n,a-B,get(a,e-a)))
TARGET=get(B,SZ)
PRIMES=[int.from_bytes(TARGET[i:i+4],'big') for i in range(0,108,4)]
def words(raw): return [int.from_bytes(raw[k:k+4],'big') for k in range(0,len(raw),4)]
PRIME_SRC='namespace dwi {\nstatic const unsigned long dwi_prime_list[27] = {'+','.join(f'{x}u' for x in PRIMES)+'};\n}\n'
PL=re.compile(r'namespace dwi \{\s*static const unsigned long dwi_prime_list\[27\]\s*=\s*\{.*?\};\s*\}\s*',re.S)
