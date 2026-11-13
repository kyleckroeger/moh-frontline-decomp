#!/usr/bin/env python3
"""vtable_order.py: do the class views declare their virtual functions in the original's order?

PortReadiness steps 2.3 and 3.2. A compiler regenerates a class's __vt__ only from a
declaration whose virtuals come in the original's order, and every virtual call a view
makes uses the slot its own declaration order gives. research/layout/vtables.tsv
(vtables.py --tsv) holds the original's order, from the relocations.

This reads every class view in the source (src/, include/game/, and with --drafts the
drafts) as text, computes the slot each declared virtual gets (bases first, overrides
keep the base's slot, new virtuals append in declaration order; MWCC also appends an
override of a secondary base's virtual, as CParticleSystemManager's table shows), and
compares the primary part with the original's:

    order      a named virtual sits at a slot that holds another method, and the original
               has that name at another slot
    rename     a named virtual sits at a slot whose method the original names differently
               (the name is not in the table: a guessed name, harmless to the code)
    placeholder  a placeholder (unknown024, vf24, Function024; a few views count in decimal)
                 names a different offset than it gets
    beyond     a virtual past the end of the original's table
    signature  the right method but a different constness or argument count

Only classes the original has a __vt__ for are compared, by (qualified) name. Text
parsing, not the compiler (MWCC's DWARF 1 has no member functions): templates are skipped,
and a view whose base can't be found is reported as such.

    vtable_order.py [--drafts]           summary per class, mismatching views listed
    vtable_order.py --class NAME         every view of one class with its computed slots
    vtable_order.py --md                 the mismatch table as Markdown
"""
import argparse, collections, csv, glob, os, re, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TSV = ROOT / "research/layout/vtables.tsv"
PLACEHOLDER = re.compile(r"^(?:unknown|unk|vf|vfunc|virt|virtual|slot|v|func|function|f)_?(?:0x)?([0-9A-Fa-f]{2,3})$", re.I)
KEYWORDS = {"int", "char", "short", "long", "float", "double", "bool", "void", "unsigned", "signed",
            "const", "volatile", "struct", "class", "enum", "union"}


# ------------------------------------------------------------------ the original

def original():
    """{qualified class: [(name, const, nargs, mangled)] for every word from +8}, plus the raw rows."""
    tables, rows = {}, collections.defaultdict(list)
    for r in csv.DictReader(open(TSV), delimiter="\t"):
        rows[r["class"]].append(r)
    for cls, rs in rows.items():
        # every word after the first part's RTTI and offset: markers for later parts' RTTI and offset
        tables[qualify(cls)] = [("<rtti>", False, None, r["value"]) if r["role"] == "rtti" else
                                ("<offset>", False, None, r["value"]) if r["role"] == "offset" else
                                method_of(r["value"], r["demangled"]) for r in rs[2:]]
    return tables, rows


def qualify(key):
    """Q24RCMP5CODEC -> RCMP::CODEC; ISceneNode -> ISceneNode."""
    if not key.startswith("Q"):
        return key
    n, i, parts = int(key[1]), 2, []
    for _ in range(n):
        m = re.match(r"\d+", key[i:])
        ln = int(m.group()); i += len(m.group())
        parts.append(key[i:i + ln]); i += ln
    return "::".join(parts)


def method_of(mangled, demangled):
    if mangled.startswith("@"):  # a secondary part's thunk: @offset@target
        mangled = mangled.split("@", 2)[2]
    if mangled == "0":
        return ("<pure>", False, None, "0")
    name = "~" if mangled.startswith("__dt__") else mangled.split("__")[0] or mangled
    if mangled.startswith("__"):
        name = "~" if mangled.startswith("__dt__") else "operator"
    const, nargs = False, None
    m = re.match(r"^(.*?)\((.*)\)( const)?$", demangled or "")
    if m:
        const = bool(m.group(3))
        args = split_top(m.group(2))
        nargs = 0 if args in ([], ["void"]) else len(args)
    return (name, const, nargs, mangled)


# ------------------------------------------------------------------ parsing views

def strip(text):
    text = re.sub(r"//[^\n]*|/\*.*?\*/", lambda m: " " * len(m.group()) if "\n" not in m.group() else
                  re.sub(r"[^\n]", " ", m.group()), text, flags=re.S)
    text = re.sub(r'"(?:\\.|[^"\\\n])*"', '""', text)
    text = re.sub(r"'(?:\\.|[^'\\\n])*'", "''", text)
    text = re.sub(r"^[ \t]*#[^\n]*(?:\\\n[^\n]*)*", "", text, flags=re.M)
    return text


