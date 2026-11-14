"""vt.py <vtable symbol>: slot offset, address, symbol for each slot of an original vtable."""
import sys,subprocess,re
R='/home/kyle/workspace/moh-frontline/'
syms={}
names={}
for l in open(R+'scratch/loop3/merge1/orig_syms.txt'):
    p=l.split()
    if len(p)>=6 and p[0]!='00000000' and p[2]=='F' or (len(p)>=6 and p[2]=='F'):
        syms.setdefault(int(p[0],16),[]).append(p[-1])
    if len(p)>=6 and p[-1]==sys.argv[1]: va=int(p[0],16); sz=int(p[-2],16)
out=subprocess.run([R+'build/tools/powerpc-eabi-objdump','-s','-j','.data','--start-address=%#x'%va,'--stop-address=%#x'%(va+sz),R+'orig/GMFE69/Moh2RelGC.elf'],capture_output=True,text=True).stdout
words=[]
for l in out.splitlines():
    m=re.match(r'^ ([0-9a-f]{8}) ((?:[0-9a-f]{8} ?){1,4})',l)
    if m: words+= [int(w,16) for w in m.group(2).split()]
for i,w in enumerate(words):
    print('%3x %08x %s'%(i*4,w,' '.join(syms.get(w,['?']))))
