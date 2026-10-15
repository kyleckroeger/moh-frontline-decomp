#!/usr/bin/env python3
"""Draft a unit manifest by matching a compiled object to the original.

The original executable keeps its relocations. Each relocation in the compiled
object is paired with the original relocation at the same code or data offset,
which fixes the address of every compiled section and every external without
guessing. Named functions and objects anchor sections that have no references.
The draft earns nothing by itself: reconstruct.py must verify every byte.
"""
import argparse
import json
import struct
from pathlib import Path

from audit import load_target
from data_strip import pool_item, section_chunks
from formats import Elf32
from sdk_build import compile_sdk
from setup import CONFIG, ROOT
from setup_compiler import setup_compiler
from tool_runner import run

SDA21 = 109
RISING_SUN = "https://github.com/lifewillbeokay/moh-rising-sun"


def relocations(elf):
    """Yield (target section index, offset, type, symbol, addend) for allocated targets."""
    symbols = list(elf.symbols())
    for section in elf.sections:
        if section["type"] != 4 or not elf.sections[section["info"]]["flags"] & 2:
            continue
        data = elf.contents(section)
        for offset in range(0, len(data), 12):
            where, info, addend = struct.unpack_from(">IIi", data, offset)
            yield section["info"], where, info & 255, symbols[info >> 8], addend


class Original:
    def __init__(self, elf):
        self.elf = elf
        self.symbols = list(elf.symbols())
        self.relocs = {where: (kind, symbol, addend) for _, where, kind, symbol, addend in relocations(elf)}
        # Target address -> [(location, kind)], to place sections that only
        # refer outward (an exception index points at its functions).
        self.referrers = {}
        for where, (kind, symbol, addend) in self.relocs.items():
            self.referrers.setdefault(symbol["address"] + addend, []).append((where, kind))
        self.section_of = lambda address: next(
            (s["name"] for s in elf.sections if s["flags"] & 2 and s["address"] <= address < s["address"] + s["size"]), None)
        self.files = [i for i, s in enumerate(self.symbols) if s["type"] == 4]

    def file_index(self, name, index=None):
        matches = [i for i in self.files if self.symbols[i]["name"] == name]
        if index is not None:
            if index not in matches:
                raise ValueError(f"Symbol {index} is not a file record for {name}")
            return index
        if len(matches) != 1:
            raise ValueError(f"{len(matches)} file records named {name}; pass --file-index")
        return matches[0]

    def locals_of(self, file_index):
        result = {}
        if file_index is None:
            return result  # A file with only global symbols has no record.
        for symbol in self.symbols[file_index + 1:]:
            if symbol["type"] == 4:
                break
            if symbol["binding"] == 0 and symbol["section"] and symbol["name"]:
                result.setdefault(symbol["name"], []).append(symbol)
        return result

    def globals(self, name):
        return [s for s in self.symbols if s["name"] == name and s["binding"] and s["section"]
                and s["type"] != 4 and s["section"] < 0xFF00]


def anchor(addresses, name, value):
    if addresses.setdefault(name, value) != value:
        raise ValueError(f"Inconsistent address for {name}: {addresses[name]:#x} vs {value:#x}")


def find(symbol, original, locals_):
    candidates = locals_.get(symbol["name"], []) if symbol["binding"] == 0 else original.globals(symbol["name"])
    if symbol["binding"] == 0 and not candidates:
        # MW numbers function statics per compilation: name$123.
        stem = symbol["name"].split("$", 1)[0]
        candidates = [s for key, items in locals_.items() if key.split("$", 1)[0] == stem for s in items]
    return candidates[0] if len(candidates) == 1 else None


CODE_SECTIONS = (".text", ".init")


def text_layout(obj, original, locals_, duplicates=()):
    """Keep functions the original has; discard absent ones; reject size changes.

    Named weak duplicates are dropped like discarded functions; the original's
    copy elsewhere is linked instead.

    Returns, per compiled code section index, its kept functions with their
    packed offsets, the discarded functions and the packed size.
    """
    layout, discarded = {}, []
    for code in (s for s in obj.sections if s["name"] in CODE_SECTIONS and s["size"]):
        functions = sorted((s for s in obj.symbols() if s["type"] == 2 and s["section"] == code["index"]),
                           key=lambda s: s["address"])
        kept, packed = [], 0
        for function in functions:
            if function["name"] in duplicates:
                if function["binding"] != 2:
                    raise ValueError(f"Weak duplicate is not weak in the compiled object: {function['name']}")
                continue
            target = find(function, original, locals_)
            absent = not (locals_.get(function["name"]) if function["binding"] == 0 else original.globals(function["name"]))
            if target is not None and target["size"] == function["size"]:
                kept.append((function, packed))
                packed += function["size"]
            elif absent:
                discarded.append(function)
            else:
                raise ValueError(f"Function differs from the original: {function['name']}")
        layout[code["index"]] = (kept, packed)
    return layout, discarded


