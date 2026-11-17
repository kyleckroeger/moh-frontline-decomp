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
    anchors         MWCC reaches a file's statics through a section anchor (`...bss.0` +
                    offset); those relocations are pointed at the static's own symbol first,
                    so that merging the static moves them too.
    ctors           a draft compiles its fragment's whole file, static initialiser included: only
                    the drafts of a __sinit function keep their .ctors entry.
    weak copies     a unit's weak functions placed at their original address (the verified
                    copies) are made strong, so another fragment's copy can't win.
    SDA21           MWCC records R_PPC_EMB_SDA21 at the instruction's low halfword; GNU ld
                    wants the instruction, so every such relocation moves back 2 bytes.
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
import argparse, collections, csv, json, re, shutil, struct, subprocess, sys
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
    LONG(_bss_start) LONG(_bss_end - _bss_start)
    LONG(_sbss_start) LONG(_sbss_end - _sbss_start)
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
  .text : { *(.text) _probe_space = .; . = . + @PROBE_SPACE@; } :p_text
  . = ALIGN(32);
  .ctors : { _ctors = .; *(.ctors) LONG(0) } :p_ctors
  . = ALIGN(32);
  .dtors : { _dtors = .; *(.dtors) LONG(0) } :p_dtors
  . = ALIGN(32);
  .rodata : { *(.rodata) *(.rodata.*) } :p_rodata
  . = ALIGN(32);
  .data : { *(.data) *(.data.*) } :p_data
  . = ALIGN(32);
  _bss_start = .;
@BSS_MAPPED@
  . = _bss_start + 0x1b9110;
  .bss : { *(.bss) *(COMMON) } :p_bss
  _bss_end = .;
  . = ALIGN(32);
  .sdata : { *(.sdata) } :p_sdata
  . = ALIGN(32);
  _sbss_start = .;
@SBSS_MAPPED@
  . = _sbss_start + 0xe64;
  .sbss : { *(.sbss) *(.scommon) } :p_sbss
  _sbss_end = .;
  . = ALIGN(32);
  .sdata2 : { *(.sdata2) } :p_sdata2
  . = ALIGN(32);
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


