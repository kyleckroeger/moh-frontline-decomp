#!/usr/bin/env python3
"""redraft.py CLASS UNIT...: move UNIT onto include/game/CLASS.h (try_header's rewrite), re-draft its
manifest with tools/port_unit.py's own draft() (CONFIG redirected to a scratch root), passing the
existing manifest's arguments plus --weak-duplicate for every weak function the compile adds that
the original has a single retained weak copy of, then verify like one_unit.py.

Outputs WORK/<unit>/unit.json (source set back to the real path) and prints:
unit status detail added-weak-dups changed-keys"""
import argparse, contextlib, io, json, os, shutil, sys, traceback
from pathlib import Path

ROOT = Path("/home/kyle/workspace/moh-frontline")
HERE = ROOT / "scratch/loop3/hdr5"
WORK = Path(os.environ.get("REDRAFT_WORK", HERE / "rd"))
os.environ["TRY_HEADER_WORK"] = str(WORK)
sys.path.insert(0, str(ROOT / "research/tools")); sys.path.insert(0, str(HERE))
sys.path.insert(0, str(ROOT / "tools"))
os.chdir(ROOT)
import try_header as th
import project_build as pb
import port_unit
from audit import load_target
from formats import Elf32
from setup_compiler import setup_compiler
from sdk_build import compile_sdk
from tool_runner import run

CONFIG = ROOT / "config/GMFE69"


def compile_only(unit, work):
    compiler, wrapper = setup_compiler(unit["compiler_version"], "GC")
    shutil.rmtree(work, ignore_errors=True)
    work.mkdir(parents=True)
    env = os.environ.copy()
    env["SN_NGC_PATH"] = "Z:" + str(compiler).replace("/", "\\")
    compile_sdk(unit, compiler, wrapper, work, lambda stage, *a: run(a, work, work / f"{stage}.log", env))
    return Elf32((work / "compiled.o").read_bytes())


def draft_args(old, source, weak, like):
    a = argparse.Namespace(id=old["id"], source=source, like=str(like),
                           original_file=old.get("original_file"), file_index=old.get("original_file_index"),
                           no_file_record="original_file_index" not in old, compiler=None, define=None,
                           category=old["category"], weak_duplicate=sorted(weak),
                           file_duplicate=list(old.get("file_duplicates", [])),
                           pool=list(old.get("pooled_sections", [])),
                           linker_stripped=old.get("linker_stripped", {}).get("functions", []) + old.get("linker_stripped", {}).get("objects", []),
                           leading_filler=old.get("leading_filler"), upstream_path=None)
    return a


def one(uid, cls, original, source_text=None):
    old = json.loads((CONFIG / f"{uid}.json").read_text())
    base = WORK / uid
    shutil.rmtree(base, ignore_errors=True)
    src = base / "src" / Path(old["source"]).name
    src.parent.mkdir(parents=True, exist_ok=True)
    src.write_text(source_text if source_text is not None else th.moved((ROOT / old["source"]).read_text(errors="replace"), cls, old["source"]))
    rel = str(src.relative_to(ROOT))
    incs = old["include_dirs"] + ([] if "include" in old["include_dirs"] else ["include"])
    xinc = [str(th.HEADER_DIR.relative_to(ROOT))] if th.HEADER_DIR != ROOT / "include" else []
    probe = dict(old, source=rel, include_dirs=xinc + incs)
    try:
        obj = compile_only(probe, base / "probe")
    except Exception:
        return uid, "no_compile", "", [], []
    functions = [s for s in obj.symbols() if s["type"] == 2 and s["section"]]
    placed = old["functions"] + old.get("label_functions", [])
    expected = {pb.function_name(f["name"], f["binding"]) for f in placed} | set(old["discarded_functions"]) \
        | set(old.get("weak_duplicates", [])) | set(old.get("file_duplicates", []))
    added = []
    for s in functions:
        if pb.function_name(s["name"], s["binding"]) in expected:
            continue
        orig = [o for o in original.symbols() if o["name"] == s["name"] and o["section"] and o["type"] in (1, 2)]
        if s["binding"] != 2 or len(orig) != 1 or orig[0]["binding"] != 2:
            return uid, "extra_nonweak", s["name"], [], []
        added.append(s["name"])
    fake = base / "fake"
    (fake / "config/GMFE69").mkdir(parents=True, exist_ok=True)
    port_unit.ROOT, port_unit.CONFIG = fake, fake / "config/GMFE69"
    try:
        with contextlib.redirect_stdout(io.StringIO()) as out:
            like = base / "like.json"
            like.write_text(json.dumps(dict(old, include_dirs=xinc + incs)))
            port_unit.draft(draft_args(old, rel, set(old.get("weak_duplicates", [])) | set(added), like))
    except Exception as e:
        return uid, "draft_fail", " ".join(str(e).split())[:200], added, []
    new = json.loads((fake / f"config/GMFE69/{uid}.json").read_text())
    new["source"] = old["source"]
    new["include_dirs"] = incs
    # the minimal change: the old manifest with only what the weak copies/labels touch from the draft
    merged = dict(old)
    for k in ("weak_duplicates", "strip_unused", "stripped_sections", "sections", "include_dirs"):
        if k in new:
            merged[k] = new[k]
        else:
            merged.pop(k, None)
    ext = dict(new["externals"])
    for k, v in old["externals"].items():
        if k in new["undefined_in_discarded_code"]:
            ext[k] = v  # resolved by hand in the old manifest (file-local RTTI and the like)
    merged["externals"] = dict(sorted(ext.items()))
    merged["undefined_in_discarded_code"] = sorted(set(new["undefined_in_discarded_code"]) - set(ext))
    merged["local_externals"] = sorted(set(old["local_externals"]) & set(ext))
    merged["sda_externals"] = new["sda_externals"]
    merged = {k: merged[k] for k in list(old) + [k for k in ("stripped_sections", "weak_duplicates") if k not in old] if k in merged}
    changed = sorted(k for k in set(old) | set(merged) if old.get(k) != merged.get(k))
    differs = sorted(k for k in set(new) | set(merged) if new.get(k) != merged.get(k))
    if differs:
        changed.append("DRAFTDIFF:" + ",".join(differs))
    (base / "draft.json").write_text(json.dumps(new, indent=2) + "\n")
    new = merged
    (base / "unit.json").write_text(json.dumps(new, indent=2) + "\n")
    try:
        pb.validate_units(original, [new])
        status, d = th.verify(dict(new, source=rel, include_dirs=xinc + new["include_dirs"]), base / "moved")
    except Exception as e:
        status, d = "lost", "validate: " + " ".join(str(e).split())[:200]
    return uid, status, d, added, changed


def job(a):
    uid, cls = a
    try:
        _, original = load_target()
        return one(uid, cls, original)
    except Exception:
        return uid, "error", traceback.format_exc().splitlines()[-1], [], []


if __name__ == "__main__":
    from concurrent.futures import ProcessPoolExecutor
    cls, units = sys.argv[1], sys.argv[2:]
    with ProcessPoolExecutor(int(os.environ.get("JOBS", "4"))) as pool:
        for uid, status, detail, added, changed in pool.map(job, [(u, cls) for u in units]):
            print(f"{uid}\t{status}\t{detail}\t{' '.join(added)}\t{' '.join(changed)}", flush=True)