def match_brace(t, i, o="{", c="}"):
    depth = 0
    for j in range(i, len(t)):
        if t[j] == o:
            depth += 1
        elif t[j] == c:
            depth -= 1
            if depth == 0:
                return j
    return len(t) - 1


def split_top(s):
    out, depth, cur = [], 0, ""
    for ch in s:
        if ch in "(<[":
            depth += 1
        elif ch in ")>]":
            depth -= 1
        if ch == "," and depth == 0:
            out.append(cur.strip()); cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


HEAD = re.compile(r"\b(class|struct|namespace)\s+([A-Za-z_]\w*)\s*(:[^{;()]*)?\{")


def parse_classes(text):
    """[{name, bases, methods:[(name, params, const, virtual)], template}] in source order."""
    t = strip(text)
    out = []

    def scan(lo, hi, scope):
        i = lo
        while True:
            m = HEAD.search(t, i, hi)
            if not m:
                return
            kind, name = m.group(1), m.group(2)
            ob = m.end() - 1
            cb = match_brace(t, ob)
            if kind == "namespace":
                scan(ob + 1, cb, scope + [name])
            else:
                before = t[max(0, m.start() - 200):m.start()]
                template = bool(re.search(r"template\s*<[^;{}]*>\s*$", before))
                bases = []
                for b in split_top((m.group(3) or ":")[1:]):
                    b = re.sub(r"\b(public|private|protected|virtual)\b", "", b).strip()
                    if b:
                        bases.append(re.sub(r"<.*", "", b).split("::")[-1].strip())
                out.append(dict(name="::".join(scope + [name]), short=name, bases=bases, template=template,
                                methods=members(ob + 1, cb, name), at=t.count("\n", 0, m.start()) + 1))
                scan(ob + 1, cb, scope + [name])
            i = cb + 1

    def members(lo, hi, cls):
        res, i, start = [], lo, lo
        while i < hi:
            ch = t[i]
            if ch == "{":
                j = match_brace(t, i)
                stmt = t[start:i]
                if "(" in stmt and not re.search(r"\b(class|struct|union|enum)\b[^(]*$", stmt):
                    res.append(stmt); i = j + 1; start = i
                    continue
                i = j + 1
                continue
            if ch == "(":
                i = match_brace(t, i, "(", ")") + 1
                continue
            if ch == ";":
                res.append(t[start:i]); start = i + 1
            i += 1
        methods = []
        for stmt in res:
            s = re.sub(r"\b(public|private|protected)\s*:", " ", stmt).strip()
            p = s.find("(")
            if p < 0 or re.match(r"^(typedef|friend|using|enum|class|struct|union)\b", s):
                continue
            head = s[:p].rstrip()
            mo = re.search(r"(operator\s*\S+|~?\s*[A-Za-z_]\w*)$", head)
            if not mo:
                continue
            name = re.sub(r"\s+", "", mo.group(1))
            if name.startswith("~"):
                name = "~"
            elif name.startswith("operator"):
                name = "operator"
            if name == cls:
                continue  # constructor
            q = match_brace(s, p, "(", ")")
            params = [norm_param(x) for x in split_top(s[p + 1:q])]
            params = [x for x in params if x and x != "void"]
            tail = s[q + 1:]
            virtual = bool(re.search(r"\bvirtual\b", head))
            if re.search(r"\bstatic\b", head):
                continue
            methods.append((name, tuple(params), bool(re.match(r"\s*const\b", tail)), virtual))
        return methods

    scan(0, len(t), [])
    return out


def norm_param(p):
    p = p.split("=")[0].strip()
    toks = re.findall(r"[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*|[*&]|\.\.\.|\[[^\]]*\]", p)
    toks = [x for x in toks if not x.startswith("[")]
    if len(toks) > 1 and re.match(r"[A-Za-z_]", toks[-1]) and toks[-1] not in KEYWORDS and \
            not (len(toks) == 2 and toks[0] in ("const", "struct", "class", "enum", "unsigned", "signed")):
        toks = toks[:-1]
    return " ".join(x.split("::")[-1] for x in toks if x not in ("struct", "class", "enum"))


# ------------------------------------------------------------------ vtables of views

