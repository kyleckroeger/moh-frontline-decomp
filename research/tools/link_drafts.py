#!/usr/bin/env python3
"""link_drafts.py: link the partial-source drafts into a non-matching, runnable ELF.

Partial functions differ in size, so they can't sit at their original addresses.
Instead of a complete relink, each draft is compiled with small data turned off, its
symbols are resolved against the original image, the compiled code and data go into
a new load segment where the arena used to start, and the original function's first
instruction becomes a branch to the draft. Everything else stays original.

    link_drafts.py                  link every draft that can be linked
    link_drafts.py --only A B       only functions whose name contains A or B
    link_drafts.py --exclude A B    leave these out, to bisect a crash
    link_drafts.py --report-only    write the figures but no ELF
    link_drafts.py --probe P.cpp --coverage --output X.elf
                                    a test image (research/tools/dolphin_boot.py) that
                                    also logs "COVER name" the first time each draft runs

Writes build/linked/Moh2RelGC_linked.elf and progress/GMFE69.linked.json. "Linked" means
every symbol a draft can reach was resolved. It doesn't mean the draft behaves
correctly; only running the game shows that. Exceptions thrown through a draft aren't
supported, because the drafts' exception tables are dropped.
"""
import argparse, bisect, collections, csv, json, os, re, struct, sys, tempfile
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT / "research/tools"), str(ROOT / "tools")]
import fast
from formats import Elf32
import sync_docs

ADDR32, ADDR16_LO, ADDR16_HI, ADDR16_HA, REL24, REL14, REL32, SDA21 = 1, 4, 5, 6, 10, 11, 26, 109
SKIP_SECTIONS = {"extab", "extabindex", ".comment", ".ctors", ".dtors"}
ARENA_LO_HI, ARENA_LO_LO = 0x801104bc, 0x801104c0  # lis/addi of __ArenaLo in OSInit.
ARENA_STACK_HI, ARENA_STACK_LO = 0x801104f4, 0x801104f8  # lis/addi of _stack_addr in OSInit.
LOOSE = set()  # Symbols matched by name and class only, since a draft's parameter types differed.


def original_symbols(elf):
    exact, by_base, prefixes = {}, collections.defaultdict(set), collections.defaultdict(set)
    every = collections.defaultdict(set)
    for s in elf.symbols():
        name, addr = s["name"], s["address"]
        if not name or not s["section"] or name.startswith(("@", "...")):
            continue
        exact.setdefault(name, addr)
        every[name].add(addr)
        if "$" in name:
            by_base[name.split("$")[0]].add(addr)
        m = re.match(r"(.+?__(?:\d+\w+?|Q\d\w+?)?)[FC]", name)
        if m:
            prefixes[loose_key(name)].add(addr)
    return exact, by_base, prefixes, every


def original_references(elf):
    """(address of the reference, address it refers to) for every original relocation."""
    symbols = list(elf.symbols())
    refs = []
    for sec in elf.sections:
        if sec["type"] != 4 or not sec["name"].startswith((".rela.text", ".rela.init")):
            continue
        data = elf.contents(sec)
        for off in range(0, len(data), 12):
            where, info, addend = struct.unpack_from(">IIi", data, off)
            refs.append((where, symbols[info >> 8]["address"] + addend))
    return sorted(refs)


def references_in(refs, start, size):
    low = bisect.bisect_left(refs, (start, 0))
    high = bisect.bisect_left(refs, (start + size, 0))
    return [target for _, target in refs[low:high]]


def choose_address(candidates, hints):
    """Pick among file-local symbols sharing a name: the one the original function uses or is nearest."""
    candidates = sorted(candidates)
    if not hints:
        return candidates[0]
    return min(candidates, key=lambda c: min(abs(c - h) for h in hints))


def loose_key(name):
    """A mangled name without its parameter types: drafts use their own views of types."""
    m = re.match(r"(.+?)__(\d+)(\w+)", name)
    if m and m[1]:
        return f"{m[1]}__{m[2]}{m[3][:int(m[2])]}"
    m = re.match(r"(.+?)__F", name)
    return f"{m[1]}__F" if m else name


