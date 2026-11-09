#!/usr/bin/env python3
"""Turn a text input script into a build of research/probes/input_probe.cpp.

  input_script.py SCRIPT.txt OUT.cpp

Then link it into a test image:

  python3 research/tools/link_drafts.py --probe OUT.cpp --output build/linked/test.elf

Script lines (times are pad updates, counted from boot; `#` starts a comment):

  trace N                  print the state trace every N updates (default 30)
  at N                     move the cursor to update N
  wait N                   move the cursor forward N updates
  press A+START [N]        hold buttons for N updates (default 6), then release for 6
  hold A+START N           the same, without the default
  stick X Y N [BUTTONS]    main stick at X,Y (-128..127, up is +Y) for N updates
  cstick X Y N [BUTTONS]   C stick
  trigger L|R N [BUTTONS]  press a trigger fully for N updates

Buttons: LEFT RIGHT DOWN UP Z R L A B X Y START (the D-pad is LEFT/RIGHT/DOWN/UP).
"""
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
TEMPLATE = ROOT / "research/probes/input_probe.cpp"
BUTTONS = dict(LEFT=0x1, RIGHT=0x2, DOWN=0x4, UP=0x8, Z=0x10, R=0x20, L=0x40,
               A=0x100, B=0x200, X=0x400, Y=0x800, START=0x1000)


def buttons(spec):
    value = 0
    for name in spec.split("+"):
        if name:
            value |= BUTTONS[name.upper()]
    return value


def parse(text):
    steps, cursor, trace = [(0, 0, 0, 0, 0, 0, 0, 0)], 0, 30

    def add(length, button=0, sx=0, sy=0, cx=0, cy=0, tl=0, tr=0):
        nonlocal cursor
        steps.append((cursor, button, sx, sy, cx, cy, tl, tr))
        cursor += length
        steps.append((cursor, 0, 0, 0, 0, 0, 0, 0))

    for number, line in enumerate(text.splitlines(), 1):
        words = line.split("#")[0].split()
        if not words:
            continue
        op, rest = words[0].lower(), words[1:]
        try:
            if op == "trace":
                trace = int(rest[0])
            elif op == "at":
                cursor = int(rest[0])
            elif op == "wait":
                cursor += int(rest[0])
            elif op == "press":
                add(int(rest[1]) if len(rest) > 1 else 6, buttons(rest[0]))
                cursor += 6  # released long enough for the game to see the next press
            elif op == "hold":
                add(int(rest[1]), buttons(rest[0]))
            elif op in ("stick", "cstick"):
                x, y, n = int(rest[0]), int(rest[1]), int(rest[2])
                b = buttons(rest[3]) if len(rest) > 3 else 0
                if op == "stick":
                    add(n, b, sx=x, sy=y)
                else:
                    add(n, b, cx=x, cy=y)
            elif op == "trigger":
                n = int(rest[1])
                b = buttons(rest[2]) if len(rest) > 2 else 0
                b |= BUTTONS[rest[0].upper()]
                add(n, b, tl=255 if rest[0].upper() == "L" else 0, tr=255 if rest[0].upper() == "R" else 0)
            else:
                raise ValueError("unknown command " + op)
        except (IndexError, KeyError, ValueError) as e:
            sys.exit(f"line {number}: {line.strip()}: {e}")
    # Later steps at the same update win; keep the order stable.
    merged = {}
    for s in steps:
        merged[s[0]] = s
    return [merged[k] for k in sorted(merged)], trace


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    steps, trace = parse(Path(sys.argv[1]).read_text())
    rows = "\n".join("    {%d, 0x%x, %d, %d, %d, %d, %d, %d}," % s for s in steps)
    block = (f"// SCRIPT BEGIN\nstatic const Step s_script[] = {{\n{rows}\n}};\n"
             f"static const unsigned int TRACE_EVERY = {trace};\n// SCRIPT END")
    source = re.sub(r"// SCRIPT BEGIN.*?// SCRIPT END", lambda m: block, TEMPLATE.read_text(), flags=re.S)
    Path(sys.argv[2]).parent.mkdir(parents=True, exist_ok=True)
    Path(sys.argv[2]).write_text(source)
    print(f"{sys.argv[2]}: {len(steps)} steps, last at update {steps[-1][0]}")


if __name__ == "__main__":
    main()
