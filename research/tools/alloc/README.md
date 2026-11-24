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
- `sim.py`: replays simplify on a tracer graph (K varies 23-28 per function; approximate; superseded).
- `sim2.py` (2026-11-24, read from simplify at 0x507b50): EXACT simplify replay. Degree starts at the
  adjacency-list length, a removed node lowers only its own list's degrees, K = 29 for GPRs, and
  copies deleted by coalescing leave +1 'stale' degree on every web live across them (named locals,
  reference locals and single-use returning inlines add none). Reproduces the real push order on
  all 18 records tried.
- `fit.py <tracer json> <nmap output>`: runs sim2, scores colours against nmap's map of the
  original, and tries every node at every index / extra degree: names the variable whose numbering
  is wrong and what it must be (param, late user local, temp). About 2 min per run.
- `hdrtest.py`: checks whether a shared-header edit renumbers compiler-local names in other units.

The rules these established are in research/notes/matching-rules.md; the per-row triage of the 54
open rows on 2026-11-18 is research/notes/matching-triage-2026-11-18.tsv.

## Scheduler probe (structural run 2026-11-24)

- `sched.py <src> <unit> [out] [-f substr]`: compiles under ptrace with an int3-patched copy of the
  real compiler (made on first use in `$SCHED_CC`, default `scratch/sched_cc`; seven breakpoints,
  no other change). For each block of both scheduler passes (`scheduleinstructions` 0x506e10, pass
  0 before allocation, 1 after) it prints every node in input order (latency, slack, height,
  unscheduled predecessors, successor edges with latency), then every pick with its cycle. It also
  prints each pcode the allocator inserts or deletes between the passes, with the caller (0x57a596 =
  copy coalescing).
- Scheduler.c, read from the binary: 0x506f20 builds the dependency graph from the last
  instruction up (0x507230; edges carry the producer's latency, so height = longest latency path
  to the block end) and list-schedules top-down, issue width and unit checks from the machine
  table. The selector 0x5070f0 takes the first ready node in input order; a later ready node
  replaces it if it is urgent (cycle >= slack) and the current one is not, else if it makes more
  successors ready (predecessor count 1), else if its height is greater, else (when 0x5e86a0 is
  set) by an opcode-class byte. Post-allocation, register reuse adds anti and output edges, so the
  "makes more ready" test often decides there.
- Operands print as `=` for a definition, objects by name; `!` after an opcode marks pcode flag 4.
  `SCHED_MACHINE=1` prints the machine table and the 0x5e86a0 flag. The last line (`optab`) lists
  each opcode's machine-table bytes (unit class, latency, stage cycles) and its class byte.
- `ssim.py <dump> [line]`: replays the scheduler on a sched.py dump with the gekko (750) machine
  model (table 0x5d3a18: width 2; can_issue 0x578ec0, issue 0x578e30, advance 0x578aa0: a six-entry
  completion queue retiring two a cycle, two integer units with a same-cycle dependence check, a
  two-stage load/store and a three-stage FP pipeline) and checks every block against the real
  picks (313 of 313 blocks in five functions). The class-byte tie-break applies only before
  allocation (0x5e86a0 is set while virtual registers exist). `schedule(nodes, optab, prera=)` is
  usable on a generated graph to search source orders without compiling (see the GetExtents
  generator in scratch/loop5/sched/geo.py).
- A block scheduled before allocation gets flag 8 and is skipped after allocation unless a pcode
  insertion or deletion clears it (0x4dc1b0 / 0x4dc1f0 / 0x4dc230), e.g. a coalesced copy or spill
  code. The rule this gives is in research/notes/matching-rules.md (GetPeekLocation).