class Unit:
    """One compiled draft, with its sections placed at load addresses."""
    def __init__(self, path, base):
        self.elf = Elf32(Path(path).read_bytes())
        self.place, self.blob = {}, bytearray()
        self.base = base
        for i, sec in enumerate(self.elf.sections):
            if sec["type"] not in (1, 8) or sec["name"] in SKIP_SECTIONS or not sec["size"]:
                continue
            align = max(sec["alignment"], 4)
            while (base + len(self.blob)) % align:
                self.blob.append(0)
            self.place[i] = base + len(self.blob)
            self.blob += self.elf.contents(sec) if sec["type"] == 1 else bytes(sec["size"])
        self.symbols = list(self.elf.symbols())
        self.relocs = []
        for sec in self.elf.sections:
            if sec["type"] == 4 and sec["info"] in self.place:
                data = self.elf.contents(sec)
                for off in range(0, len(data), 12):
                    where, info, addend = struct.unpack_from(">IIi", data, off)
                    self.relocs.append((sec["info"], where, info & 255, self.symbols[info >> 8], addend))
        self.funcs = {}
        for s in self.symbols:
            if s["name"] and s["section"] in self.place and s["size"] and s["type"] == 2:
                self.funcs[s["name"]] = (s["section"], s["address"], s["size"])
        self.hints = []  # What the original versions of this draft's functions refer to.


def resolve(sym, unit, wanted, orig):
    """The address of a symbol, or None. Own copies are used only for what's being linked."""
    exact, by_base, loose, every = orig
    name = sym["name"]
    defined = sym["section"] in unit.place
    if defined and (not name or name.startswith("@") or name in wanted):
        return unit.place[sym["section"]] + sym["address"]
    if name in exact:
        return choose_address(every[name], unit.hints) if unit.hints else exact[name]
    if "$" in name:
        hits = by_base.get(name.split("$")[0], ())
        if len(hits) == 1:
            return next(iter(hits))
    if defined:
        return unit.place[sym["section"]] + sym["address"]
    hits = loose.get(loose_key(name), ())
    if len(hits) == 1:
        LOOSE.add(name)
        return next(iter(hits))
    return None


def reachable(unit, name, wanted, orig):
    """Unresolved symbols a function can reach through the draft's own code."""
    seen, missing, todo = set(), set(), [name]
    while todo:
        f = todo.pop()
        if f in seen or f not in unit.funcs:
            continue
        seen.add(f)
        sec, start, size = unit.funcs[f]
        for rsec, where, kind, sym, addend in unit.relocs:
            if rsec != sec or not start <= where < start + size:
                continue
            if kind == SDA21 and sym["name"]:
                missing.add("small data: " + sym["name"])
            target = resolve(sym, unit, wanted, orig)
            if target is None:
                missing.add(sym["name"])
            elif sym["name"] in unit.funcs and sym["section"] in unit.place and target == unit.place[sym["section"]] + sym["address"]:
                todo.append(sym["name"])
    return missing


def apply_relocs(unit, wanted, orig):
    unresolved = 0
    for sec, where, kind, sym, addend in unit.relocs:
        target = resolve(sym, unit, wanted, orig)
        if target is None:
            unresolved += 1
            continue
        value = target + addend
        pos = unit.place[sec] - unit.base + where
        here = unit.base + pos
        if kind == ADDR32:
            struct.pack_into(">I", unit.blob, pos, value & 0xffffffff)
        elif kind == ADDR16_LO:
            struct.pack_into(">H", unit.blob, pos, value & 0xffff)
        elif kind == ADDR16_HI:
            struct.pack_into(">H", unit.blob, pos, (value >> 16) & 0xffff)
        elif kind == ADDR16_HA:
            struct.pack_into(">H", unit.blob, pos, ((value + 0x8000) >> 16) & 0xffff)
        elif kind == REL24:
            word = struct.unpack_from(">I", unit.blob, pos)[0]
            struct.pack_into(">I", unit.blob, pos, (word & 0xfc000003) | ((value - here) & 0x03fffffc))
        elif kind == REL14:
            word = struct.unpack_from(">I", unit.blob, pos)[0]
            struct.pack_into(">I", unit.blob, pos, (word & 0xffff0003) | ((value - here) & 0xfffc))
        elif kind == REL32:
            struct.pack_into(">I", unit.blob, pos, (value - here) & 0xffffffff)
        elif kind != SDA21:
            raise ValueError(f"relocation type {kind} on {sym['name']}")
    return unresolved