def resolve_includes(path, seen=None):
    """Files whose class definitions `path` can see: its quoted includes, recursively."""
    seen = seen if seen is not None else []
    for m in re.finditer(r'^\s*#\s*include\s*"([^"]+)"', open(path, errors="replace").read(), re.M):
        for cand in (Path(path).parent / m.group(1), ROOT / "include" / m.group(1)):
            if cand.exists():
                c = str(cand.resolve())
                if c not in seen:
                    seen.append(c)
                    resolve_includes(c, seen)
                break
    return seen


_PARSED = {}


def classes_of(path):
    if path not in _PARSED:
        _PARSED[path] = parse_classes(open(path, errors="replace").read())
    return _PARSED[path]


def view_table(cls, visible, depth=0):
    """Every word of the view's table from +8: [(name, params, const, owner, secondary)], with
    ("<rtti>",) and ("<offset>",) entries where a secondary base's part starts. MWCC lays out
    the primary base's table, then each secondary base's part (RTTI, offset, its slots), then
    the class's new virtuals and its overrides of secondary-base virtuals, in declaration
    order (CParticleSystemManager, CBulletDecalManager). None when a base can't be found."""
    if depth > 20:
        return None, "base cycle"
    slots, primary_done = [], False
    for b in cls["bases"]:
        bc = find(b, visible, before=cls)
        if bc is None:
            return None, f"base {b} not found"
        tab, err = view_table(bc, visible, depth + 1)
        if tab is None:
            return None, err
        if not tab:
            continue
        if not primary_done:
            slots, primary_done = list(tab), True
        else:
            slots += [("<rtti>", (), False, b, True), ("<offset>", (), False, b, True)]
            slots += [e[:4] + (True,) for e in tab]
    inherited = len(slots)  # an overload declared in the class itself is a new slot, not an override
    for name, params, const, virtual in cls["methods"]:
        hit = override(slots[:inherited], name, params, const)
        if hit is not None:
            secondary = slots[hit][4]
            slots[hit] = (name, params, const, cls["name"], secondary)
            if secondary:  # the thunk stays in the base's part; the method gets its own slot
                slots.append((name, params, const, cls["name"], False))
            continue
        if virtual:
            slots.append((name, params, const, cls["name"], False))
    return slots, None


def override(slots, name, params, const):
    same = [i for i, s in enumerate(slots) if s[0] == name]
    if name == "~":
        return same[0] if same else None
    exact = [i for i in same if s_sig(slots[i]) == (params, const)]
    if exact:
        return exact[0]
    loose = [i for i in same if len(slots[i][1]) == len(params) and slots[i][2] == const]
    return loose[0] if len(loose) == 1 else None


def s_sig(s):
    return (s[1], s[2])


def find(short, visible, before=None):
    """The class definition named `short` that the file sees (its own first, then includes)."""
    for path, cs in visible:
        cands = [c for c in cs if c["short"] == short]  # a base may be a template (dwi::IVisitor<T>)
        if cands:
            if before is not None and before in cs:
                prior = [c for c in cands if c["at"] <= before["at"]]
                if prior:
                    return prior[-1]
            return cands[0]
    return None


def compare(slots, orig):
    """[(kind, slot index, offset, view entry, original entry)]"""
    out = []
    for i, entry in enumerate(slots):
        name, params, const = entry[:3]
        off = 8 + 4 * i
        o = orig[i] if i < len(orig) else None
        if name in ("<rtti>", "<offset>"):
            if o is None or o[0] != name:
                out.append(("order", i, off, f"{name} of {entry[3]}", o[3] if o else "-"))
            continue
        ph = PLACEHOLDER.match(name)
        if ph:
            digits = ph.group(1)  # hex in most views, decimal in a few (unknown012 = +12)
            if int(digits, 16) != off and not (digits.isdigit() and int(digits) == off):
                out.append(("placeholder", i, off, name, o[3] if o else "-"))
            continue
        if o is None:
            out.append(("beyond", i, off, name, "-"))
        elif o[0] == "<pure>":
            continue
        elif o[0] != name:
            where = [j for j, x in enumerate(orig) if x[0] == name]
            out.append(("order" if where else "rename", i, off, name,
                        o[3] + (f" ({name} at +{8 + 4 * where[0]:#x})" if where else "")))
        elif o[1] != const or (o[2] is not None and o[2] != len(params)):
            out.append(("signature", i, off, f"{name}({', '.join(params)}){' const' if const else ''}", o[3]))
    return out


