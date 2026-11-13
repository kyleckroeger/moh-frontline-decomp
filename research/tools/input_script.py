#!/usr/bin/env python3
"""Turn a text input script into a build of research/probes/input_probe.cpp,
or into a Dolphin input movie.

  input_script.py SCRIPT.txt OUT.cpp     the probe plays the script (times: pad updates)
  input_script.py SCRIPT.txt OUT.dtm     a Dolphin movie (times: controller polls)
  input_script.py --real-pad OUT.cpp     the probe only traces; the pad is real (for movies)
  input_script.py --show MOVIE.dtm       print a movie's header and its input changes

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

The same script drives either. In the probe, a step starts on a game update, so two
images replay it identically whatever their speed. A movie (.dtm, Dolphin's format:
a 256-byte header, then 8 bytes per poll of controller 1) is played by Dolphin itself
and works on any image, including the untouched original, but its steps start on a
controller poll, which is emulated time: Dolphin polls the pad about 60 times a second
whatever the game does, and while a movie plays it reads the disc at the real drive's
speed, so the shell appears later than in probe runs (research/movies/m1_movie.txt).
`trace` only applies to the probe.
"""
import re
import struct
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


def probe_source(steps, trace, real_pad=False):
    rows = "\n".join("    {%d, 0x%x, %d, %d, %d, %d, %d, %d}," % s for s in steps)
    block = (f"// SCRIPT BEGIN\nstatic const Step s_script[] = {{\n{rows}\n}};\n"
             f"static const unsigned int TRACE_EVERY = {trace};\n"
             f"static const int REAL_PAD = {int(real_pad)};\n// SCRIPT END")
    return re.sub(r"// SCRIPT BEGIN.*?// SCRIPT END", lambda m: block, TEMPLATE.read_text(), flags=re.S)


# Dolphin's movie format (Source/Core/Core/Movie.h: DTMHeader, ControllerState).
HEADER = 256
GAME_ID = b"GMFE69"
# ControllerState byte 0 bits from bit 0: Start A B X Y Z DPadUp DPadDown;
# byte 1: DPadLeft DPadRight L R disc reset is_connected reserved.
DTM_BITS = dict(START=(0, 0), A=(0, 1), B=(0, 2), X=(0, 3), Y=(0, 4), Z=(0, 5), UP=(0, 6), DOWN=(0, 7),
                LEFT=(1, 0), RIGHT=(1, 1), L=(1, 2), R=(1, 3))
CONNECTED = 0x40  # byte 1 bit 6


def dtm_record(step):
    _, button, sx, sy, cx, cy, tl, tr = step
    b = [0, CONNECTED]
    for name, (byte, bit) in DTM_BITS.items():
        if button & BUTTONS[name]:
            b[byte] |= 1 << bit
    # Analog triggers follow the digital L/R as on the real pad.
    tl = max(tl, 255 if button & BUTTONS["L"] else 0)
    tr = max(tr, 255 if button & BUTTONS["R"] else 0)
    return bytes(b + [tl, tr, (sx + 128) & 255, (sy + 128) & 255, (cx + 128) & 255, (cy + 128) & 255])


def dtm(steps, tail=3600):
    """The movie's bytes: one record per controller poll, from the steps' start times."""
    count = steps[-1][0] + tail
    body = bytearray()
    for i, s in enumerate(steps):
        end = steps[i + 1][0] if i + 1 < len(steps) else count
        body += dtm_record(s) * (end - s[0])
    h = bytearray(HEADER)
    h[0:4] = b"DTM\x1a"
    h[4:10] = GAME_ID
    h[10] = 0          # bWii
    h[11] = 1          # controllers: GameCube port 1
    h[12] = 0          # bFromSaveState
    struct.pack_into("<QQQQI", h, 13, count, count, 0, 0, 0)  # frames, inputs, lag, unique id, rerecords
    h[49:49 + 32] = b"input_script.py".ljust(32, b"\0")
    h[137] = 0         # bSaveConfig: off, Dolphin keeps the harness's own settings
    h[151] = 1         # memcards: slot A
    h[152] = 0         # bClearSave
    return bytes(h) + bytes(body)


def show(path):
    data = Path(path).read_bytes()
    frames, inputs = struct.unpack_from("<QQ", data, 13)
    print(f"{path}: {data[4:10].decode()} controllers {data[11]:#x} frames {frames} inputs {inputs} "
          f"records {(len(data) - HEADER) // 8}")
    last = None
    for i in range(HEADER, len(data), 8):
        rec = data[i:i + 8]
        if rec != last:
            print(f"  poll {(i - HEADER) // 8}: {rec.hex()}")
            last = rec


def main():
    args = sys.argv[1:]
    if len(args) == 2 and args[0] == "--show":
        return show(args[1])
    if len(args) == 2 and args[0] == "--real-pad":
        out, source = args[1], probe_source([(0, 0, 0, 0, 0, 0, 0, 0)], 30, real_pad=True)
        steps = None
    elif len(args) == 2:
        steps, trace = parse(Path(args[0]).read_text())
        out = args[1]
        source = dtm(steps) if out.endswith(".dtm") else probe_source(steps, trace)
    else:
        sys.exit(__doc__)
    Path(out).parent.mkdir(parents=True, exist_ok=True)
    (Path(out).write_bytes if isinstance(source, bytes) else Path(out).write_text)(source)
    if steps:
        unit = "poll" if out.endswith(".dtm") else "update"
        print(f"{out}: {len(steps)} steps, last at {unit} {steps[-1][0]}")
    else:
        print(f"{out}: trace-only probe (real pad)")


if __name__ == "__main__":
    main()
