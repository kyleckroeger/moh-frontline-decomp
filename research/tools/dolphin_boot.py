#!/usr/bin/env python3
"""Boot a linked ELF in Dolphin from WSL, and bisect drafts that crash it.

The game's ELF is loaded from the disc by a boot loader, so testing a linked
build means putting it in a disc image. This tool keeps a private copy of the
player's ISO in a Windows work directory, writes the ELF after the last file on
the disc, points the FST entry at it, and runs Dolphin with a logging profile.

  dolphin_boot.py prepare --iso ORIGINAL.iso --dolphin Dolphin.exe --work DIR
  dolphin_boot.py prepare --work DIR          (refresh the settings of an existing DIR)
  dolphin_boot.py run    --work DIR ELF [--secs 35] [--movie M.dtm] [--tag NAME]
  dolphin_boot.py control --work DIR ELF [...]  (run ELF with every draft restored)
  dolphin_boot.py bisect --work DIR ELF [--secs 35] [--reference CONTROL.log | --reach N]
  dolphin_boot.py diff A.log B.log
  dolphin_boot.py prove --route NAME TEST.log CONTROL.log   (mark the drafts TEST ran proven)

DIR must be on the Windows side (for example under /mnt/c/.../Temp) so Dolphin
can read it. `run` prints one JSON line: exception number, SRR0, LR, the
logged stack, how many movies played and whether the game booted. `bisect`
restores patched functions in the linked ELF to their original first
instruction, halving the set until it finds the draft that causes the crash,
then repeats until the rest boot cleanly. It prints and writes
DIR/bisect.json. One run takes about a minute.

Comparing play: link the drafts with research/probes/input_probe.cpp (the
input script and a state trace) and research/probes/clock_probe.cpp (a fixed
frame time, a fixed random seed and a frame-buffer hash, "FRAME n hash h").
With the clock probe, the game's state no longer depends on CPU timing, so the
control (`control`: the same ELF with every patched function restored) and the
drafted image must log identical TRACE and FRAME lines, and `bisect
--reference CONTROL.log` finds the first draft that changes them. Without it,
bisect a hang with --reach: changed code changes CPU timing and the game's
frame timing then drifts a little from the reference (11-09).

A Dolphin input movie (--movie, from input_script.py) plays the pad instead of
the probe; link with `input_script.py --real-pad` to keep the trace.

The original ELF comes from orig/GMFE69/Moh2RelGC.elf. No game data is
written inside the repository.
"""

import argparse
import glob
import json
import os
import re
import shutil
import struct
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ORIG = os.path.join(ROOT, "orig", "GMFE69", "Moh2RelGC.elf")
RUN_PS1 = os.path.join(os.path.dirname(os.path.abspath(__file__)), "dolphin", "run.ps1")
POWERSHELL = "/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe"
ELF_NAME = "Moh2RelGC.elf"
ALIGN = 0x8000

LOGGER_INI = """[Options]
Verbosity = 3
WriteToFile = True
WriteToConsole = False
WriteToWindow = False
[Logs]
OSREPORT = True
OSREPORT_HLE = True
BOOT = True
CORE = True
MI = True
"""
# 64 MB of MEM1 instead of 24: the drafts' code segment takes about 510 KB from the
# game's heap, and with 24 MB Mission 1 runs out while loading. Control and test
# images get the same memory, so comparisons stay fair. The clock is fixed too:
# with the real time, two runs of the same image drift apart within a minute.
# The speed limit is off: emulated time doesn't depend on it, and runs finish sooner.
DOLPHIN_INI = """[Analytics]
PermissionAsked = True
Enabled = False
[Interface]
ConfirmStop = False
UsePanicHandlers = False
[Display]
RenderToMain = True
[Core]
CPUThread = False
RAMOverrideEnable = True
MEM1Size = 67108864
EnableCustomRTC = True
CustomRTCValue = 946684800
EmulationSpeed = 0.00000000
"""
# Frame buffer copies go to main memory, where clock_probe.cpp hashes them.
GFX_SETTINGS = {"Hacks": {"XFBToTextureEnable": "False"}}
# Functions the test probes replace: never restored by bisect or control.
PROBES = ["iPAD_update__Fv", "Wait__7CScreenFv", "MathFunSRandom__FUi"]


def winpath(path):
    return subprocess.run(["wslpath", "-w", path], capture_output=True, text=True, check=True).stdout.strip()


