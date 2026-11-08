#!/usr/bin/env python3
"""Regenerate the figures and unit tables in the docs from the verified snapshot.

Run after `progress_report.py --capture`. `--check` changes nothing and fails if
a document is out of date; the tests run it, so CI catches docs left behind. It
reads only tracked files, so it needs neither the game nor the compilers. Prose
stays hand-written: only text between `<!-- sync_docs:NAME -->` markers and the
counts and rows of the Game.md unit tables are generated.
"""
import argparse
import csv
import json
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = "config/GMFE69"

# Priority order for the PC port; see docs/Remaining.md. Unlisted files land in "Other".
AREAS = [
    (1, "Scripting and level objects", "bsbifunc.cpp objcreate.cpp buddy.cpp proxtrig.cpp staticobjfactory.cpp "
     "static_obj.cpp bsfile.cpp publisher.cpp weaponfactory.cpp thrown_obj.cpp impact.cpp explosion.cpp "
     "player.cpp player_weapon_object.cpp"),
    (1, "Collision", "cdbgeom.cpp box.cpp cdbobject.cpp capsule.cpp matrix.cpp quaternion.cpp"),
    (1, "AI", "AISplinePath.cpp Soldier_object.cpp AIObject.cpp AIFilter.cpp CoverPoint.cpp PathNodeList.cpp "
     "ObstacleAvoidance.cpp anim_obj.cpp"),
    (1, "Animation", "anim_callback.cpp animst.cpp skelblend.cpp animtran.cpp animmorph.cpp dmcluster.cpp "
     "dmesh.cpp dmmorph.cpp"),
    (2, "Rendering", "particlesystem.cpp compartment.cpp scene.cpp staticmesh.cpp font.cpp decal.cpp"),
    (2, "Audio", "path.c salloc.c soundschedule.cpp sound.cpp smixer.c spktplay.c sfrsf.c spat2hdr.c stream.cpp "
     "coda.cpp srandom.c sover.c"),
    (2, "Shell and HUD", "ShellMenu.cpp UserInterface.cpp pumhandler.cpp popup.cpp"),
    (2, "System", "debug_prof.cpp toilet.cpp input.cpp aramalloc.cpp support.c ipad.cpp"),
    (2, "Video", "rcmp_main.cpp getpic.cpp hwvideodisplaylist.cpp swvideodisplaylist.cpp rcmpbase.cpp"),
]
# Functions whose source file the original's symbols do not identify.
UNNAMED_FILES = [(r"SND|iSND|SFILTER", (2, "Audio")), (r"TRKSupp|random", (2, "System"))]
MULTIPLAYER = {"BIFunc_UpdateFragCount__FPPiPv", "DrawFragCount__13UserInterfaceFb",
               "InitMultiplayer__13CPlayerObjectFP12MPPlayerDatab", "BIFunc_MPAddPlayerHealth__FPPiPv"}


def kb(size):
    return f"{size / 1000:.1f} KB"


def remaining(snapshot):
    """Unfinished functions from the snapshot's code map, as (file, name, size)."""
    rows = []
    for unit in snapshot["code_map"]["units"]:
        for r in unit["ranges"]:
            if r.get("kind") == "function":
                rows.append((unit.get("file_name") or "?", r.get("name") or "", r["size"]))
    return rows


def categories(snapshot):
    result = defaultdict(lambda: {"bytes": 0, "functions": 0, "units": 0, "compilers": set()})
    for unit in snapshot["units"]:
        entry = result[unit["category"]]
        entry["bytes"] += unit["code_bytes"]
        entry["functions"] += unit["functions"]
        entry["units"] += 1
        entry["compilers"].add(unit["compiler_version"])
    return result


def compilers(entry):
    return ", ".join(f"`GC/{v}`" for v in sorted(entry["compilers"]))


def status_block(snapshot):
    p, c, rows = snapshot["progress"], categories(snapshot), remaining(snapshot)
    game, lib = c["reconstructed_game"], c["restored_library"]
    return f"""**{p['matching_code_bytes']:,} of {p['total_executable_code_bytes']:,} code bytes ({p['percent']:.2f}%)** are built from source and verified, covering **{p['matching_functions']:,} functions in {len(snapshot['units']):,} units**:

| Category | Code bytes | Functions | Units | Compilers |
| --- | ---: | ---: | ---: | --- |
| `reconstructed_game` (game code and EA engine) | {game['bytes']:,} | {game['functions']:,} | {game['units']:,} | {compilers(game)} |
| `restored_library` (Dolphin SDK, MSL, MW runtime, EA audio libraries) | {lib['bytes']:,} | {lib['functions']:,} | {lib['units']:,} | {compilers(lib)} |

About {kb(p['total_executable_code_bytes'] - p['matching_code_bytes'])} is left, in {len(rows)} functions across {len({r[0] for r in rows} - {"?"})} source files; [Remaining.md](docs/Remaining.md) breaks it down by area."""