def plan(units, drafts, data, probes=(), probe_origin=None):
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
    # One original file's writable statics. Its fragments (units, drafts, data files) each
    # compile their own copy of a file static they use; in the original there is one. Every
    # writable data object (.data/.sdata/.bss/.sbss) that several objects of one file define
    # becomes one: renamed name.FILE (or kept as name when some copy is global), global, and
    # weak in all but one copy (one with the original's size, then a data file's, a unit's,
    # a draft's). Statics reached from another fragment with extern are
    # renamed in the reaching objects too. Statics of the same name in other files stay apart.
    origin = original_files(units, drafts, data)
    origin.update(probe_origin or {})
    rename = collections.defaultdict(dict)
    WRITABLE = {".data", ".sdata", ".bss", ".sbss"}
    defs = collections.defaultdict(list)      # (file, base name) -> [(object, real name, binding)]
    for o in units + data + drafts + list(probes):
        elf = Elf32(Path(o).read_bytes())
        for sym in elf.symbols():
            if sym["type"] != 1 or not (0 < sym["section"] < 0xFF00) or not sym["name"] or sym["name"].startswith(("@", "...")):
                continue
            if elf.sections[sym["section"]]["name"] not in WRITABLE or sym["binding"] not in (0, 1, 2):
                continue
            if "$" in sym["name"]:
                continue  # a function's own static: each function has its own
            base = sym["name"]
            defs[(origin.get(o), base)].append((o, sym["name"], sym["binding"]))
    orig_sizes = collections.defaultdict(set)
    for sym in Elf32((ROOT / "orig/GMFE69/Moh2RelGC.elf").read_bytes()).symbols():
        if sym["type"] == 1 and sym["name"]:
            orig_sizes[sym["name"]].add(sym["size"])
    sizes = {}
    for (f, base), copies in defs.items():
        for o, real, _ in copies:
            sym = next((x for x in symbols(o) if x["name"] == real and x["type"] == 1 and 0 < x["section"] < 0xFF00), None)
            sizes[(o, real)] = sym["size"] if sym else -1
    # The copy that stays: one with the original's size (a draft's view may be larger or smaller),
    # then a data file's, then a unit's (verified code), then a draft's.
    unify_rank = lambda d: (d[0] in probes,
                            sizes.get((d[0], d[1])) not in orig_sizes.get(re.sub(r"\$\d+$", "", d[1]), {sizes.get((d[0], d[1]))}),
                            0 if d[0] in data else 1 if d[0] in units else 2, d[0])
    merged = {}
    for (f, base), copies in defs.items():
        if f is None or len({c[0] for c in copies}) < 2 or not any(c[2] == 0 for c in copies):
            continue  # one copy, or all global (the duplicate rule above handles those)
        global_copy = any(c[2] in (1, 2) for c in copies)
        unique = base if global_copy else f"{base}.{f}"
        copies.sort(key=unify_rank)
        for i, (o, real, binding) in enumerate(copies):
            if real != unique:
                rename[o][real] = unique
            globalize[o].add(unique)
            weaken[o].discard(unique)
            if i:
                weaken[o].add(unique)
        merged[(f, base)] = unique
        notes.append(f"file static {base} ({f}): {len(copies)} copies merged, kept {sl.label(copies[0][0])}")
    for name, users in found["fragment_local"].items():
        for u in users:
            f = origin.get(u)
            unique = merged.get((f, name))
            if unique is None:
                cands = [(ff, uq) for (ff, bn), uq in merged.items() if bn == name]
                if len(cands) == 1:
                    unique = cands[0][1]
            if unique is None:
                # a static function or a single copy: make that copy global under name.FILE
                every = [(o, sym["name"]) for o in units + data + drafts for sym in symbols(o)
                         if sym["binding"] == 0 and 0 < sym["section"] < 0xFF00 and sym["type"] in (1, 2)
                         and re.sub(r"\$\d+$", "", sym["name"]) == name
                         and origin.get(o) in (f, None)]
                if not every:
                    every = [(o, sym["name"]) for o in units + data + drafts for sym in symbols(o)
                             if sym["binding"] == 0 and 0 < sym["section"] < 0xFF00 and sym["type"] in (1, 2)
                             and re.sub(r"\$\d+$", "", sym["name"]) == name]
                    if len({origin.get(o) for o, _ in every}) != 1:
                        notes.append(f"fragment_local {name} ({f}): no definer")
                        continue
                ff = origin.get(every[0][0])
                unique = f"{name}.{ff}"
                every.sort(key=lambda d: rank(d[0]))
                for i, (o, real) in enumerate(every):
                    rename[o][real] = unique
                    globalize[o].add(unique)
                    if i:
                        weaken[o].add(unique)
                merged[(ff, name)] = unique
            if unique != name:
                rename[u][name] = unique
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
    settings, address = {}, {}
    for row in csv.DictReader(open(ROOT / "research/drafts/index.tsv"), delimiter="\t"):
        settings[Path(row["draft"]).stem] = row["compile_settings_from"]
        if row["address"]:
            address.setdefault(Path(row["draft"]).stem, int(row["address"], 16))
    # A draft's file when its settings' config doesn't name one: the unit just below its address.
    starts = []
    for o in units:
        c = json.load(open(ROOT / "config/GMFE69" / (Path(o).parent.name + ".json")))
        for sec in c["sections"]:
            if sec["name"] == ".text" and c.get("original_file"):
                starts.append((int(sec["address"], 16), c["original_file"]))
    starts.sort()
    for o in drafts:
        stem = Path(o).stem.replace(".weak", "")
        cfg = settings.get(stem)
        if cfg:
            origin[o] = json.load(open(ROOT / cfg)).get("original_file")
        if not origin.get(o) and stem in address:
            below = [f for a, f in starts if a <= address[stem]]
            if below:
                origin[o] = below[-1]
    for o in data:
        stem = Path(o).name.replace(".cpp.o", "").replace(".o", "")
        origin[o] = stem.split("_", 1)[1] + ".cpp" if stem.startswith(("bss_", "init_")) else stem + ".cpp"
    return origin


