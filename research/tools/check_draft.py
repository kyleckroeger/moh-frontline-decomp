#!/usr/bin/env python3
"""check_draft.py: is a draft logically the same as the original, even if its bytes differ?

Compiles each draft and compares its function with the original by content, not by
address or register: functions called, floating constants, strings and
tables, named globals, field offsets, and how similar the instruction sequence is.

    check_draft.py <function substring>...   detail for matching drafts
    check_draft.py --all                     write research/drafts/status.tsv

Statuses, for code that is not yet linked and verified:
    code_exact    every instruction matches; still needs one_unit.py and reconstruct.py
    consistent    calls, constants, strings and globals all agree; registers or layout differ
    differs       at least one of those disagrees: a logic bug or an unfinished draft
    no_compile    the draft does not compile

"consistent" is not proof of behaviour, and similarity is not a measure of correctness.
"""
import collections, csv, difflib, json, re, struct, subprocess, sys, tempfile
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fast, score
from formats import Elf32
SDA21, LO, HA, REL24 = 109, 4, 6, 10
BRANCH = re.compile(r"^b[a-z]*[+-]?$")


def relocs(elf):
    symbols = list(elf.symbols())
    out = collections.defaultdict(dict)
    for s in elf.sections:
        if s["type"] == 4 and elf.sections[s["info"]]["name"] == ".text":
            data = elf.contents(s)
            for off in range(0, len(data), 12):
                where, info, addend = struct.unpack_from(">IIi", data, off)
                out[s["info"]][where] = (info & 255, symbols[info >> 8], addend)
    return out


class Image:
    """Reads bytes behind a symbol, for the linked original or a compiled object."""
    def __init__(self, path):
        self.elf = Elf32(Path(path).read_bytes())
        self.linked = self.elf.kind != 1
        self.rel = relocs(self.elf)
        self.symbols = list(self.elf.symbols())
        self.named = {}
        self.sizes = {}
        for sym in self.symbols:
            n = sym["name"]
            if n and sym["section"] and sym["section"] < 0xFF00 and not n.startswith(("@", "...")) and sym["type"] in (0, 1):
                self.named.setdefault((sym["section"], sym["address"]), n)
                self.sizes.setdefault((sym["section"], sym["address"]), sym.get("size", 0))
        self.data_relocs = set()
        for sec in self.elf.sections:
            if sec["type"] == 4 and sec["info"] and self.elf.sections[sec["info"]]["name"] not in (".text", "extab", "extabindex"):
                data = self.elf.contents(sec)
                for off in range(0, len(data), 12):
                    self.data_relocs.add((sec["info"], struct.unpack_from(">I", data, off)[0]))

    def name_at(self, sym, addend):
        """The named object a section-relative reference lands on, if there is one."""
        if not sym["section"] or sym["section"] >= 0xFF00:
            return None
        return self.named.get((sym["section"], sym["address"] + addend))

    def containing(self, sym, addend):
        """The named object around a reference into uninitialised data, if there is one."""
        if not sym["section"] or sym["section"] >= 0xFF00 or self.elf.sections[sym["section"]]["type"] != 8:
            return None
        where = sym["address"] + addend
        near = [a for s, a in self.named if s == sym["section"] and a <= where]
        if not near:
            return None
        start = (sym["section"], max(near))
        return self.named[start] if where < start[1] + self.sizes[start] else None  # Not a neighbour.

    def is_table(self, sym, addend):
        """A switch jump table: its words are relocated code addresses."""
        if not sym["section"] or sym["section"] >= 0xFF00:
            return False
        where = sym["address"] + addend
        if (sym["section"], where) in self.data_relocs:
            sec = self.elf.sections[sym["section"]]
            word = self.read(sym, addend, 4)
            if not self.linked:
                return True
            text = next(s for s in self.elf.sections if s["name"] == ".text")
            value = int.from_bytes(word, "big")
            return text["address"] <= value < text["address"] + text["size"] and sec["name"] != ".text"
        return False

    def read(self, sym, addend, size):
        if not sym["section"] or sym["section"] >= 0xFF00:
            return None
        sec = self.elf.sections[sym["section"]]
        if sec["type"] == 8:
            return bytes(size)
        off = sym["address"] + addend - (sec["address"] if self.linked else 0)
        return self.elf.contents(sec)[off:off + size]


