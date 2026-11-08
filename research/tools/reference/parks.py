"""parks.py: PARKED log entries whose function address is still uncovered, with draft paths and the score text."""
import json, re, subprocess
from pathlib import Path
owned = set()
for p in Path("config/GMFE69").glob("*.json"):
    if p.name == "project.json": continue
    for f in json.loads(p.read_text()).get("functions", []): owned.add(int(f["address"], 16))
syms = {}
for l in subprocess.run(["build/tools/powerpc-eabi-nm", "-S", "orig/GMFE69/Moh2RelGC.elf"], capture_output=True, text=True).stdout.splitlines():
    p = l.split()
    if len(p) == 4 and p[2] in "TtWw": syms[int(p[0], 16)] = (int(p[1], 16), p[3])
seen = set()
for i, line in enumerate(open("scratch/AUTOPILOT.md"), 1):
    if not re.search(r"PARK|near-miss|\d+ (off|lines)", line): continue
    for m in re.finditer(r"0x(80[0-9a-f]{6})", line):
        a = int(m.group(1), 16)
        if a in syms and a not in owned and a not in seen:
            seen.add(a)
            drafts = re.findall(r"scratch/[\w/.\-]+\.(?:cpp|c)", line)
            score = re.search(r"(\d+) (off|lines|raw|instr)", line)
            print(f"{i:5} {a:08x} {syms[a][0]:5} {score.group(0) if score else '?':10} {syms[a][1][:55]:55} {' '.join(d for d in drafts if Path(d).exists())[:80]}")