def fst_files(f):
    """Return (fst_offset, [(name, data_offset, size, entry_index)])."""
    f.seek(0x424)
    off, size = struct.unpack(">II", f.read(8))
    f.seek(off)
    fst = f.read(size)
    count = struct.unpack_from(">I", fst, 8)[0]
    strings = count * 12
    out = []

    def walk(i, end, prefix):
        while i < end:
            word, a, b = struct.unpack_from(">III", fst, i * 12)
            at = strings + (word & 0xFFFFFF)
            name = fst[at:fst.index(b"\0", at)].decode("latin1")
            if word >> 24:
                walk(i + 1, b, prefix + name + "/")
                i = b
            else:
                out.append((prefix + name, a, b, i))
                i += 1

    walk(1, count, "")
    return off, out


def segments(data):
    phoff = struct.unpack_from(">I", data, 0x1C)[0]
    size, count = struct.unpack_from(">HH", data, 0x2A)
    return [struct.unpack_from(">8I", data, phoff + size * i) for i in range(count)]


def file_offset(segs, va):
    for s in segs:
        if s[0] == 1 and s[4] and s[2] <= va < s[2] + s[4]:
            return s[1] + va - s[2]
    return None


def function_names(data):
    shoff = struct.unpack_from(">I", data, 0x20)[0]
    shnum = struct.unpack_from(">H", data, 0x30)[0]
    sections = [struct.unpack_from(">10I", data, shoff + 40 * i) for i in range(shnum)]
    names = {}
    for s in sections:
        if s[1] != 2:
            continue
        strtab = sections[s[6]]
        for at in range(s[4], s[4] + s[5], 16):
            name_at, value, size, info, _, shndx = struct.unpack_from(">IIIBBH", data, at)
            if size and shndx and info & 0xF == 2:
                start = strtab[4] + name_at
                names.setdefault(value, data[start:data.index(b"\0", start)].decode("latin1"))
    return names


class Linked:
    """The original and linked images, and the function entries the link patched."""

    def __init__(self, path, keep=()):
        self.orig = open(ORIG, "rb").read()
        self.linked = open(path, "rb").read()
        self.osegs, self.lsegs = segments(self.orig), segments(self.linked)
        self.names = function_names(self.orig)
        self.patches = []
        for s in self.osegs:
            if not (s[4] and s[6] & 1):
                continue
            for o in range(0, s[4] - 3, 4):
                va = s[2] + o
                lo = file_offset(self.lsegs, va)
                if lo is None or va not in self.names:
                    continue
                old = self.orig[s[1] + o:s[1] + o + 4]
                new = self.linked[lo:lo + 4]
                if old != new and new[0] >> 2 == 18 and self.names[va] not in keep:
                    self.patches.append(va)

    def build(self, active):
        """The linked image with every patch outside `active` restored."""
        out = bytearray(self.linked)
        for va in self.patches:
            if va not in active:
                lo, oo = file_offset(self.lsegs, va), file_offset(self.osegs, va)
                out[lo:lo + 4] = self.orig[oo:oo + 4]
        return bytes(out)

    def name(self, va):
        return self.names.get(va, hex(va))


def put_elf(work, elf):
    """Write `elf` after the last file on the disc and point the FST at it."""
    iso = os.path.join(work, "patched.iso")
    with open(iso, "r+b") as f:
        fst_off, files = fst_files(f)
        entry = [x for x in files if x[0] == ELF_NAME][0]
        end = max(a + b for name, a, b, _ in files if name != ELF_NAME)
        at = (end + ALIGN - 1) // ALIGN * ALIGN
        f.seek(0, 2)
        if at + len(elf) > f.tell():
            sys.exit("ELF does not fit after the last file on the disc")
        f.seek(at)
        f.write(elf)
        f.seek(fst_off + entry[3] * 12 + 4)
        f.write(struct.pack(">II", at, len(elf)))


def set_ini(path, settings):
    """Set keys in an ini file, keeping the rest of it."""
    lines = open(path).read().splitlines() if os.path.exists(path) else []
    for section, values in settings.items():
        head = f"[{section}]"
        if head not in lines:
            lines += [head]
        at = lines.index(head) + 1
        end = at
        while end < len(lines) and not lines[end].startswith("["):
            end += 1
        for key, value in values.items():
            for i in range(at, end):
                if lines[i].split("=")[0].strip() == key:
                    lines[i] = f"{key} = {value}"
                    break
            else:
                lines.insert(at, f"{key} = {value}")
                end += 1
    with open(path, "w") as f:
        f.write("\n".join(lines) + "\n")


