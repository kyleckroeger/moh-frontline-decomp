#!/usr/bin/env python3
"""weakfix.py CLASS UNIT...: move UNIT onto include/game/CLASS.h (try_header's rewrite), list the
weak copies of inline functions the compile adds as weak_duplicates (externals -> the original's
retained weak copy), add the new undefined names those copies reference to
undefined_in_discarded_code, and verify like one_unit.py (validate_units first).

Writes WORK/<unit>/unit.json (the adjusted config) and WORK/<unit>/src/<file>; prints a row:
unit status detail added."""
import json, os, sys, traceback
from pathlib import Path

ROOT = Path("/home/kyle/workspace/moh-frontline")
WORK = Path(os.environ.get("WEAKFIX_WORK", ROOT / "scratch/loop3/hdr2/try"))
os.environ["TRY_HEADER_WORK"] = str(WORK)
sys.path.insert(0, str(ROOT / "research/tools"))
sys.path.insert(0, str(ROOT / "tools"))
os.chdir(ROOT)
import try_header as th
import project_build as pb
from audit import load_target
from formats import Elf32
from setup import setup
from setup_compiler import setup_compiler
from sdk_build import compile_sdk
from tool_runner import run

CONFIG = ROOT / "config/GMFE69"


def compile_only(unit, work):
    compiler, wrapper = setup_compiler(unit["compiler_version"], "GC")
    import shutil
    shutil.rmtree(work, ignore_errors=True)
    work.mkdir(parents=True)
    env = os.environ.copy()
    env["SN_NGC_PATH"] = "Z:" + str(compiler).replace("/", "\\")
    compile_sdk(unit, compiler, wrapper, work, lambda stage, *a: run(a, work, work / f"{stage}.log", env))
    return Elf32((work / "compiled.o").read_bytes())


def one(uid, cls, original):
    unit = json.loads((CONFIG / f"{uid}.json").read_text())
    base = WORK / uid
    src = base / "src" / Path(unit["source"]).name
    src.parent.mkdir(parents=True, exist_ok=True)
    src.write_text(th.moved((ROOT / unit["source"]).read_text(errors="replace"), cls))
    unit = dict(unit, source=str(src.relative_to(ROOT)))
    try:
        obj = compile_only(unit, base / "probe")
    except Exception:
        return uid, "no_compile", "", []
    syms = list(obj.symbols())
    functions = [s for s in syms if s["type"] == 2 and s["section"]]
    placed = unit["functions"] + unit.get("label_functions", [])
    expected = {pb.function_name(f["name"], f["binding"]) for f in placed} | set(unit["discarded_functions"]) \
        | set(unit.get("weak_duplicates", [])) | set(unit.get("file_duplicates", []))
    extra = [s for s in functions if pb.function_name(s["name"], s["binding"]) not in expected]
    missing = expected - {pb.function_name(s["name"], s["binding"]) for s in functions} \
        - set(unit.get("weak_duplicates", [])) - set(unit.get("file_duplicates", []))
    added = []
    for s in extra:
        orig = [o for o in original.symbols() if o["name"] == s["name"] and o["section"] and o["type"] in (1, 2)]
        if s["binding"] != 2 or len(orig) != 1 or orig[0]["binding"] != 2:
            return uid, "extra_nonweak", s["name"], []
        added.append((s["name"], orig[0]["address"]))
    unit = json.loads(json.dumps(unit))
    if added:
        unit["weak_duplicates"] = sorted(set(unit.get("weak_duplicates", [])) | {n for n, _ in added})
        for n, a in added:
            unit["externals"][n] = f"{a:#x}"
    # names the weak copies reference that the unit didn't
    undefined = {s["name"] for s in syms if s["binding"] and not s["section"] and s["name"]}
    pool_names = {pb.pool_symbol(r["address"]) for r in unit.get("pool_references", [])}
    new_undef = sorted(undefined - set(unit["externals"]) - set(unit["undefined_in_discarded_code"]) - pool_names)
    if new_undef:
        unit["undefined_in_discarded_code"] = sorted(set(unit["undefined_in_discarded_code"]) | set(new_undef))
    (base / "unit.json").write_text(json.dumps(unit, indent=2) + "\n")
    detail = ""
    if missing:
        detail = "missing " + ",".join(sorted(missing))
    try:
        pb.validate_units(original, [unit])
        status, d = th.verify(unit, base / "moved")
    except Exception as e:
        status, d = "lost", "validate: " + " ".join(str(e).split())[:200]
    return uid, status, (detail + " " + d).strip(), [n for n, _ in added] + ["U:" + n for n in new_undef]


def job(a):
    uid, cls = a
    try:
        _, original = load_target()
        return one(uid, cls, original)
    except Exception:
        return uid, "error", traceback.format_exc().splitlines()[-1], []


if __name__ == "__main__":
    from concurrent.futures import ProcessPoolExecutor
    cls, units = sys.argv[1], sys.argv[2:]
    with ProcessPoolExecutor(int(os.environ.get("JOBS", "4"))) as pool:
        for uid, status, detail, added in pool.map(job, [(u, cls) for u in units]):
            print(f"{uid}\t{status}\t{detail}\t{' '.join(added)}", flush=True)