def pool_reference(obj, section, offset, target, pool):
    """Record the original address of the pool object a retained reference uses."""
    containing = [s for s in obj.symbols() if s["section"] == section["index"] and s["type"] == 1
                  and s["size"] and s["address"] <= offset < s["address"] + s["size"]]
    if len(containing) != 1:
        raise ValueError(f"Pool reference is not inside one compiler object: {section['name']}+{offset:#x}")
    start = containing[0]["address"]
    anchor(pool, (section["name"], start, pool_item(obj, section, start)), target - (offset - start))


def match(obj, original, file_index, duplicates=(), pooled=()):
    """Return section addresses, external addresses and the stripped text layout.

    References into a pooled section are recorded per item in `pool` instead
    of placing the section.
    """
    symbols = list(obj.symbols())
    locals_ = original.locals_of(file_index)
    names = {s["index"]: s["name"] for s in obj.sections}
    addresses, externals, pool = {}, {}, {}
    layout, discarded = text_layout(obj, original, locals_, duplicates)

    def position(section, offset):
        """Offset after stripping: discarded code has no position."""
        if section not in layout:
            return offset
        for function, packed in layout[section][0]:
            if function["address"] <= offset < function["address"] + function["size"]:
                return packed + offset - function["address"]
        return None

    # Named functions and objects anchor their sections.
    for symbol in symbols:
        if (symbol["section"] and symbol["section"] < 0xFF00 and symbol["type"] in (1, 2)
                and not symbol["name"].startswith("@") and names[symbol["section"]] not in pooled
                and symbol["name"] not in duplicates):
            target = find(symbol, original, locals_)
            offset = position(symbol["section"], symbol["address"])
            if target is not None and target["size"] == symbol["size"] and offset is not None:
                anchor(addresses, names[symbol["section"]], target["address"] - offset)
    if not any(names[i] in addresses for i in layout):
        raise ValueError("No compiled function was found in the original file record"
                         if file_index is not None else "No compiled global function was found in the original")
    # Relocations inside a weak duplicate (a dropped virtual table's entries)
    # must not pin anything: the duplicate is not placed.
    dropped_spans = [(s["section"], s["address"], s["address"] + s["size"]) for s in symbols
                     if s["name"] in duplicates and s["section"] and s["section"] < 0xFF00]

    def in_dropped(section, where):
        return any(section == d and a <= where < b for d, a, b in dropped_spans)

    # Relocations pin the rest; repeat until no new section is placed.
    changed = True
    while changed:
        changed = False
        for section, where, kind, symbol, addend in relocations(obj):
            if in_dropped(section, where):
                continue
            base = addresses.get(names[section])
            where = position(section, where)
            if base is None or where is None or base + where not in original.relocs:
                continue
            okind, osymbol, oaddend = original.relocs[base + where]
            if okind != kind:
                raise ValueError(f"Relocation type differs at {base + where:#x}")
            target = osymbol["address"] + oaddend
            if symbol["section"] == 0 or symbol["name"] in duplicates:
                if osymbol["name"] and osymbol["name"] != symbol["name"] and not addend:
                    raise ValueError(f"Compiled code refers to {symbol['name']} where the original "
                                     f"refers to {osymbol['name']} (at {base + where:#x})")
                anchor(externals, symbol["name"], target - addend)
            elif symbol["section"] < 0xFF00 and names[symbol["section"]] in pooled:
                pool_reference(obj, obj.sections[symbol["section"]], symbol["address"] + addend, target, pool)
            elif symbol["section"] < 0xFF00:
                offset = position(symbol["section"], symbol["address"] + addend)
                if offset is None:
                    if section in layout:
                        raise ValueError(f"Kept code references discarded code at {base + where:#x}")
                    continue  # data of discarded code (e.g. a jump table), trimmed later
                name = names[symbol["section"]]
                if name not in addresses:
                    changed = True
                anchor(addresses, name, target - offset)
    # A section nothing placed refers to (extabindex) is pinned by its own
    # references to placed code: the original relocation from the section of
    # the same name to the same target fixes its base.
    for section, where, kind, symbol, addend in relocations(obj):
        name = names[section]
        if name in addresses or symbol["section"] not in layout or names[symbol["section"]] not in addresses:
            continue
        if in_dropped(section, where):
            continue
        offset = position(symbol["section"], symbol["address"] + addend)
        if offset is None:
            continue
        target = addresses[names[symbol["section"]]] + offset
        sources = [w for w, k in original.referrers.get(target, []) if k == kind and original.section_of(w) == name]
        if len(sources) == 1:
            anchor(addresses, name, sources[0] - where)
            changed = True
    while changed:
        changed = False
        for section, where, kind, symbol, addend in relocations(obj):
            if in_dropped(section, where):
                continue
            base = addresses.get(names[section])
            where = position(section, where)
            if base is None or where is None or base + where not in original.relocs:
                continue
            okind, osymbol, oaddend = original.relocs[base + where]
            if (symbol["section"] and symbol["section"] < 0xFF00 and names[symbol["section"]] not in addresses
                    and names[symbol["section"]] not in pooled and symbol["name"] not in duplicates):
                offset = position(symbol["section"], symbol["address"] + addend)
                if offset is not None and okind == kind:
                    anchor(addresses, names[symbol["section"]], osymbol["address"] + oaddend - offset)
                    changed = True
    return addresses, externals, layout, discarded, pool


