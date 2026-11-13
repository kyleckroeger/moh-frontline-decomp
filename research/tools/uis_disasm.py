#!/usr/bin/env python3
"""Disassemble Medal of Honor: Frontline shell screen scripts (UI Studio .scr files).

  uis_disasm.py FILE.scr --events            list the event handlers (offset, control, flags, event)
  uis_disasm.py FILE.scr --dis START END     disassemble a range of byte code
  uis_disasm.py FILE.scr --find MSG          the handlers that send shell message MSG (GameMessage)

The .scr files are in DATA/SHELL/shell.viv on the disc (extract with gcdisc.py, then
bs_disasm.read_viv). Big-endian. The format follows src/game/UIStudio.c (PatchScrData),
UISUtils.c (UISFindEventPC, UISExecuteFnc) and UISStack.c (UISStackProcess, the
interpreter); docs/ShellScripts.md describes it. Output is derived from game data: do not
commit it.
"""
import struct
import sys

# operand bytes after the opcode
OPLEN = {2: 1, 0x71: 1, 69: 1, 81: 1, 0x70: 1, 88: 1,
         15: 4, 16: 4, 17: 4, 67: 4, 78: 4, 99: 4, 100: 4, 101: 4,
         85: 6, 0x6b: 6, 86: 6, 87: 6, 95: 6, 0x6e: 6, 96: 6, 104: 6, 89: 7}
NAMES = {2: "thread", 0x71: "thread71", 3: "stopthread", 8: "unload", 88: "advrate", 6: "rate",
         7: "unrate", 10: "callfnc", 0x76: "thread76", 11: "MSG", 12: "thread7", 13: "pushdata",
         14: "strncpy", 15: "push", 16: "push", 17: "pushf", 18: "f2i", 19: "i2f", 20: "f2i2",
         21: "i2f2", 106: "i2p", 24: "load", 25: "store", 105: "addr", 26: "pick", 27: "put",
         28: "and", 29: "or", 30: "not", 31: "land", 32: "lor", 33: "lnot", 34: "abs", 35: "fabs",
         36: "neg", 37: "add", 38: "sub", 39: "mul", 40: "div", 41: "fneg", 42: "fadd", 43: "fsub",
         44: "fmul", 45: "fdiv", 46: "inc", 47: "dec", 48: "finc", 49: "fdec", 50: "le", 51: "ge",
         52: "lt", 53: "gt", 54: "eq", 55: "ne", 56: "fle", 57: "fge", 58: "flt", 59: "fgt",
         60: "feq", 61: "fne", 62: "jnz", 63: "jz", 64: "jmp", 65: "dup", 66: "drop", 67: "call",
         68: "ret", 69: "threadid", 70: "act6", 9: "sibling", 71: "act5", 72: "unloadself",
         73: "strlen", 74: "strget", 75: "strset", 76: "format", 77: "act2", 78: "skip4",
         82: "swap", 79: "evon", 80: "evoff", 81: "modal", 0x70: "modal70", 84: "act5b",
         85: "loadoff", 0x6b: "addroff", 86: "storeoff", 87: "addroff2", 90: "mod",
         99: "dupn", 100: "dropn", 101: "copyn", 111: "table", 91: "aget", 93: "agetp", 95: "agetoff",
         97: "agetpp", 92: "aset", 94: "asetp", 96: "asetoff", 98: "asetpp", 0x6c: "aaddr",
         0x6d: "aaddrp", 0x6e: "aaddroff", 102: "afill", 103: "afillp", 104: "afilloff", 89: "vis", 0x73: "hasrate",
         0x72: "act8", 0x74: "setflag", 0x75: "getflag"}


def u32(d, o):
    return struct.unpack_from(">I", d, o)[0]


def events(d):
    """(pc, kind, control index, sub, flags, event id) for every handler."""
    out = []
    n, ctl = u32(d, 0), u32(d, 4)
    for i in range(n):
        e = ctl + 20 * i
        control, subc, subs, evc, evs = struct.unpack_from(">5I", d, e)
        for s in range(subc):
            sub = u32(d, subs + 4 * s)
            sctl, sevc, sevs = struct.unpack_from(">3I", d, sub)
            for k in range(sevc):
                eid, state, pc = struct.unpack_from(">HhI", d, sevs + 8 * k)
                if eid != 0xFFFF:
                    out.append((pc, "sub", i, s, 0, eid))
        for k in range(evc):
            flags, eid, pc = struct.unpack_from(">HHI", d, evs + 8 * k)
            if eid != 0xFFFF:
                out.append((pc, "ctl", i, control, flags, eid))
    return sorted(out)


def dis(d, start, end):
    pc = start
    while pc < end:
        op = d[pc]
        n = OPLEN.get(op, 0)
        arg = d[pc + 1:pc + 1 + n]
        name = NAMES.get(op, "op%02x" % op)
        if n == 4:
            v = struct.unpack(">i", arg)[0]
            a = "%#x" % (v & 0xFFFFFFFF) if op == 67 else str(v)
            if op == 17:
                a = repr(struct.unpack(">f", arg)[0])
        elif n == 6:
            v, s = struct.unpack(">ih", arg)
            a = "%#x,%d" % (v & 0xFFFFFFFF, s)
        elif n:
            a = arg.hex()
        else:
            a = ""
        yield pc, name, a
        pc += 1 + n


def main():
    d = open(sys.argv[1], "rb").read()
    if sys.argv[2] == "--events":
        for ev in events(d):
            print("%#06x %s ctl%d %s flags=%#x id=%d" % ev)
    elif sys.argv[2] == "--dis":
        for pc, name, a in dis(d, int(sys.argv[3], 0), int(sys.argv[4], 0)):
            print("%#06x  %-10s %s" % (pc, name, a))
    elif sys.argv[2] == "--find":
        msg = int(sys.argv[3])
        evs = events(d)
        starts = [e[0] for e in evs]
        for k, (pc0, kind, i, x, f, eid) in enumerate(evs):
            end = starts[k + 1] if k + 1 < len(starts) else len(d)
            prev = []
            for pc, name, a in dis(d, pc0, end):
                prev.append((name, a))
                if name == "MSG" and len(prev) >= 3 and prev[-3] == ("push", str(msg)):
                    print("MSG %d at %#x, after handler %#x (%s, control %d at %#x, flags %#x, event %d; check that the handler reaches it)" % (
                        msg, pc, pc0, kind, i, x, f, eid))


if __name__ == "__main__":
    main()