def run(work, elf, secs, tag="run", movie=None, shot=15):
    put_elf(work, elf)
    # A fresh, empty memory card every run: a save left by one run changes the next.
    for card in glob.glob(os.path.join(work, "user", "GC", "*", "Card ?")):
        for name in os.listdir(card):
            os.remove(os.path.join(card, name))
    extra = []
    if movie:
        shutil.copy(movie, os.path.join(work, "movie.dtm"))
        extra = ["-movie", winpath(os.path.join(work, "movie.dtm"))]
    config = json.load(open(os.path.join(work, "dolphin_boot.json")))
    log = os.path.join(work, "user", "Logs", "dolphin.log")
    # A Dolphin left over from an interrupted run keeps the log open.
    subprocess.run(["/mnt/c/Windows/System32/taskkill.exe", "/IM", "Dolphin.exe", "/F"], capture_output=True)
    for _ in range(10):
        try:
            if os.path.exists(log):
                os.remove(log)
            break
        except PermissionError:
            time.sleep(2)
    subprocess.run(
        ["timeout", str(secs + 40), POWERSHELL, "-NoProfile", "-ExecutionPolicy", "Bypass",
         "-File", winpath(os.path.join(work, "run.ps1")), "-exe", config["dolphin"],
         "-iso", winpath(os.path.join(work, "patched.iso")), "-user", winpath(os.path.join(work, "user")),
         "-secs", str(secs), "-out", winpath(os.path.join(work, tag)), "-shot", str(shot)] + extra,
        cwd=work, capture_output=True)
    text = open(log, errors="replace").read() if os.path.exists(log) else ""
    text = text[text.find("Calling entry point"):]
    with open(os.path.join(work, tag + ".log"), "w") as f:
        f.write(text)
    trace = re.findall(r"TRACE (.*)", text)
    frames = re.findall(r"FRAME (.*)", text)
    exc = re.search(r"Unhandled Exception (\d+)", text)
    srr0 = re.search(r"SRR0 = (0x[0-9a-f]+)", text)
    lr = re.search(r"LR   = (0x[0-9a-f]+)", text)
    return dict(
        exc=int(exc.group(1)) if exc else None,
        srr0=srr0.group(1) if srr0 else None,
        lr=lr.group(1) if lr else None,
        stack=re.findall(r"0x[0-9a-f]{8}:\s+0x[0-9a-f]{8}\s+(0x[0-9a-f]{8})", text)[:8],
        movies=len(re.findall("Ave FrameRate", text)),
        booted="booted from" in text,
        lines=text.count("\n"),
        trace_lines=len(trace),
        frame_lines=len(frames),
        movie_end="Movie End" in text,
        covered=len(re.findall(r"COVER (\S+)", text)),
        slips=len(re.findall(r"SLIP \d+", text)),
        last_trace=trace[-1].strip() if trace else None)


def good(result):
    return result["exc"] is None and result["movies"] >= 1


def traces(path, skip=0, no_hash=False, ignore_frames=()):
    """The probes' TRACE and FRAME lines from a saved run log, without Dolphin's timestamps,
    from update (or frame) `skip` on. `no_hash` drops the player-object hash from TRACE lines:
    it also covers fields that stale stack memory reaches (see docs/Dolphin.md).
    `ignore_frames` drops the FRAME lines of those frame numbers: frames that depend on CPU
    timing even in the original (docs/Dolphin.md)."""
    out = []
    for m in re.finditer(r"((?:TRACE|FRAME) (\d+).*)", open(path, errors="replace").read()):
        if m.group(1).startswith("FRAME") and int(m.group(2)) in ignore_frames:
            continue
        if int(m.group(2)) >= skip:
            line = m.group(1).strip()
            if no_hash and line.startswith("TRACE"):
                line = re.sub(r" hash [0-9a-f]+$", "", line)
            out.append(line)
    return out


def first_difference(a, b):
    """(index, line in a, line in b) of the first differing trace line, or None."""
    for i, (x, y) in enumerate(zip(a, b)):
        if x != y:
            return i, x, y
    if len(a) != len(b):
        i = min(len(a), len(b))
        return i, a[i] if i < len(a) else None, b[i] if i < len(b) else None
    return None


def cmd_diff(args):
    a, b = (traces(args.a, args.skip, args.no_hash, args.ignore_frames),
            traces(args.b, args.skip, args.no_hash, args.ignore_frames))
    d = first_difference(a, b)
    if d is None:
        print("traces identical:", len(a), "lines")
    else:
        print(f"first difference at trace line {d[0]}:\n  {args.a}: {d[1]}\n  {args.b}: {d[2]}")
        sys.exit(1)


