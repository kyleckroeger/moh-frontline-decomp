#!/usr/bin/env python3
"""pure_link.py: link the pure-source image with GNU ld (endgame plan W1.2, PortReadiness step 2.4).

source_link.py counts what a link would report. This performs the link: every unit
(build/reconstruction/units/*/compiled.o, the compiler's own output), every draft and
every data file (as source_link.py compiles them, with the generated vtable stopgap), with
no original bytes, at a fresh layout. Before linking, it makes the fragmentation of
original files work as one file would:

    fragment-local  a static one fragment defines and another reaches with extern: every
                    copy the same original file's fragments define is renamed name.FILE and
                    made global, weak but one, so they merge into one object as in the
                    original file, and the reaching fragments refer to it. Statics of the
                    same name in other files stay apart.
    duplicates      the same global in several objects (inline copies, file statics
                    several fragments define, .bss declared in a unit and a data file):
                    one copy stays, the others are made weak.
    drafts          a draft file holds a whole fragment; only the functions it is the draft
                    of (research/drafts/index.tsv) stay strong, the rest are weak.
    stripped        names only dead-stripped code reaches: a local definition is made
                    global, else a trap stub stands in.

The linker script places the sections in the original's order from 0x80007100 and builds
the tables MWLD makes: _rom_copy_info and _bss_init_info (at the end of .init, as in the
original), _eti_init_info (at the end of extabindex), _ctors/_dtors (zero-terminated),
_SDA_BASE_/_SDA2_BASE_ (+0x8000), the 64 KB stack, the debugger stack and the arena.

    pure_link.py [--jobs N]     compile (as source_link.py does), link, write
                                build/pure/Moh2RelGC_pure.elf and build/pure/link.log
Run tools/reconstruct.py first. Prints the link result and the decisions it made.
"""
import argparse, collections, csv, json, re, shutil, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT / "research/tools"), str(ROOT / "tools")]
import source_link as sl  # noqa: E402
from formats import Elf32  # noqa: E402

OUT = ROOT / "build/pure"
LD = ROOT / "build/tools/powerpc-eabi-ld"
OBJCOPY = ROOT / "build/tools/powerpc-eabi-objcopy"

