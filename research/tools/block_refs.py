"""Classify uncovered functions that reference a file's anonymous ...rodata.0/...data.0 block."""
import json, subprocess, re, collections
from pathlib import Path
ELF="orig/GMFE69/Moh2RelGC.elf"; OBJ="build/tools/powerpc-eabi-objdump"; NM="build/tools/powerpc-eabi-nm"
owned=set()
for p in Path("config/GMFE69").glob("*.json"):
    if p.name=="project.json": continue
    for f in json.loads(p.read_text()).get("functions",[]): owned.add(int(f["address"],16))
funcs=[]
for l in subprocess.run([NM,"-n","-S",ELF],capture_output=True,text=True).stdout.splitlines():
    p=l.split()
    if len(p)==4 and p[2] in "TtWw": funcs.append((int(p[0],16),int(p[1],16),p[3]))
dis=subprocess.run([OBJ,"-dr","--start-address=0x80003100","--stop-address=0x80160000",ELF],capture_output=True,text=True).stdout
# collect relocations: address -> (type, symbol, addend)
rel={}
for l in dis.splitlines():
    m=re.match(r"\s+([0-9a-f]+): (R_PPC_\S+)\s+(\S+)",l)
    if m:
        sym=m.group(3); add=0
        mm=re.match(r"(.*?)([+-]0x[0-9a-f]+)$",sym)
        if mm: sym=mm.group(1); add=int(mm.group(2),16)
        rel[int(m.group(1),16)]=(m.group(2),sym,add)
stats=collections.Counter(); nb=collections.Counter()
for a,s,n in funcs:
    if a in owned: continue
    lo=[(t,sym,add) for x,(t,sym,add) in rel.items() if a<=x<a+s and sym.startswith("...") and ("rodata.0" in sym or "data.0" in sym)]
    lo=[r for r in lo if r[0] in ("R_PPC_ADDR16_LO","R_PPC_ADDR16_HA","R_PPC_EMB_SDA21")]
    if not lo: continue
    los=[r for r in lo if r[0]=="R_PPC_ADDR16_LO"]
    kind="data" if any("data.0" in r[1] and "rodata" not in r[1] for r in lo) else "rodata"
    zero=[r for r in los if r[2]==0]
    cls = "base_cse" if zero else "addend_only"
    stats[(kind,cls)]+=s; nb[(kind,cls)]+=1
for k in sorted(stats): print(k, nb[k], "functions", stats[k], "bytes")
