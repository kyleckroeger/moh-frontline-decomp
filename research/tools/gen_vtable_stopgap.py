#!/usr/bin/env python3
"""gen_vtable_stopgap.py: write the stopgap vtables for the pure-source link (endgame plan W1, D1).

A virtual table is emitted by the compiler from its class's declaration, in the unit that
defines the class's key function. Where that unit is still a fragment that mustn't own the
file's data, the table is undefined in the pure-source link. Until those files merge, this
writes each such table as data, slot by slot, from the original's relocations. It also writes
the RTTI records the tables reach (and their base lists and names) as file-local objects, as
the original files do.

The list of tables is research/data_stopgap/vtables.txt. Once a unit owns a table,
source_link.py reports it as a duplicate. Delete the name from the list and run this again.
The output is research/data_stopgap/vtables.cpp. Only source_link.py reads it. It earns no
progress, and check_data.py doesn't check it: that tool compares base lists by their linked
bytes. This tool checks it instead: it compiles the file and compares every word and
relocation target, by name or by string, with the original.

    gen_vtable_stopgap.py            regenerate and check
    gen_vtable_stopgap.py --check    check only
"""
import argparse, json, re, subprocess, sys, tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(ROOT / "research/tools"))
from check_data import Image, ORIGINAL, compile_file  # noqa: E402

LIST = ROOT / "research/data_stopgap/vtables.txt"
OUT = ROOT / "research/data_stopgap/vtables.cpp"
SETTINGS = "config/GMFE69/ConfigGC.json"


def enclosing(img, sec, address):
    best = None
    for s in img.sorted[sec]:
        if s["address"] <= address < s["address"] + max(s["size"], 1):
            best = s
    return best


def words(img, sym):
    """[(value, None) or (None, (target symbol, addend))] for each word of a data object."""
    sec = sym["section"]
    raw = img.read(sec, sym["address"], sym["size"])
    out = []
    for off in range(0, sym["size"], 4):
        rel = img.relocs[sec].get(sym["address"] + off)
        if rel:
            out.append((None, (rel[1], rel[2])))
        else:
            out.append((int.from_bytes(raw[off:off + 4], "big"), None))
    return out


class Gen:
    def __init__(self, img):
        self.img = img
        self.externs = set()
        self.defs = []          # emitted definitions, in dependency order
        self.done = {}          # original address -> C name
        self.pins = []

    def string(self, sec, address):
        raw = self.img.read(sec, address, 256)
        text = raw[:raw.index(b"\0")].decode("latin-1")
        return '"' + text.replace("\\", "\\\\").replace('"', '\\"') + '"'

    def ref(self, target, addend):
        """C expression for a relocated word."""
        img = self.img
        sec = target["section"]
        if not sec or sec >= 0xFF00:
            raise SystemExit(f"unresolved target {target['name']}")
        address = target["address"] + addend
        obj = enclosing(img, sec, address)
        secname = img.section_name(sec)
        if obj and obj["type"] == 2:                       # a function
            if obj["binding"] == 0:
                raise SystemExit(f"file-local function in a table: {obj['name']}")
            self.externs.add(obj["name"])
            return f"(void*){obj['name']}" + (f" + {address - obj['address']}" if address != obj["address"] else "")
        if obj and obj["name"].startswith("__RTTI__"):
            return f"(void*){self.record(obj)}"
        if obj and obj["name"].startswith("__vt__") and obj["binding"]:
            self.externs.add(obj["name"])
            return f"(void*)((char*){obj['name']} + {address - obj['address']})"
        if secname in (".rodata", ".sdata2"):              # a name string
            return f"(void*){self.string(sec, address)}"
        if obj and obj["name"].startswith("@") and secname in (".data", ".sdata"):
            return f"(void*){self.blob(obj)}" + (f" + {(address - obj['address']) // 4}" if address != obj["address"] else "")
        raise SystemExit(f"unhandled target {obj and obj['name']} in {secname} at {address:#x}")

    def body(self, sym):
        parts = []
        for value, rel in words(self.img, sym):
            parts.append(self.ref(*rel) if rel else ("0" if value == 0 else f"(void*){value:#x}"))
        return parts

    def record(self, sym):
        if sym["address"] in self.done:
            return self.done[sym["address"]]
        name = sym["name"]
        if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", name):
            name = f"rtti_{sym['address']:08x}"             # file-local, so the name is free
        self.done[sym["address"]] = name
        body = self.body(sym)                               # defines its base list (and bases) first
        if any(d.startswith(f"static void* {name}[") for d in self.defs):
            return name                                     # one file-local record per name here
        self.defs.append(f"static void* {name}[{len(body)}] = {{{', '.join(body)}}};")
        self.pins.append((name, sym["address"]))
        return name

    def blob(self, sym):
        if sym["address"] in self.done:
            return self.done[sym["address"]]
        name = f"bases_{sym['address']:08x}"
        self.done[sym["address"]] = name
        body = self.body(sym)
        self.defs.append(f"static void* {name}[{len(body)}] = {{{', '.join(body)}}};")
        return name

    def table(self, sym):
        body = self.body(sym)
        self.defs.append(f"void* {sym['name']}[{len(body)}] = {{\n    " + ",\n    ".join(body) + "\n};")


