#!/usr/bin/env python3
"""Find which level scripts call given built-ins, and where classes are placed.

  bs_find.py LEVELDIR --calls NAME...     per level.viv: the classes whose code calls BIFunc_NAME
  bs_find.py LEVELDIR --uses CLASS...     per level.viv: the other files naming CLASS
                                          (levelfile.lfc and M_S_p.bpd list placed objects)

LEVELDIR holds DATA/M/M_S/level.viv as extracted by gcdisc.py. Decoding comes from
bs_disasm.py and the original ELF (orig/GMFE69/Moh2RelGC.elf). Output is derived from game
data: do not commit it.
"""
import glob
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import bs_disasm as B  # noqa: E402

ELF = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "orig", "GMFE69", "Moh2RelGC.elf")


def calls(files, elf, opcodes, wanted):
    builtins = B.builtin_table(elf, files["bsbifunc.dat"])
    hits = {}
    for name in files:
        if not name.endswith(".cbs"):
            continue
        for words in B.parse_cbs(files[name]):
            try:
                for _, _, text in B.decode(words, opcodes, builtins):
                    for w in wanted:
                        if text.startswith("CALL " + w + " "):
                            hits.setdefault(w, set()).add(name[:-4])
            except ValueError:
                pass
    return " ".join("%s:%s" % (k, ",".join(sorted(v))) for k, v in sorted(hits.items()))


def uses(files, wanted):
    out = []
    for cls in wanted:
        key = cls.lower().encode()
        where = [n for n, d in files.items() if not n.startswith(cls.lower() + ".") and key in d.lower()]
        if where:
            out.append("%s:%s" % (cls, ",".join(sorted(where))))
    return " ".join(out)


def main():
    if len(sys.argv) < 4 or sys.argv[2] not in ("--calls", "--uses"):
        sys.exit(__doc__)
    elf = B.Elf(ELF)
    opcodes = B.opcode_table(elf)
    for viv in sorted(glob.glob(os.path.join(sys.argv[1], "DATA", "*", "*", "level.viv"))):
        files = B.read_viv(viv)
        if sys.argv[2] == "--calls":
            line = calls(files, elf, opcodes, sys.argv[3:])
        else:
            line = uses(files, sys.argv[3:])
        print(os.path.basename(os.path.dirname(viv)), line)


if __name__ == "__main__":
    main()
