#!/usr/bin/env python3
"""bss_decls.py: write definitions for the original's zero-initialised globals no source defines.

PortReadiness step 2.1. A pure-source link needs every .bss and .sbss global defined
somewhere. This takes the ones source_symbols.py reports missing (function statics,
NAME$123, aren't counted: they come with their function), finds the original file each
belongs to (build/audit/file_map.json), and writes one file per original file:

    research/data/bss/<original file>.cpp

Types come from the extern declarations that drafts and units already use (the most
common spelling wins), or from a class's static member declaration. A type no source
spells out becomes a byte view sized from the original ("struct T { unsigned char
data[N]; }"): the name is kept, the layout waits for the shared headers (step 3 of
PortReadiness). check_data.py then checks every definition's section and size.

The output is a starting point, written once: edit the files by hand afterwards (better
types, comments). Rerunning refuses to overwrite a file unless --force is given.

    bss_decls.py [--force]        write research/data/bss/
    bss_decls.py --dry-run        print what would be written
"""
import argparse, collections, csv, glob, json, re, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from formats import Elf32  # noqa: E402

ORIGINAL = ROOT / "orig/GMFE69/Moh2RelGC.elf"
SYMBOLS = ROOT / "research/layout/source_symbols.tsv"
FILE_MAP = ROOT / "build/audit/file_map.json"
OUT = ROOT / "research/data/bss"
BUILTIN = {"char", "short", "int", "long", "float", "double", "bool", "void", "unsigned", "signed"}
QUALIFIERS = {"const", "volatile", "struct", "class", "union", "enum", "static"}
SCALAR_TYPEDEFS = {  # Dolphin SDK and EA spellings
    "u8": "unsigned char", "s8": "signed char", "u16": "unsigned short", "s16": "short",
    "u32": "unsigned long", "s32": "long", "u64": "unsigned long long", "s64": "long long",
    "f32": "float", "f64": "double", "BOOL": "int", "uint": "unsigned int",
}


def original_symbols():
    elf = Elf32(ORIGINAL.read_bytes())
    names = {i: s["name"] for i, s in enumerate(elf.sections)}
    out = collections.defaultdict(list)
    for s in elf.symbols():
        if s["name"] and 0 < s["section"] < 0xFF00 and names[s["section"]] in (".bss", ".sbss"):
            out[s["name"]].append(dict(s, section_name=names[s["section"]]))
    return out


def owner_of(files, section, address):
    for f in files:
        for r in f["ranges"]:
            if r["section"] == section and int(r["start"], 16) <= address < int(r["end"], 16):
                return f["name"]
    return None


def sources():
    paths = glob.glob(str(ROOT / "src/**/*.c*"), recursive=True) + glob.glob(str(ROOT / "research/drafts/*.c*"))
    # Without comments: prose like "static so this fragment can refer to it." isn't a type.
    strip = re.compile(r"//[^\n]*|/\*.*?\*/", re.S)
    return {p: strip.sub(" ", open(p, errors="replace").read()) for p in paths}


def demangle_static(name):
    """member__<len>Class -> (member, Class); None for a plain name."""
    m = re.match(r"^(\w+?)__(\d+)(\w+)$", name)
    if m and len(m[3]) == int(m[2]):
        return m[1], m[3]
    return None


def declared_type(name, texts):
    """The most common (type, array suffix) source gives this global, or None."""
    member = demangle_static(name)
    if member:
        field, _ = member
        pat = re.compile(r"\bstatic\s+([^;(){}=]*?)\b" + re.escape(field) + r"\s*((?:\[[^\]]*\])*)\s*;")
    else:
        pat = re.compile(r"\bextern\s+(?:\"C\"\s+)?([^;(){}=]*?)\b" + re.escape(name) + r"\s*((?:\[[^\]]*\])*)\s*;")
    seen = collections.Counter()
    for text in texts.values():
        for m in pat.finditer(text):
            seen[(" ".join(m[1].split()), m[2])] += 1
    return seen.most_common(1)[0][0] if seen else None


def typedef_line(name, texts):
    """A one-line typedef of `name` from source, if one exists."""
    pat = re.compile(r"^\s*typedef\s[^;{}]*(?:\b" + re.escape(name) + r"\s*;|\(\s*\*\s*" + re.escape(name) + r"\s*\)\s*\([^;{}]*;)", re.M)
    for text in texts.values():
        m = pat.search(text)
        if m:
            return " ".join(m[0].split())
    return None


def base_words(type_text):
    return [w for w in re.findall(r"[A-Za-z_]\w*", type_text) if w not in QUALIFIERS]


def element_size(type_text):
    if "*" in type_text:
        return 4
    words = set(base_words(type_text))
    for typedef, builtin in SCALAR_TYPEDEFS.items():
        if typedef in words:
            words = set(builtin.split())
    if "double" in words or words == {"long", "long"} or "long long" in type_text:
        return 8
    if "short" in words:
        return 2
    if "char" in words or "bool" in words:
        return 1
    if words & {"int", "long", "float", "unsigned", "signed"}:
        return 4
    return None


