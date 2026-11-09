#!/usr/bin/env python3
"""Boot a linked ELF in Dolphin from WSL, and bisect drafts that crash it.

The game's ELF is loaded from the disc by a boot loader, so testing a linked
build means putting it in a disc image. This tool keeps a private copy of the
player's ISO in a Windows work directory, writes the ELF after the last file on
the disc, points the FST entry at it, and runs Dolphin with a logging profile.

  dolphin_boot.py prepare --iso ORIGINAL.iso --dolphin Dolphin.exe --work DIR
  dolphin_boot.py run    --work DIR ELF [--secs 35]
  dolphin_boot.py bisect --work DIR ELF [--secs 35] [--reference CONTROL.log]
  dolphin_boot.py diff A.log B.log

DIR must be on the Windows side (for example under /mnt/c/.../Temp) so Dolphin
can read it. `run` prints one JSON line: exception number, SRR0, LR, the
logged stack, how many movies played and whether the game booted. `bisect`
restores patched functions in the linked ELF to their original first
instruction, halving the set until it finds the draft that causes the crash,
then repeats until the rest boot cleanly. It prints and writes
DIR/bisect.json. One run takes about a minute.

The original ELF comes from orig/GMFE69/Moh2RelGC.elf. No game data is
written inside the repository.
"""

import argparse
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
"""


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


def run(work, elf, secs, tag="run"):
    put_elf(work, elf)
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
         "-secs", str(secs), "-out", winpath(os.path.join(work, tag))],
        cwd=work, capture_output=True)
    text = open(log, errors="replace").read() if os.path.exists(log) else ""
    text = text[text.find("Calling entry point"):]
    with open(os.path.join(work, tag + ".log"), "w") as f:
        f.write(text)
    trace = re.findall(r"TRACE (.*)", text)
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
        last_trace=trace[-1].strip() if trace else None)


def good(result):
    return result["exc"] is None and result["movies"] >= 1


def traces(path):
    """The probe's trace lines from a saved run log, without Dolphin's timestamps."""
    return [m.group(1).strip() for m in re.finditer(r"TRACE (.*)", open(path, errors="replace").read())]


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
    d = first_difference(traces(args.a), traces(args.b))
    if d is None:
        print("traces identical:", len(traces(args.a)), "lines")
    else:
        print(f"first difference at trace line {d[0]}:\n  {args.a}: {d[1]}\n  {args.b}: {d[2]}")
        sys.exit(1)


def cmd_prepare(args):
    os.makedirs(os.path.join(args.work, "user", "Config"), exist_ok=True)
    with open(os.path.join(args.work, "user", "Config", "Logger.ini"), "w") as f:
        f.write(LOGGER_INI)
    with open(os.path.join(args.work, "user", "Config", "Dolphin.ini"), "w") as f:
        f.write(DOLPHIN_INI)
    shutil.copy(RUN_PS1, os.path.join(args.work, "run.ps1"))
    dolphin = args.dolphin if re.match(r"^[A-Za-z]:\\", args.dolphin) else winpath(args.dolphin)
    json.dump({"dolphin": dolphin}, open(os.path.join(args.work, "dolphin_boot.json"), "w"))
    print("copying", args.iso)
    shutil.copy(args.iso, os.path.join(args.work, "patched.iso"))
    print("ready:", args.work)


def cmd_run(args):
    print(json.dumps(run(args.work, open(args.elf, "rb").read(), args.secs)))


def cmd_bisect(args):
    linked = Linked(args.elf, args.keep)
    print("patched functions:", len(linked.patches), flush=True)
    log = open(os.path.join(args.work, "bisect.log"), "a")

    reference = traces(args.reference) if args.reference else None

    def go(active, label):
        r = run(args.work, linked.build(set(active)), args.secs, "bisect-run")
        r.update(label=label, active=len(active))
        if reference is not None:
            d = first_difference(reference, traces(os.path.join(args.work, "bisect-run.log")))
            r["differs_at"] = None if d is None else d[0]
        print(json.dumps(r), flush=True)
        log.write(json.dumps(r) + "\n")
        log.flush()
        return r

    def good(r):
        if r["exc"] is not None or r["movies"] < 1:
            return False
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
    s.add_argument("--iso", required=True)
    s.add_argument("--dolphin", required=True, help="Dolphin.exe, as a WSL or Windows path")
    s.add_argument("--work", required=True)
    s.set_defaults(func=cmd_prepare)
    s = sub.add_parser("diff", help="compare the traces of two saved run logs")
    s.add_argument("a")
    s.add_argument("b")
    s.set_defaults(func=cmd_diff)
    for name, func in (("run", cmd_run), ("bisect", cmd_bisect)):
        s = sub.add_parser(name)
        s.add_argument("--work", required=True)
        s.add_argument("--secs", type=int, default=35)
        s.add_argument("--keep", nargs="*", default=["iPAD_update__Fv"],
                       help="patched functions that are test probes, never restored by bisect")
        s.add_argument("--reference", help="bisect: a control run's log; a run is good only if its trace matches")
        s.add_argument("elf")
        s.set_defaults(func=func)
    args = p.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
