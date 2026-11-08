"""Per string block: functions using it (address order), covered or not."""
import json, subprocess, re, collections
from pathlib import Path
exec(open("research/tools/block_refs.py").read().split("stats=collections")[0])
users=collections.defaultdict(list)
for a,s,n in funcs:
    syms={(sym) for x,(t,sym,add) in rel.items() if a<=x<a+s and t in ("R_PPC_ADDR16_LO","R_PPC_ADDR16_HA") and sym.startswith("...") and "data.0" in sym}
    for sym in syms: users[sym].append((a,s,n,a in owned))
# map block symbol -> address via nm
addr={}
for l in subprocess.run([NM,"-n",ELF],capture_output=True,text=True).stdout.splitlines():
    p=l.split()
    if len(p)==3 and p[2].startswith("..."): addr.setdefault(p[2],int(p[0],16))
rows=[]
for sym,u in users.items():
    u.sort(); unc=sum(s for a,s,n,c in u if not c)
    if unc: rows.append((unc,sym,u))
for unc,sym,u in sorted(rows,reverse=True):
    first=u[0]
    print(f"{unc:6d} {sym} @{addr.get(sym,0):#x} users={len(u)} covered={sum(c for *_,c in u)} first={first[2][:50]} {first[0]:#x} {'COVERED' if first[3] else 'open'} {first[1]}")

print("---- per file")
fm=json.load(open("build/audit/file_map.json"))["files"]
def file_of(a):
    for f in fm:
        for r in f["ranges"]:
            if r["section"] in (".text",) and int(r["start"],16)<=a<int(r["end"],16): return f
    return None
per=collections.defaultdict(list)
for sym,u in users.items():
    for a,s,n,c in u:
        f=file_of(a); per[f["name"] if f else "?"].append((a,s,n,c))
rows=[]
for name,u in per.items():
    u=sorted(set(u)); unc=sum(s for a,s,n,c in u if not c)
    if not unc: continue
    f=next((f for f in fm if f["name"]==name),None)
    tx=[r for r in (f["ranges"] if f else []) if r["section"]==".text"]
    # covered bytes before the first uncovered user, and any covered in file
    firstopen=next(x for x in u if not x[3])
    covered_before=[x for x in u if x[3] and x[0]<firstopen[0]]
    rows.append((unc,name,len(u),firstopen,len(covered_before),tx[0]["start"] if tx else "?"))
for unc,name,nu,fo,cb,ts in sorted(rows,reverse=True):
    print(f"{unc:6d} {name:28s} users={nu:3d} file.text@{ts} first-open={fo[2][:45]} {fo[0]:#x} ({fo[1]}) covered-users-before={cb}")
