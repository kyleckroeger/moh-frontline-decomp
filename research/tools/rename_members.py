#!/usr/bin/env python3
"""rename_members.py: one spelling per data member, so units can move onto a shared header
(endgame W2.3; decision 2026-11-17: one class at a time, data members only).

Header moves are mostly blocked by member spellings: the same field is `m_prev` in one unit's view,
`data00` in another's and `m_head` in a third's. Member names never reach the object code, so a
unit renamed consistently still verifies byte for byte; one_unit.py checks that for every file
changed, and a file that doesn't verify is put back.

    rename_members.py CLASS                report: every view's names by offset, the canonical
                                           name chosen for each, and the renames per unit
    rename_members.py CLASS --apply [--only UNIT ...] [--jobs 4]
                                           rename in src/, verify each unit, revert failures

Layouts come from DWARF (`-sym on`, as layout_lint.py), with base classes flattened to absolute
offsets. A member is identified by its offset and size. The canonical name for a field is the
header's (include/game/CLASS.h), unless the header's is a placeholder (dataXX, unknownXX, padXX,
fieldXX); then it is the most common real name among the matched views, and the report says the
header should take it. Skipped, and listed: arrays or structs laid over several fields, a view
that already has the canonical name at another offset, names that another class in the same file
also uses as a member (a token rename would rename that one too), and names that occur in one of
the original's symbols.

The rename is token by token over the whole file (comments and string literals untouched), all
renames of a file at once, so swaps are safe. Types aren't changed: a view whose type differs
(int against a pointer) still needs a hand fix before it moves; the report lists those.
"""
import argparse, collections, glob, json, re, shutil, subprocess, sys, tempfile
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path[:0] = [str(ROOT / "research/tools"), str(ROOT / "tools")]
import layout_lint  # noqa: E402
from formats import Elf32  # noqa: E402

PLACEHOLDER = re.compile(r"^(data|unknown|unk|pad|field|_?pad|m_data|m_unknown|m_unk|m_field|m_value|m_flag)_?[0-9a-fA-F]+$")
HEADER_SETTINGS = "config/GMFE69/ConfigGC.json"


def placeholder(name):
    return bool(PLACEHOLDER.match(name))


def units_with_view(cls):
    """(unit id, settings path, source) for each game unit whose source defines CLASS."""
    pat = re.compile(r"\b(class|struct)\s+" + re.escape(cls) + r"\b[^;{]*\{")
    out = []
    for f in sorted(glob.glob(str(ROOT / "config/GMFE69/*.json"))):
        t = json.load(open(f))
        if t.get("category") != "reconstructed_game" or "source" not in t:
            continue
        src = ROOT / t["source"]
        if src.exists() and pat.search(src.read_text(errors="replace")):
            out.append((Path(f).stem, str(Path(f).relative_to(ROOT)), t["source"]))
    return out


def flatten(views, cls, base=0, seen=None):
    """[(offset, size, name, type)] of CLASS's data members, bases included."""
    seen = seen or set()
    if cls in seen:
        return []
    seen.add(cls)
    v = next((x for x in views if x["name"] == cls and x["size"]), None)
    if not v:
        return []
    out = []
    for bname, boff in v["bases"]:
        if boff is not None:
            out += flatten(views, bname, base + boff, seen)
    for name, off, bitoff, bitsize, size, ty in v["members"]:
        if name.startswith("__vptr") or bitsize:
            continue
        out.append((base + off, size, name, ty))
    return out


def class_chain(views, cls, seen=None):
    """CLASS and its bases (by name)."""
    seen = seen if seen is not None else set()
    if cls in seen:
        return seen
    seen.add(cls)
    for v in views:
        if v["name"] == cls:
            for bname, _ in v["bases"]:
                class_chain(views, bname, seen)
    return seen


def header_layout(cls):
    stub = f'#include "game/{cls}.h"\n{cls}* g_rename_members_probe;\nint rename_members_size = sizeof({cls});\n'
    with tempfile.TemporaryDirectory(dir=ROOT / "scratch") as d:
        p = Path(d) / "probe.cpp"
        p.write_text(stub)
        _, views, err = layout_lint.compile_sym(("header", HEADER_SETTINGS, str(p.relative_to(ROOT))))
    if views is None:
        raise SystemExit(f"header doesn't compile: {err}")
    return flatten(views, cls)


def symbol_words():
    words = set()
    for s in Elf32((ROOT / "orig/GMFE69/Moh2RelGC.elf").read_bytes()).symbols():
        if s["name"]:
            words.add(s["name"])
            for m in re.finditer(r"(\d+)([A-Za-z_])", s["name"]):
                n = int(m.group(1))
                words.add(s["name"][m.start(2):m.start(2) + n])
    return words


def canonical(cls, header, views):
    """{(off, size): (name, from_header)} and the variants seen."""
    variants = collections.defaultdict(collections.Counter)
    for unit, members in views.items():
        for off, size, name, ty in members:
            variants[(off, size)][name] += 1
    canon = {}
    for off, size, name, ty in header:
        if not placeholder(name):
            canon[(off, size)] = (name, True)
            continue
        real = [(n, c) for n, c in variants[(off, size)].most_common() if not placeholder(n)]
        canon[(off, size)] = (real[0][0], False) if real else (name, True)
    return canon, variants