def referenced(obj, index, kept, addresses, duplicates=()):
    """Whether retained code, or a placed section, refers into a section.

    References to a weak duplicate do not count: they link to the original copy.
    """
    names = {s["index"]: s["name"] for s in obj.sections}
    for section, where, _, symbol, _ in relocations(obj):
        if symbol["section"] != index or symbol["name"] in duplicates:
            continue
        if names[section] in CODE_SECTIONS:
            if any(f["section"] == section and f["address"] <= where < f["address"] + f["size"] for f, _ in kept):
                return True
        elif names[section] in addresses:
            return True
    return False


def kept_span(obj, index, kept, original, locals_, base=None, duplicates=()):
    """Compiled [start, end) of objects the original kept, if MW trimmed the ends.

    Weak duplicates (a compiled copy of a weak table the original links from
    elsewhere) are never kept: references to them link to the original copy.
    """
    symbols = list(obj.symbols())
    objects = [s for s in symbols if s["section"] == index and s["type"] == 1 and s["size"]]
    keep = {(s["address"], s["size"]) for s in objects
            if not s["name"].startswith("@") and s["name"] not in duplicates
            and find(s, original, locals_) is not None}
    if base is not None and obj.sections[index]["type"] == 8:
        # Compiler-numbered zero-fill objects (a static initialiser's registration
        # records) are reached through a block base register, not their own
        # relocations; keep one when the original file has a numbered object
        # of the same size at the placed address.
        numbered = {(s["address"], s["size"]) for items in locals_.values() for s in items
                    if s["name"].startswith("@")}
        keep |= {(s["address"], s["size"]) for s in objects
                 if s["name"].startswith("@") and (base + s["address"], s["size"]) in numbered}
    roots = [(f["section"], f["address"], f["address"] + f["size"]) for f, _ in kept]
    # Objects that point at kept code (exception-index entries) are kept too.
    kept_ranges = [(f["section"], f["address"], f["address"] + f["size"]) for f, _ in kept]
    if not objects:
        # No objects to trim by (a .ctors entry): kept whole if it points at kept code.
        if any(where_section == index and any(symbol["section"] == c and a <= symbol["address"] + addend < b
                                              for c, a, b in kept_ranges)
               for where_section, _, _, symbol, addend in relocations(obj)):
            return 0, obj.sections[index]["size"], []
        return None
    by_section = {}
    for s in symbols:
        if s["type"] == 1 and s["size"] and s["section"] and s["section"] < 0xFF00:
            by_section.setdefault(s["section"], []).append(s)
    for where_section, where, _, symbol, addend in relocations(obj):
        target = symbol["address"] + addend
        if any(symbol["section"] == c and a <= target < b for c, a, b in kept_ranges):
            for s in by_section.get(where_section, []):
                if s["address"] <= where < s["address"] + s["size"] and s["name"] not in duplicates:
                    if where_section == index:
                        keep.add((s["address"], s["size"]))
                    roots.append((where_section, s["address"], s["address"] + s["size"]))
    visited = set()
    while roots:
        section, start, end = roots.pop()
        for where_section, where, _, symbol, addend in relocations(obj):
            if where_section != section or not start <= where < end:
                continue
            target = symbol["address"] + addend
            if symbol["section"] != index:
                # Follow kept data through other sections too (a class's
                # virtual table -> its .sdata type record -> its hierarchy).
                for s in by_section.get(symbol["section"], []):
                    key = (symbol["section"], s["address"])
                    if s["address"] <= target < s["address"] + s["size"] and key not in visited:
                        visited.add(key)
                        roots.append((symbol["section"], s["address"], s["address"] + s["size"]))
                continue
            for s in objects:
                if (s["address"] <= target < s["address"] + s["size"] and (s["address"], s["size"]) not in keep
                        and s["name"] not in duplicates):
                    keep.add((s["address"], s["size"]))
                    roots.append((index, s["address"], s["address"] + s["size"]))
    if not keep:
        return None
    start, end = min(a for a, _ in keep), max(a + n for a, n in keep)
    # Records of dropped code between kept ones (exception tables of a
    # discarded function or weak duplicate) become stripped objects.
    inner = sorted((s["address"], s["name"]) for s in objects
                   if start <= s["address"] < end and (s["address"], s["size"]) not in keep)
    return start, end, [name for _, name in inner]


