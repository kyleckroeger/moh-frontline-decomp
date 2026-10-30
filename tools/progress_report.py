#!/usr/bin/env python3
"""Export objdiff v2 progress from a locally verified, source-pinned snapshot.

CI validates the snapshot's inputs; it does not possess or rebuild the game.
See docs/Progress.md for the verification boundary and counting rules.
"""
import argparse
import hashlib
import json
from pathlib import Path

from code_map import bounds, build_code_map, validate_code_map
from setup import GAME_ID

ROOT = Path(__file__).resolve().parents[1]
SNAPSHOT = Path(f"progress/{GAME_ID}.snapshot.json")
CATEGORIES = {
    "reconstructed_game": "Accepted game fragments",
    "restored_library": "Accepted library fragments",
    "unreconstructed": "Unreconstructed code (symbol map)",
}


def read(path):
    return json.loads(path.read_text())


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def fingerprints(root):
    return {p.relative_to(root).as_posix(): digest(p)
            for folder in ("src", "include", "config", "tools")
            for p in sorted((root / folder).rglob("*"))
            if p.is_file() and "__pycache__" not in p.parts}


def require_fresh(snapshot, root):
    if snapshot["input_sha256"] != fingerprints(root):
        raise ValueError("Stale progress snapshot: rebuild, then run progress_report.py --capture")


def capture(root):
    # Import only on the local capture path; CI needs no originals or toolchains.
    from formats import verify_load_image
    from audit import load_target
    from project_build import progress, verify_unit

    report = read(root / "build/reconstruction/report.json")
    require_fresh(report, root)
    if report["status"] != "identical":
        raise ValueError("Source reconstruction has not passed")
    config = root / "config" / GAME_ID
    project = read(config / "project.json")
    units = [read(config / name) for name in project["units"]]
    original_path, original = load_target()
    if report["original_elf_sha256"] != digest(original_path):
        raise ValueError("Original differs from the verified build")
    if report["progress"] != progress(original, units, project):
        raise ValueError("Progress differs from reviewed source ranges")
    baseline = read(config / "baseline.json")
    rebuilt = (root / "build/reconstruction/rebuilt.dol").read_bytes()
    if (len(rebuilt) != baseline["normalized_dol_size"] or
            hashlib.sha1(rebuilt).hexdigest() != baseline["normalized_dol_sha1"]):
        raise ValueError("Complete rebuilt image differs")
    verify_load_image(original, rebuilt)
    from formats import Elf32
    expected = {u["id"]: u for u in report["units"]}
    if len(expected) != len(units) or set(expected) != {u["id"] for u in units}:
        raise ValueError("Verified unit inventory differs")
    for unit in units:
        work = root / "build/reconstruction/units" / unit["id"]
        for filename, key in (("compiled.o", "object_sha256"), ("compiled.elf", "linked_sha256")):
            if digest(work / filename) != expected[unit["id"]][key]:
                raise ValueError("Source-built unit differs from verified build")
        verify_unit(original, Elf32((work / "compiled.elf").read_bytes()), unit)
    # Allow-list public fields. In particular, never copy source_blobs paths.
    accepted = [(int(f["address"], 16), int(f["address"], 16) + f["size"])
                for unit in units for f in unit["functions"] + unit.get("label_functions", [])]
    return {"schema_version": 2, "verification": "local-complete-image-match",
            "target": report["target"], "original_elf_sha256": report["original_elf_sha256"],
            "dol_sha1": report["dol_sha1"], "runtime_tested": False,
            "progress": report["progress"], "input_sha256": report["input_sha256"],
            "units": report["units"], "code_map": build_code_map(original, accepted, file_owners(original, units))}


def file_owners(original, units=None):
    """Functions' recovered files (see file_map.py), keyed by (name, address)."""
    from file_map import build_file_map
    _, items, _, _ = build_file_map(original, units)
    return {(e["name"], e["address"]): (e["owner"], e["evidence"])
            for entries in items.values() for e in entries if e["owner"] is not None and e["size"]}


def measures(total, matched):
    percent = 100 * matched / total if total else 0
    return {"total_code": str(total), "matched_code": str(matched),
            "complete_code": str(matched), "matched_code_percent": percent,
            "complete_code_percent": percent, "fuzzy_match_percent": percent}


