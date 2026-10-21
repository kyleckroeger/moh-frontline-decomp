# Tooling and roadmap

Audience: human contributors and AI agents working in this checkout. This is a
**plan and a survey of candidate tools**, not accepted source. Nothing here earns
progress credit until `tools/reconstruct.py` and the snapshot process accept it.

Read `AGENTS.md`, `README.md`, `docs/Progress.md` and `docs/Dolphin.md` first.
When this document and those disagree, the code, the pinned checks and the rules
in `AGENTS.md` win.

## Where the project is

- Verified snapshot (2026-10-21): **910,228 / 1,414,572** executable bytes
  (**64.35%**), 4,522 functions in 1,222 units: **684,820** bytes of
  `reconstructed_game` (1,045 units, `docs/Game.md`) and **225,408** bytes of
  `restored_library` (177 units: Dolphin SDK, MSL, MW runtime, EA audio
  libraries; `docs/Dolphin.md`). Runtime is untested.
- Toolchain in use: decomp-toolkit 1.8.4, gc-wii-binutils 2.42-2, wibo 1.0.3,
  CodeWarrior `GC/1.2.5n` (SDK) and `GC/1.3.2` (game and EA code), linked with
  SN ProDG 3.8.1 `ngcld` (`tools/toolchain.json`, `tools/compilers.json`).
- Analysis already present: `tools/file_map.py`, `tools/code_map.py`,
  `tools/dependencies.py`, `tools/port_unit.py`, `tools/unit_diff.py`,
  `tools/baseline.py`, `tools/reconstruct.py`, `tools/progress_report.py`.
- Rough `.text` split (heuristic, see `docs/initial-audit.md`): ~72% C++ game
  code and EA libraries, ~11% Dolphin SDK, ~8% EA audio, remainder MSL/MetroTRK/UI.

The open problem is not binary access — relocations are preserved and symbols
exist. It is producing **period-compiler-exact C/C++** for code that has no
published reference, without inventing types or boundaries. What remains is
mostly register-allocation and stack-slot near misses, large functions, and
whole-file merges where a file's shared data block (`...rodata.0`,
`...bss.0`) can only be placed by one unit for the whole file.

## Part 1 — Tooling landscape

### Already in use (keep)

