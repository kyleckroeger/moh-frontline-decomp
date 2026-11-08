# runtime-libraries

Agent working notes, kept as written; dated claims may be out of date.


Since 2026-10-06, MSL C, the PPC EABI runtime, math and the debugger stubs come from the CC0 Wind Waker decomp (zeldaret/tww a1854d4) in src/tww and include/tww, and from Prime (PrimeDecomp/prime 32020a0) in src/prime/runtime and include/prime. Function-size comparison picked TWW (240/242); Pikmin, Sunshine, mkdd and Pikmin 2 fit worse. dolsdk2001/2004 and melee have no licence.

Workflow scripts live in scratch/refs/: sizecmp.py (reference fit), twwbatch.py (drafts with TWW's per-file flags, and `!name` for globals-only files), fixtrim.py (trims data to the target's objects; careful, it misfired on .ctors/.dtors), and uncovered.py. Templates are scratch/tww-runtime.json and scratch/prime-runtime.json. Local source edits and evidence are recorded in docs/Runtime.md.

Lessons: `-inline deferred` emits functions in reverse source order. Check static-vs-global against target bindings. Use a file record when the target has one, even if the functions are global (ansi_files, global_destructor_chain). Prefer stripped_objects over a leading trim when alignment would break.

As of f6d264d (13.24%): .init code units are supported, and __start, __mem, __ppc_eabi_init and 7 TRK files are done. Frontline's TRK equals TWW's binary (117/118 sizes), but TWW's TRK source is mostly non-matching (TRKMessageSend is a stub), so the rest is real decompilation. Also left: no-reference library code (Gecko C++ throw ~5 KB, mslsupp file hooks, stdio bits) and OSReset's frame issue. Related: [[agent-pipeline]], [[frontline-decomp-setup]]