def roadmap_block(snapshot):
    p, c = snapshot["progress"], categories(snapshot)
    game, lib = c["reconstructed_game"], c["restored_library"]
    return f"""- Verified snapshot: **{p['matching_code_bytes']:,} / {p['total_executable_code_bytes']:,}** executable bytes
  (**{p['percent']:.2f}%**), {p['matching_functions']:,} functions in {len(snapshot['units']):,} units: **{game['bytes']:,}** bytes of
  `reconstructed_game` ({game['units']:,} units, `docs/Game.md`) and **{lib['bytes']:,}** bytes of
  `restored_library` ({lib['units']:,} units: Dolphin SDK, MSL, MW runtime, EA audio
  libraries; `docs/Dolphin.md`). Runtime is untested. Priorities and what is left
  are in `docs/Remaining.md`."""


def remaining_block(snapshot):
    p, rows = snapshot["progress"], remaining(snapshot)
    area_of = {name: (prio, area) for prio, area, files in AREAS for name in files.split()}
    sizes, by_file, mp = Counter(), defaultdict(Counter), []
    for file, name, size in rows:
        if name in MULTIPLAYER:
            mp.append((size, name))
            continue
        key = area_of.get(file) or next((k for p, k in UNNAMED_FILES if re.match(p, name)), (2, "Other"))
        sizes[key] += size
        if file != "?":
            by_file[key][file] += size
    lines = [f"Verified: **{p['matching_code_bytes']:,} / {p['total_executable_code_bytes']:,} code bytes ({p['percent']:.2f}%)**. "
             f"Left: about {kb(sum(r[2] for r in rows))} in {len(rows)} functions across {len({r[0] for r in rows} - {"?"})} source files.", "",
             "| Priority | Area | Left | Main files |", "| ---: | --- | ---: | --- |"]
    for key in sorted(sizes, key=lambda k: (k[0], -sizes[k])):
        main = ", ".join(f"`{f}`" for f, _ in by_file[key].most_common(6))
        lines.append(f"| {key[0]} | {key[1]} | {kb(sizes[key])} | {main} |")
    if mp:
        names = ", ".join(f"`{n.split('__')[0]}`" for _, n in sorted(mp, reverse=True))
        lines.append(f"| 3 | Multiplayer (by name) | {kb(sum(s for s, _ in mp))} | {names} |")
    first = sum(v for k, v in sizes.items() if k[0] == 1)
    ceiling = 100 * (p["matching_code_bytes"] + first) / p["total_executable_code_bytes"]
    lines += ["", f"Priority 1 is {kb(first)} and would take the project to {ceiling:.1f}% at most."]
    lines += ["", sweep_line(rows)]
    return "\n".join(lines)


def sweep_line(rows):
    """Partial-source progress, from the status file research/tools/check_draft.py writes."""
    path = ROOT / "research/drafts/status.tsv"
    sizes = {name: size for _, name, size in rows}
    counts, total = Counter(), Counter()
    for row in csv.DictReader(path.read_text().splitlines(), delimiter="\t"):
        if row["function"] in sizes:
            counts[row["status"]] += 1
            total[row["status"]] += sizes[row["function"]]
    left = set(sizes) - {row["function"] for row in csv.DictReader(path.read_text().splitlines(), delimiter="\t")}
    parts = [(s, l) for s, l in (("code_exact", "code-exact, not yet linked"), ("consistent", "consistent"),
                                 ("differs", "flagged as differing"), ("no_compile", "not compiling")) if counts[s]]
    done = ", ".join(f"{counts[s]} {label} ({kb(total[s])})" for s, label in parts)
    return (f"Partial-source sweep: {done}; {len(left)} functions ({kb(sum(sizes[n] for n in left))}) have no draft yet. "
            "These are checker results, not proof of behaviour, and earn no progress credit.")


def replace_block(text, name, body):
    pattern = re.compile(rf"(<!-- sync_docs:{name} -->\n).*?(\n<!-- /sync_docs:{name} -->)", re.S)
    if not pattern.search(text):
        raise ValueError(f"Missing sync_docs:{name} markers")
    return pattern.sub(lambda m: m.group(1) + body + m.group(2), text)


# Game.md unit tables ---------------------------------------------------------

def unit_functions(unit):
    return sorted(unit["functions"] + unit.get("label_functions", []), key=lambda f: int(f["address"], 16))


def short_name(name):
    m = re.match(r"(.+?)__(?:\d|F|C|Q)", name)
    return m[1] if m and m[1] else name


def opening_comment(root, source):
    """The first paragraph of a source's opening comment, when it describes the code."""
    text = (root / source).read_text(errors="replace").lstrip()
    out = []
    if text.startswith("/*"):
        out = [x.strip(" *") for x in text[2:text.index("*/")].split("\n")]
    else:
        for line in text.split("\n"):
            if not line.startswith("//"):
                break
            part = line[2:].strip()
            if not part and out:
                break
            out.append(part)
    s = re.sub(r"\s+", " ", " ".join(x for x in out if x)).replace("|", "\\|")
    if re.search(r"notice|copyright|\(c\) ", s, re.I) or len(s) < 40:
        return ""
    s = re.sub(r"^A fragment of \S+( \([^)]*\))?\s*[:,.]?\s*", "", s)
    s = s[:1].upper() + s[1:]
    if len(s) <= 420:
        return s
    cuts = [m.end() for m in re.finditer(r"[.;:](?=\s)", s) if m.end() <= 420]
    if cuts and cuts[-1] > 80:
        return s[:cuts[-1]].rstrip(";:") + ("" if s[cuts[-1] - 1] == "." else ".")
    return s[:420].rsplit(" ", 1)[0] + " …"