| Tool | Purpose |
| --- | --- |
| decomp-toolkit 1.8.4 (`dtk`) | ELF/DOL split, symbol config, `elf2dol`; pinned in `tools/toolchain.json` |
| gc-wii-binutils 2.42-2 | `as`/`ld`/`objdump`/`readelf` for PowerPC |
| wibo 1.0.3 | runs the Windows compilers on Linux/macOS |
| CodeWarrior `GC/1.2.5n`, `GC/1.3.2` | SDK and game compiler profiles (`tools/compilers.json`) |
| SN ProDG 3.8.1 `ngcld` | links each unit at its original addresses |
| objdiff v2 report | progress export from the verified snapshot (`tools/progress_report.py`) |
| Project Python tools | complete-image verification, file map, dependency scan, unit porting |
| [Ghidra](https://github.com/NationalSecurityAgency/ghidra) via a pyghidra MCP server | decompilation and xrefs for agents; output is a hypothesis, kept out of `src/` |
| [decomp-permuter](https://github.com/simonlindholm/decomp-permuter) | randomized search on near misses, with a harness under ignored `scratch/` that rebuilds the real function in its unit |

### Worth adding — free and local (candidates to evaluate)

| Tool | Use | Where it plugs in |
| --- | --- | --- |
| Ghidra Version Tracking / [Diaphora](https://github.com/joxeankoret/diaphora) | Match functions/symbols across binaries (Rising Sun GC, PS2 builds) to transfer names and leads | Phase C. A transferred name is a lead, not a target-revision fact |
| [ppcdis](https://github.com/SeekyCt/ppcdis) | GC/Wii PowerPC disassembly and decompilation helpers | Phase C/D, complements `unit_diff.py` |
| [objdiff](https://github.com/encounter/objdiff) (standalone) | Interactive per-function assembly diff | Phase D. Complements `tools/unit_diff.py` |
| [decomp.me](https://decomp.me) | Collaborative scratch matching (GC/Wii platform) | Phase D. Its compiler versions differ from the pinned profile, so results are leads only |
| [rizin](https://github.com/rizinorg/rizin) + [Cutter](https://github.com/rizinorg/cutter) | Free RE framework with PowerPC support | Quick inspection; alternate to Ghidra |
| [Ninja](https://ninja-build.org) + [ccache](https://ccache.dev)/[sccache](https://github.com/mozilla/sccache) | Parallel build and object caching | Phase D, only if `reconstruct.py` becomes the bottleneck |
| [Dolphin](https://github.com/dolphin-emu/dolphin) GDB stub + `gdb-multiarch` | Runtime testing | Phase E. Runtime claims stay separate from matching claims |
| [decomp-academy.dev](https://decomp-academy.dev) | Onboarding practice with a live Metrowerks compiler | Contributor training only, no project credit |

### Commercial / optional

| Tool | Use | Note |
| --- | --- | --- |
| IDA Pro | PowerPC analysis | Paid; Ghidra/rizin are free alternatives |
| Binary Ninja | PowerPC analysis | Paid tiers; optional |
| BinDiff | Binary diffing | Google tool; Ghidra Version Tracking / Diaphora are free alternatives |

### Not applicable or disallowed

- **m2c** has a PowerPC backend but is not used here; Ghidra's decompiler,
  over MCP, fills that role.
- **splat** is superseded by dtk for this target.
- **Assembly-injection wrappers** (for example `metrowrap`) conflict with the
  contributing rule against inserting assembly to substitute for compiled
  C/C++ and must not be used for accepted units.

## Part 2 — Plan

### Phase A — Consolidate the library phase (done; remaining SDK files are blocked or small)

1. Continue the SDK unit queue (`scratch/remaining.txt`). Each accepted unit
   settles file boundaries and adds verified bytes.
2. Maintain `docs/ReuseMap.md`: each recovered file group mapped to candidate
   published references (dolsdk2001, dolsdk2004, MWCC MSL/runtime, NFS MW EAGL,
   Rising Sun, …), with evidence, limits and the ranked backlog.
3. Use `tools/code_map.py` and `tools/dependencies.py` to rank shared helpers
   by unfinished callers, and record the ranking (a `docs/Dependencies.md`).
4. Add mutation tests for accepted units in `tests/test_reconstruction.py`.

Exit: coverage rises; the reuse index says which remaining groups are
reference-backed versus opaque.

### Phase B — First game-code reconstruction (done: `bstimer.cpp` first, now most game bytes)

1. Start with EA-engine families that have published references (EAGL, string
   and CRC helpers, containers, MathFun equivalents).
2. Establish shared headers and layout docs with explicit, bounded claims; do
   not invent types, names or source boundaries.
3. Gate every unit with `tools/reconstruct.py` and refresh the snapshot.

Exit: first `reconstructed_game` bytes accepted (reached; 684,820 bytes now).

### Phase C — Ghidra for unreferenced game code (in use)

1. Install a JDK and Ghidra; load `Moh2RelGC.elf`; seed symbols from
   `build/audit/`.
2. Run cross-binary matching (Ghidra Version Tracking or Diaphora) against a
   sibling build to generate name/structure leads.
3. A pyghidra MCP server gives agents decompilation and xrefs (the OpenCode
   worker setup is retired). Keep dumps under `scratch/`.

Exit: leads transferred from a sibling build; first targets reconstructed from
decompilation rather than a published reference.

### Phase D — Matching-loop acceleration (in use)

1. decomp-permuter runs through a per-function harness that rebuilds the real
   function inside its unit with the manifest's flags. Its output is a hint:
   only plausible source is ported.
2. Generate `objdiff.json` with `python3 tools/objdiff_config.py` (after
   `tools/reconstruct.py`) to diff the accepted units locally with objdiff; the
   generated file is git-ignored iteration tooling. Use standalone objdiff and
   decomp.me for isolated, stubborn functions.
3. Adopt Ninja and ccache/sccache only if unit count makes the build slow.

Exit: faster iteration on functions that are close but not yet byte-exact.

### Phase E — Verification hardening and runtime

1. Broaden mutation tests.
2. Test in Dolphin via its GDB stub; report runtime separately from matching.

Exit: runtime status is measured, not assumed.

## Part 3 — Choosing the next target

1. Prefer **shared helpers with many unfinished callers**
   (`tools/dependencies.py`) — one accepted function unlocks more.
2. If the target is Dolphin SDK / MSL / runtime, stay in Phase A.
3. If the target looks like EA engine code with a published reference, go to
   Phase B.
4. If the target has no reference and needs its semantics recovered, go to
   Phase C (Ghidra).
5. If a target is semantically right but not byte-exact, go to Phase D.

## Part 4 — How agents should work

- **Verification gate.** Never claim progress without
  `python3 tools/reconstruct.py`, `python3 -m unittest discover -s tests -v`,
  and a fresh `progress_report.py --capture` for source/header/tool changes.
  Documentation-only changes do not need a rebuild.
- **No fabricated matches.** Do not invent types, original names or source
  boundaries, patch instructions, insert assembly, or exclude differences.
- **Ghidra/decompiler output is a hypothesis.** Treat names, types and layouts
  from any decompiler as leads to verify against the pinned target bytes.
- **Keep provenance.** Originals, tool binaries, decompiler dumps and bulk
  research stay under ignored `orig/`, `build/` or `scratch/`.
- **Report separately.** Original-image baseline, individual function matches,
  complete source-build result, and runtime tests are distinct claims.
- **Model roles.** A frontier model does the reconstruction directly; the
  unattended cheaper-worker pipeline was retired (it cost more review than it
  saved). The main risk is invented types, names and boundaries, which no byte
  comparison detects, so inferred views and helpers are marked in comments.
- **No model verifies.** `tools/reconstruct.py` (complete-image match), the
  tests and `progress_report.py --capture` decide whether a unit lands. Each
  landing is one commit on `main` made only after all three pass; new
  verification rules (manifest fields such as `file_duplicates` or a relaxed
  `input_alignment`) need the maintainer's approval first and are documented
  in `docs/Dolphin.md`.

## Part 5 — Open decisions

- Whether to adopt dtk-template's `configure.py`/ninja/objdiff integration or
  keep the stricter custom verifier.
- Ghidra scope and a naming policy for decompiler-derived leads.
- How far to trust cross-binary symbol transfer before target verification.

## References

- `docs/Progress.md` — verification boundary and counting rules
- `docs/Dolphin.md` — SDK revision strategy, compiler profiles, open problems
- `docs/initial-audit.md` — target layout, toolchain, per-file splits
- `docs/Licensing.md` — provenance and third-party terms
- Sibling project: <https://github.com/lifewillbeokay/moh-rising-sun>