TOKEN = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|[A-Za-z_]\w*|.', re.S)


def rename_text(text, mapping):
    out = []
    for m in TOKEN.finditer(text):
        t = m.group(0)
        out.append(mapping.get(t, t) if (t[0].isalpha() or t[0] == "_") else t)
    return "".join(out)


def plan(cls, header, views, canon, words, texts, elsewhere):
    """unit -> ({old: new}, [skipped reasons]). elsewhere[unit]: member names of the file's other
    classes; a name there can't be renamed token by token."""
    hdr_types = {(o, s): ty for o, s, n, ty in header}
    out = {}
    for unit, members in views.items():
        text = texts[unit]
        names_at = collections.defaultdict(set)
        for off, size, name, ty in members:
            names_at[name].add((off, size))
        mapping, skipped = {}, []
        for off, size, name, ty in members:
            key = (off, size)
            if key not in canon:
                if not any(o <= off < o + s for o, s in canon) or placeholder(name):
                    continue
                skipped.append(f"{name}@{off:#x}/{size}: spans header fields")
                continue
            new, _ = canon[key]
            if name == new:
                continue
            if new in names_at and names_at[new] != {key}:
                skipped.append(f"{name}->{new}: view already has {new} elsewhere")
                continue
            if new in names_at:
                continue  # a union alias: the canonical name is already there
            if name in words:
                skipped.append(f"{name}->{new}: in one of the original's symbols")
                continue
            if name in elsewhere[unit]:
                skipped.append(f"{name}->{new}: also a member of another class in the file")
                continue
            if ty != hdr_types.get(key):
                skipped.append(f"{name}->{new}: type {ty} vs header {hdr_types.get(key)} (renamed anyway)")
            mapping[name] = new
        out[unit] = (mapping, skipped)
    return out


def verify(unit):
    r = subprocess.run(["python3", "research/tools/one_unit.py", unit], cwd=ROOT, capture_output=True, text=True, timeout=1200)
    tail = (r.stdout + r.stderr).strip().splitlines()
    return unit, bool(tail) and "VERIFIED" in tail[-1], tail[-1] if tail else ""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("cls")
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("--only", nargs="*")
    ap.add_argument("--jobs", type=int, default=4)
    a = ap.parse_args()
    header = header_layout(a.cls)
    if not header:
        raise SystemExit(f"no layout for {a.cls} in include/game/{a.cls}.h")
    units = units_with_view(a.cls)
    if a.only:
        units = [u for u in units if u[0] in a.only]
    with ProcessPoolExecutor(a.jobs) as ex:
        compiled = list(ex.map(layout_lint.compile_sym, units))
    views, texts, srcs, elsewhere = {}, {}, {}, {}
    for (unit, settings, src), (_, v, err) in zip(units, compiled):
        if v is None:
            print(f"# {unit}: doesn't compile with -sym on: {err[:120]}")
            continue
        views[unit] = flatten(v, a.cls)
        chain = class_chain(v, a.cls)
        elsewhere[unit] = {m[0] for x in v if x["name"] not in chain for m in x["members"]}
        srcs[unit] = ROOT / src
        texts[unit] = srcs[unit].read_text()
    canon, variants = canonical(a.cls, header, views)
    print(f"# {a.cls}: header members and the names the {len(views)} views use")
    for off, size, name, ty in header:
        c, from_hdr = canon[(off, size)]
        others = ", ".join(f"{n} {k}" for n, k in variants[(off, size)].most_common())
        flag = "" if from_hdr else f"   <- header should take '{c}'"
        print(f"+{off:#05x} {size:3} {name:16} {ty:14} canonical {c:16} views: {others}{flag}")
    p = plan(a.cls, header, views, canon, symbol_words(), texts, elsewhere)
    todo = {u: m for u, (m, s) in p.items() if m}
    for unit in sorted(p):
        mapping, skipped = p[unit]
        if mapping or skipped:
            print(f"{unit}: " + ", ".join(f"{o}->{n}" for o, n in sorted(mapping.items())) + ("" if not skipped else "   skipped: " + "; ".join(skipped)))
    print(f"# {len(todo)} units to rename")
    if not a.apply:
        return
    backups = {}
    for unit, mapping in todo.items():
        backups[unit] = texts[unit]
        srcs[unit].write_text(rename_text(texts[unit], mapping))
    with ProcessPoolExecutor(a.jobs) as ex:
        results = list(ex.map(verify, sorted(todo)))
    kept = 0
    for unit, ok, last in results:
        if ok:
            kept += 1
        else:
            srcs[unit].write_text(backups[unit])
        print(f"{'kept    ' if ok else 'reverted'} {unit}: {last[:100]}")
    print(f"# {kept} of {len(todo)} units renamed and verified")


if __name__ == "__main__":
    main()
