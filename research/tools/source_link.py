#!/usr/bin/env python3
"""source_link.py: resolve every unit, draft and data file with no original bytes, and count what's left.

PortReadiness step 2.4. source_symbols.py asks which of the original's symbols some
source defines; this asks a linker's question too: which symbols does some
source *refer to* that nothing defines (a view naming a global the game doesn't have, a
call to a function nobody has written), and which are defined twice.

Inputs, all compiled from source:
    units   build/reconstruction/units/*/compiled.o (run tools/reconstruct.py first)
    drafts  research/drafts/index.tsv, each compiled with its listed settings
    data    research/data/**/*.cpp, with the settings each names (check_data.py's rule)

A draft may be an open version of a function some unit's fragment also contains, so in
draft objects every symbol a unit defines is made weak (source_symbols.py counts these
the same way). What's left defined twice is a real duplicate: two units, two drafts, or a
unit and a data file.

Resolution is done from the objects' symbol and relocation tables, as a linker would:
GNU ld (build/tools/powerpc-eabi-ld) stops relocating at the first unresolvable small-data
relocation, so it can't give the whole count while any are left (tried 2026-11-11: it
stopped after the units starting with "A"). Symbols the link script defines (_stack_addr,
_ctors, ...) aren't counted. "fragment_local" is a symbol one object refers to that another defines file-locally: the
original file is split into fragments (units, drafts) and its statics are reached across
them with extern; merging a file's fragments resolves it. "stripped" is a symbol only a unit's
discarded functions refer to (its config's discarded_functions: the original's linker dead-stripped
them, so the reference never reaches the link); listed, not counted as undefined. "small_data" is a symbol one object reaches through r13/r2 (SDA21) that its defining
object put in a large-data section: the two disagree on its size or type.

    source_link.py [--jobs N]     resolve; write research/layout/source_link.tsv and print the counts
    source_link.py --list KIND    print the symbols of one kind (undefined, fragment_local, duplicate, small_data,
                                  no_compile)
"""
import argparse, collections, csv, glob, json, re, shutil, struct, subprocess, sys, tempfile
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from formats import Elf32  # noqa: E402

WORK = ROOT / "build/source_link"
OUT = ROOT / "research/layout/source_link.tsv"
DEFAULT_SETTINGS = "config/GMFE69/ConfigGC.json"


def compile_one(job):
    """(source, settings, object path) -> (source, error or None)."""
    source, settings, obj = job
    from setup_compiler import setup_compiler
    t = json.load(open(ROOT / settings))
    compiler, wrapper = setup_compiler(t["compiler_version"], "GC")
    src = ROOT / source
    suffixes = [src.suffix] + ([".cpp"] if src.suffix == ".c" else [])  # some C drafts need C++
    log = ""
    with tempfile.TemporaryDirectory() as work:
        for suffix in suffixes:
            probe = Path(work) / ("probe" + suffix)
            probe.write_text(src.read_text(errors="replace"))
            cmd = [str(wrapper), str(compiler / "mwcceppc.exe"), "-c", *t["compiler_flags"], "-I-",
                   f"-I{src.parent}", *[f"-I{ROOT / p}" for p in t.get("include_dirs", []) + ([] if "include" in t.get("include_dirs", []) else ["include"])], str(probe), "-o", "probe.o"]
            for _ in range(3):  # wibo occasionally fails under parallel load
                r = subprocess.run(cmd, cwd=work, capture_output=True, text=True)
                if r.returncode == 0:
                    shutil.copy(Path(work) / "probe.o", obj)
                    return source, None
                log = (r.stdout + r.stderr)[-300:]
                if "Error" in log:
                    break
    return source, log or "no object"


def defined_globals(path):
    elf = Elf32(Path(path).read_bytes())
    return {s["name"] for s in elf.symbols()
            if s["name"] and s["binding"] in (1, 2) and 0 < s["section"] < 0xFF00}


