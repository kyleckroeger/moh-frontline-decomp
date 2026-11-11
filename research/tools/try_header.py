#!/usr/bin/env python3
"""try_header.py: what moving every matched unit to a shared class header would cost.

PortReadiness step 3.3-3.4. For each unit whose source defines its own view of CLASS,
this builds a copy of the source with that definition (and the helper types and inline
functions the header now provides) replaced by #include "game/<CLASS>.h", then compiles,
links and verifies the unit exactly as research/tools/one_unit.py does. The unchanged
source is verified the same way first, as a control.

Nothing under src/ or config/ is changed: copies and builds go to scratch/try/<unit>/.

    try_header.py CLASS [--jobs 4] [--only UNIT...]
        writes research/layout/header_moves/CLASS.tsv: unit, source, status, detail

Statuses: kept (verified with the header), lost (compiles, doesn't verify),
no_compile (the header lacks something the file uses), control_fail (the unit doesn't
verify unchanged in this harness; not counted).
"""
import argparse, glob, json, os, re, shutil, sys, traceback
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CONFIG = ROOT / "config/GMFE69"
OUT = ROOT / "research/layout/header_moves"

# What a header provides besides its class, by class: removed from the moved copy too.
PROVIDES = {
    "CVector3": {"types": ["CVector3XYZW", "CVector3Data", "CVector3Pair"], "functions": ["sqrtf"]},
}


# Spellings of the class's fields that only some views have, rewritten to the header's.
# Only forms specific to the class: anything else fails to compile and is counted.
_XYZW = "xyzw"
REWRITES = {
    "CVector3": [
        # whole-union copies and double halves of the views' "d" member
        (r"([\w\]\[.>-]+?)\.d\s*=\s*([\w\]\[.>-]+?)\.d\s*;", r"*(CVector3Data*)&\1 = *(const CVector3Data*)&\2;"),
        (r"([\w\]\[.>-]+?)\.d\.pair\[([01])\]", r"((double*)&\1)[\2]"),
        (r"(?<![\w.>])d\.pair\[([01])\]", r"((double*)this)[\1]"),
        (r"\bd\.v\[([0-3])\]", lambda m: _XYZW[int(m[1])]),
        (r"\bd\.c\.([xyzw])\b", r"\1"),
        (r"(?<=[.>])c\.([xyzw])\b", r"\1"),
        (r"(?<![\w.>])c\.([xyzw])\b", r"\1"),
    ],
}


def block_end(text, brace):
    depth, i = 1, brace + 1
    while depth and i < len(text):
        depth += {"{": 1, "}": -1}.get(text[i], 0)
        i += 1
    return i


def type_span(text, name):
    """(start, end) of a top-level class/struct/union definition of name, through its ';'."""
    m = re.search(r"^(?:class|struct|union)\s+" + re.escape(name) + r"\b[^;{]*\{", text, re.M)
    if not m:
        return None
    end = block_end(text, m.end() - 1)
    semi = text.index(";", end)
    return m.start(), semi + 1


def function_span(text, name):
    m = re.search(r"^(?:extern\s+)?(?:inline\s+)?[\w\s\*&]+\b" + re.escape(name) + r"\s*\([^;{]*\)\s*\{", text, re.M)
    if not m:
        return None
    return m.start(), block_end(text, m.end() - 1)


def moved(text, cls):
    span = type_span(text, cls)
    if not span:
        return None
    out = text[:span[0]] + f'#include "game/{cls}.h"\n' + text[span[1]:]
    extra = PROVIDES.get(cls, {})
    for name in extra.get("types", []):
        while True:
            s = type_span(out, name)
            if not s:
                break
            out = out[:s[0]] + out[s[1]:]
    for name in extra.get("functions", []):
        s = function_span(out, name)
        if s:
            out = out[:s[0]] + out[s[1]:]
    for pattern, repl in REWRITES.get(cls, []):
        out = re.sub(pattern, repl, out)
    # Out-of-line definitions of the class's methods the header now defines inline
    # (units emit the original's weak copies this way): the header's body replaces them.
    header = (ROOT / f"include/game/{cls}.h").read_text()
    inline = set(re.findall(r"^\s+(?:[\w&\*]+\s+)*?(\w+|operator\S+?)\s*\([^;{]*\)\s*(?:const\s*)?(?::[^{;]*)?\{", header, re.M))
    pat = re.compile(r"^(?:__declspec\(weak\)\s+)?(?:inline\s+)?[\w\s&\*]*?\b" + re.escape(cls)
                     + r"::(~?\w+|operator\S+?)\s*\([^;{]*\)\s*(?:const\s*)?(?::[^{;]*)?\{", re.M)
    while True:
        m = next((m for m in pat.finditer(out) if m[1] in inline or (m[1] == cls and cls in inline)), None)
        if not m:
            break
        out = out[:m.start()] + out[block_end(out, m.end() - 1):]
    return out