def covered(path, lines=None, skip=0):
    """Drafts a run first entered (link_drafts.py --coverage) after update or frame `skip`
    and before the `lines`-th trace line counted from there."""
    names, seen, started = [], 0, skip == 0
    for m in re.finditer(r"(COVER|TRACE|FRAME) (\S*)", open(path, errors="replace").read()):
        if m.group(1) == "COVER":
            if started:
                names.append(m.group(2))
        elif int(m.group(2)) >= skip:
            started = True
            seen += 1
            if lines is not None and seen >= lines:
                break
    return names


def cmd_prove(args):
    test = traces(args.test, args.skip, args.no_hash, args.ignore_frames)
    control = traces(args.control, args.skip, args.no_hash, args.ignore_frames)
    d = first_difference(test, control)
    if d is not None and not (d[0] == min(len(test), len(control)) and d[0] >= args.min_lines):
        sys.exit(f"not proven: first difference at line {d[0]}:\n  test:    {d[1]}\n  control: {d[2]}")
    same = d[0] if d else len(test)
    if same < args.min_lines:
        sys.exit(f"not proven: only {same} matching lines")
    names = covered(args.test, same, args.skip)
    if not names:
        sys.exit("no COVER lines: link the test image with link_drafts.py --coverage")
    print(f"{same} trace lines identical; {len(names)} drafts ran")
    # Keep the route that first proved a draft: mark only drafts without a mark
    # (check_draft.py --all clears a mark when its draft changes).
    with open(os.path.join(ROOT, "research", "drafts", "status.tsv")) as f:
        rows = [line.rstrip("\n").split("\t") for line in f]
    marked = {r[rows[0].index("function")] for r in rows[1:] if r[rows[0].index("proven")]}
    names = [n for n in names if n not in marked]
    if not names:
        print("no drafts without a mark")
        return
    subprocess.run([sys.executable, os.path.join(ROOT, "research", "tools", "check_draft.py"),
                    "--prove", args.route] + names, cwd=ROOT, check=True)


def cmd_prepare(args):
    config = os.path.join(args.work, "user", "Config")
    os.makedirs(config, exist_ok=True)
    with open(os.path.join(config, "Logger.ini"), "w") as f:
        f.write(LOGGER_INI)
    set_ini(os.path.join(config, "Dolphin.ini"),
            {sec: dict(l.split(" = ", 1) for l in body.strip().splitlines())
             for sec, body in re.findall(r"\[(\w+)\]\n([^\[]*)", DOLPHIN_INI)})
    set_ini(os.path.join(config, "GFX.ini"), GFX_SETTINGS)
    shutil.copy(RUN_PS1, os.path.join(args.work, "run.ps1"))
    if args.dolphin:
        dolphin = args.dolphin if re.match(r"^[A-Za-z]:\\", args.dolphin) else winpath(args.dolphin)
        json.dump({"dolphin": dolphin}, open(os.path.join(args.work, "dolphin_boot.json"), "w"))
    if args.iso:
        print("copying", args.iso)
        shutil.copy(args.iso, os.path.join(args.work, "patched.iso"))
    for need in ("patched.iso", "dolphin_boot.json"):
        if not os.path.exists(os.path.join(args.work, need)):
            sys.exit(f"{need} missing: give --iso and --dolphin")
    print("ready:", args.work)


def cmd_run(args):
    print(json.dumps(run(args.work, open(args.elf, "rb").read(), args.secs, args.tag or "run", args.movie, args.shot)))


def cmd_control(args):
    linked = Linked(args.elf, args.keep)
    print(json.dumps(run(args.work, linked.build(set()), args.secs, args.tag or "control", args.movie, args.shot)))


