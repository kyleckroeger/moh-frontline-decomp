#!/usr/bin/env python3
"""Write progress/GMFE69.port_manifest.json: where the PC port gets each function and data symbol.

The port's source-list generator reads this instead of crawling the repository.

functions: [address, size, name, status, source]
  status  verified   byte-exact in the complete-image build; source is the unit's file
          exact      a draft that compiles to the original's code but hasn't landed
          consistent a draft the content checker agrees with (not proof of behaviour)
          differs    a draft the content checker flags; read status.tsv before using it
          none       no source
data:      [address, size, name, section, source]
  source  a unit's file, or "original" while no accepted unit owns the symbol
          (research/tools/unowned_data.py breaks those down by kind)

Proven-in-play status will be added when drafts are tested in Dolphin
(docs/PortReadiness.md, step 1). Run after tools/progress_report.py; the tests
check that the committed file is current.
"""
import csv
import glob
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "progress/GMFE69.port_manifest.json"
CODE = {".init", ".text"}
DATA = {".data", ".rodata", ".sdata", ".sdata2", ".bss", ".sbss", ".sbss2", ".ctors", ".dtors"}


def symbols(path):
    d = path.read_bytes()
    shoff = struct.unpack_from(">I", d, 0x20)[0]
    shnum, shstr = struct.unpack_from(">HH", d, 0x30)
    sections = [struct.unpack_from(">10I", d, shoff + 40 * i) for i in range(shnum)]

    def cstr(at):
        return d[at:d.index(b"\0", at)].decode("latin1")

    names = {i: cstr(sections[shstr][4] + s[0]) for i, s in enumerate(sections)}
    out = []
    for s in sections:
        if s[1] != 2:
            continue
        strtab = sections[s[6]]
        for at in range(s[4], s[4] + s[5], 16):
            name_at, value, size, info, _, shndx = struct.unpack_from(">IIIBBH", d, at)
            if shndx and shndx < 0xFF00:
                out.append((names[shndx], value, size, info & 0xF, cstr(strtab[4] + name_at)))
    return out


def expected():
    units, data_ranges = {}, []
    for path in sorted(glob.glob(str(ROOT / "config/GMFE69/*.json"))):
        unit = json.load(open(path))
        if not isinstance(unit.get("sections"), list) or "source" not in unit:
            continue
        for f in unit.get("functions", []):
            units[int(f["address"], 16)] = unit["source"]
        for s in unit["sections"]:
            if s["name"] in DATA:
                start = int(s["address"], 16)
                data_ranges.append((start, start + s["size"], s["name"], unit["source"]))
    snapshot = json.load(open(ROOT / "progress/GMFE69.snapshot.json"))
    verified_units = {u["id"] for u in snapshot["units"]}
    status = {r["function"]: r for r in csv.DictReader(open(ROOT / "research/drafts/status.tsv"), delimiter="\t")}
    drafts = {int(r["address"], 16): r for r in csv.DictReader(open(ROOT / "research/drafts/index.tsv"), delimiter="\t")}

    functions, data = [], []
    for section, value, size, kind, name in symbols(ROOT / "orig/GMFE69/Moh2RelGC.elf"):
        if section in CODE and kind == 2 and size:
            if value in units:
                functions.append([value, size, name, "verified", units[value]])
            elif value in drafts:
                row = drafts[value]
                st = status.get(row["function"], {}).get("status", "consistent")
                st = {"code_exact": "exact"}.get(st, st)
                functions.append([value, size, name, st, row["draft"]])
            else:
                functions.append([value, size, name, "none", None])
        elif section in DATA and size:
            owner = next((src for lo, hi, sec, src in data_ranges if sec == section and lo <= value < hi), "original")
            data.append([value, size, name, section, owner])
    functions.sort()
    data.sort()
    summary = {}
    for f in functions:
        summary[f[3]] = summary.get(f[3], 0) + f[1]
    owned = sum(d[1] for d in data if d[4] != "original")
    return {
        "schema_version": 1,
        "target": "GMFE69",
        "verified_units": len(verified_units),
        "function_bytes_by_status": dict(sorted(summary.items())),
        "data_bytes_owned": owned,
        "data_bytes_total": sum(d[1] for d in data),
        "functions": functions,
        "data": data,
    }


def render(manifest):
    head = {k: v for k, v in manifest.items() if k not in ("functions", "data")}
    lines = ["{"] + [f" {json.dumps(k)}: {json.dumps(v)}," for k, v in head.items()]
    for key in ("functions", "data"):
        rows = manifest[key]
        lines.append(f' "{key}": [')
        lines += [" " + json.dumps(r) + ("," if i + 1 < len(rows) else "") for i, r in enumerate(rows)]
        lines.append(" ]" + ("," if key == "functions" else ""))
    return "\n".join(lines) + "\n}\n"


def main():
    text = render(expected())
    if "--check" in sys.argv:
        sys.exit(0 if OUT.exists() and OUT.read_text() == text else "port manifest is stale: run python3 tools/port_manifest.py")
    OUT.write_text(text)
    m = json.loads(text)
    print(f"wrote {OUT.relative_to(ROOT)}: {len(m['functions'])} functions {m['function_bytes_by_status']}, "
          f"data owned {m['data_bytes_owned']:,} of {m['data_bytes_total']:,} B")


if __name__ == "__main__":
    main()
