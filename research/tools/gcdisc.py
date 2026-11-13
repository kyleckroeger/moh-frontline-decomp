#!/usr/bin/env python3
"""List or extract files of a GameCube disc image through its FST.

  gcdisc.py DISC.iso -  [GLOB]        list matching files with their sizes
  gcdisc.py DISC.iso OUTDIR [GLOB]    extract them under OUTDIR (keep OUTDIR outside the repo
                                      or under scratch/: it is game data)

GLOB matches the full path, case-insensitive, for example '/DATA/[1-6]/*/level.viv'.
dolphin_boot.py's work directory holds a copy of the disc (patched.iso); only the ELF
appended by dolphin_boot.py differs from the original disc.
"""
import fnmatch
import os
import struct
import sys


def main():
    if len(sys.argv) < 3:
        sys.exit(__doc__)
    iso, out = sys.argv[1], sys.argv[2]
    pattern = sys.argv[3] if len(sys.argv) > 3 else "*"
    f = open(iso, "rb")
    f.seek(0x424)
    fst_offset, fst_size = struct.unpack(">II", f.read(8))
    f.seek(fst_offset)
    fst = f.read(fst_size)
    count = struct.unpack_from(">I", fst, 8)[0]
    names = 12 * count
    dirs = []  # (index after the directory's last entry, path)
    for i in range(1, count):
        word, a, b = struct.unpack_from(">III", fst, 12 * i)
        start = names + (word & 0xFFFFFF)
        name = fst[start:fst.index(b"\0", start)].decode("latin1")
        while dirs and i >= dirs[-1][0]:
            dirs.pop()
        path = (dirs[-1][1] if dirs else "") + "/" + name
        if word >> 24:
            dirs.append((b, path))
        elif fnmatch.fnmatch(path.upper(), pattern.upper()):
            if out == "-":
                print(path, b)
            else:
                target = os.path.join(out, path.lstrip("/"))
                os.makedirs(os.path.dirname(target), exist_ok=True)
                f.seek(a)
                with open(target, "wb") as w:
                    w.write(f.read(b))


if __name__ == "__main__":
    main()