def fix_sda21(path):
    """MWCC records R_PPC_EMB_SDA21 at the instruction's low halfword (+2); GNU ld applies it
    to the whole instruction at r_offset. Move every SDA21 relocation back to the instruction
    start, or each small-data access writes two bytes into the next instruction."""
    data = bytearray(Path(path).read_bytes())
    elf = Elf32(bytes(data))
    for sec in elf.sections:
        if sec["type"] != 4:
            continue
        for off in range(sec["offset"], sec["offset"] + sec["size"], 12):
            where, info = struct.unpack_from(">II", data, off)
            if info & 255 == 109 and where % 4 == 2:
                struct.pack_into(">I", data, off, where - 2)
    Path(path).write_bytes(data)


def anchor_to_objects(path, wanted):
    """MWCC reaches a file's statics through a section anchor (`...bss.0` + offset). Point each
    such relocation that lands inside one of the `wanted` objects at that object's own symbol
    instead (addend made relative to it), so renaming or merging the object moves the reference."""
    data = bytearray(Path(path).read_bytes())
    elf = Elf32(bytes(data))
    syms = list(elf.symbols())
    symtab = next(s for s in elf.sections if s["type"] == 2)
    index = {}
    for i, sym in enumerate(syms):
        if sym["name"] in wanted and sym["type"] == 1 and 0 < sym["section"] < 0xFF00:
            index.setdefault(sym["section"], []).append((sym["address"], sym["address"] + max(sym["size"], 1), i))
    moved = 0
    for sec in elf.sections:
        if sec["type"] != 4:
            continue
        for off in range(sec["offset"], sec["offset"] + sec["size"], 12):
            where, info, addend = struct.unpack_from(">IIi", data, off)
            target = syms[info >> 8]
            if not target["name"].startswith("...") or target["section"] not in index:
                continue
            at = target["address"] + addend
            for lo, hi, i in index[target["section"]]:
                if lo <= at < hi:
                    struct.pack_into(">Ii", data, off + 4, (i << 8) | (info & 255), at - lo)
                    moved += 1
                    break
    Path(path).write_bytes(data)
    return moved


# .sbss stays merged symbol by symbol: GNU ld wants SDA21 targets in an output section named
# exactly .sbss, and small statics are reached by their own symbols, not through an anchor.
BSS_ORIGINAL = {".bss": (0x80194840, "_bss_start", "p_bss")}


def original_statics():
    """(file, name) -> address of every writable data object in the original (locals by the
    STT_FILE symbol before them), and name -> address of the globals."""
    by_file, globals_ = {}, {}
    current = None
    for sym in Elf32((ROOT / "orig/GMFE69/Moh2RelGC.elf").read_bytes()).symbols():
        if sym["type"] == 4:
            current = sym["name"]
        elif sym["type"] == 1 and sym["name"] and 0 < sym["section"] < 0xFF00:
            if sym["binding"] == 0:
                by_file[(current, sym["name"])] = sym["address"]
            else:
                globals_[sym["name"]] = sym["address"]
    return by_file, globals_


def bss_placement(paths, origin):
    """Place each object's .bss/.sbss so its named statics keep the original file's layout
    (shifted into this image's .bss/.sbss region by a constant). A file's fragments then overlay
    and share their statics, as the original file's code does: MWCC reaches statics from one
    section anchor plus offsets, so separate copies can't be merged symbol by symbol."""
    by_file, globals_ = original_statics()
    placed = collections.defaultdict(list)   # section -> [(object path, offset in region)]
    for path in paths:
        elf = Elf32(Path(path).read_bytes())
        f = origin.get(path)
        for sec in elf.sections:
            if sec["name"] not in BSS_ORIGINAL or not sec["size"]:
                continue
            base, _, _ = BSS_ORIGINAL[sec["name"]]
            deltas = set()
            for sym in elf.symbols():
                if sym["section"] != sec["index"] or sym["type"] != 1 or not sym["name"] or sym["name"].startswith(("@", "...")) or "$" in sym["name"]:
                    continue
                addr = by_file.get((f, sym["name"])) if sym["binding"] == 0 else globals_.get(sym["name"])
                if addr is None:
                    addr = globals_.get(sym["name"]) or by_file.get((f, sym["name"]))
                if addr is not None:
                    deltas.add(addr - sym["address"])
            if len(deltas) == 1:
                start = deltas.pop()
                if start >= base and start + sec["size"] <= base + {".bss": 0x1b9110, ".sbss": 0xe64}[sec["name"]]:
                    placed[sec["name"]].append((path, start - base))
    return placed


