#!/usr/bin/env python3
"""mismatch_kinds.py [OUT]: sort the unmatched drafts by the kind of mismatch, for research/plans/matching.md.

Compiles every draft in research/drafts/index.tsv and compares its instructions with the
original's (the same rows score.py and varmap.py use). The comparison is repeated with
the registers blanked out (every rN and fN becomes R):

    register     with registers blanked, the two sequences are identical. Only register
                 allocation differs: varmap.py, declaration order, const copies.
    order        same instructions with registers blanked, in a different order: scheduling
                 or block layout. Statement order, inline boundaries, loop shape.
    near         with registers blanked, the draft has at most 4 instructions the original
                 lacks and lacks at most 4 it has (one expression form, a missing or extra copy, a branch form).
    structural   more than that: the source shape is wrong somewhere (or the draft is
                 unfinished).
    no_compile   the draft doesn't compile with its unit's settings.

Columns: kind, size, real (score.py's differing lines), blind (positional differences with
registers blanked), extra and missing (instructions the draft has more or fewer of than the
original, registers blanked), frame (the stack frame differs), function, draft. Sorted by kind,
then by blind difference. A measurement, not a diagnosis: the kind says which tools to try first.
"""
import collections, csv, json, re, sys, tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "research/tools"))
import fast, varmap, score  # noqa: E402

KINDS = ["register", "order", "near", "structural", "no_compile"]


def blind(rows):
    """Registers blanked, and the immediates a relocation fills in (lis/addi/lfs/lwz... of a
    symbol's address, which read 0 in the draft's object) blanked too."""
    out = []
    for x in rows:
        x = re.sub(r"\b[fr]\d+\b", "R", x)
        x = re.sub(r"^(lis R,)-?\d+$", r"\1K", x)
        x = re.sub(r"^(addi R,R,)-?\d+$", r"\1K", x)
        out.append(x)
    return out


def frame(rows):
    for x in rows[:6]:
        m = re.match(r"stwu r1,(-\d+)\(r1\)", x)
        if m:
            return m.group(1)
    return None


def classify(row):
    a = row["address"].replace("0x", "")
    e = hex(int(a, 16) + int(row["size"]))
    text = open(ROOT / row["draft"], errors="replace").read()
    with tempfile.TemporaryDirectory() as w:
        obj, _ = fast.compile_object(json.load(open(ROOT / row["compile_settings_from"])), text, w)
        if not obj:
            return dict(kind="no_compile")
        m = varmap.rows(["--disassemble=" + row["function"], obj])
        o = varmap.rows(["--start-address=0x" + a, "--stop-address=" + e, fast.ELF])
    if not m:
        return dict(kind="no_compile")
    real = sum(1 for x, y in zip(o, m) if not score.eq(x, y)) + abs(len(o) - len(m))
    bm, bo = blind(m), blind(o)
    bdiff = sum(1 for x, y in zip(bo, bm) if not score.eq(x, y)) + abs(len(bo) - len(bm))
    cm, co = collections.Counter(bm), collections.Counter(bo)
    extra, missing = sum((cm - co).values()), sum((co - cm).values())
    if real == 0:
        kind = "register"  # data-only difference (score.py can't see small-data constants)
    elif bdiff == 0:
        kind = "register"
    elif extra == 0 and missing == 0:
        kind = "order"
    elif max(extra, missing) <= 4:
        kind = "near"
    else:
        kind = "structural"
    return dict(kind=kind, real=real, blind=bdiff, extra=extra, missing=missing,
                frame="" if frame(m) == frame(o) else f"{frame(m)}/{frame(o)}")


def main():
    out = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "research/plans/matching_kinds.tsv"
    rows = []
    for row in csv.DictReader(open(ROOT / "research/drafts/index.tsv"), delimiter="\t"):
        try:
            r = classify(row)
        except Exception as ex:  # one bad draft must not stop the survey
            r = dict(kind="no_compile", note=str(ex)[:60])
        r.update(size=row["size"], function=row["function"], draft=row["draft"])
        rows.append(r)
        print(f'{r["kind"]:10} {r.get("blind", "-"):>5} {row["function"][:70]}', flush=True)
    rows.sort(key=lambda r: (KINDS.index(r["kind"]), r.get("blind", 0), -int(r["size"])))
    cols = ["kind", "size", "real", "blind", "extra", "missing", "frame", "function", "draft"]
    with open(out, "w") as f:
        f.write("\t".join(cols) + "\n")
        for r in rows:
            f.write("\t".join(str(r.get(c, "")) for c in cols) + "\n")
    c = collections.Counter(r["kind"] for r in rows)
    b = collections.Counter()
    for r in rows:
        b[r["kind"]] += int(r["size"])
    for k in KINDS:
        print(f"{k:10} {c[k]:3} functions {b[k] / 1024:6.1f} KB")


if __name__ == "__main__":
    main()
