# Tooling and roadmap

Audience: human contributors and AI agents working in this checkout. This is a
**plan and a survey of candidate tools**, not accepted source. Nothing here earns
progress credit until `tools/reconstruct.py` and the snapshot process accept it.

Read `AGENTS.md`, `README.md`, `docs/Progress.md` and `docs/Dolphin.md` first.
When this document and those disagree, the code, the pinned checks and the rules
in `AGENTS.md` win.

## Where the project is

- Verified snapshot: **51,416 / 1,414,572** executable bytes (**3.634739%**),
  271 functions, all `restored_library`. `reconstructed_game` is still **0**.
  Runtime is untested.
- 50 accepted units, all Dolphin SDK (`docs/Dolphin.md`, `config/GMFE69/`).
- Toolchain in use: decomp-toolkit 1.8.4, gc-wii-binutils 2.42-2, wibo 1.0.3,
  CodeWarrior `GC/1.2.5n` (`tools/toolchain.json`, `tools/compilers.json`).
- Analysis already present: `tools/file_map.py`, `tools/code_map.py`,
  `tools/dependencies.py`, `tools/port_unit.py`, `tools/unit_diff.py`,
  `tools/baseline.py`, `tools/reconstruct.py`, `tools/progress_report.py`.
- Rough `.text` split (heuristic, see `docs/initial-audit.md`): ~72% C++ game
  code and EA libraries, ~11% Dolphin SDK, ~8% EA audio, remainder MSL/MetroTRK/UI.

The open problem is not binary access — relocations are preserved and symbols
exist. It is producing **period-compiler-exact C/C++** for code that has no
published reference, without inventing types or boundaries.

## Part 1 — Tooling landscape

### Already in use (keep)

| Tool | Purpose |
| --- | --- |
| decomp-toolkit 1.8.4 (`dtk`) | ELF/DOL split, symbol config, `elf2dol`; pinned in `tools/toolchain.json` |
| gc-wii-binutils 2.42-2 | `as`/`ld`/`objdump`/`readelf` for PowerPC |
| wibo 1.0.3 | runs the Windows compilers on Linux/macOS |
| CodeWarrior `GC/1.2.5n` | working SDK compiler profile (`tools/compilers.json`) |
| objdiff v2 report | progress export from the verified snapshot (`tools/progress_report.py`) |
| Project Python tools | complete-image verification, file map, dependency scan, unit porting |

### Worth adding — free and local (candidates to evaluate)

| Tool | Use | Where it plugs in |
| --- | --- | --- |
| [Ghidra](https://github.com/NationalSecurityAgency/ghidra) (+ [GhidraMCP](https://github.com/LaurieWired/GhidraMCP)) | Decompile game code, xrefs, data layout; expose to agents over MCP | Phase C. Seed with dtk symbols; keep decompilation dumps under ignored `scratch/` |
| Ghidra Version Tracking / [Diaphora](https://github.com/joxeankoret/diaphora) | Match functions/symbols across binaries (Rising Sun GC, PS2 builds) to transfer names and leads | Phase C. A transferred name is a lead, not a target-revision fact |
| [ppcdis](https://github.com/SeekyCt/ppcdis) | GC/Wii PowerPC disassembly and decompilation helpers | Phase C/D, complements `unit_diff.py` |
| [decomp-permuter](https://github.com/simonlindholm/decomp-permuter) | Randomized search for a byte-matching C variant | Phase D. Needs a per-function compile + diff script wrapping the pinned compiler |
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

- **m2c** is MIPS-only and does not apply to PowerPC.
- **splat** is superseded by dtk for this target.
- **Assembly-injection wrappers** (for example `metrowrap`) conflict with the
  contributing rule against inserting assembly to substitute for compiled
  C/C++ and must not be used for accepted units.

## Part 2 — Plan

### Phase A — Consolidate the library phase (now; no new tools)

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

### Phase B — First game-code reconstruction (reuse-driven)

1. Start with EA-engine families that have published references (EAGL, string
   and CRC helpers, containers, MathFun equivalents).
2. Establish shared headers and layout docs with explicit, bounded claims; do
   not invent types, names or source boundaries.
3. Gate every unit with `tools/reconstruct.py` and refresh the snapshot.

Exit: first `reconstructed_game` bytes accepted (currently 0).

### Phase C — Ghidra for unreferenced game code

1. Install a JDK and Ghidra; load `Moh2RelGC.elf`; seed symbols from
   `build/audit/`.
2. Run cross-binary matching (Ghidra Version Tracking or Diaphora) against a
   sibling build to generate name/structure leads.
3. Wire a maintained Ghidra MCP server into OpenCode under `mcp.servers` so
   agents can query decompilation and xrefs. Keep dumps under `scratch/`.

Exit: leads transferred from a sibling build; first targets reconstructed from
decompilation rather than a published reference.

### Phase D — Matching-loop acceleration

1. Adapt decomp-permuter with a per-function compile + diff script.
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
- **Model roles.** Assign models by phase, not by task. Reference-backed work
  (Phase A: Dolphin SDK, MSL and MW-runtime units ported from a published,
  attributable source) may be done by a cheaper unattended worker model.
  Reconstruction without a byte-match reference (Phases B and C) uses a frontier
  model, because its main risk is invented types, names and boundaries, which no
  byte comparison detects.
- **No model verifies.** `tools/reconstruct.py`, the tests and a deterministic
  policy check (worker scope, unchanged accepted units, no assembly substitution,
  exact progress accounting) decide whether work is ready for review. A human or
  frontier-model reviewer then decides whether to accept it, checking fidelity,
  provenance and licensing. Scripts commit unreviewed work only to per-job
  branches. Merging to `main` and `progress_report.py --capture` happen only
  after review.

## Part 5 — Open decisions

- Whether the unattended-worker policy check should become a committed tool
  with tests, rather than a local script.
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
