"""CodeWarrior compilation and shared SN linking with typed small-data externals."""
import re
import struct
from functools import lru_cache

from data_strip import pool_symbol
from formats import Elf32
from setup import ROOT


@lru_cache(maxsize=4)
def external_symbol_index(original):
    """Index the immutable parsed target once for all source-unit dependencies."""
    file_name, definitions = None, {}
    for symbol in original.symbols():
        if symbol["type"] == 4:
            file_name = symbol["name"]
        if symbol["section"]:
            # Weak (2) definitions resolve like globals (1); locals stay file-scoped.
            key = (symbol["name"], min(symbol["binding"], 1), file_name if not symbol["binding"] else None)
            definitions.setdefault(key, []).append(symbol)
    return definitions


def resolve_external(original, unit, name):
    """Resolve a context dependency, including explicitly scoped private symbols."""
    local = name in unit.get("local_externals", [])
    key = (name, 0 if local else 1, unit.get("original_file") if local else None)
    matches = external_symbol_index(original).get(key, [])
    if len(matches) != 1 or matches[0]["address"] != int(unit["externals"][name], 16):
        raise ValueError(f"External is not an original definition: {name}")
    return matches[0]


def stripped_address(index):
    """Outside GameCube main memory, so retained code cannot use it by accident."""
    return 0x10000000 + index * 0x100000


LEADING_FILLER = "__leading_filler"


def compile_sdk(unit, compiler, wrapper, work, execute):
    # A fragment of a larger file whose earlier zero-initialized data sets the
    # alignment phase of its own: a generated prefix declares a filler object
    # of that size first, so the compiler lays out the rest as in the file.
    prefix = []
    if "leading_filler" in unit:
        (work / "leading_filler.h").write_text(f"static char {LEADING_FILLER}[{unit['leading_filler']}];\n")
        prefix = [f"-I{work}", "-prefix", "leading_filler.h"]
    execute("compile", wrapper, compiler / "mwcceppc.exe", "-c",
            *unit["compiler_flags"], *prefix, "-I-",
            *[f"-I{ROOT / path}" for path in unit["include_dirs"]],
            "-ir", ROOT / unit.get("source_root", "src/dolphin"), ROOT / unit["source"], "-o", "compiled.o")


def sdk_link_script(unit, obj, original, tools, work, execute):
    """Zero-size anchors supply ELF section identity, never code or data bytes.

    SN's --fix-sda selects r2/r13 from the final address. SDA21
    relocations also require a section-relative definition, not SHN_ABS.
    Explicit input selectors keep anchors out of compiled data sections.
    """
    symbols = list(obj.symbols())
    small = set()
    for section in obj.sections:
        if section["type"] != 4 or not obj.sections[section["info"]]["flags"] & 2:
            continue
        for offset in range(0, section["size"], 12):
            _, info, _ = struct.unpack_from(">IIi", obj.contents(section), offset)
            symbol = symbols[info >> 8]
            if info & 255 == 109 and symbol["section"] == 0:
                small.add(symbol["name"])
    script = ["SECTIONS {"]
    for section in unit["sections"]:
        name = section["name"]
        base = int(section["address"], 16) - section.get("linked_offset", 0)
        script.append(f"{name} {base:#x} : {{ input.o({name}) }}")
    # SN's linker has no /DISCARD/. Park sections the original linker
    # dead-stripped outside the image; they never enter the rebuilt context.
    # Pooled sections (data_strip.py) are parked the same way.
    parked = unit.get("stripped_sections", []) + unit.get("pooled_sections", [])
    for index, name in enumerate(parked):
        script.append(f"{name} {stripped_address(index):#x} : {{ input.o({name}) }}")
    originals = list(original.symbols())
    # A pooled item links at the original address of its identical bytes.
    pool = {pool_symbol(r["address"]): r["address"] for r in unit.get("pool_references", [])}
    for index, (name, address) in enumerate(list(unit["externals"].items()) + sorted(pool.items())):
        if name not in small:
            # Template names such as offsetPtr<v>__FRPvi need quoting in the script.
            symbol = f'"{name}"' if not re.fullmatch(r"[A-Za-z0-9_.$@]+", name) else name
            script.append(f"{symbol} = {address};")
            continue
        if name in pool:
            section = next(s for s in original.sections if s["flags"] & 2 and s["type"] in (1, 8)
                           and s["address"] <= int(address, 16) < s["address"] + s["size"])
        else:
            section = original.sections[resolve_external(original, unit, name)["section"]]
        flags = "a" + ("w" if section["flags"] & 1 else "")
        kind = "nobits" if section["type"] == 8 else "progbits"
        anchor = f"anchor{index}"
        (work / f"{anchor}.s").write_text(
            f'.section {section["name"]},"{flags}",@{kind}\n.global {name}\n{name}:\n')
        execute(anchor, tools / "powerpc-eabi-as", f"{anchor}.s", "-o", f"{anchor}.o")
        anchor_obj = Elf32((work / f"{anchor}.o").read_bytes())
        if any(s["size"] for s in anchor_obj.sections if s["flags"] & 2):
            raise ValueError("External anchor unexpectedly contains bytes")
        script.append(f"{section['name']}.{anchor} {address} : {{ {anchor}.o({section['name']}) }}")
    for name in ("_SDA_BASE_", "_SDA2_BASE_"):
        value = next(s["address"] for s in originals if s["name"] == name)
        script.append(f"{name} = {value:#x};")
    return "\n".join(script + ["}"]) + "\n"
