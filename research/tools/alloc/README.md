# Register-allocator tracing for MWCC GC 1.3.2 (matching run 2026-11-17/18)

These tools read the compiler's real register allocation instead of guessing from the output.
They were written in scratch/loop4/ during the run and copied here unchanged; paths inside still
point at scratch/ for the patched compilers, so recreate those first.

## Patched compiler copies (not committed; make them from build/compiler/GC/1.3.2/mwcceppc.exe)

- `scratch/loop4/regrules2/nosched/mwcceppc.exe`: the post-allocation scheduler call NOPed
  (5 bytes at file offset 0x3337e, VA 0x433f7e, set to 0x90). Same registers and instructions as
  the real compiler, emitted in allocation order. For analysis only, never for scoring.
- `scratch/loop4/ba/trace/mwcceppc.exe`: the nosched copy with int3 (0xcc) at file offsets
  0xfca80 (VA 0x507a30, the colouring entry) and 0x106e30 (VA 0x4fd680, vreg numbering).
  Copy the DLL and the other exes of the same directory next to each.

## Tools

- `tr.py <src> <unit> <out.json>`: runs the trace copy under ptrace (about 2 s per compile) and
  dumps every virtual register's number and name and the colouring stack with neighbours and
  final registers. Known issues: writes last.o to a fixed path (give each parallel user a private
  copy); FPR name lists can carry names from earlier functions in the same file.
- `cl.py`, `pdump.py`: print the dump (callee-saved webs in colour order; any function).
- `nmap.py <src> <func> [F|R]`: maps every allocator node to its operands and to the original's
  registers, then says STUCK (the original's colours are impossible on our graph: a structural or
  pre-allocation order difference) or feasible (only index order differs; `WHY=1` lists what must
  be coloured first). Simulators ORDER, PERM, ISIM.
- `sim.py`: replays simplify on a tracer graph (K varies 23-28 per function; approximate).
- `hdrtest.py`: checks whether a shared-header edit renumbers compiler-local names in other units.

The rules these established are in research/notes/matching-rules.md; the per-row triage of the 54
open rows on 2026-11-18 is research/notes/matching-triage-2026-11-18.tsv.