SCRIPT = """
ENTRY(__start)
PHDRS
{
  p_init PT_LOAD;
  p_extab PT_LOAD;
  p_extabindex PT_LOAD;
  p_text PT_LOAD;
  p_ctors PT_LOAD;
  p_dtors PT_LOAD;
  p_rodata PT_LOAD;
  p_data PT_LOAD;
  p_bss PT_LOAD;
  p_sdata PT_LOAD;
  p_sbss PT_LOAD;
  p_sdata2 PT_LOAD;
  p_sbss2 PT_LOAD;
}
SECTIONS
{
  . = 0x80007100;
  .init : {
    *(.init)
    . = ALIGN(4);
    _rom_copy_info = .;
    LONG(ADDR(.init)) LONG(ADDR(.init)) LONG(SIZEOF(.init))
    LONG(ADDR(extab)) LONG(ADDR(extab)) LONG(SIZEOF(extab))
    LONG(ADDR(extabindex)) LONG(ADDR(extabindex)) LONG(SIZEOF(extabindex))
    LONG(ADDR(.text)) LONG(ADDR(.text)) LONG(SIZEOF(.text))
    LONG(ADDR(.ctors)) LONG(ADDR(.ctors)) LONG(SIZEOF(.ctors))
    LONG(ADDR(.dtors)) LONG(ADDR(.dtors)) LONG(SIZEOF(.dtors))
    LONG(ADDR(.rodata)) LONG(ADDR(.rodata)) LONG(SIZEOF(.rodata))
    LONG(ADDR(.data)) LONG(ADDR(.data)) LONG(SIZEOF(.data))
    LONG(ADDR(.sdata)) LONG(ADDR(.sdata)) LONG(SIZEOF(.sdata))
    LONG(ADDR(.sdata2)) LONG(ADDR(.sdata2)) LONG(SIZEOF(.sdata2))
    LONG(0) LONG(0) LONG(0)
    _bss_init_info = .;
    LONG(ADDR(.bss)) LONG(SIZEOF(.bss))
    LONG(ADDR(.sbss)) LONG(SIZEOF(.sbss))
    LONG(ADDR(.sbss2)) LONG(SIZEOF(.sbss2))
    LONG(0) LONG(0)
  } :p_init
  . = ALIGN(32);
  extab : { *(extab) } :p_extab
  . = ALIGN(32);
  extabindex : {
    *(extabindex)
    _eti_init_info = .;
    LONG(ADDR(extabindex)) LONG(_eti_init_info) LONG(ADDR(.text)) LONG(SIZEOF(.text))
    LONG(0) LONG(0) LONG(0) LONG(0)
  } :p_extabindex
  . = ALIGN(32);
  .text : { *(.text) } :p_text
  . = ALIGN(32);
  .ctors : { _ctors = .; *(.ctors) LONG(0) } :p_ctors
  .dtors : { _dtors = .; *(.dtors) LONG(0) } :p_dtors
  . = ALIGN(32);
  .rodata : { *(.rodata) *(.rodata.*) } :p_rodata
  . = ALIGN(32);
  .data : { *(.data) *(.data.*) } :p_data
  . = ALIGN(32);
  .bss : { *(.bss) *(COMMON) } :p_bss
  . = ALIGN(32);
  .sdata : { *(.sdata) } :p_sdata
  .sbss : { *(.sbss) *(.scommon) } :p_sbss
  . = ALIGN(32);
  .sdata2 : { *(.sdata2) } :p_sdata2
  .sbss2 : { *(.sbss2) } :p_sbss2
  _SDA_BASE_ = ADDR(.sdata) + 0x8000;
  _SDA2_BASE_ = ADDR(.sdata2) + 0x8000;
  . = ALIGN(8);
  _stack_end = .;
  . = . + 0x10000;
  _stack_addr = .;
  . = . + 0x2000;
  _db_stack_addr = .;
  _db_stack_end = _stack_addr;
  . = ALIGN(32) + 0x10;
  __ArenaLo = .;
  __ArenaHi = 0x81700000;
  /DISCARD/ : { *(.comment) }
}
"""


def own_functions():
    """draft file -> the functions it is the draft of."""
    own = collections.defaultdict(set)
    for row in csv.DictReader(open(ROOT / "research/drafts/index.tsv"), delimiter="\t"):
        own[str(Path(row["draft"]).stem)].add(row["function"])
    return own


def symbols(path):
    return list(Elf32(Path(path).read_bytes()).symbols())