def compile_draft(draft, settings, work):
    template = dict(json.load(open(ROOT / settings)))
    # With pooling on, neighbouring globals are reached from a section anchor plus fixed
    # offsets, which would leave the draft using a private copy instead of the original's.
    template["compiler_flags"] = list(template["compiler_flags"]) + ["-sdata", "0", "-sdata2", "0", "-pool", "off"]
    text = (ROOT / draft).read_text(errors="replace")
    # As in check_draft.py; a few .c drafts only compile as C++, and some names are mangled.
    os.environ["SUFFIX"] = "cpp"
    obj, log = fast.compile_object(template, text, work)
    if not obj and draft.endswith(".c"):
        os.environ["SUFFIX"] = "c"
        obj, log = fast.compile_object(template, text, work)
    return obj, log


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--only", nargs="*", default=[])
    ap.add_argument("--exclude", nargs="*", default=[])
    ap.add_argument("--report-only", action="store_true")
    ap.add_argument("--probe", nargs="*", default=[],
                    help="extra source files whose functions replace originals of the same name "
                         "(test harnesses such as research/probes/input_probe.cpp); not counted as progress")
    ap.add_argument("--coverage", action="store_true",
                    help="enter each draft through a stub that logs 'COVER name' with OSReport on its first call")
    ap.add_argument("--output", default="build/linked/Moh2RelGC_linked.elf")
    args = ap.parse_args()
    os.chdir(ROOT)
    snapshot = json.load(open("progress/GMFE69.snapshot.json"))
    open_sizes = {name: size for _, name, size in sync_docs.remaining(snapshot)}
    rows = [r for r in csv.DictReader(open("research/drafts/index.tsv"), delimiter="\t") if r["function"] in open_sizes]
    if args.only:
        rows = [r for r in rows if any(k in r["function"] for k in args.only)]
    rows = [r for r in rows if not any(k in r["function"] for k in args.exclude)]
    groups = collections.defaultdict(list)
    for r in rows:
        groups[(r["draft"], r["compile_settings_from"])].append(r)

    original = Path(fast.ELF).read_bytes()
    orig_elf = Elf32(original)
    orig = original_symbols(orig_elf)
    probes = set()
    for path in args.probe:
        path = str(Path(path).resolve())
        first = Path(path).read_text().splitlines()[0]
        settings = first.split("settings:")[1].strip() if "settings:" in first else "config/GMFE69/ipad.json"
        names = re.findall(r"^//\s*replaces:\s*(\S+)", Path(path).read_text(), re.M)
        for name in names:
            groups[(path, settings)].append(dict(function=name, address=f"{orig[0][name]:x}"))
            probes.add(name)
            open_sizes.setdefault(name, 0)
    refs = original_references(orig_elf)
    arena = (orig[0]["__ArenaLo"] + 31) & ~31
    linked, failed, blob, patches = [], [], bytearray(), []
    unresolved_data = 0
    for (draft, settings), group in sorted(groups.items()):
        with tempfile.TemporaryDirectory() as work:
            obj, log = compile_draft(draft, settings, work)
            if not obj:
                failed += [(r["function"], "does not compile") for r in group]
                continue
            while (arena + len(blob)) % 32:
                blob.append(0)
            unit = Unit(obj, arena + len(blob))
        unit.hints = [t for r in group for t in references_in(refs, int(r["address"], 16), open_sizes[r["function"]])]
        wanted = {r["function"] for r in group}
        good = []
        for r in group:
            name = "__sinit_probe_cpp" if r["function"].startswith("__sinit_") else r["function"]
            if name not in unit.funcs:
                failed.append((r["function"], "not emitted"))
                continue
            missing = reachable(unit, name, wanted | {"__sinit_probe_cpp"}, orig)
            if missing:
                failed.append((r["function"], "; ".join(sorted(missing))[:300]))
                continue
            good.append((r, name))
        if not good:
            continue
        unresolved_data += apply_relocs(unit, wanted | {"__sinit_probe_cpp"}, orig)
        for r, name in good:
            sec, start, _ = unit.funcs[name]
            patches.append((int(r["address"], 16), unit.place[sec] + start))
            if r["function"] not in probes:
                linked.append(r["function"])
        blob += unit.blob

    p = snapshot["progress"]
    linked_bytes = sum(open_sizes[f] for f in linked)
    total = p["matching_code_bytes"] + linked_bytes
    figures = dict(
        exact_bytes=p["matching_code_bytes"], linked_draft_bytes=linked_bytes, linked_bytes=total,
        total_bytes=p["total_executable_code_bytes"],
        linked_percent=round(100 * total / p["total_executable_code_bytes"], 2),
        linked_functions=len(linked), failed_functions=len(failed), segment_bytes=len(blob),
        symbols_matched_loosely=len(LOOSE), unresolved_unreachable_references=unresolved_data,
        failed=dict(sorted(failed)))
    if not args.only and not args.exclude and not args.probe:
        Path("progress/GMFE69.linked.json").write_text(json.dumps(figures, indent=1) + "\n")
    print(f"linked {len(linked)} functions, {linked_bytes:,} bytes -> {total:,} ({figures['linked_percent']}%); "
          f"failed {len(failed)}; segment {len(blob):,} bytes; {len(LOOSE)} symbols matched loosely; "
          f"{unresolved_data} unresolved references outside reachable code")
    for name, why in sorted(failed):
        print(f"  {name[:60]}: {why[:160]}")
    if args.coverage:
        names = {int(r["address"], 16): r["function"] for g in groups.values() for r in g}
        patches = coverage_stubs(patches, {a: names[a] for a, _ in patches if names[a] not in probes},
                                 orig[0]["OSReport"], arena, blob)
    if not args.report_only:
        write_elf(original, orig_elf, patches, arena, blob, args.output)