def describe(img, sym, addend, mnem):
    """What a relocation refers to, independent of where it was placed."""
    name = sym["name"]
    if name and not name.startswith(("@", "...")) and not name.startswith("__pool_"):
        return ("global", plain(name) + (f"+{addend}" if addend else ""))
    at = img.name_at(sym, addend)
    if at:
        return ("global", plain(at))
    around = img.containing(sym, addend)  # Middle of a variable: no contents to compare.
    if around:
        return ("global", plain(around))
    if mnem in ("lfs", "lfsu", "lfsx"):
        raw = img.read(sym, addend, 4)
        return ("const", f"{struct.unpack('>f', raw)[0]:g} ({raw.hex()})") if raw and len(raw) == 4 else ("unknown", name)
    if mnem in ("lfd", "lfdu"):
        raw = img.read(sym, addend, 8)
        return ("const", raw.hex()) if raw and len(raw) == 8 else ("unknown", name)
    return ("data", item(img, sym, addend))


def plain(name):
    """Drop a function-static's compiler-numbered suffix (init$765)."""
    return re.sub(r"\$\d+$", "", name)


def callee(name):
    """A called function, without parameter types: drafts may use their own views."""
    m = re.match(r"(.+?)__(\d+)(\w+)", name)
    if m and m[1]:
        return f"{m[3][:int(m[2])]}::{m[1]}"
    m = re.match(r"(.+?)__Q\d((?:\d+\w+?)+?)F", name)
    if m and m[1]:
        return name.split("F")[0] if False else f"{m[2]}::{m[1]}"
    return name.split("__F")[0]


HELPERS = re.compile(r"^(_savegpr|_restgpr|_savefpr|_restfpr)_")


def item(img, sym, addend):
    at = img.name_at(sym, addend)
    if at:
        return plain(at)
    if img.is_table(sym, addend):
        return "jump table"
    raw = img.read(sym, addend, 256) or b""
    end = raw.find(b"\0")
    if end > 0 and all(32 <= c < 127 or c in (9, 10, 13) or c >= 0xa0 for c in raw[:end]):
        return raw[:end].decode("latin-1")
    head = raw[:4]
    return "zero" if not any(head) else "bytes:" + head.hex()