def verify(unit, work):
    """one_unit.py's flow, for a unit dict and work directory; returns (ok, detail)."""
    sys.path.insert(0, str(ROOT / "tools"))
    import project_build as pb
    from audit import load_target
    from formats import Elf32
    from setup import setup
    from setup_compiler import setup_compiler
    from sdk_build import compile_sdk, sdk_link_script
    from tool_runner import run
    from data_strip import strip_objects
    from extab_patch import apply_patches, load_patches
    uid = unit["id"]
    _, original = load_target()
    tools = setup()
    compiler, wrapper = setup_compiler(unit["compiler_version"], "GC")
    linker, _ = setup_compiler("3.8.1", "ProDG")
    shutil.rmtree(work, ignore_errors=True)
    work.mkdir(parents=True)
    env = os.environ.copy()
    env["SN_NGC_PATH"] = "Z:" + str(compiler).replace("/", "\\")

    def execute(stage, *args):
        run(args, work, work / f"{stage}.log", env)
    try:
        compile_sdk(unit, compiler, wrapper, work, execute)
    except Exception:
        log = (work / "compile.log").read_text(errors="replace") if (work / "compile.log").exists() else ""
        lines = [l.strip() for l in log.splitlines() if l.strip().startswith("#") and "Error" not in l]
        return "no_compile", " ".join(lines[:4])[:300]
    try:
        pb.check_pool(original, Elf32((work / "compiled.o").read_bytes()), unit)
        units = [json.loads((CONFIG / n).read_text()) for n in json.loads((CONFIG / "project.json").read_text())["units"]]
        patches = load_patches([u for u in units if u["id"] != uid] + [unit]).get(uid, [])
        (work / "input.o").write_bytes(strip_objects(apply_patches((work / "compiled.o").read_bytes(), patches), unit))
        obj = Elf32((work / "input.o").read_bytes())
        pb.validate_object(obj, unit)
        (work / "source.ld").write_text(sdk_link_script(unit, obj, original, tools, work, execute))
        kept_text = [(f["section"], f["address"], f["address"] + f["size"]) for f in obj.symbols()
                     if f["type"] == 2 and f["name"] not in unit["discarded_functions"]
                     and obj.sections[f["section"]]["name"] in pb.CODE_SECTIONS] if unit["strip_unused"] else []
        roots = [f["name"] for f in unit["functions"] + unit.get("label_functions", []) if f["binding"]] + sorted(
            {s["name"] for s in obj.symbols() if s["binding"] and s["type"] == 0 and s["section"]
             and s["section"] < 0xFF00 and obj.sections[s["section"]]["name"] in pb.CODE_SECTIONS
             and any(i == s["section"] and a <= s["address"] < b for i, a, b in kept_text)})
        strip_args = ["--strip-unused"] + [a for n in roots for a in ("--undefined", n)] if unit["strip_unused"] else []
        if unit.get("retain_function_symbols", False):
            (work / "functions.keep").write_text("".join(f["name"] + "\n" for f in unit["functions"] + unit.get("label_functions", [])))
            strip_args += ["--retain-symbols-file", "functions.keep"]
        execute("link", wrapper, linker / "ngcld.exe", *strip_args, "--fix-sda", "-T", "source.ld", "-o", "compiled.elf")
        pb.verify_unit(original, Elf32((work / "compiled.elf").read_bytes()), unit)
        return "kept", ""
    except Exception as e:
        return "lost", " ".join(str(e).split())[:300]


def attempt(job):
    uid, cls = job
    unit = json.loads((CONFIG / f"{uid}.json").read_text())
    base = ROOT / "scratch/try" / uid
    status, detail = verify(unit, base / "control")
    if status != "kept":
        return uid, unit["source"], "control_fail", detail
    text = (ROOT / unit["source"]).read_text(errors="replace")
    new = moved(text, cls)
    copy = base / "src" / Path(unit["source"]).name
    copy.parent.mkdir(parents=True, exist_ok=True)
    copy.write_text(new)
    unit = dict(unit, source=str(copy.relative_to(ROOT)), include_dirs=unit["include_dirs"] + ["include"])
    try:
        status, detail = verify(unit, base / "moved")
    except Exception:
        status, detail = "error", traceback.format_exc().splitlines()[-1]
    return uid, unit["source"], status, detail


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("cls")
    ap.add_argument("--jobs", type=int, default=4)
    ap.add_argument("--only", nargs="*")
    a = ap.parse_args()
    units = json.loads((CONFIG / "project.json").read_text())["units"]
    pat = re.compile(r"^(?:class|struct)\s+" + re.escape(a.cls) + r"\b[^;{]*\{", re.M)
    jobs = []
    for name in units:
        uid = name[:-5] if name.endswith(".json") else name
        unit = json.loads((CONFIG / f"{uid}.json").read_text())
        src = ROOT / unit["source"]
        if src.exists() and pat.search(src.read_text(errors="replace")) and (not a.only or uid in a.only):
            jobs.append((uid, a.cls))
    print(f"{len(jobs)} units define their own {a.cls}", flush=True)
    rows = []
    with ProcessPoolExecutor(a.jobs) as pool:
        for row in pool.map(attempt, jobs):
            rows.append(row)
            print("\t".join(row[:3]), flush=True)
    OUT.mkdir(parents=True, exist_ok=True)
    out = OUT / f"{a.cls}.tsv"
    if a.only and out.exists():  # merge a partial rerun into the full table
        old = {r.split("\t")[0]: r for r in out.read_text().splitlines()[1:]}
        for r in rows:
            old[r[0]] = "\t".join(r)
        lines = sorted(old.values())
    else:
        lines = sorted("\t".join(r) for r in rows)
    out.write_text("unit\tsource\tstatus\tdetail\n" + "\n".join(lines) + "\n")
    from collections import Counter
    print(Counter(l.split("\t")[2] for l in lines))


if __name__ == "__main__":
    main()
