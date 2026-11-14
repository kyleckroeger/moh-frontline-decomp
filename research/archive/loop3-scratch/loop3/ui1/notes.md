# ui1 notes (2026-11-12)
Landed 349761a7 NEW UserInterface_init: UserInterface::Init (1,160 B).
- MWCC 1.3.2: an inline's `char path[20] = ""` template is created once at the inline's DEFINITION
  (low label @4, placed where defined); all expansions share it. A function's own identical local
  never shares it. Holder expansion is unnecessary and harmful (it creates the format string early).
- Inlined loop with its own counter: counter/IV regs swapped (186 lines, ~15 forms inert);
  `LoadHeadshots(int& i)` with `int i;` in Init -> 0.
Probes: p*.cpp, v*.txt + run.py (variant runner), df.py (instruction diff).
