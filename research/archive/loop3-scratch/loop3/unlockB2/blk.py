"""blk.py LO HI: list items (nm labels) of an original .rodata block with contents."""
import subprocess,re,sys
ELF='orig/GMFE69/Moh2RelGC.elf'
lo,hi=int(sys.argv[1],16),int(sys.argv[2],16)
nm=[l.split() for l in subprocess.run(['build/tools/powerpc-eabi-nm','-n',ELF],capture_output=True,text=True).stdout.splitlines()]
L=sorted(set((int(p[0],16),p[-1]) for p in nm if len(p)>=3 and lo<=int(p[0],16)<hi))
secs=subprocess.run(['build/tools/powerpc-eabi-readelf','-SW',ELF],capture_output=True,text=True).stdout
m=re.search(r'\.rodata\s+PROGBITS\s+(\S+)\s+(\S+)',secs); va,off=int(m.group(1),16),int(m.group(2),16)
D=open(ELF,'rb').read(); get=lambda a,n:D[a-va+off:a-va+off+n]
for i,(a,n) in enumerate(L):
    e=L[i+1][0] if i+1<len(L) else hi
    if e==a: print(f'{a-lo:5d} {a:#x} {n} (alias)'); continue
    b=get(a,e-a)
    s=b.split(b'\0')[0]
    if len(s)>=1 and all(32<=c<127 or c in (9,10,13) or c>=0xa0 for c in s) and len(s)+1>=len(b)-3:
        print(f'{a-lo:5d} {a:#x} {e-a:4d} {n} STR {s!r}')
    else:
        print(f'{a-lo:5d} {a:#x} {e-a:4d} {n} HEX {b[:64].hex()}')
