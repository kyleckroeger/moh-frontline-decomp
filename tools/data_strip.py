"""Emulate MW's per-object data dead-stripping on a compiled object.

SN's linker cannot drop single objects from a section. Each object a manifest
section names in `stripped_objects` is moved, with its trailing padding, to the
end of its compiler section. The existing trimmed-tail checks then keep it out
of the compared and credited bytes and reject retained references into it.

Relocations located in bytes the original linker removed (parked sections,
trimmed slices and stripped objects) are pointed at their own section, so that
dead data does not keep discarded functions alive in SN's link.

A unit may list `weak_duplicates`: weak functions, or weak data objects such
as a class's virtual table, that it compiles but whose copy in the original
came from another file, as MW's linker kept the first weak definition and
dropped later ones. A listed virtual table may be compiled global (the unit
defines a weak destructor out of line, which makes it the key function) as
long as the original copy is weak. A unit may also list `file_duplicates`:
functions or data objects of any binding it compiles (a static helper it inlines, or a global
function of the same file that a recursive inline reaches) whose original copy
another unit of the same original source file places. The compiled copy is
renamed `<name>$duplicate` and every relocation to it is pointed at a new
undefined symbol of the original name,
which the unit resolves to the retained copy; SN's unused-code stripping then
drops a renamed function, and a data section holding only renamed objects is
a stripped section.

A unit may list `pooled_sections`: compiler data sections whose objects the
original file shares with code outside the unit (a pool of `@N` constants or
strings), so the unit cannot place the section itself. Each compiler object
that retained code refers to is listed in `pool_references` with the original
address of identical bytes. Its references are pointed at a new undefined
symbol, `__pool_<address>`, which the unit resolves to that address, and the
compiled section is parked like a stripped one. The build compares every
listed object with the original bytes; pool bytes earn no credit.

A manifest section may set `input_alignment` to 4 when the original linker
placed an 8-aligned compiler data section at an address that is 4 mod 8 (MW's
linker did; SN's would pad the start or the size). Zero-initialized data may go
lower, to 2 or 1, when every object in it allows that alignment: MW packs
one-byte function-static guards at odd `.sbss` addresses. Only the section
header's alignment changes; the bytes are still compared with the original.

The rewrite permutes bytes and keeps the file size (apart from the symbol
tables that weak duplicates and pool references move); the original compiler
output is still what the build report hashes.
"""
import re
import struct

from formats import Elf32

STT_OBJECT, STT_SECTION = 1, 3
R_PPC_ADDR32 = 1
SECTION_HEADER_SIZE, ALIGNMENT_FIELD = 40, 32
STB_GLOBAL, STB_WEAK, STT_FUNC = 1, 2, 2
DUPLICATE_SUFFIX = "$duplicate"