def verified_weak_copies(units):
    """unit object -> the weak functions it places at their original address (its config's
    functions with weak binding that aren't listed as weak or file duplicates): the copies the
    verified build checked. In the pure link these become strong, so a fragment's copy compiled
    from another view (GetWorldLinearVelocity: 20 bytes in some, 44 in others) can't win."""
    out = {}
    for o in units:
        c = json.load(open(ROOT / "config/GMFE69" / (Path(o).parent.name + ".json")))
        dup = set(c.get("weak_duplicates", [])) | set(c.get("file_duplicates", []))
        names = {f["name"] for f in c.get("functions", []) if f.get("binding") == 2 and f["name"] not in dup}
        if names:
            out[o] = names
    return out


def strengthen(path, names):
    """Turn these weak definitions into global ones (the symbol table's binding nibble)."""
    data = bytearray(Path(path).read_bytes())
    elf = Elf32(bytes(data))
    for sec in elf.sections:
        if sec["type"] != 2:
            continue
        strings = elf.contents(elf.sections[sec["link"]])
        for off in range(sec["offset"], sec["offset"] + sec["size"], 16):
            name_off, info = struct.unpack_from(">I", data, off)[0], data[off + 12]
            end = strings.index(b"\0", name_off)
            if info >> 4 == 2 and strings[name_off:end].decode("latin-1") in names and struct.unpack_from(">H", data, off + 14)[0]:
                data[off + 12] = (1 << 4) | (info & 15)
    Path(path).write_bytes(data)


DROP_CTORS = set()


def drafts_without_sinit(drafts):
    """Drafts that aren't the draft of a file's static initialiser (research/drafts/index.tsv)."""
    own = own_functions()
    return {o for o in drafts if not any(f.startswith("__sinit_") for f in own.get(Path(o).stem.replace(".weak", ""), ()))}


_ORIGINAL_DATA = None


def original_data_addresses():
    """(name, size) -> address for data objects that occur once in the original."""
    global _ORIGINAL_DATA
    if _ORIGINAL_DATA is None:
        seen = collections.Counter()
        addr = {}
        for sym in Elf32((ROOT / "orig/GMFE69/Moh2RelGC.elf").read_bytes()).symbols():
            if sym["type"] == 1 and sym["name"] and sym["size"]:
                key = (sym["name"], sym["size"])
                seen[key] += 1
                addr[key] = sym["address"]
        _ORIGINAL_DATA = {k: a for k, a in addr.items() if seen[k] == 1}
    return _ORIGINAL_DATA


DATA_SECTIONS = (".data", ".rodata", ".bss", ".sdata", ".sdata2", ".sbss", ".sbss2")


def align_like_original(path):
    """Give a data section 32-byte alignment when an object in it (32 bytes or more) was 32-byte
    aligned in the original and sits at a multiple of 32 in the section. DMA (ARAM, GX textures
    and vertex arrays, the DSP) ignores the low 5 bits of an address, and the original linker
    32-aligned some buffers by link order alone (g_frame_cache: route 1_2's animation reads
    landed 16 bytes early)."""
    data = bytearray(Path(path).read_bytes())
    elf = Elf32(bytes(data))
    orig = original_data_addresses()
    want = set()
    for sym in elf.symbols():
        if sym["type"] != 1 or not sym["name"] or sym["size"] < 32 or sym["address"] % 32:
            continue
        a = orig.get((sym["name"], sym["size"]))
        if a is not None and a % 32 == 0:
            want.add(sym["section"])
    shoff = struct.unpack_from(">I", data, 32)[0]
    changed = 0
    for sec in elf.sections:
        if sec["index"] in want and sec["name"].startswith(DATA_SECTIONS) and sec["alignment"] < 32:
            struct.pack_into(">I", data, shoff + sec["index"] * 40 + 32, 32)
            changed += 1
    if changed:
        Path(path).write_bytes(data)
    return changed


