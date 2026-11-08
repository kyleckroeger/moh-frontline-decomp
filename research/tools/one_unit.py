"""one_unit.py <id>: compile, link and verify one manifest like build_project (research helper)."""
import json, os, shutil, sys
from pathlib import Path
sys.path.insert(0, "tools")
import project_build as pb
from audit import load_target
from formats import Elf32
from setup import CONFIG, ROOT, setup
from setup_compiler import setup_compiler
from sdk_build import compile_sdk, sdk_link_script
from tool_runner import run
from data_strip import strip_objects
from extab_patch import apply_patches, load_patches

uid = sys.argv[1]
unit = json.loads((CONFIG / f"{uid}.json").read_text())
_, original = load_target()
pb.validate_units(original, [unit])
tools = setup()
compiler, wrapper = setup_compiler(unit["compiler_version"], "GC")
linker, _ = setup_compiler("3.8.1", "ProDG")
work = ROOT / "scratch/one" / uid
shutil.rmtree(work, ignore_errors=True); work.mkdir(parents=True)
env = os.environ.copy(); env["SN_NGC_PATH"] = "Z:" + str(compiler).replace("/", "\\")
def execute(stage, *args): run(args, work, work / f"{stage}.log", env)
compile_sdk(unit, compiler, wrapper, work, execute)
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
linked = Elf32((work / "compiled.elf").read_bytes())
try:
    pb.verify_unit(original, linked, unit)
    print(f"{uid}: VERIFIED")
except ValueError as e:
    print(f"{uid}: FAIL {e}")
    for s in pb.allocated(linked):
        print(f"  {s['name']:12} {s['address']:#x} {s['size']}")
    print("  expected", [(s["name"], s["address"], s["size"]) for s in unit["sections"]])