def compare_functions(obj, original, file_index):
    """Print compiled vs original function layout to diagnose a failed match."""
    locals_ = original.locals_of(file_index)
    for symbol in sorted(obj.symbols(), key=lambda s: s["address"]):
        if symbol["type"] != 2 or not symbol["section"]:
            continue
        found = locals_.get(symbol["name"], []) if symbol["binding"] == 0 else original.globals(symbol["name"])
        where = ", ".join(f"{s['address']:#x} size {s['size']}" for s in found) or "absent"
        flag = "" if len(found) == 1 and found[0]["size"] == symbol["size"] else "  <--"
        print(f"  +{symbol['address']:#06x} size {symbol['size']:5}  {symbol['name']:40} original: {where}{flag}")


def adapted_from(like, reference, unit_id, config=CONFIG):
    """The provenance a --like template gives a drafted unit. Only a template
    from outside this project (a Rising Sun manifest) is a source the unit was
    adapted from; a template from config/ only lends compiler settings, and
    re-porting a unit keeps its own recorded provenance."""
    if "id" not in reference:
        return None
    if not like.resolve().is_relative_to(config.resolve()):
        return {"repository": RISING_SUN, "manifest": like.name}
    if reference["id"] == unit_id:
        return reference.get("adapted_from")
    return None