def append_symbols(data, obj, renames, additions):
    """Rename symbols and append undefined globals; return (data, new indices).

    The symbol and string tables are rewritten after the end of the file
    (section headers record where), so no other offset moves. `renames` maps
    a symbol index to a new name; `additions` lists (name, type) pairs, where
    a name may be a string-table offset to reuse.
    """
    symtab = next(s for s in obj.sections if s["type"] == 2)
    strtab = obj.sections[symtab["link"]]
    table = bytearray(obj.contents(symtab))
    strings = bytearray(obj.contents(strtab))
    for index, name in renames.items():
        struct.pack_into(">I", table, index * 16, len(strings))
        strings += name.encode() + b"\0"
    indices = []
    for name, kind in additions:
        if isinstance(name, str):
            offset = len(strings)
            strings += name.encode() + b"\0"
        else:
            offset = name
        indices.append(len(table) // 16)
        table += struct.pack(">IIIBBH", offset, 0, 0, 1 << 4 | kind, 0, 0)
    out = bytearray(data)
    section_headers = struct.unpack_from(">I", data, 32)[0]
    for section, contents in ((symtab, table), (strtab, strings)):
        out += bytes(-len(out) % 4)
        header = section_headers + section["index"] * SECTION_HEADER_SIZE
        struct.pack_into(">II", out, header + 16, len(out), len(contents))
        out += contents
    return out, indices


def redirect_relocations(data, obj, out, target):
    """Point relocations at new symbols: target(symbol index, addend) -> (index, addend) or None."""
    for rela in obj.sections:
        if rela["type"] != 4:
            continue
        for entry in range(rela["offset"], rela["offset"] + rela["size"], 12):
            _, info, addend = struct.unpack_from(">IIi", data, entry)
            redirect = target(info >> 8, addend)
            if redirect is not None:
                struct.pack_into(">Ii", out, entry + 4, redirect[0] << 8 | info & 255, redirect[1])


def redirect_weak_duplicates(data, unit):
    """Rename each listed weak copy and refer to the original name instead."""
    names = unit.get("weak_duplicates", []) + unit.get("file_duplicates", [])
    if not names:
        return data
    obj = Elf32(data)
    symbols = list(obj.symbols())
    symtab = next(s for s in obj.sections if s["type"] == 2)
    found = []
    for name in names:
        matches = [i for i, s in enumerate(symbols) if s["name"] == name and s["section"]]
        symbol = symbols[matches[0]] if len(matches) == 1 else None
        code = symbol is not None and obj.sections[symbol["section"]]["name"] in (".text", ".init")
        # A virtual table may be compiled global: defining a weak destructor out
        # of line makes it the class's key function. The original copy must
        # still be weak (project_build checks that), and the compiled table is
        # dropped like any other duplicate.
        vtable = symbol is not None and name.startswith("__vt__") and (symbol["type"], code) == (STT_OBJECT, False)
        # A file duplicate is a function or data object (such as a class's
        # file-local RTTI record) of any binding whose original copy another
        # unit of the same original source file places.
        if name in unit.get("file_duplicates", []):
            if symbol is None or (symbol["type"], code) not in ((STT_FUNC, True), (STT_OBJECT, False)):
                raise ValueError(f"File duplicate is not one compiled function or data object: {name}")
            found.append(matches[0])
            continue
        if (symbol is None or symbol["binding"] not in ((STB_WEAK, STB_GLOBAL) if vtable else (STB_WEAK,))
                or (symbol["type"], code) not in ((STT_FUNC, True), (STT_OBJECT, False))):
            raise ValueError(f"Weak duplicate is not one compiled weak function or data object: {name}")
        found.append(matches[0])
    # The new global reuses the original name's string-table offset.
    original_names = [struct.unpack_from(">I", obj.contents(symtab), i * 16)[0] for i in found]
    out, added = append_symbols(data, obj, {i: symbols[i]["name"] + DUPLICATE_SUFFIX for i in found},
                                [(offset, symbols[i]["type"]) for offset, i in zip(original_names, found)])
    redirect = dict(zip(found, added))
    redirect_relocations(data, obj, out, lambda index, addend: (redirect[index], addend) if index in redirect else None)
    return bytes(out)


def pool_symbol(address):
    """The undefined symbol a pooled item's references use."""
    return f"__pool_{int(address, 16):08x}"


def pool_item(obj, section, offset):
    """Size of the compiler object (an `@N` constant or string) starting at an offset."""
    starting = [s for s in obj.symbols() if s["section"] == section["index"] and s["type"] == STT_OBJECT
                and s["size"] and s["address"] == offset]
    if len(starting) != 1:
        raise ValueError(f"Pool item is not one compiler object: {section['name']}+{offset:#x}")
    return starting[0]["size"]


def redirect_pool_references(data, unit):
    """Refer to each listed pool item's original address instead of its compiled copy."""
    pooled = unit.get("pooled_sections", [])
    if not pooled:
        return data
    obj = Elf32(data)
    symbols = list(obj.symbols())
    by_name = {s["name"]: s for s in obj.sections}
    items = {}  # section index -> [(start, end, address)]
    for reference in unit.get("pool_references", []):
        if reference["section"] not in pooled or reference["section"] not in by_name:
            raise ValueError(f"Pool reference is outside a pooled section: {reference['section']}")
        start = reference["offset"]
        items.setdefault(by_name[reference["section"]]["index"], []).append(
            (start, start + reference["size"], reference["address"]))
    addresses = sorted({address for group in items.values() for _, _, address in group})
    out, added = append_symbols(data, obj, {}, [(pool_symbol(a), 0) for a in addresses])
    symbol_of = dict(zip(addresses, added))

    def target(index, addend):
        symbol = symbols[index]
        if symbol["section"] not in items:
            return None
        offset = symbol["address"] + addend
        for start, end, address in items[symbol["section"]]:
            if start <= offset < end:
                return symbol_of[address], offset - start
        return None  # Discarded code's references stay with the parked section.
    redirect_relocations(data, obj, out, target)
    return bytes(out)


def natural_alignment(size):
    """Largest power of two up to 8 dividing an object's size."""
    alignment = 1
    while alignment < 8 and size % (alignment * 2) == 0 and size:
        alignment *= 2
    return alignment


def c_identifier(name):
    """MW numbers function statics (`name$123`); manifests use the C name."""
    match = re.fullmatch(r"([A-Za-z_]\w*)[$.]\d+", name)
    return match.group(1) if match else name


def section_chunks(obj, section, names):
    """Split a section at object starts; return (chunks, stripped flags)."""
    objects = sorted((s["address"], s["size"], c_identifier(s["name"])) for s in obj.symbols()
                     if s["section"] == section["index"] and s["type"] == STT_OBJECT and s["size"])
    starts = sorted({a for a, _, _ in objects} | {0})
    bounds = list(zip(starts, starts[1:] + [section["size"]]))
    stripped = [False] * len(bounds)
    for name in names:
        found = [o for o in objects if o[2] == name]
        if len(found) != 1:
            raise ValueError(f"Stripped object is not one compiler object: {name}")
        address, size, _ = found[0]
        if sum(1 for o in objects if o[0] == address) != 1:
            raise ValueError(f"Stripped object shares its address: {name}")
        index = starts.index(address)
        if address + size > bounds[index][1]:
            raise ValueError(f"Stripped object overlaps the next object: {name}")
        stripped[index] = True
    return bounds, stripped


def offset_map(bounds, stripped, size):
    """Kept chunks keep their order; stripped chunks follow them at the end."""
    order = [i for i, s in enumerate(stripped) if not s] + [i for i, s in enumerate(stripped) if s]
    new_start, cursor = {}, 0
    for i in order:
        new_start[i] = cursor
        cursor += bounds[i][1] - bounds[i][0]

    def remap(offset):
        if offset == size:
            return size
        for i, (start, end) in enumerate(bounds):
            if start <= offset < end:
                return new_start[i] + offset - start
        raise ValueError("Offset lies outside its section")
    return order, remap


def strip_objects(data, unit):
    """Return a rewritten object for linking, or the input when nothing applies."""
    data = redirect_pool_references(redirect_weak_duplicates(data, unit), unit)
    obj = Elf32(data)
    out = bytearray(data)
    by_name = {s["name"]: s for s in obj.sections}
    remaps, dead = {}, {}  # section index -> remap function / dead [start, end) ranges
    for manifest in unit["sections"]:
        names = manifest.get("stripped_objects", [])
        section = by_name[manifest["name"]]
        if names:
            if not unit["strip_unused"] or manifest["name"] in (".text", ".init"):
                raise ValueError("Object stripping requires native unused-code stripping of data")
            bounds, stripped = section_chunks(obj, section, names)
            order, remap = offset_map(bounds, stripped, section["size"])
            remaps[section["index"]] = remap
            if section["type"] != 8:
                old = obj.contents(section)
                out[section["offset"]:section["offset"] + section["size"]] = b"".join(
                    old[bounds[i][0]:bounds[i][1]] for i in order)
        start = manifest.get("linked_offset", 0)
        if manifest["name"] not in (".text", ".init"):
            dead[section["index"]] = [(0, start), (start + manifest["size"], section["size"])]
    for name in unit.get("stripped_sections", []) + unit.get("pooled_sections", []):
        dead[by_name[name]["index"]] = [(0, by_name[name]["size"])]

    section_headers = struct.unpack_from(">I", data, 32)[0]
    for manifest in unit["sections"]:
        if "input_alignment" in manifest:
            section = by_name[manifest["name"]]
            relaxed = manifest["input_alignment"]
            if manifest["name"] in (".text", ".init") or relaxed not in (1, 2, 4) or relaxed >= section["alignment"]:
                raise ValueError("Input alignment may only relax compiler data alignment")
            if relaxed < 4:
                # Below 4 only for zero-initialized data whose objects allow it
                # (one-byte function-static guards, which MW packs at odd addresses).
                natural = max((natural_alignment(o["size"]) for o in obj.symbols()
                               if o["section"] == section["index"] and o["type"] == STT_OBJECT), default=1)
                if section["type"] != 8 or natural > relaxed:
                    raise ValueError("Input alignment is below the section objects' natural alignment")
            elif section["alignment"] != 8:
                raise ValueError("Input alignment may only relax 8-aligned compiler data to 4")
            struct.pack_into(">I", out, section_headers + section["index"] * SECTION_HEADER_SIZE + ALIGNMENT_FIELD, relaxed)

    symbol_table = next(s for s in obj.sections if s["type"] == 2)
    for i, symbol in enumerate(obj.symbols()):
        remap = remaps.get(symbol["section"])
        if remap and symbol["type"] != STT_SECTION:
            struct.pack_into(">I", out, symbol_table["offset"] + i * 16 + 4, remap(symbol["address"]))
    symbols = list(obj.symbols())
    section_symbol = {s["section"]: i for i, s in enumerate(symbols) if s["type"] == STT_SECTION}
    for rela in obj.sections:
        if rela["type"] != 4:
            continue
        target = rela["info"]
        for entry in range(rela["offset"], rela["offset"] + rela["size"], 12):
            location, info, addend = struct.unpack_from(">IIi", data, entry)
            symbol = symbols[info >> 8]
            if target in remaps:
                location = remaps[target](location)
            if symbol["type"] == STT_SECTION and symbol["section"] in remaps:
                addend = remaps[symbol["section"]](addend)
            if any(a <= location < b for a, b in dead.get(target, [])):
                # The original link removed these bytes. SN rejects R_PPC_NONE,
                # so refer to the dead section itself instead of a live symbol.
                if info & 255 != R_PPC_ADDR32:
                    raise ValueError("Unsupported relocation in stripped data")
                info, addend = section_symbol[target] << 8 | R_PPC_ADDR32, 0
            struct.pack_into(">IIi", out, entry, location, info, addend)
    return bytes(out)
