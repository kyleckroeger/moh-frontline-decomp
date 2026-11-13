#!/usr/bin/env python3
"""check_data.py: do the data tables written in research/data/ match the original?

PortReadiness step 2: the port compiles source only, so every data symbol needs a
definition in source. Data can be byte-exact even where the code that uses it isn't
matched yet. Each file in research/data/ defines one or more of the original's data
symbols, under their original names. This compiles each file and, for every data symbol
it defines, compares with the symbol of the same name in the original ELF:

    size and section     must agree (.data, .rodata, .sdata, ...)
    bytes                must agree, except the words a relocation fills in
    relocations          must point at the same thing: the same named symbol and offset,
                         or, for a compiler-local target (a string or float literal),
                         the same bytes there

A file chooses its compiler settings with a line "// settings: config/GMFE69/<unit>.json"
(default: ConfigGC.json, a game unit). A symbol defined more than once in the
original (file statics) needs "// address: <name> 0x8xxxxxxx".

    check_data.py                       check every file, print one line per symbol
    check_data.py research/data/x.cpp   detail for one file (differing bytes)
"""
import collections, glob, json, re, struct, subprocess, sys, tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from formats import Elf32  # noqa: E402

ORIGINAL = ROOT / "orig/GMFE69/Moh2RelGC.elf"
DEFAULT_SETTINGS = "config/GMFE69/ConfigGC.json"
DATA_SECTIONS = {".data", ".rodata", ".sdata", ".sdata2", ".bss", ".sbss", ".sbss2"}
ADDR32, UADDR32 = 1, 24


class Image:
    def __init__(self, path):
        self.elf = Elf32(Path(path).read_bytes())
        self.symbols = list(self.elf.symbols())
        self.by_name = collections.defaultdict(list)
        for s in self.symbols:
            if s["name"] and 0 < s["section"] < 0xFF00:
                self.by_name[s["name"]].append(s)
        self.relocs = collections.defaultdict(dict)  # section index -> {offset or address: (sym, addend)}
        for sec in self.elf.sections:
            if sec["type"] == 4 and sec["info"]:
                data = self.elf.contents(sec)
                for off in range(0, len(data), 12):
                    where, info, addend = struct.unpack_from(">IIi", data, off)
                    self.relocs[sec["info"]][where] = (info & 255, self.symbols[info >> 8], addend)
        self.linked = self.elf.kind != 1
        # For naming relocation targets: named symbols by section, sorted by address.
        self.sorted = collections.defaultdict(list)
        for s in self.symbols:
            if s["name"] and 0 < s["section"] < 0xFF00 and s["type"] in (0, 1, 2):
                self.sorted[s["section"]].append(s)
        for v in self.sorted.values():
            v.sort(key=lambda s: s["address"])

    def section_name(self, index):
        return self.elf.sections[index]["name"]

    def read(self, section, address, size):
        sec = self.elf.sections[section]
        if sec["type"] == 8:
            return bytes(size)
        start = address - (sec["address"] if self.linked else 0)
        return self.elf.contents(sec)[start:start + size]

    def target(self, sym, addend):
        """Describe what a relocation points at, comparably across images."""
        if not sym["section"] or sym["section"] >= 0xFF00:
            return ("extern", sym["name"], addend)
        address = sym["address"] + addend
        sec = sym["section"]
        # The enclosing named symbol, if any.
        best = None
        for s in self.sorted[sec]:
            if s["address"] <= address and (address < s["address"] + max(s["size"], 1)):
                best = s
        if best and not re.match(r"^(@|\.\.\.|\.)", best["name"]) and best["type"] in (1, 2):
            return ("symbol", best["name"], address - best["address"])
        if best and best["type"] == 2 and address == best["address"]:
            return ("symbol", best["name"], 0)
        # A literal: compare its contents (up to a NUL, or 8 bytes for numbers).
        raw = self.read(sec, address, 256)
        end = raw.find(b"\0")
        text = raw[:end] if end >= 0 else raw[:8]
        return ("literal", self.section_name(sec), text)