def export(snapshot, root):
    require_fresh(snapshot, root)
    if snapshot["schema_version"] != 2 or snapshot["verification"] != "local-complete-image-match":
        raise ValueError("Unsupported verification snapshot")
    config = root / "config" / GAME_ID
    project = read(config / "project.json")
    manifests = [read(config / name) for name in project["units"]]
    verified = snapshot["units"]
    if len(verified) != len(manifests) or len({u["id"] for u in verified}) != len(verified):
        raise ValueError("Duplicate or missing verified units")
    by_id = {u["id"]: u for u in verified}
    units, intervals = [], []
    totals = {key: 0 for key in CATEGORIES}
    function_count = 0
    for manifest in manifests:
        unit = by_id[manifest["id"]]
        functions = manifest["functions"] + manifest.get("label_functions", [])
        size = sum(f["size"] for f in functions)
        category = manifest["category"]
        if (category not in ("reconstructed_game", "restored_library") or
                unit["category"] != category or unit["code_bytes"] != size or
                unit["functions"] != len(functions)):
            raise ValueError("Verified unit measures differ from source manifest")
        items = []
        for f in functions:
            start = int(f["address"], 16)
            if f["size"] <= 0:
                raise ValueError("Nonpositive function size")
            intervals.append((start, start + f["size"]))
            items.append({"name": f["name"], "size": str(f["size"]),
                          "fuzzy_match_percent": 100,
                          "metadata": {"virtual_address": str(start)}})
        totals[category] += size
        function_count += len(functions)
        units.append({"name": manifest["id"], "measures": measures(size, size),
                      "functions": items,
                      "metadata": {"complete": True, "source_path": manifest["source"],
                                   "progress_categories": [category]}})
    intervals.sort()
    if any(b[0] < a[1] for a, b in zip(intervals, intervals[1:])):
        raise ValueError("Overlapping source coverage")
    total = project["progress"]["code_bytes"]
    matched = sum(totals.values())
    p = snapshot["progress"]
    if (total != sum(project["progress"]["executable_sections"].values()) or matched > total or
            p["matching_code_bytes"] != matched or p["total_executable_code_bytes"] != total or
            p["matching_functions"] != function_count or
            p["categories"] != {k: totals[k] for k in ("reconstructed_game", "restored_library")}):
        raise ValueError("Snapshot totals differ from source coverage")
    totals["unreconstructed"] = total - matched
    baseline = read(config / "baseline.json")
    sections = sorted([{"name": name, "address": baseline["sections"][name], "size": size}
                       for name, size in project["progress"]["executable_sections"].items()],
                      key=lambda s: int(s["address"], 16))
    code_map = snapshot["code_map"]
    validate_code_map(code_map, intervals, sections)
    for mapped in code_map["units"]:
        functions, mapped_sections = [], []
        for item in mapped["ranges"]:
            record = {"name": item["name"], "size": str(item["size"]), "fuzzy_match_percent": 0,
                      "metadata": {"virtual_address": str(bounds(item)[0])}}
            (mapped_sections if item["kind"] == "unidentified" else functions).append(record)
        units.append({"name": mapped["name"],
                      "measures": measures(sum(r["size"] for r in mapped["ranges"]), 0),
                      "functions": functions, "sections": mapped_sections,
                      "metadata": {"complete": False, "auto_generated": True,
                                   "progress_categories": ["unreconstructed"]}})
    if len({u["name"] for u in units}) != len(units):
        raise ValueError("Duplicate accepted/mapped unit name")
    # Unknown function/unit/data denominators are intentionally not fabricated.
    return {"version": 2, "measures": measures(total, matched), "units": units,
            "categories": [{"id": key, "name": name,
                            "measures": measures(totals[key], 0 if key == "unreconstructed" else totals[key])}
                           for key, name in CATEGORIES.items()]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--capture", action="store_true", help="Verify local build and replace public snapshot")
    parser.add_argument("--output", type=Path, default=ROOT / "build/progress/report.json")
    args = parser.parse_args()
    if args.capture:
        snapshot = capture(ROOT)
        report = export(snapshot, ROOT)
        (ROOT / SNAPSHOT).parent.mkdir(parents=True, exist_ok=True)
        (ROOT / SNAPSHOT).write_text(json.dumps(snapshot, indent=2) + "\n")
    else:
        report = export(read(ROOT / SNAPSHOT), ROOT)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    print(f"Verified snapshot: {report['measures']['matched_code']} / {report['measures']['total_code']} code bytes")


if __name__ == "__main__":
    main()