def prepare(objs, weaken, globalize, rename, strong=None):
    """Copy each object with its symbol changes; return the paths to link."""
    work = OUT / "objs"
    shutil.rmtree(work, ignore_errors=True)
    work.mkdir(parents=True)
    out = []
    for i, o in enumerate(objs):
        dst = work / f"{i:04d}_{Path(o).parent.name if '/units/' in o else Path(o).stem}.o"
        args = []
        src = o
        if rename.get(o) or globalize.get(o):
            anchored = str(dst) + ".a"
            shutil.copy(o, anchored)
            anchor_to_objects(anchored, set(rename.get(o, {})) | set(globalize.get(o, ())))
            src = anchored
        if rename.get(o):
            first = str(dst) + ".r"
            subprocess.run([str(OBJCOPY), *[f"--redefine-sym={a}={b}" for a, b in sorted(rename[o].items())], src, first],
                           check=True)
            src = first
        for name in sorted(globalize.get(o, ())):
            args.append(f"--globalize-symbol={name}")
        if args:
            subprocess.run([str(OBJCOPY), *args, src, str(dst) + ".g"], check=True)
            src = str(dst) + ".g"
        if o in DROP_CTORS:
            # A draft holds a whole fragment, so it compiles its file's static initialiser again;
            # the units already run it. Only a draft of the initialiser itself keeps .ctors.
            subprocess.run([str(OBJCOPY), "-R", ".ctors", "-R", ".rela.ctors", src, str(dst) + ".c"], check=True)
            src = str(dst) + ".c"
        weak = [f"--weaken-symbol={name}" for name in sorted(weaken.get(o, ()))]
        if weak:  # a separate pass: objcopy weakens before it globalizes
            subprocess.run([str(OBJCOPY), *weak, src, str(dst)], check=True)
        else:
            shutil.copy(src, dst)
        fix_sda21(dst)
        align_like_original(dst)
        if strong and strong.get(o):
            strengthen(dst, strong[o] - weaken.get(o, set()))
        out.append(str(dst))
    return out


def vtable_mismatches(elf_path):
    """Names of the linked __vt__ tables whose words differ from the original's
    (research/layout/vtables.tsv): a fragment's reduced class view can emit a weak copy with
    other slots, and the linker keeps whichever weak copy it meets first."""
    elf = Elf32(Path(elf_path).read_bytes())
    syms = list(elf.symbols())
    names = collections.defaultdict(set)
    for s in syms:
        if s["type"] in (1, 2) and s["name"]:
            names[s["address"]].add(s["name"])
    want = collections.defaultdict(dict)
    for r in csv.DictReader(open(ROOT / "research/layout/vtables.tsv"), delimiter="\t"):
        if r["role"] == "slot":  # the RTTI and offset words don't steer virtual calls
            want[r["vtable"]][int(r["word"], 16)] = r["value"]
    bad = []
    for s in syms:
        if s["type"] != 1 or s["name"] not in want:
            continue
        sec = next((x for x in elf.sections if x["type"] == 1 and x["address"] <= s["address"] < x["address"] + x["size"]), None)
        if not sec:
            continue
        raw = elf.contents(sec)[s["address"] - sec["address"]:][:s["size"]]
        for word, value in want[s["name"]].items():
            if word + 4 > len(raw):
                bad.append(s["name"]); break
            x = struct.unpack_from(">I", raw, word)[0]
            if value.startswith("__RTTI__"):
                continue
            if re.fullmatch(r"-?\d+", value):
                if (x - (1 << 32) if x >= 1 << 31 else x) != int(value):
                    bad.append(s["name"]); break
            elif value not in names.get(x, ()):
                bad.append(s["name"]); break
    return sorted(set(bad))


