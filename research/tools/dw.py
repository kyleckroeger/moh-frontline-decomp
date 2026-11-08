"""dw.py <manifest> [src]: compile a unit with -sym on and dump DWARF 1 subroutines with their params/locals and registers; also checks -sym on leaves .text unchanged (research helper)."""
import json,struct,subprocess,sys,tempfile,copy
import fast
def elf_sections(b):
    shoff=struct.unpack('>I',b[32:36])[0]; n=struct.unpack('>H',b[48:50])[0]; si=struct.unpack('>H',b[50:52])[0]
    sh=[struct.unpack('>10I',b[shoff+i*40:shoff+i*40+40]) for i in range(n)]
    st=sh[si]; names=b[st[4]:st[4]+st[5]]
    return {names[s[0]:names.index(b'\0',s[0])].decode():(s[4],s[5]) for s in sh}
def parse(d):
    i=0; out=[]; 
    while i+4<=len(d):
        ln=struct.unpack('>I',d[i:i+4])[0]
        if ln<8:
            i+=max(ln,4); out.append(None); continue
        tag=struct.unpack('>H',d[i+4:i+6])[0]; a={}; j=i+6; e=i+ln
        while j<e:
            at=struct.unpack('>H',d[j:j+2])[0]; j+=2; f=at&15
            if f in(1,2,7): v=struct.unpack('>I',d[j:j+4])[0]; j+=4
            elif f==3: n=struct.unpack('>H',d[j:j+2])[0]; v=d[j+2:j+2+n]; j+=2+n
            elif f==4: n=struct.unpack('>I',d[j:j+4])[0]; v=d[j+4:j+4+n]; j+=4+n
            elif f==5: v=struct.unpack('>H',d[j:j+2])[0]; j+=2
            elif f==6: v=struct.unpack('>Q',d[j:j+8])[0]; j+=8
            elif f==8: k=d.index(b'\0',j); v=d[j:k].decode('latin1'); j=k+1
            else: break
            a[at]=v
        out.append((tag,a)); i+=ln
    return out
def dump(t,srctext):
    t=copy.deepcopy(t); fl=t['compiler_flags']+['-sym','on']; t['compiler_flags']=fl
    obj,log=fast.compile_object(t,srctext,tempfile.mkdtemp())
    if not obj: raise SystemExit(log[:800])
    b=open(obj,'rb').read(); s=elf_sections(b)
    return obj,parse(b[s['.debug'][0]:s['.debug'][0]+s['.debug'][1]]) if '.debug' in s else (obj,None)
if __name__=='__main__':
    t=json.load(open(sys.argv[1])); src=open(sys.argv[2] if len(sys.argv)>2 else t['source']).read()
    plain,_=fast.compile_object(t,src,tempfile.mkdtemp())
    obj,es=dump(t,src)
    a=subprocess.run([fast.OBJDUMP,'-d',plain],capture_output=True,text=True).stdout.split('\n',3)[3]
    c=subprocess.run([fast.OBJDUMP,'-d',obj],capture_output=True,text=True).stdout.split('\n',3)[3]
    print('text identical with -sym on:',a==c)
    if es is None: print('no .debug section'); sys.exit()
    print(len(es),'entries')
    for e in es:
        if e is None: continue
        tag,at=e
        name=at.get(0x38)
        loc=at.get(0x23)
        if tag in(6,0x2e,5,0x15,0x0a,0x34,0x0b,4) and name:
            print(hex(tag),name,loc.hex() if isinstance(loc,bytes) else loc)
    print('--- all named')
    import collections
    print(collections.Counter(hex(e[0]) for e in es if e))
    for e in es[-40:]:
        if e: print(hex(e[0]),e[1].get(0x38),{hex(k):(v.hex() if isinstance(v,bytes) else v) for k,v in e[1].items() if k not in(0x38,0x12)})
