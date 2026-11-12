# Port readiness plan

This repository exists to be the source of a native PC port of Frontline
([moh-frontline-pc](https://github.com/kyleckroeger/moh-frontline-pc)), but it stays a
decompilation: it delivers source, data and `progress/GMFE69.port_manifest.json` that
match the original, all checked on GameCube. Platform layers, shims, PC build files and
PC-specific fixes belong in the port's repository, not here. The port's
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

Static review, 2026-11-10 (no Dolphin): the drafts `check_draft.py` newly flagged after
the `SweepAllExtents` fix were reviewed against the original's code. None was a logic bug.
`BSCleanUpBIFuncs`, `displayPromptBuffer`, `BSInitBIFuncs` and `BSResetBIFuncs` were
flagged because the original steps a pointer through an array (`addi r31,r31,4`), which the
checker counted as a field address; it now ignores in-place increments, and all four are at
100%. `CreateCollisionVolume`'s +0x50 is the volume allocator reached from a shared global
base, and its +0x18 is `&m_scriptData`, which the draft also passes.
`TLT_LoadFromRepository` (73.7%) matches the original statement by statement: its two
"missing" offsets are `g_Repository`'s fields, which the original reads through a base
register. `NonVisDestructFarthestSoldiers`' extra +0 is the same indexing difference.
The `SweepAllExtents` fix still needs one Dolphin run to confirm that Mission 1 gets past
the hang.

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

Started 2026-11-10: `research/data/` holds the input, script and collision tables, all
byte-exact including relocations (`research/tools/check_data.py`, tested by
`tests/test_data_tables.py`): `g_config_keymap`, `g_BIFCRCArray`, `masterEventTable`,
`g_theCollisionTable` (4,147 bytes). Two format facts came out of them. A script
built-in's CRC is the standard CRC-32 of its name in upper case, without the `BIFunc_`
prefix, for all 288 built-ins. The collision table is the lower triangle of a symmetric
matrix over 29 collision types, indexed `a + b * (b + 1) / 2` for `a <= b`.

2026-11-11: `.bss` and `.sbss` done. `research/tools/bss_decls.py` wrote
`research/data/bss/`, one file per original file (44 files, 404 globals, all the right
section and size by `check_data.py`). Types come from the drafts' and units' own extern
declarations; 25 types no source spells out yet are byte views sized from the original,
and 9 globals have no type at all. A file static unique to one original file is defined
global, because units and drafts split that file into fragments that reach it with
extern. What's left in `.bss`/`.sbss` is function statics (`name$123`), which come with
their functions.

First pure-source resolution (`research/tools/source_link.py`, from symbol and
relocation tables: GNU ld stops at the first unresolvable small-data relocation):
1,236 units, 147 drafts and 48 data files; 197 undefined, 163 fragment-local, 35
duplicate. The undefined are 181 initialised data symbols (sub-step 2, next), one
`extabindex` entry and 15 names the original doesn't have (wrong mangling such as
`Multiply__7CMatrixFR7CMatrixRC7CMatrixRC7CMatrix`, view names in mangled names,
SDK aliases such as `PSVECNormalize`). Fragment-local symbols are a file's statics
reached across its fragments; they resolve when a file's fragments are merged. The
duplicates are 22 between drafts of one file and 13 between units (`AIObject_threat`
and `AIObject_visionseek` both define the `ComputeStatus*` functions). Five drafts
reached a large global through r13 because their view was too small; padded to the
original size, all five match better.

Initialised data, 2026-11-12: `research/data/init/` defines the game's undefined
initialised globals from the original's bytes and relocations, one file per original
file as for `.bss` (constants, switches, the default and blend light sets, fog types,
the identity matrix, filename hooks); all exact by `check_data.py`. A file the file map
doesn't name is inferred from the neighbours in the original's section and the users,
and marked; the rest are in `init/unassigned.cpp`. The libraries' data followed: EA's hooks (`SNDAEMS_*`, `REALabortmessage`), print channels, AEMS component table, mix and reverb maps, XA ADPCM tables, MPEG decoder tables, the built-in font (`FONTstandfile`, a whole font file) and MSL's `__float_nan`/`__float_huge`. `axDspSlave` and its length stay platform-owned. The shell's screen-tool exports came last: `PauseLibraryInfo`, `ShellLibraryInfo`, `PauseScreenInfo` and `ShellScreenInfo` with the 40 file-name and object-slot arrays they point at (`init/is*.cpp`). The 84 vtables wait for step 3.

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

Layout lint, 2026-11-10: `research/tools/layout_lint.py` compiles all 1,198 game units and
drafts with `-sym on` and compares 1,299 classes by name (`research/layout/conflicts.tsv`,
`classes.tsv`). There are no vtable-pointer or base-class conflicts. There are 164
conflicts by member name and 6 by scalar width. 24 of them are "draft only": a draft
holds an offset that no matched file does. The 23 outside multiplayer were all checked
against the original, and all are names reused for different fields, not bugs. For
example, a BSP leaf has three count/list pairs, and one draft calls the waypoint pair
"triggers". The rest are matched files that disagree on names, e.g. `CQuaternion` is
xyzw in 34 files and wxyz in 16. They are byte-exact either way, so they are work for
the shared headers, not bugs. On the drafts as they stood before the 2026-11-09 fixes,
the lint flags `SweepAllExtents` and `CreateCollisionVolume`. It misses `BSGO_Soldier`
and the light pool, because no matched file has a view of those classes under the same
name. `layout_lint.py --check FILE SETTINGS` lints one draft in seconds. The lint isn't in
the test suite: a full run takes several minutes, and the naming conflicts must be
resolved by step 3.3 first.

Layout lint rerun, 2026-11-12 (full recompile, about a minute with 4 jobs): 1,185 files,
1,304 classes, 170 conflicts (163 member, 6 width, 1 vtable pointer), 18 draft-only;
168 and 16 after the two draft fixes below.
`research/notes/layout-conflicts.md` decides each width conflict and the most frequent
member conflicts from the original's loads and stores. One was a real draft bug: the
PopUpMessageHandler constructor draft built `CColor` as one word where the original stores
four bytes (fixed). `CQuaternion` is wxyz (scalar at +0, from `Rotate`). The new
vtable-pointer row is `decal.cpp`'s CRenderBin view (+0 against +0x1c in 77 files): wrong
but harmless, listed there with the other verified-unit views for the header step.

Shared headers, 2026-11-11: `include/game/CVector3.h` (with `msl_sqrtf.h`). Decision
point reached on the first class. `research/tools/try_header.py CLASS` moves every unit
with its own view onto the header in a copy, rewriting the view's field spellings, and
verifies it as `one_unit.py` does (the unchanged unit first, as a control). Of 256 units,
109 kept their match, 115 lost it and 32 didn't compile
(`research/layout/header_moves/CVector3.tsv`). Decision (user): move the 109; the other
147 keep their own view and count as guards. The complete image stays identical
(86.19%).

What the header is based on. The original keeps weak out-of-line copies of 12 CVector3
methods; they give `Normalize()` returning the length, `Scale` with the factor first,
`Abs`, `Cross`, `DotDotDot`, `Add`, `Sub`, and both constructors. Fields are plain `x, y,
z, w`: MWCC 1.3.2 can't reach an anonymous struct's members anywhere (inside the class or
out, with `gcc_extensions` on or off), so the views' `d.v[i]`, `d.c.x` and `c.x` can't
alias them and are rewritten. Why one header can't keep every match: copy behaviour. Some
matched files need copies as two doubles, others memberwise floats, and the original's one
out-of-line `operator=` returns by value, which adds a stack temporary that matched code
such as `CCamera::GetPosition` lacks. Three variants were tried on a sample (by-value
`operator=`, by-reference, implicit assignment); by-reference kept the most. Units that
emit the original's weak copies themselves lose them once the header makes the method
inline (8 "unexpected compiler function set"). Also learnt: the original has no static
`CMatrix::Multiply(CMatrix&, const CMatrix&, const CMatrix&)`, only the member
`Multiply(const CMatrix&, const CMatrix&)`, and its `CMatrix` constructor takes a
defaulted argument (`__defctor__7CMatrixFv`). CMatrix and ISceneNode are next.

CMatrix, 2026-11-12: `include/game/CMatrix.h`, four CVector3 rows `right, front, up, pos`
(named after the original's `SetRight`/`SetFront`/`SetUp`/`SetPos`), 16-byte aligned, every
out-of-line member the original has, its two weak copies (`__defctor__`, `PostMultiply`)
and a few inferred getters. Moving a file rewrites the views' `m[4][4]`, `m[16]` and
forward/position spellings. Of 148 units with their own view, 53 kept their match and were
moved, and 3 more after renaming their stripped extab labels; 87 still have their own CVector3 view, which the header's `game/CVector3.h`
redefines; 5 lost or need code changes (`research/notes/shared-headers.md`,
`research/layout/header_moves/CMatrix.tsv`). The alignment is the original's (it realigns
the stack for local matrices); player_ctor's CPlayerObject view, with matrices at 8-aligned
offsets, is the one conflict.

ISceneNode, 2026-11-12: `include/game/ISceneNode.h` with its bases (IDestructible, ISubject,
IObserver: the chain the original's RTTI gives) and the original's vtable order. Every
ISceneNode virtual is weak in the original, so the header defines them inline (Moh2.cpp's
matched bodies). Of 179 units with their own view, 54 still have their own CVector3 or
CMatrix view; of the other 125, 12 kept their match and were moved. 63 lost only because
MWCC emits a weak copy of each inline virtual a unit calls, which the unit configs didn't
list. The original's objects had those copies too (the linker kept Moh2.cpp's), so the
configs now list them as `weak_duplicates`: 53 of the 63 kept their match, 52 were moved (and compartment_ctor after renaming its stripped extab labels)
(`research/notes/shared-headers.md`).

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
   change any source. Both exist as of 2026-11-10 (`layout_lint.py`, `vtables.py`).
4. Prove 1_1's opening, fixing drafts as they're found (step 1.3-1.4).
5. `.bss` declarations and the first data tables, then the first pure-source link
   attempt (step 2). The first four tables are done (2026-11-10).
6. Shared headers for the first three classes, then the decision point (step 3.3-3.4).

Step 4 fills gaps between these and stays in port order.

Compiling this source for PC is the port's job, in its own repository. Here, a PC
compiler (clang, installed in WSL on 2026-11-09) may only check the decomp's own source,
for example that every file declares a class with the same layout (step 3.1).