def definition(sym, typed, texts, prelude, copies=1):
    """One line defining `sym` (an original symbol), adding what it needs to prelude."""
    name, size = sym["name"], sym["size"]
    member = demangle_static(name)
    field, cls = member if member else (name, None)
    # A file static stays static only where several original files have one by that name.
    # Otherwise it is global: the original file is split into fragments (units, drafts)
    # that reach it with extern, until they are merged back into one file.
    storage = "static " if sym["binding"] == 0 and copies > 1 else ""
    note = ""
    if typed:
        type_text, array = typed
        type_text = " ".join(w for w in type_text.split() if w not in ("static", "extern"))
        words = base_words(type_text)
        unknown = [w for w in words if w not in BUILTIN and w not in SCALAR_TYPEDEFS]
        for w in words:
            if w in SCALAR_TYPEDEFS:
                prelude.setdefault(w, f"typedef {SCALAR_TYPEDEFS[w]} {w};")
        if array.strip() == "[]" or not array:
            known = element_size(type_text)
            if array.strip() == "[]":
                array = f"[{size // known}]" if known else ""
        if unknown and "*" in type_text:
            for w in unknown:
                prelude.setdefault(w, typedef_line(w, texts) if typedef_line(w, texts) and "(" not in typedef_line(w, texts) else f"struct {w};")
        elif unknown:
            w = unknown[-1]
            line = typedef_line(w, texts)
            if line and re.search(r"\(\s*\*\s*" + re.escape(w) + r"\s*\)\s*\(", line) and len(unknown) == 1:
                prelude.setdefault(w, f"typedef void (*{w})(); // view: the real signature is in the source that calls it")
            else:
                count = 1
                for n in re.findall(r"\[(\d+)\]", array):
                    count *= int(n)
                each = size // max(count, 1)
                type_text = f"struct {w}" if "struct" in type_text.split() else w
                prelude.setdefault(w, f"struct {w} {{ unsigned char data[{each:#x}]; }}; // view: size from the original")
        decl = f"{type_text} {field}{array}"
    else:
        decl, note = f"unsigned char {field}[{size:#x}]", " // no source names its type yet"
    if cls:
        prelude.setdefault("class " + cls, [])  # members gathered, written by render()
        prelude["class " + cls].append(f"static {decl};")
        return f"{decl.replace(field, cls + '::' + field, 1)};{note}"
    return f"{storage}{decl};{note}"


def render(key, value):
    if isinstance(value, list):
        return f"{key} {{\npublic:\n" + "".join(f"    {m}\n" for m in value) + "};"
    return value


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--force", action="store_true")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()
    # Missing, or defined only by an earlier run of this tool: rerunning gives the same files.
    def ours(row):
        return [d for d in (row[4].split() if len(row) > 4 else []) if not d.startswith("data:research/data/bss/")]
    missing = []
    for r in (l.rstrip("\n").split("\t") for l in open(SYMBOLS)):
        if r[0] in ("missing", "defined", "duplicate") and r[1] in (".bss", ".sbss") and "$" not in r[3]:
            if r[0] == "missing" or len(ours(r)) < len(r[4].split()):
                missing.append(r[:4] + [" ".join(ours(r))])
    orig = original_symbols()
    files = json.load(open(FILE_MAP))["files"]
    texts = sources()
    drafts = collections.defaultdict(set)  # draft path -> original files of its functions
    for row in csv.DictReader(open(ROOT / "research/drafts/index.tsv"), delimiter="\t"):
        drafts[row["draft"]].add(owner_of(files, ".text", int(row["address"], 16)))
    by_file = collections.defaultdict(list)
    for name, defined_by in sorted({(r[3], r[4] if len(r) > 4 else "") for r in missing}):
        # A file static the original has in several files: skip the copies in files
        # some unit or draft already defines it for.
        covered = set()
        for src in defined_by.split():
            kind, _, which = src.partition(":")
            if kind == "unit":
                covered.add(json.load(open(ROOT / f"config/GMFE69/{which}.json")).get("original_file"))
            elif kind == "draft":
                covered |= drafts.get(which, set())
        for sym in orig[name]:
            owner = owner_of(files, sym["section_name"], sym["address"])
            if (len(orig[name]) == 1 and defined_by.split()) or (owner in covered and len(orig[name]) > 1):
                continue
            by_file[owner or "unassigned"].append(sym)
    for file, syms in sorted(by_file.items()):
        prelude, body = {}, []
        for sym in sorted(syms, key=lambda s: s["address"]):
            line = definition(sym, declared_type(sym["name"], texts), texts, prelude, len(orig[sym["name"]]))
            if sym["binding"] == 0 and len(orig[sym["name"]]) == 1:
                line += " // file-local in the original"
            pin = f" // address: {sym['name']} {sym['address']:#x}" if len(orig[sym["name"]]) > 1 or sym["binding"] == 0 else ""
            body.append(f"{line}{pin}")
        stem = re.sub(r"\.\w+$", "", file)
        path = OUT / f"{stem}.cpp"
        text = (f"// {file}: its zero-initialised globals ({sum(s['size'] for s in syms):#x} bytes), "
                f"written by research/tools/bss_decls.py\n// and checked by check_data.py. "
                f"Views marked \"view\" hold the size only; real layouts come with the shared headers.\n\n"
                + "".join(render(k, v) + "\n" for k, v in sorted(prelude.items(), key=lambda kv: isinstance(kv[1], list)) if v) + ("\n" if prelude else "") + "\n".join(body) + "\n")
        if args.dry_run:
            print(f"==> {path.relative_to(ROOT)}\n{text}")
            continue
        if path.exists() and not args.force:
            print(f"kept {path.relative_to(ROOT)} (exists; --force overwrites)")
            continue
        OUT.mkdir(parents=True, exist_ok=True)
        path.write_text(text)
        print(f"wrote {path.relative_to(ROOT)}: {len(syms)} symbols")


if __name__ == "__main__":
    main()
