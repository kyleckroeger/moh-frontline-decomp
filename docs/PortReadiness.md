# Port readiness plan

This repository exists to be the source of a native PC port of Frontline
([moh-frontline-pc](https://github.com/kyleckroeger/moh-frontline-pc)). The port's
strategy report (in that repo, `reports/`) measured this decomp on 2026-11-09 and found
that matching percentage overstates how ready the code is for a port. This file is the
plan for closing that gap. It sets the work order for everything except
[Remaining.md](Remaining.md)'s area priorities, which still apply inside each step:
collision, scripting, AI and animation first, multiplayer last.

## What "86% matched" doesn't tell the port

A PC build compiles source only. It can't use the original image's bytes, and it can't
trust code that has never run. Four things decide whether it can be built, and the match
figure measures none of them directly.

| Measure | Now (2026-11-09) | Needed for the port |
| --- | --- | --- |
| Code with source | 100%: 86.19% exact, the rest drafts | Have it |
| Code shown to behave like the original | Boot to the main menu with all drafts; gameplay never run | Every draft proven in play |
| Data defined in source | `.data` 47,356 B unowned, `.rodata` 37,221 B, `.bss` 1,157,481 B in 113 symbols, 84 vtables (`research/tools/unowned_data.py`) | Every symbol defined in source |
| One declaration per class | 794 files declare their own views; `CVector3` in 256, `ISceneNode` 170, `CMatrix` 142, `CPlayerObject` 96 | One shared header per class the port compiles |

Most of the unowned data is cheap. The `.bss` gap is buffers that need only a typed
declaration. The `.rodata` and `.sdata2` gaps are mostly string and float literals that
come with their functions, which already have source. What needs real work is 160 named
`.data` tables (30.8 KB, e.g. `masterEventTable`, `g_BIFCRCArray`, `g_config_keymap`) and
the 84 vtables, which a compiler only regenerates correctly from correct class
declarations.

## The milestone: Frontline from source, on GameCube

**A pure-source image**: every unit, draft and data file linked together with no
original bytes. It boots in Dolphin and plays recorded input identically to the original.
That is the port's first native milestone minus the platform layer. Reaching it here
means every later port bug is a port bug, not a decomp bug. Steps 1 to 3 build toward it
and can run in parallel. Step 4 continues alongside.

## Step 1. Prove the drafts in play

The linked build (`research/tools/link_drafts.py`) and the Dolphin harness
(`research/tools/dolphin_boot.py`) already boot and bisect. What's missing is input
and a comparison.

1. **Drive input.** Generate Dolphin input movies (`.dtm`) from a small script format,
   and play them with the harness. Confirm that a movie replays the original
   deterministically: two runs, same frames.
2. **Compare.** Dump frames, or hash them, at fixed intervals. The control is the linked
   image with every patch restored, which `dolphin_boot.py` already builds. It has the
   same memory layout as the drafted image, so any difference comes from a draft. The
   first differing frame starts a bisect.
3. **Routes.** Start with the opening of level 1_1: movement, firing, a grenade, pickups,
   a door, an AI fight. Then the first minute of each of the 27 levels.
4. **Order.** First the 5 drafts flagged `differs`, then the drafts the port needs
   first: `CInputManager::Update`, `TLT_LoadFromRepository` and
   `TLT_LoadLevelContentsFile`, then the priority-1 areas.
5. **Record.** Add a `proven` column to `research/drafts/status.tsv`, and report proven
   bytes as a third figure next to exact and linked.

First result (2026-11-09): the opening of Mission 1 crashed while loading with all
drafts active. The cause was `CreateCollisionVolume`'s view of `BSGO_Basic`, which put the
virtual table pointer first (CodeWarrior places it where the first virtual function is
declared). `check_draft.py` had rated the draft consistent with only 80.5% of its field
offsets agreeing. So any draft under 100% offsets is now a suspect, and those get
reviewed first: 29 are under 95%, including `TLT_LoadFromRepository` at 73.7%.

Already checked: drafts read and write the original's globals, not private copies
(`link_drafts.py` resolves every named symbol to the original). As of 2026-11-09, 11
symbols in 7 drafts stay private, and all are function-local statics (init guards, local
buffers) plus a padding array in the multiplayer `InitMultiplayer` draft. Their
originals are only used by the functions being replaced, so they aren't a risk.

Exit: every non-multiplayer draft is proven, or bisected and fixed. Multiplayer drafts
are recorded as unproven.

## Step 2. Put all data in source

1. **`.bss` and `.sbss`.** Declare each unowned global with its type and size, in a
   draft beside its users.
2. **Named tables.** Write them as source in `research/data/`, checked by a new tool. It
   compiles each table and compares its bytes, and its relocations against `.rela.data`,
   with the original symbol. Data can be byte-exact even where the code around it isn't.
   Order: input, script and collision tables first (`g_config_keymap`, `masterEventTable`,
   `g_BIFCRCArray`, `g_theCollisionTable`), shell images and UI after, multiplayer last.
   Hardware blobs the port replaces, like `axDspSlave` (DSP code), are listed as
   platform-owned rather than transcribed.
3. **Vtables** come from step 3.
4. **Pure-source link.** Link every unit, draft and data file with no original bytes, and
   count undefined and duplicate symbols. That count is the step's progress figure.

Exit: the pure-source link has no undefined symbols, then boots, then passes the step 1
routes.

## Step 3. One declaration per class

The port can't compile 256 differing `CVector3`s. The port report recommends doing this
reconciliation here, not on a port branch, so that every later match doesn't conflict
with it.

1. **Layout lint.** Compile units with `-sym on` and read the struct layouts from DWARF
   (the method `varmap.py` uses). For each class, report its size and the fields every
   view agrees on, plus any conflicts: different types or sizes at one offset. Run it
   in the tests.
2. **Vtables from the original.** `.rela.data` gives the exact target of every slot in
   every `__vt__` symbol. A generator writes each class's virtual method order from it.
3. **Shared headers** in `include/game/`. Do value types first (`CVector3`, `CMatrix`),
   then the classes the port's boot path compiles (`ISceneNode`, `CScene`,
   `CPlayerObject`, `CCamera`, `CScreen`, `CInputManager`). Every move passes the
   complete-image build, so the verified figure can't drop.
4. **Decision point after the first three classes.** Measure how many fragments kept
   their match. If moves often cost matches, keep a fragment's own view behind a guard
   and report how many guards there are.

Exit: every class the pure-source link compiles has one header, and the lint is clean.

## Step 4. Exact matching, re-ranked for the port

Matching remains the only proof of exact code, and it continues within the source-form
rules in [Remaining.md](Remaining.md). The order changes:

1. Land the 15 `code_exact` drafts (6.4 KB) that only need the complete build.
2. Then drafts on the port's first path: input, level loading, the scene tick, and the
   script VM.
3. Then the rest of the priority-1 areas, then priority 2, then multiplayer.

## Step 5. What the port reads from here

The port's source-list generator needs one generated file, not a crawl of the repo.

- `progress/GMFE69.port_manifest.json`, written by `tools/port_manifest.py` (the
  tests fail if it's stale; `land.sh` regenerates it). For each function symbol: its
  address, size, status (verified, exact, consistent, differs, none) and source file. For
  each data symbol: its owning unit's file, or `original`. First version 2026-11-09:
  1,210,428 B of function symbols verified, 6,440 exact, 178,544 consistent, 10,144
  differs, none without source; data owned 723,834 of 1,976,252 B. Verified code outside
  sized function symbols (about 8.8 KB) isn't listed. A `proven` status comes with step 1.
- When the port repo has a build, a long-lived `port` branch here, merged from `main`.
- Format knowledge goes in `docs/` as it's recovered from loaders: scripts
  (`research/tools/bs_disasm.py`), `.lfc`, `.mvd`, `.skl`, `.dmf`, `.emt`.

## Order of work

1. Input movies and frame comparison (step 1.1-1.2). This finds bad drafts, and nothing
   else does.
2. `port_manifest.json` and the `unowned_data.py` figures in the generated docs (step 5).
   These are cheap, and they make progress visible.
3. Layout lint and vtable generator (step 3.1-3.2). These are tools only; they don't
   change any source.
4. Prove 1_1's opening, fixing drafts as they're found (step 1.3-1.4).
5. `.bss` declarations and the first data tables, then the first pure-source link
   attempt (step 2).
6. Shared headers for the first three classes, then the decision point (step 3.3-3.4).

Step 4 fills gaps between these and stays in port order.

On the port side, the next measurement is a compile-only pass of this source with a PC
compiler (`clang -fsyntax-only` per unit). Its error count is the real to-do list for
step 3. It needs a native toolchain installed in WSL, which hasn't been approved yet.