def compile_file(path, settings):
    from setup_compiler import setup_compiler
    t = json.load(open(ROOT / settings))
    compiler, wrapper = setup_compiler(t["compiler_version"], "GC")
    work = tempfile.mkdtemp()
    src = Path(work) / ("probe" + Path(path).suffix)
    src.write_text(Path(path).read_text())
    cmd = [str(wrapper), str(compiler / "mwcceppc.exe"), "-c", *t["compiler_flags"], "-I-",
           f"-I{Path(path).resolve().parent}", *[f"-I{ROOT / p}" for p in t.get("include_dirs", []) + ([] if "include" in t.get("include_dirs", []) else ["include"])],
           str(src), "-o", "probe.o"]
    for _ in range(3):
        r = subprocess.run(cmd, cwd=work, capture_output=True, text=True)
        if r.returncode == 0:
            return Path(work) / "probe.o", ""
        if "Error" in r.stdout + r.stderr:
            break
    return None, r.stdout + r.stderr


def check_file(path, orig, verbose=False):
    text = Path(path).read_text()
    m = re.search(r"//\s*settings:\s*(\S+)", text)
    settings = m.group(1) if m else DEFAULT_SETTINGS
    pinned = {n: int(a, 16) for n, a in re.findall(r"//\s*address:\s*(\S+)\s+(0x[0-9a-fA-F]+)", text)}
    obj, log = compile_file(path, settings)
    if not obj:
        return [(path, "-", "no_compile", log.strip().splitlines()[-3:])]
    mine = Image(obj)
    out = []
    for s in mine.symbols:
        if not s["name"] or s["type"] != 1 or not 0 < s["section"] < 0xFF00:
            continue
        if s["name"].startswith(("@", "...")) or s["binding"] == 0 and "$" in s["name"]:
            continue  # literals and function statics come with their users
        if mine.section_name(s["section"]) not in DATA_SECTIONS:
            continue
        name = s["name"]
        cands = orig.by_name.get(name, [])
        if name in pinned:
            cands = [c for c in cands if c["address"] == pinned[name]]
        if len(cands) != 1:
            out.append((path, name, "not_found" if not cands else "ambiguous", [hex(c["address"]) for c in cands]))
            continue
        o = cands[0]
        problems = []
        osec, msec = orig.section_name(o["section"]), mine.section_name(s["section"])
        if osec != msec:
            problems.append(f"section {msec}, original {osec}")
        if o["size"] != s["size"]:
            problems.append(f"size {s['size']:#x}, original {o['size']:#x}")
        size = min(o["size"], s["size"])
        ob = bytearray(orig.read(o["section"], o["address"], size))
        mb = bytearray(mine.read(s["section"], s["address"], size))
        orel = {a - o["address"]: v for a, v in orig.relocs[o["section"]].items() if o["address"] <= a < o["address"] + size}
        mrel = {a - s["address"]: v for a, v in mine.relocs[s["section"]].items() if s["address"] <= a < s["address"] + size}
        for k in set(orel) | set(mrel):
            for buf in (ob, mb):
                buf[k:k + 4] = bytes(4)
            if k not in orel or k not in mrel:
                problems.append(f"+{k:#x}: relocation only in {'original' if k in orel else 'source'}")
                continue
            if orel[k][1]["name"] and orel[k][1]["name"] == mrel[k][1]["name"] and orel[k][2] == mrel[k][2]:
                continue  # the same symbol and addend as recorded (one past an array's end too)
            ot, mt = orig.target(orel[k][1], orel[k][2]), mine.target(mrel[k][1], mrel[k][2])
            if ot[0] == "literal" and mt[0] == "literal":
                same = ot[2] == mt[2]
            else:
                same = ot[1:] == mt[1:] and ot[0] != "literal"
            if not same:
                problems.append(f"+{k:#x}: points at {mt}, original {ot}")
        diffs = [i for i in range(size) if ob[i] != mb[i]]
        if diffs:
            problems.append(f"{len(diffs)} bytes differ, first at +{diffs[0]:#x}")
            if verbose:
                for i in diffs[:16]:
                    problems.append(f"    +{i:#x}: {mb[i]:02x}, original {ob[i]:02x}")
        out.append((path, name, "exact" if not problems else "differs", problems))
    return out


def main():
    orig = Image(ORIGINAL)
    files = sys.argv[1:] or sorted(glob.glob(str(ROOT / "research/data/**/*.c*"), recursive=True))
    total = collections.Counter()
    for f in files:
        for path, name, status, info in check_file(f, orig, verbose=bool(sys.argv[1:])):
            total[status] += 1
            print(f"{status:10} {name:32} {Path(path).name}")
            if status != "exact":
                for line in info:
                    print("    " + str(line))
    print(dict(total))
    return 0 if set(total) <= {"exact"} else 1


if __name__ == "__main__":
    sys.exit(main())