def generate(img, names):
    g = Gen(img)
    for name in names:
        cands = [s for s in img.by_name.get(name, []) if s["type"] == 1 and s["binding"]]
        if len(cands) != 1:
            raise SystemExit(f"{name}: {len(cands)} global definitions in the original")
        g.table(cands[0])
    out = ["// GENERATED by research/tools/gen_vtable_stopgap.py from research/data_stopgap/vtables.txt.",
           "// Do not edit. A stopgap for the pure-source link (research/plans/endgame.md, W1 and D1):",
           "// each table's slots, and the RTTI records it reaches, copied from the original's",
           "// relocations, for tables whose key-function units can't own them yet. Not progress.",
           "// settings: " + SETTINGS, "",
           'extern "C" {']
    out += [f"extern char {n}[];" for n in sorted(g.externs)]
    out += [""] + g.defs + ["}", ""]
    return "\n".join(out)


def check(img):
    obj, log = compile_file(OUT, SETTINGS)
    if not obj:
        raise SystemExit("does not compile:\n" + log)
    mine = Image(obj)
    names = LIST.read_text().split()
    problems = 0
    for name in names:
        o = [s for s in img.by_name[name] if s["type"] == 1 and s["binding"]][0]
        m = [s for s in mine.by_name[name] if s["type"] == 1][0]
        problems += compare(img, o, mine, m, name, set())
    print(f"{len(names)} tables checked, {problems} problems")
    return problems


def compare(img, o, mine, m, label, seen):
    if o["size"] != m["size"]:
        print(f"{label}: size {m['size']} vs {o['size']}")
        return 1
    bad = 0
    ow, mw = words(img, o), words(mine, m)
    for i, (a, b) in enumerate(zip(ow, mw)):
        if (a[0] is None) != (b[0] is None):
            print(f"{label}+{4 * i:#x}: relocation in only one"); bad += 1; continue
        if a[0] is not None:
            bad += a[0] != b[0]
            continue
        ot, oa = a[1]; mt, ma = b[1]
        oo = enclosing(img, ot["section"], ot["address"] + oa) if ot["section"] and ot["section"] < 0xFF00 else ot
        mo = enclosing(mine, mt["section"], mt["address"] + ma) if mt["section"] and mt["section"] < 0xFF00 else mt
        osec = img.section_name(ot["section"])
        if osec in (".rodata", ".sdata2"):
            os_ = img.read(ot["section"], ot["address"] + oa, 256); ms = mine.read(mt["section"], mt["address"] + ma, 256)
            if os_[:os_.index(b"\0")] != ms[:ms.index(b"\0")]:
                print(f"{label}+{4 * i:#x}: string differs"); bad += 1
            continue
        if oo["name"].startswith("@"):                      # a base list: compare it in turn
            if (oo["address"], label) not in seen:
                seen.add((oo["address"], label))
                bad += compare(img, oo, mine, mo, f"{label}>{oo['name']}", seen)
            continue
        if oo["name"].startswith("__RTTI__") and oo["binding"] == 0:   # file-local: compare contents
            if ("rec", oo["address"]) not in seen:
                seen.add(("rec", oo["address"]))
                bad += compare(img, oo, mine, mo, f"{label}>{oo['name']}", seen)
            continue
        if oo["name"] != mo["name"] or (ot["address"] + oa - oo["address"]) != (mt["address"] + ma - mo["address"]):
            print(f"{label}+{4 * i:#x}: {mo['name']} vs {oo['name']}"); bad += 1
    return bad


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true")
    a = ap.parse_args()
    img = Image(ORIGINAL)
    if not a.check:
        OUT.write_text(generate(img, LIST.read_text().split()))
        print(f"wrote {OUT.relative_to(ROOT)}")
    sys.exit(1 if check(img) else 0)


if __name__ == "__main__":
    main()