def coverage_stubs(patches, names, osreport, arena, blob):
    """Route each patch in `names` through a stub that reports the draft's first call.

    The stub tests its flag byte, and on the first call saves the argument registers
    (r3-r10, f1-f8, CR), calls OSReport("COVER name\n"), restores them and branches on to
    the draft. r11, r12 and r0 are free at a function's entry. Appends to `blob`."""
    def ha(v):
        return ((v + 0x8000) >> 16) & 0xffff

    def d_form(op, rd, ra, imm):
        return (op << 26) | (rd << 21) | (ra << 16) | (imm & 0xffff)
    STUB = 53 * 4
    while (arena + len(blob)) % 32:
        blob.append(0)
    todo = [(a, t) for a, t in patches if a in names]
    base = arena + len(blob)
    strings = base + STUB * len(todo)
    text, out = bytearray(), []
    pool = bytearray()
    entries = []
    for addr, target in todo:
        entries.append((addr, target, len(pool)))
        pool += f"COVER {names[addr]}\n".encode() + b"\0"
    while len(pool) % 4:
        pool.append(0)
    flag_base = strings + len(pool)
    for i, (addr, target, soff) in enumerate(entries):
        here = base + len(text)
        flag, string = flag_base + i, strings + soff
        code = [d_form(15, 11, 0, ha(flag)), d_form(34, 12, 11, flag), d_form(11, 0, 12, 0)]
        go = len(code)  # patched below: bne to the final branch
        code += [0, d_form(14, 12, 0, 1), d_form(38, 12, 11, flag),
                 d_form(37, 1, 1, -0x80), 0x7C0802A6, d_form(36, 0, 1, 0x84), 0x7D800026, d_form(36, 12, 1, 0x6c)]
        code += [d_form(36, r, 1, 8 + 4 * (r - 3)) for r in range(3, 11)]
        code += [d_form(54, f, 1, 0x28 + 8 * (f - 1)) for f in range(1, 9)]
        code += [d_form(15, 3, 0, ha(string)), d_form(14, 3, 3, string), 0x4CC63182]
        call = here + 4 * len(code)
        code += [0x48000001 | ((osreport - call) & 0x03fffffc)]
        code += [d_form(32, 12, 1, 0x6c), 0x7D8FF120]
        code += [d_form(32, r, 1, 8 + 4 * (r - 3)) for r in range(3, 11)]
        code += [d_form(50, f, 1, 0x28 + 8 * (f - 1)) for f in range(1, 9)]
        code += [d_form(32, 0, 1, 0x84), 0x7C0803A6, d_form(14, 1, 1, 0x80)]
        last = len(code)
        code += [0x48000000 | ((target - (here + 4 * last)) & 0x03fffffc)]
        code[go] = (16 << 26) | (4 << 21) | (2 << 16) | ((4 * (last - go)) & 0xfffc)
        assert len(code) * 4 == STUB, len(code)
        text += b"".join(struct.pack(">I", w) for w in code)
        out.append((addr, here))
    blob += text + pool + bytes(len(entries))
    redirected = dict(out)
    return [(a, redirected.get(a, t)) for a, t in patches]


