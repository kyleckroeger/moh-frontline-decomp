#!/usr/bin/env python3
"""source_symbols.py: which of the original's symbols would a pure-source link define?

PortReadiness step 2.4 measures progress toward a pure-source image by linking every
unit, draft and data file with no original bytes and counting undefined and duplicate
symbols. Until that link exists, this counts the same thing from symbol tables:

    units   build/reconstruction/units/*/compiled.o (run tools/reconstruct.py first)
    drafts  research/drafts/index.tsv, each compiled with its listed settings
    data    research/data/*.cpp

For every named symbol in the original ELF it reports whether some source defines it
("defined"), none does ("missing"), or more than one non-weak definition exists
("duplicate"). Compiler-local names (@123 literals, ...section markers) aren't counted:
they come with the function or table that uses them. A name the original defines k
times (a file static in k files) needs k definitions.

    source_symbols.py [--jobs N]          summary by section and kind, writes
                                          research/layout/source_symbols.tsv
    source_symbols.py --missing SECTION   list missing symbols of one section, largest first
"""
import argparse, collections, csv, glob, json, os, subprocess, sys, tempfile
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from formats import Elf32  # noqa: E402

ORIGINAL = ROOT / "orig/GMFE69/Moh2RelGC.elf"
OUT = ROOT / "research/layout/source_symbols.tsv"
CACHE = ROOT / "build/layout/source_symbols_drafts.json"
SECTIONS = {".init", ".text", ".ctors", ".dtors", ".rodata", ".data", ".bss", ".sdata", ".sbss",
            ".sdata2", ".sbss2", "extab", "extabindex"}
STB_LOCAL, STB_GLOBAL, STB_WEAK = 0, 1, 2


def tracked(name):
    return bool(name) and not name.startswith(("@", "...", ".")) and name not in ("probe.cpp", "probe.c")


def defined(path):
    """[(name, binding, section name, size)] for symbols an object defines."""
    elf = Elf32(Path(path).read_bytes())
    out = []
    for s in elf.symbols():
        if not tracked(s["name"]) or not 0 < s["section"] < 0xFF00 or s["type"] not in (1, 2):
            continue
        sec = elf.sections[s["section"]]["name"]
        if sec in SECTIONS:
            out.append((s["name"], s["binding"], sec, s["size"]))
    return out


def compile_draft(job):
    draft, settings = job
    from setup_compiler import setup_compiler
    t = json.load(open(ROOT / settings))
    compiler, wrapper = setup_compiler(t["compiler_version"], "GC")
    src = ROOT / draft
    suffixes = [src.suffix] + ([".cpp"] if src.suffix == ".c" else [])
    with tempfile.TemporaryDirectory() as work:
        for suffix in suffixes:
            probe = Path(work) / ("probe" + suffix)
            probe.write_text(src.read_text(errors="replace"))
            cmd = [str(wrapper), str(compiler / "mwcceppc.exe"), "-c", *t["compiler_flags"], "-I-",
                   f"-I{src.parent}", *[f"-I{ROOT / p}" for p in t.get("include_dirs", [])], str(probe), "-o", "probe.o"]
            for _ in range(3):
                r = subprocess.run(cmd, cwd=work, capture_output=True, text=True)
                if r.returncode == 0:
                    return draft, defined(Path(work) / "probe.o")
                if "Error" in r.stdout + r.stderr:
                    break
    return draft, None


def sources(jobs):
    out = {}
    for o in sorted(glob.glob(str(ROOT / "build/reconstruction/units/*/compiled.o"))):
        out["unit:" + Path(o).parent.name] = defined(o)
    if not out:
        raise SystemExit("no unit objects: run tools/reconstruct.py first")
    work = [(r["draft"], r["compile_settings_from"])
            for r in csv.DictReader(open(ROOT / "research/drafts/index.tsv"), delimiter="\t")]
    work += [(os.path.relpath(f, ROOT), "config/GMFE69/ConfigGC_dualconfig.json")
             for f in sorted(glob.glob(str(ROOT / "research/data/*.cpp")))]
    # research/data files name their own settings.
    for i, (f, s) in enumerate(work):
        if f.startswith("research/data/"):
            for line in open(ROOT / f):
                if line.startswith("// settings:"):
                    work[i] = (f, line.split(":", 1)[1].strip())
    seen = {}
    with ProcessPoolExecutor(jobs) as ex:
        for f, syms in ex.map(compile_draft, sorted(set(work)), chunksize=4):
            if syms is None:
                print("does not compile:", f, file=sys.stderr)
                continue
            # A draft file appears once per function it holds; count its symbols once.
            seen[f] = syms
    for f, syms in seen.items():
        out[("data:" if f.startswith("research/data/") else "draft:") + f] = syms
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--missing")
    a = ap.parse_args()
    orig = Elf32(ORIGINAL.read_bytes())
    want = collections.defaultdict(list)  # name -> [(section, size)]
    for s in orig.symbols():
        if tracked(s["name"]) and 0 < s["section"] < 0xFF00 and s["type"] in (1, 2):
            sec = orig.sections[s["section"]]["name"]
            if sec in SECTIONS:
                want[s["name"]].append((sec, s["size"]))
    have = collections.defaultdict(list)  # name -> [(source, binding)]
    for src, syms in sources(a.jobs).items():
        for name, binding, sec, size in syms:
            have[name].append((src, binding))
    rows = []
    for name, copies in want.items():
        defs = have.get(name, [])
        strong = [d for d in defs if d[1] != STB_WEAK]
        # Drafts and units may both define a function while its draft is still open (a
        # unit's fragment can cover other functions of the same file), so a duplicate
        # counts only between two units, or two drafts, or a unit and a data file.
        kinds = collections.Counter(src.split(":")[0] for src, _ in strong)
        if not defs:
            status = "missing"
        elif max(kinds.values(), default=0) > len(copies) or kinds["unit"] + kinds["data"] > len(copies):
            status = "duplicate"
        elif len(defs) < len(copies):
            status = "missing"  # a file static needed in several files, defined in fewer
        else:
            status = "defined"
        for sec, size in copies:
            rows.append((status, sec, size, name, " ".join(sorted({d[0] for d in defs}))[:200]))
    if a.missing:
        for st, sec, size, name, by in sorted(rows, key=lambda r: -r[2]):
            if st == "missing" and sec == a.missing:
                print(f"{size:8} {name}")
        return
    OUT.parent.mkdir(exist_ok=True)
    with open(OUT, "w") as o:
        o.write("status\tsection\tsize\tsymbol\tdefined_by\n")
        for r in sorted(rows, key=lambda r: (r[0], r[1], -r[2], r[3])):
            o.write("\t".join(map(str, r)) + "\n")
    table = collections.defaultdict(lambda: collections.Counter())
    for st, sec, size, name, by in rows:
        table[sec][st + "_n"] += 1
        table[sec][st + "_b"] += size
    print(f"{'section':12} {'defined':>18} {'missing':>18} {'duplicate':>14}")
    for sec in sorted(table):
        t = table[sec]
        print(f"{sec:12} {t['defined_n']:6} {t['defined_b']:10,} B {t['missing_n']:6} {t['missing_b']:10,} B "
              f"{t['duplicate_n']:5} {t['duplicate_b']:8,} B")


if __name__ == "__main__":
    main()