def object_table_matches(path, name, want):
    """Does this object's compiled copy of a vtable match the original's words?"""
    elf = Elf32(Path(path).read_bytes())
    syms = list(elf.symbols())
    sym = next((x for x in syms if x["name"] == name and 0 < x["section"] < 0xFF00 and x["type"] == 1), None)
    if not sym:
        return False
    raw = elf.contents(elf.sections[sym["section"]])[sym["address"]:sym["address"] + sym["size"]]
    relocs = {}
    for sec in elf.sections:
        if sec["type"] == 4 and sec["info"] == sym["section"]:
            data = elf.contents(sec)
            for off in range(0, len(data), 12):
                where, info, addend = struct.unpack_from(">IIi", data, off)
                if sym["address"] <= where < sym["address"] + sym["size"]:
                    relocs[where - sym["address"]] = (syms[info >> 8], addend)
    for word, value in want.items():
        if word + 4 > len(raw):
            return False
        if word in relocs:
            target, addend = relocs[word]
            if value.startswith("__RTTI__"):
                continue
            if target["name"] != value or addend:
                return False
        else:
            x = struct.unpack_from(">I", raw, word)[0]
            if not re.fullmatch(r"-?\d+", value) or (x - (1 << 32) if x >= 1 << 31 else x) != int(value):
                return False
    return True


def override_object(names):
    """Compile the original's copies of these tables (gen_vtable_stopgap.py's generator)."""
    import gen_vtable_stopgap as gv
    from check_data import Image, ORIGINAL, compile_file
    src = OUT / "vt_override.cpp"
    src.write_text(gv.generate(Image(ORIGINAL), names))
    obj, log = compile_file(src, gv.SETTINGS)
    if not obj:
        raise SystemExit("vt_override.cpp does not compile:\n" + log)
    shutil.copy(obj, OUT / "vt_override.o")
    fix_sda21(OUT / "vt_override.o")
    return str(OUT / "vt_override.o")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--probe", nargs="*", default=[],
                    help="test harnesses (research/probes/*.cpp, input_script.py output): each is compiled with its "
                         "'// settings:' line and replaces the functions its '// replaces:' lines name")
    ap.add_argument("--output", default=str(OUT / "Moh2RelGC_pure.elf"))
    ap.add_argument("--probe-space", type=lambda v: int(v, 0), default=0,
                    help="bytes reserved after .text (symbol _probe_space) for research/tools/entry_trace.py")
    a = ap.parse_args()
    units = sorted(str(p) for p in (ROOT / "build/reconstruction/units").glob("*/compiled.o")
                   if json_exists(p))
    built, failed = sl.compile_all(a.jobs)
    if failed:
        print("failed to compile:", ", ".join(sorted(failed)))
    drafts = [o for s, _, o in built if s.startswith("research/drafts/")]
    data = [o for s, _, o in built if s.startswith(("research/data/", "research/data_stopgap/"))]
    probes, replaced, probe_origin = [], set(), {}
    for path in a.probe:
        from check_data import compile_file
        text = Path(path).read_text()
        m = re.search(r"//\s*settings:\s*(\S+)", text)
        obj, log = compile_file(Path(path), m.group(1) if m else sl.DEFAULT_SETTINGS)
        if not obj:
            raise SystemExit(f"probe {path} does not compile:\n{log}")
        dst = OUT / (Path(path).stem + ".probe.o")
        OUT.mkdir(parents=True, exist_ok=True)
        shutil.copy(obj, dst)
        fix_sda21(dst)
        probes.append(str(dst))
        replaced |= set(re.findall(r"//\s*replaces:\s*(\S+)", text))
        cfg = json.load(open(ROOT / (m.group(1) if m else sl.DEFAULT_SETTINGS)))
        probe_origin[str(dst)] = cfg.get("original_file")
    weaken, globalize, rename, stripped, notes = plan(units, drafts, data, probes, probe_origin)
    for o in units + data + drafts:
        names = {x["name"] for x in symbols(o) if x["section"] and 0 < x["section"] < 0xFF00 and x["binding"] in (1, 2)}
        for n in replaced & names:
            weaken[o].add(n)
    OUT.mkdir(parents=True, exist_ok=True)
    override = sorted(set((OUT / "vt_override.txt").read_text().split())) if (OUT / "vt_override.txt").exists() else []
    for _ in range(3):
        r, objs, errors = link(units, data, drafts, weaken, globalize, rename, stripped, override, probes, a.output,
                               a.probe_space)
        bad = vtable_mismatches(a.output) if r.returncode == 0 else []
        if not bad:
            break
        override = sorted(set(override) | set(bad))
        (OUT / "vt_override.txt").write_text("\n".join(override) + "\n")
        print(f"vtables differing from the original, regenerated: {', '.join(bad)}")
    notes += [f"vtable {n}: the original's copy (vt_override.o) replaces the compiled ones" for n in override]
    (OUT / "decisions.txt").write_text("\n".join(notes) + "\n")
    kinds = collections.Counter(re.sub(r".*?: ", "", l).split("`")[0].strip() for l in errors)
    print(f"objects: {len(units)} units, {len(data)} data, {len(drafts)} drafts; "
          f"weakened {sum(map(len, weaken.values()))}, globalized {sum(map(len, globalize.values()))}, "
          f"trap stubs: {len(stripped)}, vtables from the original: {len(override)}")
    print(f"link exit {r.returncode}, {len(errors)} error lines")
    for k, n in kinds.most_common(8):
        print(f"  {n:5} {k}")
    print(f"notes: {len(notes)} (build/pure/decisions.txt); log build/pure/link.log")
    return r.returncode