def build_elf(original, patches, arena, blob):
    """Return (ELF bytes, new arena start) with the patches and the new load segment."""
    data = bytearray(original)
    phoff, shoff = struct.unpack_from(">II", data, 28)
    phsize, phnum, shsize, shnum = struct.unpack_from(">HHHH", data, 42)
    headers = [list(struct.unpack_from(">8I", data, phoff + i * phsize)) for i in range(phnum)]

    def offset(addr):
        for h in headers:
            if h[0] == 1 and h[4] and h[2] <= addr < h[2] + h[4]:
                return h[1] + addr - h[2]
        raise ValueError(hex(addr))
    for addr, target in patches:
        struct.pack_into(">I", data, offset(addr), 0x48000000 | ((target - addr) & 0x03fffffc))
    # OSInit sets the arena start twice: first from __ArenaLo, then from _stack_addr when
    # the boot information block has none. Both must move past the new segment.
    end = (arena + len(blob) + 31) & ~31
    for hi, lo in ((ARENA_LO_HI, ARENA_LO_LO), (ARENA_STACK_HI, ARENA_STACK_LO)):
        struct.pack_into(">H", data, offset(hi) + 2, ((end + 0x8000) >> 16) & 0xffff)
        struct.pack_into(">H", data, offset(lo) + 2, end & 0xffff)
    # The game's disc loader reads every segment as empty if the program headers move away
    # from the start of the file, so make room for the new one where the table already is.
    first = min(h[1] for h in headers if h[4])
    table_end = phoff + phsize * (phnum + 1)
    shift = (max(table_end - first, 0) + 31) & ~31
    data[first:first] = bytes(shift)
    for h in headers:
        if h[1] >= first:
            h[1] += shift
    if shoff >= first:
        struct.pack_into(">I", data, 32, shoff + shift)
        for i in range(shnum):
            at = shoff + shift + i * shsize + 16
            section_offset, = struct.unpack_from(">I", data, at)
            if section_offset >= first:
                struct.pack_into(">I", data, at, section_offset + shift)
    while len(data) % 32:
        data.append(0)
    headers.append([1, len(data), arena, 0, len(blob), len(blob), 7, 32])
    data += blob
    for i, h in enumerate(headers):
        struct.pack_into(">8I", data, phoff + i * phsize, *h)
    struct.pack_into(">H", data, 44, len(headers))
    return data, end


def write_elf(original, elf, patches, arena, blob, output="build/linked/Moh2RelGC_linked.elf"):
    data, end = build_elf(original, patches, arena, blob)
    out = ROOT / output
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_bytes(data)
    print(f"wrote {out.relative_to(ROOT)}: {len(patches)} patches, arena now starts at {end:#x}")


if __name__ == "__main__":
    main()