def analyse(img, rows):
    """rows: (location, mnemonic, operands) -> feature summary."""
    calls, refs, offsets, seq = [], collections.Counter(), collections.Counter(), []
    bases = {}
    rel = next(iter(img.rel.values())) if img.linked else None
    text = next(i for i, s in enumerate(img.elf.sections) if s["name"] == ".text")
    rel = img.rel[text]
    pending = {}
    for where, mn, ops in rows:
        mn = mn.rstrip("+-")
        seq.append(mn)
        r = rel.get(where) or rel.get(where + 2)
        args = ops.split(",") if ops else []
        dest = args[0] if args else ""
        if r and r[0] == REL24:
            if mn == "bl" and not HELPERS.match(r[1]["name"]):
                calls.append(callee(r[1]["name"]))
            continue
        if mn == "bl" and not r:
            m = re.search(r"<(.+?)>", ops)
            if not (m and HELPERS.match(m[1])):
                calls.append(callee(m[1]) if m else ops)
            continue
        if r and r[0] == HA:
            pending[dest] = r
            bases.pop(dest, None)
            continue
        if r and r[0] == LO:
            sym, add = r[1], r[2]
            if mn == "addi":
                nm = sym["name"]
                if nm.startswith("..."):
                    bases[dest] = (sym, add)
                    if add and img.name_at(sym, add):  # At offset 0 it's only the block base.
                        refs[("global", plain(img.name_at(sym, add)))] += 1
                else:
                    refs[describe(img, sym, add, mn)] += 1
                    if not nm.startswith("@"):
                        bases[dest] = (sym, add)
            else:
                refs[describe(img, sym, add, mn)] += 1
            continue
        if r and r[0] == SDA21:
            refs[describe(img, r[1], r[2], mn)] += 1
            continue
        m = re.match(r"(-?\d+)\((r\d+)\)", args[1]) if len(args) > 1 else None
        if m and mn[0] in "ls":
            imm, base = int(m[1]), m[2]
            if base in bases and not bases[base][0]["name"]:
                pass
            if base in bases and bases[base][0]["name"].startswith("..."):
                refs[describe(img, bases[base][0], bases[base][1] + imm, mn)] += 1
            elif base in ("r10", "r11", "r12") and (imm < 0 or imm >= 0xF00):
                pass  # register saves and restores in the prologue and epilogue, not fields
            elif base not in ("r1", "r2", "r13"):
                offsets[(mn[0], imm)] += 1
            if mn[0] == "l":
                bases.pop(dest, None)
            continue
        if mn == "addi" and len(args) == 3 and args[1] in bases and bases[args[1]][0]["name"].startswith("..."):
            refs[describe(img, bases[args[1]][0], bases[args[1]][1] + int(args[2]), mn)] += 1
            bases.pop(dest, None)
            continue
        if (mn == "addi" and len(args) == 3 and args[1] not in ("r1", "r2", "r13") and args[1] not in bases
                and args[0] != args[1]):
            # A field's address, e.g. a member passed by reference (SweepAllExtents passed
            # m_exclusions from +264 where the original uses +280; loads alone missed it).
            # 0 and +-1 are arithmetic (copies, counters); large values are address halves.
            # "addi rX,rX,n" steps a pointer through an array (BSCleanUpBIFuncs' +4 loop
            # over g_pPopUpMessageHandler), so it isn't counted: a draft that indexes the
            # array instead has no such instruction.
            if 2 <= int(args[2], 0) < 0x2000:
                offsets[("a", int(args[2], 0))] += 1
        if dest in bases and dest.startswith("r") and not mn.startswith(("cmp", "st", "b")):
            if mn == "mr" and args[1] in bases:
                bases[dest] = bases[args[1]]
            else:
                bases.pop(dest)
        elif mn == "mr" and len(args) == 2 and args[1] in bases:
            bases[dest] = bases[args[1]]
    return dict(calls=calls, refs=refs, offsets=offsets, seq=seq)


def disasm(args):
    out = subprocess.run([fast.OBJDUMP, "-d", *args], capture_output=True, text=True).stdout
    rows = []
    for line in out.splitlines():
        m = re.match(r"^\s*([0-9a-f]+):\t[0-9a-f ]+\t(\S+)\s*(.*)$", line)
        if m:
            rows.append((int(m[1], 16), m[2], re.sub(r"\s+", "", m[3].split("<")[0]) + (m[3][m[3].find("<"):] if "<" in m[3] else "")))
    return rows


ORIG = None


def compare(draft_text, settings, function, address, size):
    global ORIG
    ORIG = ORIG or Image(fast.ELF)
    a = int(address, 16)
    target = analyse(ORIG, disasm([f"--start-address={a:#x}", f"--stop-address={a + int(size):#x}", fast.ELF]))
    with tempfile.TemporaryDirectory() as work:
        obj, log = fast.compile_object(json.load(open(settings)), draft_text, work)
        if not obj:
            return dict(status="no_compile", log=log[-400:])
        # The compiler names static initialisers after the file it compiled.
        emitted = "__sinit_probe_cpp" if function.startswith("__sinit_") else function
        rows = disasm(["--disassemble=" + emitted, obj])
        if not rows:
            return dict(status="no_compile", log="function not emitted")
        ours = analyse(Image(obj), rows)
    sim = difflib.SequenceMatcher(None, target["seq"], ours["seq"], autojunk=False).ratio()
    # Reuse of an address in a register changes how often a global appears, not
    # which one is used, so globals compare by presence; values compare by count.
    def counted(refs):
        return collections.Counter({k: (1 if k[0] == "global" else v) for k, v in refs.items()})
    missing = counted(target["refs"]) - counted(ours["refs"])
    extra = counted(ours["refs"]) - counted(target["refs"])
    osim = sum((target["offsets"] & ours["offsets"]).values()) / max(1, sum((target["offsets"] | ours["offsets"]).values()))
    lines = score.real(settings, draft_text, function, address, size)
    # Code order follows the compiler's block layout, not execution order, so the same
    # calls in another order are reported but don't fail a draft on their own.
    same = sorted(target["calls"]) == sorted(ours["calls"]) and not missing and not extra
    status = "differs" if not same else "code_exact" if lines and lines[0] == 0 else "consistent"
    return dict(status=status, similarity=sim, offsets=osim, lines=lines[0] if lines else None,
                length=(len(target["seq"]), len(ours["seq"])), calls_same=target["calls"] == ours["calls"],
                target_calls=target["calls"], our_calls=ours["calls"], missing=missing, extra=extra,
                offsets_missing=target["offsets"] - ours["offsets"], offsets_extra=ours["offsets"] - target["offsets"])