def compile_all(jobs):
    for d in ("drafts", "data"):
        (WORK / d).mkdir(parents=True, exist_ok=True)
    work = []
    for i, row in enumerate(csv.DictReader(open(ROOT / "research/drafts/index.tsv"), delimiter="\t")):
        work.append((row["draft"], row["compile_settings_from"], None))
    seen, todo = set(), []
    for source, settings, _ in work:
        if source not in seen:  # one draft file may hold several open functions
            seen.add(source)
            todo.append((source, settings, str(WORK / "drafts" / (Path(source).stem + ".o"))))
    for f in sorted(glob.glob(str(ROOT / "research/data/**/*.cpp"), recursive=True)):
        rel = str(Path(f).relative_to(ROOT))
        m = re.search(r"//\s*settings:\s*(\S+)", Path(f).read_text())
        todo.append((rel, m.group(1) if m else DEFAULT_SETTINGS,
                     str(WORK / "data" / (rel.replace("/", "_").replace("research_data_", "") + ".o"))))
    failed = {}
    with ProcessPoolExecutor(jobs) as pool:
        for source, err in pool.map(compile_one, todo):
            if err:
                failed[source] = err
    return [t for t in todo if t[0] not in failed], failed


SMALL = {".sdata", ".sbss", ".sdata2", ".sbss2"}
SDA21 = 109
# Defined by the link script, not by source.
LINKER = {"_stack_addr", "_stack_end", "_db_stack_addr", "_db_stack_end", "_ctors", "_dtors",
          "_bss_init_info", "_rom_copy_info", "_eti_init_info", "_SDA_BASE_", "_SDA2_BASE_",
          "__ArenaLo", "__ArenaHi"}  # _eti_init_info: MWLD's exception-table index, the tail of extabindex


def discarded(path):
    """A unit's functions that the original's linker dead-stripped (its config's
    discarded_functions and discarded_local_functions); empty for drafts and data."""
    if "/units/" not in str(path):
        return set()
    config = ROOT / "config/GMFE69" / (Path(path).parent.name + ".json")
    if not config.exists():
        return set()
    t = json.load(open(config))
    return set(t.get("discarded_functions", [])) | set(t.get("discarded_local_functions", []))


def live_references(elf, syms, dead):
    """Names referenced by some relocation outside the dead functions."""
    spans = collections.defaultdict(list)
    for s in syms:
        if s["name"] in dead and s["type"] == 2 and 0 < s["section"] < 0xFF00:
            spans[s["section"]].append((s["address"], s["address"] + s["size"]))
    live = set()
    for sec in elf.sections:
        if sec["type"] != 4:
            continue
        data = elf.contents(sec)
        for off in range(0, len(data), 12):
            where, info = struct.unpack_from(">II", data, off)
            if not any(lo <= where < hi for lo, hi in spans.get(sec["info"], ())):
                live.add(syms[info >> 8]["name"])
    return live


def table(path):
    """(defined {name: (binding, section)}, referenced names, names reached with SDA21,
    names defined file-locally, names referenced only from dead-stripped functions)."""
    elf = Elf32(Path(path).read_bytes())
    syms = list(elf.symbols())
    dead = discarded(path)
    defined, referenced, small, local = {}, set(), set(), set()
    for s in syms:
        if s["name"] and s["binding"] == 0 and 0 < s["section"] < 0xFF00 and s["type"] in (1, 2):
            local.add(re.sub(r"\$\d+$", "", s["name"]))
        if not s["name"] or s["binding"] not in (1, 2) or s["type"] == 4:
            continue
        if s["section"] == 0:
            referenced.add(s["name"])
        elif s["section"] < 0xFF00:
            defined[s["name"]] = (s["binding"], elf.sections[s["section"]]["name"])
        elif s["section"] == 0xFFF2:  # common
            defined[s["name"]] = (2, ".bss" if s["size"] > 8 else ".sbss")
    for sec in elf.sections:
        if sec["type"] == 4:
            data = elf.contents(sec)
            for off in range(0, len(data), 12):
                info = struct.unpack_from(">I", data, off + 4)[0]
                if info & 255 == SDA21:
                    sym = syms[info >> 8]
                    if sym["name"] and sym["binding"] in (1, 2):
                        small.add(sym["name"])
    stripped = set()
    if dead:
        stripped = referenced - live_references(elf, syms, dead)
        referenced -= stripped
    return defined, referenced, small, local, stripped


