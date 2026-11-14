import sys, json
from pathlib import Path
sys.path.insert(0, "tools")
from formats import Elf32
uid, workdir = sys.argv[1], sys.argv[2]
unit = json.loads(Path(f"config/GMFE69/{uid}.json").read_text())
want = {f["name"]: f.get("size") for f in unit["functions"]}
obj = Elf32(Path(workdir, "compiled.o").read_bytes())
for s in obj.symbols():
    if s["name"] in want and s["type"] == 2 and want[s["name"]] != s["size"]:
        print("  size differs:", s["name"], "orig", want[s["name"]], "variant", s["size"])