def new_row(root, unit, columns):
    functions = unit_functions(unit)
    base, original = unit["source"].split("/")[-1], unit.get("original_file")
    cell = f"`{base}`" + (f" (fragment of `{original}`)" if original and original != base else "")
    note = opening_comment(root, unit["source"])
    if not note:
        names = list(dict.fromkeys(short_name(f["name"]) for f in functions))
        note = "Functions: " + ", ".join(f"`{n}`" for n in names[:12]) + (f" and {len(names) - 12} more" if len(names) > 12 else "") + "."
    size = sum(f["size"] for f in functions)
    return f"| {cell} | {len(functions)} | {size:,} | " + ("— | " if columns == 5 else "") + f"`{functions[0]['address']}`: {note} |"


def sync_tables(root, text, units):
    lines = text.split("\n")
    blocks, start = [], None
    for i, line in enumerate(lines):
        if line.startswith("|") and start is None:
            start = i
        elif not line.startswith("|") and start is not None:
            blocks.append((start, i))
            start = None
    tables = [b for b in blocks if lines[b[0]].startswith("| File | Functions | Bytes |")]
    if [lines[a].count(" | ") for a, _ in tables] != [4, 3]:
        raise ValueError("Game.md unit tables not found")
    by_source = {u["source"]: u for u in units}
    game = {u["source"] for u in units if u.get("category") == "reconstructed_game"}
    seen, output = set(), []

    def owner(cell, folder):
        m = re.search(r"\(fragment `([^`]+)`\)", cell) or re.match(r"`([^`]+)`", cell)
        if not m:
            return None, False
        for base in (folder, "src/game", "src/ea"):
            if f"{base}/{m[1]}" in by_source:
                return f"{base}/{m[1]}", True
        return None, True

    for (a, b), folder, columns in zip(tables, ("src/game", "src/ea"), (5, 4)):
        rows = []
        for line in lines[a + 2:b]:
            cells = re.split(r"(?<!\\) \| ", line.strip()[2:-2], maxsplit=columns - 1)
            source, named = owner(cells[0], folder)
            if named and source is None:
                continue  # A unit since merged into another.
            if source is None:
                rows.append(line)
                continue
            seen.add(source)
            functions = unit_functions(by_source[source])
            size = sum(f["size"] for f in functions)
            if cells[1].strip() != str(len(functions)) or cells[2].strip().replace(",", "") != str(size):
                cells[1], cells[2] = str(len(functions)), f"{size:,}"
                missing = list(dict.fromkeys(short_name(f["name"]) for f in functions
                                             if short_name(f["name"]) not in cells[-1] and f["name"] not in cells[-1]))
                if missing:
                    extra = ", ".join(f"`{m}`" for m in missing[:14]) + (f" and {len(missing) - 14} more" if len(missing) > 14 else "")
                    cells[-1] = cells[-1].rstrip() + f" Also: {extra}."
                line = "| " + " | ".join(c.strip() for c in cells) + " |"
            rows.append(line)
        for source in sorted(game - seen, key=lambda s: int(unit_functions(by_source[s])[0]["address"], 16)):
            if source.startswith(folder + "/"):
                rows.append(new_row(root, by_source[source], columns))
                seen.add(source)
        output.append((a, b, rows))
    for a, b, rows in reversed(output):
        lines[a + 2:b] = rows
    return "\n".join(lines)


def expected(root=ROOT):
    snapshot = json.loads((root / "progress/GMFE69.snapshot.json").read_text())
    project = json.loads((root / CONFIG / "project.json").read_text())
    key = next(k for k, v in project.items() if isinstance(v, list) and v and str(v[0]).endswith(".json"))
    units = [json.loads((root / CONFIG / name).read_text()) for name in project[key]]
    return {
        "README.md": replace_block((root / "README.md").read_text(), "status", status_block(snapshot)),
        "docs/Roadmap.md": replace_block((root / "docs/Roadmap.md").read_text(), "status", roadmap_block(snapshot)),
        "docs/Remaining.md": replace_block((root / "docs/Remaining.md").read_text(), "status", remaining_block(snapshot)),
        "docs/Game.md": sync_tables(root, (root / "docs/Game.md").read_text(), units),
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="Fail if a document is out of date; change nothing")
    args = parser.parse_args()
    stale = []
    for name, text in expected().items():
        if (ROOT / name).read_text() != text:
            stale.append(name)
            if not args.check:
                (ROOT / name).write_text(text)
    if args.check and stale:
        sys.exit("Out of date: " + ", ".join(stale) + " (run python3 tools/sync_docs.py)")
    print("Updated: " + ", ".join(stale) if stale else "Docs are current")


if __name__ == "__main__":
    main()