def report(row, r):
    print(f"{row['function']}\n  {row['draft']}\n  status {r['status']}", end="")
    if r["status"] == "no_compile":
        print("\n  " + r["log"])
        return
    print(f"  similarity {r['similarity']:.1%}  offsets {r['offsets']:.1%}  lines {r['lines']}  length {r['length']}")
    if not r["calls_same"]:
        d = list(difflib.unified_diff(r["target_calls"], r["our_calls"], "original", "draft", lineterm="", n=0))[2:]
        print("  calls (in code order): " + "; ".join(x for x in d if not x.startswith("@@"))[:800])
    for label, c in (("missing", r["missing"]), ("extra", r["extra"])):
        if c:
            print(f"  {label}: " + "; ".join(f"{k[0]} {k[1]!r}" + (f" x{v}" if v > 1 else "") for k, v in c.items())[:800])
    if r["offsets"] < 1:
        # Field offsets (load/store displacements off non-stack registers). Counts differ
        # when the compiler reuses a loaded value; an offset only one side has is the flag.
        only_t = sorted(set(k[1] for k in r["offsets_missing"]) - set(k[1] for k in r["offsets_extra"]))
        only_o = sorted(set(k[1] for k in r["offsets_extra"]) - set(k[1] for k in r["offsets_missing"]))
        both = sorted(set(k[1] for k in r["offsets_missing"]) & set(k[1] for k in r["offsets_extra"]))
        print(f"  offsets only in original: {[hex(x) for x in only_t]}")
        print(f"  offsets only in draft:    {[hex(x) for x in only_o]}")
        if both:
            print(f"  offsets used a different number of times: {[hex(x) for x in both]}")


def main():
    rows = list(csv.DictReader(open("research/drafts/index.tsv"), delimiter="\t"))
    if sys.argv[1:] == ["--all"]:
        with open("research/drafts/status.tsv", "w") as out:
            out.write("status\tsimilarity\toffsets\tlines\tsize\tfunction\tcalls_same\tmissing\textra\tdraft\n")
            for row in rows:
                text = open(row["draft"], errors="replace").read()
                r = compare(text, row["compile_settings_from"], row["function"], row["address"], row["size"])
                if r["status"] == "no_compile":
                    out.write(f"no_compile\t\t\t\t{row['size']}\t{row['function']}\t\t\t\t{row['draft']}\n")
                else:
                    out.write(f"{r['status']}\t{r['similarity']:.3f}\t{r['offsets']:.3f}\t{r['lines']}\t{row['size']}\t{row['function']}\t{r['calls_same']}\t{sum(r['missing'].values())}\t{sum(r['extra'].values())}\t{row['draft']}\n")
                print(r["status"], row["function"][:60], flush=True)
        return
    for row in rows:
        if any(k in row["function"] for k in sys.argv[1:]):
            report(row, compare(open(row["draft"], errors="replace").read(), row["compile_settings_from"], row["function"], row["address"], row["size"]))


if __name__ == "__main__":
    main()
