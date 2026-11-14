import sys,subprocess
O='../../../build/tools/powerpc-eabi-objdump'; ELF='../../../orig/GMFE69/Moh2RelGC.elf'
syms={}
for l in open('syms.txt'):
    p=l.split()
    if len(p)>=5 and p[-2].startswith('0') and ('F' in p or '.text' in l):
        try: syms.setdefault(int(p[0],16),p[-1])
        except: pass
addr=int(sys.argv[1],16); size=int(sys.argv[2],16)
out=subprocess.run([O,'-s','--start-address=%#x'%addr,'--stop-address=%#x'%(addr+size),ELF],capture_output=True,text=True).stdout
b=bytearray()
for l in out.splitlines()[4:]:
    p=l.split()
    for h in p[1:5]:
        try: b+=bytes.fromhex(h)
        except: pass
for i in range(0,len(b),4):
    v=int.from_bytes(b[i:i+4],'big')
    if i in [int(x,16) for x in sys.argv[3:]] or not sys.argv[3:]: print(hex(i),hex(v),syms.get(v,''))