def cmd_bisect(args):
    linked = Linked(args.elf, args.keep)
    print("patched functions:", len(linked.patches), flush=True)
    log = open(os.path.join(args.work, "bisect.log"), "a")

    reference = traces(args.reference, args.skip, args.no_hash, args.ignore_frames) if args.reference else None
    if reference is not None and args.lines:
        reference = reference[:args.lines]

    def go(active, label):
        r = run(args.work, linked.build(set(active)), args.secs, "bisect-run", args.movie)
        r.update(label=label, active=len(active))
        if reference is not None:
            got = traces(os.path.join(args.work, "bisect-run.log"), args.skip, args.no_hash,
                         args.ignore_frames)[:len(reference)]
            d = first_difference(reference, got)
            r["differs_at"] = None if d is None else d[0]
        print(json.dumps(r), flush=True)
        log.write(json.dumps(r) + "\n")
        log.flush()
        return r

    def good(r):
        if r["exc"] is not None or r["movies"] < 1:
            return False
        if args.reach:
            # A hang or crash, not drift: changed code changes CPU timing, and the
            # game's frame timing then drifts from the reference after a few
            # thousand updates even when a draft is correct.
            return r["trace_lines"] >= args.reach
        return reference is None or r["differs_at"] is None

    if not good(go([], "control")):
        sys.exit("control failed: the image with every patch restored does not boot")
    remaining, culprits = list(linked.patches), []
    while True:
        if good(go(remaining, "remaining")):
            break
        candidates = remaining
        while len(candidates) > 1:
            half = len(candidates) // 2
            a, b = candidates[:half], candidates[half:]
            if not good(go(a, "half")):
                candidates = a
            elif not good(go(b, "half")):
                candidates = b
            else:
                print("interaction: neither half fails alone", flush=True)
                break
        print("CULPRIT", [linked.name(v) for v in candidates], flush=True)
        culprits += candidates
        remaining = [v for v in remaining if v not in candidates]
        if len(candidates) > 1:
            break
    result = dict(culprits=[linked.name(v) for v in culprits], clean=len(remaining))
    json.dump(result, open(os.path.join(args.work, "bisect.json"), "w"), indent=1)
    print("DONE", json.dumps(result))


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("prepare")
    s.add_argument("--iso", help="the player's disc image (first time only)")
    s.add_argument("--dolphin", help="Dolphin.exe, as a WSL or Windows path (first time only)")
    s.add_argument("--work", required=True)
    s.set_defaults(func=cmd_prepare)
    s = sub.add_parser("diff", help="compare the traces of two saved run logs")
    s.add_argument("--skip", type=int, default=0, help="ignore lines before this update or frame")
    s.add_argument("--no-hash", action="store_true", help="compare TRACE lines without the player-object hash")
    s.add_argument("--ignore-frames", type=lambda v: {int(x) for x in v.split(",")}, default=set(),
                   help="comma-separated frame numbers whose FRAME lines are left out (timing-dependent even in the original)")
    s.add_argument("a")
    s.add_argument("b")
    s.set_defaults(func=cmd_diff)
    s = sub.add_parser("prove", help="mark the drafts a matching run entered as proven in status.tsv")
    s.add_argument("--route", required=True)
    s.add_argument("--min-lines", type=int, default=100, help="at least this many identical trace lines")
    s.add_argument("--skip", type=int, default=0,
                   help="ignore lines before this update or frame; drafts first entered before it are not marked")
    s.add_argument("--no-hash", action="store_true", help="compare TRACE lines without the player-object hash")
    s.add_argument("--ignore-frames", type=lambda v: {int(x) for x in v.split(",")}, default=set(),
                   help="comma-separated frame numbers whose FRAME lines are left out (timing-dependent even in the original)")
    s.add_argument("test")
    s.add_argument("control")
    s.set_defaults(func=cmd_prove)
    for name, func in (("run", cmd_run), ("control", cmd_control), ("bisect", cmd_bisect)):
        s = sub.add_parser(name)
        s.add_argument("--work", required=True)
        s.add_argument("--secs", type=int, default=35)
        s.add_argument("--movie", help="a Dolphin input movie (.dtm) to play")
        s.add_argument("--shot", type=int, default=15, help="seconds between window captures")
        s.add_argument("--tag", help="run/control: name of the saved log (default: the command)")
        s.add_argument("--keep", nargs="*", default=PROBES,
                       help="patched functions that are test probes, never restored by bisect")
        s.add_argument("--reference", help="bisect: a control run's log; a run is good only if its trace matches")
        s.add_argument("--reach", type=int, help="bisect: a run is good if it doesn't crash and logs at least N trace lines")
        s.add_argument("--lines", type=int, help="bisect: compare only the first N trace lines (a run must reach them)")
        s.add_argument("--no-hash", action="store_true", help="bisect: compare TRACE lines without the player-object hash")
        s.add_argument("--ignore-frames", type=lambda v: {int(x) for x in v.split(",")}, default=set(),
                       help="comma-separated frame numbers whose FRAME lines are left out (timing-dependent even in the original)")
        s.add_argument("--skip", type=int, default=0,
                       help="bisect: ignore lines before this update or frame (the intro movies and shell "
                            "are paced by emulated time, so their frames differ when CPU timing does)")
        s.add_argument("elf")
        s.set_defaults(func=func)
    args = p.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