def plan(units, drafts, data):
    found = sl.resolve(units, drafts, data)
    tables = {o: sl.table(o) for o in units + drafts + data}
    unit_defs = set().union(*(tables[o][0] for o in units))
    own = own_functions()
    weaken = collections.defaultdict(set)
    globalize = collections.defaultdict(set)
    notes = []
    # Drafts: only their own functions stay strong.
    for o in drafts:
        stem = Path(o).stem.replace(".weak", "")
        for name, (binding, _) in tables[o][0].items():
            if binding == 1 and (name in unit_defs or name not in own.get(stem, ())):
                weaken[o].add(name)
    # Strong duplicates that remain: keep one (a unit before a data file before a draft).
    strong = collections.defaultdict(list)
    for o, t in tables.items():
        for name, (binding, _) in t[0].items():
            if binding == 1 and name not in weaken[o]:
                strong[name].append(o)
    rank = lambda o: (0 if o in units else 1 if o in data else 2, o)
    for name, objs in strong.items():
        if len(objs) > 1:
            objs.sort(key=rank)
            for o in objs[1:]:
                weaken[o].add(name)
            notes.append(f"duplicate {name}: kept {sl.label(objs[0])}, weakened {len(objs) - 1}")
    # Fragment-local reach-ins: a static of one original file, reached from another fragment
    # of that file. Every copy the file's fragments define gets one file-unique global name
    # (name.FILE), weak except one (a data file's, else a unit's), so they merge into one
    # object as in the original file; the reaching fragments refer to that name.
    origin = original_files(units, drafts, data)
    rename = collections.defaultdict(dict)
    for name, users in found["fragment_local"].items():
        by_file = collections.defaultdict(set)
        for u in users:
            by_file[origin.get(u)].add(u)
        for f, us in by_file.items():
            definers = []
            for o in units + data + drafts:
                if origin.get(o) != f:
                    continue
                for sym in symbols(o):
                    if sym["binding"] == 0 and 0 < sym["section"] < 0xFF00 and sym["type"] in (1, 2) \
                            and re.sub(r"\$\d+$", "", sym["name"]) == name:
                        definers.append((o, sym["name"]))
            if not definers:
                # A draft's compile settings may come from a neighbouring file's config: when
                # all definers belong to one file, that file's copy is meant.
                every = [(o, sym["name"]) for o in units + data + drafts for sym in symbols(o)
                         if sym["binding"] == 0 and 0 < sym["section"] < 0xFF00 and sym["type"] in (1, 2)
                         and re.sub(r"\$\d+$", "", sym["name"]) == name]
                files = {origin.get(o) for o, _ in every}
                if len(files) != 1:
                    notes.append(f"fragment_local {name} ({f}): no definer in that file")
                    continue
                f = files.pop()
                definers = every
                notes.append(f"fragment_local {name}: reached from another file's object, defined in {f}")
            definers.sort(key=lambda d: rank(d[0]))
            unique = f"{name}.{f}"
            for i, (o, real) in enumerate(definers):
                rename[o][real] = unique
                globalize[o].add(unique)
                if i:
                    weaken[o].add(unique)
            for u in us:
                rename[u][name] = unique
            if len({d[0] for d in definers}) > 1:
                notes.append(f"fragment_local {name} ({f}): {len(definers)} copies merged, kept {sl.label(definers[0][0])}")
    # Names only dead-stripped code reaches: where an object defines one locally (__start.c's
    # static __init_registers, reached by name from its inline asm), that copy is made global;
    # the rest get a trap stub (build/pure/stripped.s), never reached at run time.
    stripped = []
    for name in sorted(found["stripped"]):
        local = [o for o in units + data for s in symbols(o)
                 if s["binding"] == 0 and 0 < s["section"] < 0xFF00 and s["name"] == name]
        if local:
            globalize[local[0]].add(name)
            notes.append(f"stripped {name}: globalized the local copy in {sl.label(local[0])}")
        else:
            stripped.append(name)
    return weaken, globalize, rename, stripped, notes


def original_files(units, drafts, data):
    """object -> its original file name (units and drafts from their configs; data files
    from their file name)."""
    origin = {}
    for o in units:
        c = json.load(open(ROOT / "config/GMFE69" / (Path(o).parent.name + ".json")))
        origin[o] = c.get("original_file") or Path(c["source"]).name
    settings = {}
    for row in csv.DictReader(open(ROOT / "research/drafts/index.tsv"), delimiter="\t"):
        settings[Path(row["draft"]).stem] = row["compile_settings_from"]
    for o in drafts:
        cfg = settings.get(Path(o).stem.replace(".weak", ""))
        if cfg:
            origin[o] = json.load(open(ROOT / cfg)).get("original_file")
    for o in data:
        stem = Path(o).name.replace(".cpp.o", "").replace(".o", "")
        origin[o] = stem.split("_", 1)[1] + ".cpp" if stem.startswith(("bss_", "init_")) else stem + ".cpp"
    return origin