def draft(args):
    reference = json.loads(Path(args.like).read_text()) if args.like else {}
    unit = {"id": args.id, "source": args.source, "category": args.category,
            "compiler_family": "GC", "compiler_version": args.compiler or reference["compiler_version"],
            "compiler_flags": reference.get("compiler_flags", []), "include_dirs": reference.get("include_dirs", []),
            "strip_unused": False}
    if "source_root" in reference:
        unit["source_root"] = reference["source_root"]
    if args.define:
        names = tuple(f"-D{d.split('=')[0]}=" for d in args.define)
        unit["compiler_flags"] = [f for f in unit["compiler_flags"] if not f.startswith(names)]
        unit["compiler_flags"] += [f"-D{d}" for d in args.define]
    _, elf = load_target()
    original = Original(elf)
    file_index = None if args.no_file_record else original.file_index(args.original_file or Path(args.source).name, args.file_index)
    work = ROOT / "scratch/port" / args.id
    work.mkdir(parents=True, exist_ok=True)
    compiler, wrapper = setup_compiler(unit["compiler_version"], "GC")
    compile_sdk(unit, compiler, wrapper, work, lambda stage, *a: run(a, work, work / f"{stage}.log"))
    obj = Elf32((work / "compiled.o").read_bytes())
    try:
        addresses, externals, layout, discarded, pool = match(obj, original, file_index, set(args.weak_duplicate),
                                                             set(args.pool))
        kept = [k for kept_, _ in layout.values() for k in kept_]
    except ValueError:
        compare_functions(obj, original, file_index)
        raise
    sections, stripped = [], []
    dropped = discarded or args.weak_duplicate
    for section in obj.sections:
        if not section["flags"] & 2 or not section["size"] or section["name"] in args.pool:
            continue
        if section["name"] not in addresses:
            if dropped and not referenced(obj, section["index"], kept, addresses, set(args.weak_duplicate)):
                stripped.append(section["name"])
                continue
            raise ValueError(f"Could not place compiled section {section['name']}")
        if section["index"] in layout:
            if not layout[section["index"]][0]:
                raise ValueError(f"Code section {section['name']} has no original function")
            sections.append({"name": section["name"], "address": hex(addresses[section["name"]]),
                             "size": layout[section["index"]][1], "type": 1})
            continue
        span = kept_span(obj, section["index"], kept, original, original.locals_of(file_index),
                         addresses[section["name"]], set(args.weak_duplicate))
        if span is None and dropped:
            stripped.append(section["name"])
            continue
        start, end, inner = span if span and dropped else (0, section["size"], [])
        inner_size = 0
        if inner:
            bounds, flags = section_chunks(obj, section, inner)
            inner_size = sum(b - a for (a, b), f in zip(bounds, flags) if f)
        entry = {"name": section["name"], "address": hex(addresses[section["name"]] + start),
                 "size": end - start - inner_size, "type": section["type"]}
        if (start, end) != (0, section["size"]) or inner:
            entry.update(linked_offset=start, linked_size=section["size"])
        if inner:
            entry["stripped_objects"] = inner
        if section["alignment"] == 8 and addresses[section["name"]] % 8 == 4:
            entry["input_alignment"] = 4  # MW's linker placed it at 4 mod 8.
        sections.append(entry)
    code = [(int(c["address"], 16), int(c["address"], 16) + c["size"]) for c in sections if c["name"] in CODE_SECTIONS]
    functions = [{"name": s["name"], "address": hex(s["address"]), "size": s["size"], "binding": s["binding"]}
                 for s in original.symbols if s["type"] == 2 and s["section"]
                 and any(a <= s["address"] < b for a, b in code)]
    obj_symbols = list(obj.symbols())
    undefined = sorted({s["name"] for s in obj_symbols if s["binding"] and not s["section"] and s["name"]}
                       | set(args.weak_duplicate))
    for name in undefined:
        if name not in externals and len(original.globals(name)) == 1:
            externals[name] = original.globals(name)[0]["address"]
    unit["strip_unused"] = bool(discarded or args.weak_duplicate)
    locals_ = original.locals_of(file_index)
    sda = sorted({symbol["name"] for _, _, kind, symbol, _ in relocations(obj) if kind == SDA21 and not symbol["section"]})
    unit.update(
        sections=sections, functions=functions,
        externals={name: hex(externals[name]) for name in undefined if name in externals},
        undefined_in_discarded_code=[name for name in undefined if name not in externals],
        discarded_functions=[f["name"] for f in discarded],
        upstream=dict(reference.get("upstream", {}), **({"path": args.upstream_path} if args.upstream_path else {})),
        local_externals=sorted(n for n in externals if n in undefined and n in locals_ and not original.globals(n)),
        discarded_local_functions=[f["name"] for f in discarded if f["binding"] == 0], sda_externals=sda)
    if file_index is not None:
        unit.update(original_file=original.symbols[file_index]["name"], original_file_index=file_index)
    else:
        unit["discarded_local_functions"] = []
    if stripped:
        unit["stripped_sections"] = stripped
    if args.weak_duplicate:
        unit["weak_duplicates"] = sorted(args.weak_duplicate)
    if args.pool:
        unit["pooled_sections"] = args.pool
        unit["pool_references"] = [{"section": name, "offset": start, "size": size, "address": hex(address)}
                                   for (name, start, size), address in sorted(pool.items())]
    if args.like:
        adapted = adapted_from(Path(args.like), reference, args.id)
        if adapted:
            unit["adapted_from"] = adapted
    output = CONFIG / f"{args.id}.json"
    output.write_text(json.dumps(unit, indent=2) + "\n")
    print(f"Drafted {output.relative_to(ROOT)}: {len(functions)} functions, "
          f"{sum(f['size'] for f in functions)} code bytes at " + ", ".join(hex(a) for a, _ in code))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("id")
    parser.add_argument("source", help="Source path relative to the repository root")
    parser.add_argument("--like", help="Existing manifest to copy compiler settings and provenance from")
    parser.add_argument("--original-file", help="Original file-record name (default: source basename)")
    parser.add_argument("--file-index", type=int)
    parser.add_argument("--no-file-record", action="store_true",
                        help="The original has no file record for this source (only global symbols)")
    parser.add_argument("--compiler")
    parser.add_argument("--define", action="append", help="Replace or add a -D definition, e.g. SDK_REVISION=0")
    parser.add_argument("--category", default="restored_library")
    parser.add_argument("--weak-duplicate", action="append", default=[],
                        help="A weak function or data object (a vtable) this unit compiles whose original copy came from another file")
    parser.add_argument("--pool", action="append", default=[],
                        help="A compiler data section whose items link at their original pool addresses")
    parser.add_argument("--upstream-path", help="Path of the source in the upstream repository")
    draft(parser.parse_args())
