"""scan.py: find reference min/max (stack-address select) and 'bge; b; fmr' float selects in the
ORIGINAL code of every draft in research/drafts/index.tsv; report how the draft writes selects."""
import csv,re,subprocess,sys
OD='build/tools/powerpc-eabi-objdump'; ELF='orig/GMFE69/Moh2RelGC.elf'
def dis(a,size):
    a=int(a,16); out=subprocess.run([OD,'-d','--start-address=%#x'%a,'--stop-address=%#x'%(a+int(size)),ELF],capture_output=True,text=True).stdout
    r=[]
    for l in out.splitlines():
        m=re.match(r'^\s*([0-9a-f]+):\t[0-9a-f ]+\t(\S+)\s*(.*?)\s*(<.*>)?$',l)
        if m: r.append((int(m.group(1),16),m.group(2),m.group(3).replace(' ','')))
    return r
CB=re.compile(r'^b(lt|le|eq|ge|gt|ne|nl|ng)[-+]?$')
def tgt(ops):
    t=ops.split(',')[-1]
    try: return int(t,16)
    except: return None
def scan(ins):
    idx={a:i for i,(a,_,_) in enumerate(ins)}; hits=[]
    for i,(a,op,ops) in enumerate(ins):
        if not CB.match(op): continue
        L=tgt(ops)
        if L not in idx: continue
        j=idx[L]
        # compare feeding the branch
        cmp=next((ins[k] for k in range(i-1,max(i-6,-1),-1) if ins[k][1] in('fcmpo','fcmpu','cmpw','cmplw','cmpwi','cmplwi')),None)
        cmps='%s %s'%(cmp[1],cmp[2]) if cmp else '?'
        # find unconditional b within 3 after the cond branch
        for k in range(i+1,min(i+4,len(ins))):
            if ins[k][1]=='b':
                M=tgt(ins[k][2]); body=ins[i+1:k]
                if j!=k+1: break
                tb=ins[j]
                # P1 reference select: addi rX,r1,N on both arms (same rX)
                if len(body)==1 and body[0][1]=='addi' and tb[1]=='addi':
                    x=body[0][2].split(','); y=tb[2].split(',')
                    if x[1]=='r1' and y[1]=='r1' and x[0]==y[0]:
                        use=next(('%s %s'%(o,p) for (_,o,p) in ins[j+1:j+12] if re.search(r'\b0\(%s\)'%x[0],p)),'')
                        hits.append(('refsel',a,cmps,op,'%s|%s'%(body[0][2],tb[2]),use))
                # P2 float select: bge L; [..] b M; L: fmr
                elif tb[1]=='fmr' and (M is not None and M==tb[0]+4):
                    hits.append(('fsel',a,cmps,op,' '.join(o+' '+p for _,o,p in body)+' | fmr '+tb[2],''))
                break
    return hits
def draftinfo(path):
    t=open(path,errors='replace').read()
    return dict(tern=len(re.findall(r'\?[^:;]*:',t)),ptrtern=len(re.findall(r'\?\s*&\w[^:;]*:\s*&',t)),
        minmax=len(re.findall(r'\b(Min|Max|MIN|MAX|min|max|fmin|fmax|Clamp|clamp)\s*\(',t)),dwi=('dwi::min' in t or 'dwi::max' in t))
def main():
  out=[]
  for row in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\t'):
      h=scan(dis(row['address'],row['size']))
      if not h: continue
      d=draftinfo(row['draft'])
      for kind,a,cmps,op,arms,use in h:
          out.append([row['function'],row['real_lines'],row['size'],kind,'%x'%a,cmps,op,arms,use,str(d['tern']),str(d['ptrtern']),str(d['minmax']),str(d['dwi']),row['draft'],row['compile_settings_from']])
  hdr=['function','real_lines','size','kind','addr','compare','branch','arms','use','draft_ternaries','draft_ptr_ternaries','draft_minmax_calls','draft_dwi','draft','settings']
  with open(sys.argv[1],'w') as f:
      f.write('\t'.join(hdr)+'\n')
      for r in out: f.write('\t'.join(r)+'\n')
  print(len(out),'hits in',len({r[0] for r in out}),'functions')

if __name__=='__main__': main()