def prepare(objs, weaken, globalize, rename):
    """Copy each object with its symbol changes; return the paths to link."""
    work = OUT / "objs"
    shutil.rmtree(work, ignore_errors=True)
    work.mkdir(parents=True)
    out = []
    for i, o in enumerate(objs):
        dst = work / f"{i:04d}_{Path(o).parent.name if '/units/' in o else Path(o).stem}.o"
        args = []
        src = o
        if rename.get(o):
            first = str(dst) + ".r"
            subprocess.run([str(OBJCOPY), *[f"--redefine-sym={a}={b}" for a, b in sorted(rename[o].items())], o, first],
                           check=True)
            src = first
        for name in sorted(globalize.get(o, ())):
            args.append(f"--globalize-symbol={name}")
        if args:
            subprocess.run([str(OBJCOPY), *args, src, str(dst) + ".g"], check=True)
            src = str(dst) + ".g"
        weak = [f"--weaken-symbol={name}" for name in sorted(weaken.get(o, ()))]
        if weak:  # a separate pass: objcopy weakens before it globalizes
            subprocess.run([str(OBJCOPY), *weak, src, str(dst)], check=True)
        else:
            shutil.copy(src, dst)
        out.append(str(dst))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--jobs", type=int, default=4)
    a = ap.parse_args()
    units = sorted(str(p) for p in (ROOT / "build/reconstruction/units").glob("*/compiled.o")
                   if json_exists(p))
    built, failed = sl.compile_all(a.jobs)
    if failed:
        print("failed to compile:", ", ".join(sorted(failed)))
    drafts = [o for s, _, o in built if s.startswith("research/drafts/")]
    data = [o for s, _, o in built if s.startswith(("research/data/", "research/data_stopgap/"))]
    weaken, globalize, rename, stripped, notes = plan(units, drafts, data)
    OUT.mkdir(parents=True, exist_ok=True)
    objs = prepare(units + data + drafts, weaken, globalize, rename)
    (OUT / "link.ld").write_text(SCRIPT)
    (OUT / "objects.rsp").write_text("\n".join(objs) + "\n")
    stub = OUT / "stripped.s"
    stub.write_text(".text\n" + "".join(f'.globl "{n}"\n"{n}":\n  trap\n' for n in stripped))
    subprocess.run([str(ROOT / "build/tools/powerpc-eabi-as"), "-mgekko", "-o", str(OUT / "stripped.o"), str(stub)],
                   check=True)
    cmd = [str(LD), "-T", str(OUT / "link.ld"), "-o", str(OUT / "Moh2RelGC_pure.elf"),
           "--no-warn-mismatch", "-z", "max-page-size=0x20", f"@{OUT / 'objects.rsp'}", str(OUT / "stripped.o")]
    r = subprocess.run(cmd, capture_output=True, text=True)
    (OUT / "link.log").write_text(r.stdout + r.stderr)
    (OUT / "decisions.txt").write_text("\n".join(notes) + "\n")
    errors = [l for l in (r.stdout + r.stderr).splitlines() if "undefined reference" in l or "error" in l.lower()]
    kinds = collections.Counter(re.sub(r".*?: ", "", l).split("`")[0].strip() for l in errors)
    print(f"objects: {len(units)} units, {len(data)} data, {len(drafts)} drafts; "
          f"weakened {sum(map(len, weaken.values()))}, globalized {sum(map(len, globalize.values()))}, "
          f"stripped defined 0: {len(stripped)}")
    print(f"link exit {r.returncode}, {len(errors)} error lines")
    for k, n in kinds.most_common(8):
        print(f"  {n:5} {k}")
    print(f"notes: {len(notes)} (build/pure/decisions.txt); log build/pure/link.log")
    return r.returncode


def json_exists(obj):
    return (ROOT / "config/GMFE69" / (obj.parent.name + ".json")).exists()


if __name__ == "__main__":
    sys.exit(main())
