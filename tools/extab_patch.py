"""Apply the listed extab padding patches to a compiled object before linking.

MWCC leaves the two alignment bytes of some exception actions uninitialized:
the bytes after the registers of a new/new[] cleanup (DELETEPOINTERCOND) and
after the unused byte of a 32-bit catch block, each before a 4-byte pointer.
The original build has other leftover values there, and no compiler version
or source form reproduces them. The user approved (2026-10-27) patching only
those two bytes, one function at a time, from config/GMFE69/extab_patches.json.

Each entry names its unit, the function (symbol name), the offset of the two
bytes from the start of the function's extab record, the action byte, and the bytes our
compiler emits there and the original's bytes. The record is found through
the function's extabindex entry, and its range table and action lists are
walked as the runtime walks them: the offset must be the padding field of a
reachable DELETEPOINTERCOND (11) or CATCHBLOCK_32 (16) action, and the bytes
there must be exactly the listed compiler output, or the build fails. The
patched bytes are then verified like every other byte of the image.
"""
import json
import struct

from formats import Elf32
from setup import CONFIG

PATCHES = CONFIG / "extab_patches.json"
# Action type -> record size, as ExPPC_NextAction advances (15 is variable).
ACTION_SIZES = {2: 8, 3: 12, 4: 8, 5: 12, 6: 12, 7: 12, 8: 16, 9: 20, 10: 8, 11: 12, 12: 12, 13: 4, 16: 16}
# Patchable action type -> offset of its two padding bytes before the pointer.
PADDING = {11: 6, 16: 2}


def load_patches(units):
    """The patch list by unit id; every entry must name a unit function."""
    entries = json.loads(PATCHES.read_text())["patches"] if PATCHES.exists() else []
    functions = {u["id"]: {f["name"] for f in u["functions"]} for u in units}
    by_unit, seen = {}, set()
    for entry in entries:
        key = (entry["unit"], entry["function"], entry["offset"])
        if entry["unit"] not in functions or entry["function"] not in functions[entry["unit"]]:
            raise ValueError(f"Extab patch names no unit function: {entry['unit']} {entry['function']}")
        if key in seen:
            raise ValueError(f"Duplicate extab patch: {key}")
        seen.add(key)
        compiled, original = bytes.fromhex(entry["compiled"]), bytes.fromhex(entry["original"])
        if entry["action"] not in ("0x8b", "0x90"):
            raise ValueError(f"Extab patch is not for a new/new[] cleanup action: {key}")
        if len(compiled) != 2 or len(original) != 2 or compiled == original:
            raise ValueError(f"Extab patch must change exactly two bytes: {key}")
        by_unit.setdefault(entry["unit"], []).append(entry)
    return by_unit


def relocation_targets(obj, symbols, section_index):
    """{offset: (symbol section, symbol address + addend)} for one section's relocations."""
    targets = {}
    for section in obj.sections:
        if section["type"] != 4 or section["info"] != section_index:
            continue
        data = obj.contents(section)
        for offset in range(0, len(data), 12):
            where, info, addend = struct.unpack_from(">IIi", data, offset)
            symbol = symbols[info >> 8]
            targets[where] = (symbol["section"], symbol["address"] + addend)
    return targets


def extab_record(obj, function):
    """(extab section, record offset, record size) for a function of a compiled object."""
    symbols = list(obj.symbols())
    by_name = {s["name"]: s for s in obj.sections}
    defined = [s for s in symbols if s["name"] == function and s["type"] == 2 and s["section"]]
    if len(defined) != 1 or "extab" not in by_name or "extabindex" not in by_name:
        raise ValueError(f"Extab patch function has no unique compiled definition and extab: {function}")
    index, extab = by_name["extabindex"], by_name["extab"]
    targets = relocation_targets(obj, symbols, index["index"])
    if index["size"] % 12:
        raise ValueError("Unexpected extabindex layout")
    entries = [targets.get(offset + 8) for offset in range(0, index["size"], 12)
               if targets.get(offset) == (defined[0]["section"], defined[0]["address"])]
    if len(entries) != 1 or entries[0] is None or entries[0][0] != extab["index"]:
        raise ValueError(f"Extab patch function has no unique extabindex entry: {function}")
    start = entries[0][1]
    records = [s for s in symbols if s["section"] == extab["index"] and s["type"] == 1 and s["address"] == start]
    if len(records) != 1 or not records[0]["size"] or start + records[0]["size"] > extab["size"]:
        raise ValueError(f"Extab patch function's record is not one compiler object: {function}")
    return extab, start, records[0]["size"]


def record_actions(record):
    """{offset: action type} of every action the record's code ranges reach."""
    field, = struct.unpack_from(">H", record, 0)
    large = field >> 3 & 1
    position, chains = (4 if large else 2), []
    while True:
        if large:
            start, _, action = struct.unpack_from(">IHH", record, position)
            position += 8
        else:
            start, _, action = struct.unpack_from(">HHH", record, position)
            position += 6
        if start == 0:
            break
        chains.append(action)
    actions = {}
    for offset in chains:
        while offset not in actions:
            if offset + 4 > len(record):
                raise ValueError("Extab action outside its record")
            kind = record[offset] & 0x7F
            if kind == 1:  # A branch continues the list elsewhere in the record.
                offset = struct.unpack_from(">H", record, offset + 2)[0]
                continue
            if kind == 15:
                size = 12 + struct.unpack_from(">H", record, offset + 2)[0] * 4
            elif kind in ACTION_SIZES or kind in (0, 14):
                size = ACTION_SIZES.get(kind, 2)
            else:
                raise ValueError(f"Unknown extab action {kind}")
            if offset + size > len(record):
                raise ValueError("Extab action outside its record")
            actions[offset] = kind
            if kind in (0, 14) or record[offset] & 0x80:
                break
            offset += size
    return actions


def apply_patches(data, patches):
    """Return the compiled object with the unit's listed extab padding replaced."""
    if not patches:
        return data
    obj, out = Elf32(data), bytearray(data)
    for entry in patches:
        extab, start, size = extab_record(obj, entry["function"])
        record = obj.contents(extab)[start:start + size]
        offset = int(entry["offset"], 16)
        # The field must be the padding of a reachable action that has one.
        if not any(PADDING.get(kind) == offset - action and record[action] == int(entry["action"], 16)
                   for action, kind in record_actions(record).items()):
            raise ValueError(f"Extab patch is not the padding of a new/new[] cleanup action: "
                             f"{entry['unit']} {entry['function']} +{offset:#x}")
        found = bytes(record[offset:offset + 2])
        if found != bytes.fromhex(entry["compiled"]):
            raise ValueError(f"Extab patch found {found.hex()}, not the listed compiler output "
                             f"{entry['compiled']}: {entry['unit']} {entry['function']} +{offset:#x}")
        at = extab["offset"] + start + offset
        out[at:at + 2] = bytes.fromhex(entry["original"])
    return bytes(out)