def resolve(units, drafts, data):
    """What a linker would report, from symbol and relocation tables.

    GNU ld stops relocating at the first unresolvable SDA21 relocation, so it can't give
    the whole count while any are left; this can. In drafts, a symbol some unit defines
    counts as weak (a draft may be an open copy of code a unit fragment also holds)."""
    tables = {o: table(o) for o in units + drafts + data}
    unit_defs = set().union(*(tables[o][0] for o in units))
    strong, any_def, where = collections.defaultdict(list), {}, {}
    locals_ = set().union(*(t[3] for t in tables.values()))
    for o, (defined, _, _, _, _) in tables.items():
        for name, (binding, section) in defined.items():
            weak = binding == 2 or (o in drafts and name in unit_defs)
            any_def.setdefault(name, section)
            if not weak:
                strong[name].append(o)
                where[name] = section
    found = collections.defaultdict(lambda: collections.defaultdict(set))
    for o, (_, referenced, small, _, stripped) in tables.items():
        for name in stripped:
            if name not in any_def and name not in LINKER:
                # Reached only from functions the original's linker dead-stripped (the unit's
                # discarded functions, e.g. block headers, unused SDK variants): MWLD drops
                # them and the reference with them. Listed, not counted as undefined.
                found["stripped"][name].add(o)
        for name in referenced:
            if name not in any_def and name not in LINKER:
                # Static in another fragment of the same original file: resolves once the
                # fragments are merged into one file, not a missing definition.
                found["fragment_local" if name in locals_ else "undefined"][name].add(o)
        for name in small:
            section = where.get(name, any_def.get(name))
            if section and section not in SMALL:
                found["small_data"][name].add(o)
    for name, objs in strong.items():
        if len(objs) > 1:
            found["duplicate"][name] |= set(objs)
    return found


def label(path):
    p = Path(path)
    if "/units/" in path:
        return "unit:" + p.parent.name
    return ("draft:" if "/drafts/" in path else "data:") + p.name.replace(".weak.o", "").replace(".o", "")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--list")
    a = ap.parse_args()
    if a.list:
        for row in csv.reader(open(OUT), delimiter="\t"):
            if row[0] == a.list:
                print("\t".join(row[1:]))
        return
    units = sorted(glob.glob(str(ROOT / "build/reconstruction/units/*/compiled.o")))
    if not units:
        raise SystemExit("no unit objects: run tools/reconstruct.py first")
    built, failed = compile_all(a.jobs)
    drafts = [o for s, _, o in built if s.startswith("research/drafts/")]
    data = [o for s, _, o in built if s.startswith("research/data/")]
    found = resolve(units, drafts, data)
    OUT.parent.mkdir(parents=True, exist_ok=True)
    with open(OUT, "w") as f:
        f.write("kind\tsymbol\tobjects\n")
        for kind in ("undefined", "fragment_local", "duplicate", "small_data", "stripped"):
            for sym in sorted(found[kind]):
                objs = sorted({label(o) for o in found[kind][sym]})
                f.write(f"{kind}\t{sym}\t{' '.join(objs)[:300]}\n")
        for source, err in sorted(failed.items()):
            f.write(f"no_compile\t{source}\t{' '.join(err.split())[:200]}\n")
    print(f"objects: {len(units)} units, {len(drafts)} drafts, {len(data)} data files; {len(failed)} failed to compile")
    for kind in ("undefined", "fragment_local", "duplicate", "small_data", "stripped"):
        print(f"{kind:15} {len(found[kind]):6} symbols")
    print(f"wrote {OUT.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