def link(units, data, drafts, weaken, globalize, rename, stripped, override, probes=(), output=None, probe_space=0):
    weaken = {o: set(v) for o, v in weaken.items()}
    generate = []
    if override:
        want = collections.defaultdict(dict)
        for r in csv.DictReader(open(ROOT / "research/layout/vtables.tsv"), delimiter="\t"):
            want[r["vtable"]][int(r["word"], 16)] = r["value"]
        for n in override:
            holders = [o for o in units + data + drafts
                       if any(x["name"] == n and 0 < x["section"] < 0xFF00 for x in symbols(o))]
            good = [o for o in holders if object_table_matches(o, n, want[n])]
            for o in holders:
                if good and o == good[0]:
                    weaken.get(o, set()).discard(n)   # the matching compiled copy wins
                else:
                    weaken.setdefault(o, set()).add(n)
            if not good:
                generate.append(n)                   # no copy matches: the original's, generated
    DROP_CTORS.clear()
    DROP_CTORS.update(drafts_without_sinit(drafts))
    objs = prepare(units + data + drafts + list(probes), weaken, globalize, rename, verified_weak_copies(units))
    origin = original_files(units, drafts, data)
    renamed = {o: p for o, p in zip(units + data + drafts, objs)}
    placed = bss_placement(units + data + drafts, origin)
    script = SCRIPT.replace("@PROBE_SPACE@", hex(probe_space))
    for name, (_, start, phdr) in BSS_ORIGINAL.items():
        lines = [f'  {name}.{i} ({start} + {off:#x}) (NOLOAD) : {{ "{renamed[o]}"({name}) }} :{phdr}'
                 for i, (o, off) in enumerate(sorted(placed.get(name, []), key=lambda x: x[1]))]
        script = script.replace("@BSS_MAPPED@", "\n".join(lines))
    script = script.replace("@SBSS_MAPPED@", "")
    (OUT / "link.ld").write_text(script)
    (OUT / "bss_placed.txt").write_text("".join(f"{n} {off:#x} {renamed[o]}\n" for n, v in placed.items() for o, off in v))
    (OUT / "objects.rsp").write_text("\n".join(objs) + "\n")
    stub = OUT / "stripped.s"
    stub.write_text(".text\n" + "".join(f'.globl "{n}"\n"{n}":\n  trap\n' for n in stripped))
    subprocess.run([str(ROOT / "build/tools/powerpc-eabi-as"), "-mgekko", "-o", str(OUT / "stripped.o"), str(stub)],
                   check=True)
    extra = [override_object(generate)] if generate else []
    cmd = [str(LD), "-T", str(OUT / "link.ld"), "-o", str(output or OUT / "Moh2RelGC_pure.elf"),
           "--no-warn-mismatch", "--no-check-sections", "-z", "max-page-size=0x20", f"@{OUT / 'objects.rsp'}", str(OUT / "stripped.o"), *extra]
    r = subprocess.run(cmd, capture_output=True, text=True)
    (OUT / "link.log").write_text(r.stdout + r.stderr)
    errors = [l for l in (r.stdout + r.stderr).splitlines() if "undefined reference" in l or "error" in l.lower()]
    return r, objs, errors


def json_exists(obj):
    return (ROOT / "config/GMFE69" / (obj.parent.name + ".json")).exists()


if __name__ == "__main__":
    sys.exit(main())