def files(drafts):
    fs = sorted(glob.glob(str(ROOT / "src/**/*.cpp"), recursive=True) +
                glob.glob(str(ROOT / "src/**/*.h"), recursive=True) +
                glob.glob(str(ROOT / "src/**/*.c"), recursive=True) +
                glob.glob(str(ROOT / "include/game/*.h")))
    if drafts:
        fs += sorted(glob.glob(str(ROOT / "research/drafts/**/*.c*"), recursive=True))
    return fs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--drafts", action="store_true")
    ap.add_argument("--class", dest="cls")
    ap.add_argument("--md", action="store_true")
    a = ap.parse_args()
    orig, _ = original()
    results = collections.defaultdict(list)  # class -> [(file, line, slots, problems or error)]
    for f in files(a.drafts):
        if "virtual" not in open(f, errors="replace").read():
            continue
        visible = [(f, classes_of(f))] + [(p, classes_of(p)) for p in resolve_includes(f)]
        for c in classes_of(f):
            if c["name"] not in orig or c["template"]:
                continue
            if not any(m[3] for m in c["methods"]) and not c["bases"]:
                continue  # declares no virtuals: nothing to compare
            slots, err = view_table(c, visible)
            rel = os.path.relpath(f, ROOT)
            if slots is None:
                results[c["name"]].append((rel, c["at"], None, err))
            else:
                results[c["name"]].append((rel, c["at"], slots, compare(slots, orig[c["name"]])))
    if a.cls:
        for rel, line, slots, probs in results.get(a.cls, []):
            print(f"{rel}:{line}: {len(slots or [])} slots", "" if slots is not None else probs)
            for p in (probs if slots is not None else []):
                print("   ", p)
        return
    rows = []
    for cls in sorted(results):
        views = results[cls]
        bad = [v for v in views if v[2] is None or any(p[0] != "signature" for p in v[3])]
        sig = [v for v in views if v[2] is not None and v[3] and all(p[0] == "signature" for p in v[3])]
        rows.append((cls, len(views), bad, sig))
    if a.md:
        # one row per view that differs; "kept off" = the file says its table is not emitted
        # there (a deliberate first-virtual order that keeps MWCC from emitting the table)
        print("| Class | File | Kinds | First difference (view at offset: original) | Kept off |")
        print("| --- | --- | --- | --- | --- |")
        for cls, n, bad, sig in rows:
            for rel, line, slots, probs in bad + sig:
                text = open(ROOT / rel, errors="replace").read()
                kept = "yes" if re.search(r"not (be )?emitted|isn't emitted|not emit", text) else ""
                if slots is None:
                    print(f"| {cls} | `{rel[len('src/game/'):] if rel.startswith('src/game/') else rel}:{line}` | base | {probs} | {kept} |")
                    continue
                kinds = collections.Counter(p[0] for p in probs)
                first = next((p for p in probs if p[0] != "signature"), probs[0])
                orig_txt = first[4].replace("|", "\\|")
                print(f"| {cls} | `{rel[len('src/game/'):] if rel.startswith('src/game/') else rel}:{line}` | "
                      f"{', '.join(f'{k} {v}' for k, v in sorted(kinds.items()))} | "
                      f"`{first[3]}` at +{first[2]:#x}: `{orig_txt}` | {kept} |")
        return
    tot = sum(n for _, n, _, _ in rows)
    print(f"{len(rows)} classes with views, {tot} views; {sum(len(b) for _, _, b, _ in rows)} differ in order, "
          f"{sum(len(s) for *_, s in rows)} only in signature")
    for cls, n, bad, sig in rows:
        print(f"{cls}: {n} views, {len(bad)} order, {len(sig)} signature")
        for rel, line, slots, probs in bad:
            if slots is None:
                print(f"    {rel}:{line}: {probs}")
            else:
                for p in probs:
                    if p[0] != "signature":
                        print(f"    {rel}:{line}: {p[0]} slot {p[1]} +{p[2]:#x}: view {p[3]}, original {p[4]}")
        for rel, line, slots, probs in sig:
            for p in probs:
                print(f"    {rel}:{line}: signature +{p[2]:#x}: view {p[3]}, original {p[4]}")


if __name__ == "__main__":
    try:
        main()
    except BrokenPipeError:
        pass
