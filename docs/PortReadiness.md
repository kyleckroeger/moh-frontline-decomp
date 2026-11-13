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

Input and comparison (2026-11-13, [details](Dolphin.md#testing-drafts-in-the-dolphin-emulator)):
`input_script.py` writes probe scripts and Dolphin movies; `clock_probe.cpp` holds frames to
emulated time and hashes the frame buffer; `link_drafts.py --coverage` and `dolphin_boot.py
prove` fill the new `proven` column of `research/drafts/status.tsv`. Both modes are
deterministic. On the Mission 1 route, 49 drafts are proven. `SweepAllExtents` was fixed:
it needed the original's 32-byte frame, because some original code reads stale stack memory.
`CParticleSystem::RenderSystem` changed particle frames because the draft kept a value in
f19 across `CMatrix::Multiply`, whose hand-written `PSMultMatrix` overwrites f14-f19
without saving them. Fixed, and proven by comparing its graphics commands per call
(`research/probes/gx_capture_probe.cpp`); frame hashes also depend on CPU timing.

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

Pure-source link, 2026-11-13 (`source_link.py --jobs 4`): 1,257 units, 98 drafts and 85 data
files, all compiled. **84 undefined, 152 fragment-local, 53 duplicate**, 0 small-data. Of
the 84 undefined, 67 are vtables (64 global, 3 weak). There are 5 string-block device
calls (`ShellMenuBlockItem` and four like it), which the units strip, and 12 others. Those
are the SDK aliases (`PSVECNormalize` ...), MSL's `nextafter`/`__double_max`, `axDspSlave`,
`_eti_init_info`, `__CARDIsWritable`, one draft's `PATHI_choosebeat` and
`CUcodeRenderBin::Render`. Why the vtables stay undefined is in step 3.2 below.

Whole-file merges, 2026-11-13 (agent merge1; the maintainer's rule: a file's vtables are
defined by merging its fragments into one unit once the file is complete, not from
`research/data/`). 80 original files had all their functions verified but were still split
into fragment units (`scratch/loop3/merge1/complete_files.tsv`: 30 own a global vtable, 21
more have fragment-local statics). Twelve were merged into one unit each, named after the
file and owning its `.text`, `.rodata`, `.data` and `.sdata`. Nine own a vtable, now defined
from source with its base lists and the file's local RTTI records: AIPlayerDoodad, AIPlayerObject,
AISoldierObject, AITankDoodad, AITankObject, AISoldierDoodad, csg_volume, collisionvolume and
bullet. Three are EA files whose static becomes file-local again: locatbig, memalloc and
printstr. These units compile with RTTI on and declare the destructor first as the key
function. When a file has other `.data` (switch tables) or several classes' RTTI, the
original's order only comes out with `-inline auto,deferred` and the functions written in
reverse. The re-run (`source_link.py --jobs 4`, 1,240 units, 80 drafts, 85 data files) gives
**92 undefined, 144 fragment-local, 51 duplicate**, 0 small-data. The merges removed 9
undefined vtables (55 global ones are left) and 3 fragment-local symbols. The undefined count
rose because 18 drafts no longer compile in the link: commit 4c75217f moved them onto the
shared headers, but their settings templates lack the `include` directory. 17 of the
undefined symbols are functions that only those drafts define. Without them the count would
be 75.

Include fix, 2026-11-13 (agent merge2): `source_link.py`, `source_symbols.py`,
`layout_lint.py` and `check_data.py` now append `include/` to a source's include directories,
as `fast.py` does since 4c75217f (`brchk.py` already did). Re-run (1,240 units, 98 drafts,
85 data files, all compiled): **75 undefined, 149 fragment-local, 53 duplicate**, 0
small-data. The 18 drafts compile again and define their 17 functions; they also bring back
5 fragment-local and 2 duplicate symbols that the failed compiles had hidden.

More whole-file merges, 2026-11-13 (agent merge2, `scratch/loop3/merge2/notes.md`): fader,
sprite, animated and animated_volume (vtable owners: `__vt__9CFaderBin`,
`__vt__13CFaderControl`, `__vt__7CSprite`, `__vt__19CSpriteNoTextureBin`, `__vt__9CAnimated`,
`__vt__15CAnimatedVolume` now defined from source), then smixfx, sdspmix, stretch, animseq,
animmot and SearchNode (file statics). Re-run (1,217 units, 98 drafts, 84 data files, all
compiled): **70 undefined, 137 fragment-local, 53 duplicate**, 0 small-data: 5 vtables fewer
undefined and 12 fragment-local symbols gone. The remaining vtable owners (Tank_object,
weapon, attachobject, thrown_obj, sphere, fiber, hierobject, light, thrownbullet, animplayer)
have fragments on private CVector3/CMatrix views; each needs them moved onto the shared
headers before it can be one unit.

More whole-file merges, 2026-11-13 (agent merge3, `scratch/loop3/merge3/notes.md`): filesys,
bsmachin, filedev, dvdStatus_base, UIStudio, UISUtils, snddrv, targimpl, propdat (with the
former propdat_offsetptr unit), soundschedule and collision, plus smixfx.c's last function
(MIX_getwetbuffer) moved from smixfram into smixfx. Re-run (1,162 units, 98 drafts, 79 data
files, all compiled): **70 undefined, 109 fragment-local, 50 duplicate**, 0 small-data: 28
fragment-local symbols gone (file statics now defined by their file's unit; the per-file .bss
data files of filedev, dvdStatus_base, UISUtils, targimpl and soundschedule removed, snddrv's
record out of research/data/bss/unassigned.cpp). Not merged: Gecko_ExceptionPPC (needs GC 1.3
for ExPPC_UnwindStack and 1.3.2 for __unexpected), bsutil and world_volume (an inline
function's out-of-line copy and its inlined copies need different local orders), the vtable
owners (fragments on private CVector3 views that the shared header's copy changes), and alarm,
pumhandler, world_object, anim_obj (local dwi templates or private scene-node views).

Vtable owners merged, 2026-11-13 (agent merge4, `scratch/loop3/merge4/notes.md`; the
coordinator's ruling: a merged unit may keep a file-private CVector3/CMatrix view, marked
inferred, where the shared header's copies don't keep the match). Tank_object, attachobject,
sphere and fiber are one unit each and define `__vt__11CTankObject`, `__vt__17CAttachableObject`,
`__vt__10CVolSphere` and `__vt__9CVolFiber` with their files' RTTI records. Tank_object and
attachobject declare CStaticObject : IMovingSceneNode (and CAttachableObject, CHierObject) from
the original's vtables on the shared headers. sphere and fiber keep a file-private CVector3
that copies through an aligned base as the shared header does, but with the implicit
assignment. `tools/port_unit.py`: the exception records of dropped weak duplicates no longer
pin a section's address. Re-run (1,143 units, 98 drafts, 79 data files, all compiled):
**66 undefined, 109 fragment-local, 50 duplicate**, 0 small-data, with 4 vtables fewer
undefined. Not merged: weapon and thrown_obj (fragments on private vector views and
placeholder scene-node views that conflict with CStaticObject's members), hierobject, light,
thrownbullet and animplayer (not attempted).

More vtable owners merged, 2026-11-13 (agent merge5, `scratch/loop3/merge5/notes.md`):
hierobject, weapon, thrown_obj, thrownbullet and animplayer are one unit each and define
`__vt__11CHierObject`, `__vt__7CWeapon`, `__vt__13CThrownObject`, `__vt__13CThrownBullet` and
`__vt__15CAnimatedPlayer` with their files' RTTI records (thrownbullet also
`MP_THROWNBULLET_DRAW_DELAY`, so `research/data/init/thrownbullet.cpp` is gone, and its file
static `SECB_NullifyPointer`). hierobject's CStaticObject (attachobject's declaration with the
motion, path, volume and light members named in its padding) is reused by weapon and
thrown_obj; CBullet, CAnimObject and CSoldierObject are declared from their vtables. File-private
views: hierobject and animplayer use the shared CVector3 with the assignment through the
double-pair union; weapon a vector/matrix pair declared before the shared scene-node headers
(include guards predefined); thrown_obj and thrownbullet constructor-free vectors with private
copies of ISceneNode/IMovingSceneNode (two inline bodies rewritten for the view). Re-run
(1,117 units, 98 drafts, 78 data files, all compiled): **61 undefined, 108 fragment-local,
50 duplicate**, 0 small-data, with 5 vtables fewer undefined. Not merged: light (all 45
functions match; the weak `__vt__17CAnimLightManager` and its RTTI record come out after the
other vtables instead of right after `__vt__19CInstancedAnimLight`, and its two weak functions
in the other order), anim_obj, alarm, pumhandler and world_object (not attempted).

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

   Done 2026-11-13: `research/layout/vtables.tsv` (`vtables.py --tsv`) has every word of the
   113 tables. That is 115 RTTI and offset pairs and 2,721 slots, with 30 pure-virtual zeros.
   Two classes have a second part, and their overrides of the second base are appended after
   it. `vtable_order.py` compares each class view with these tables
   (`research/notes/vtable-order.md`). 1,029 of 1,173 views agree in every slot they declare.
   `include/game/ISceneNode.h` agrees in all 53 slots, so no header needed a fix. Most of the
   144 that differ are deliberate. A fragment that defines a key function but mustn't own
   its class's table declares another virtual first, so MWCC doesn't emit the table there.
   The others are reduced views and 20 mislabelled slots (wrong names at the right offsets),
   listed for the header moves. Every one of the 64 undefined global vtables has its key
   function in a verified unit or draft that keeps the table off on purpose: the table's
   bytes lie outside that fragment. Defining them needs a unit that owns the bytes, or a
   rule for emitting tables from `research/data/`. That is a decision for the maintainer.
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

CVector3 copy constructor, 2026-11-12 (`research/notes/cvector3-header-variants.md`): the
original's weak copies show a user copy constructor in compartment.cpp and hierobject.cpp,
whose other units only matched without one, so the split was the constructor's form, not
different headers. The header now keeps the fields in an 8-byte-aligned plain base,
`CVector3Base`, and the copy constructor copies it; the pair-cast form and no copy
constructor each break a different group. 42 more units moved (matrix_ps, thrown_obj_init and
compartment_intersect among them) and the 168 already on it still match. A re-run over the 110
units still on their own view moved 4 more, each after renaming its stripped extab labels; the
rest differ in code (76) or don't compile (33). Three of the four then moved onto CMatrix.h too, and two onto ISceneNode.h.

CMatrix, 2026-11-12: `include/game/CMatrix.h`, four CVector3 rows `right, front, up, pos`
(named after the original's `SetRight`/`SetFront`/`SetUp`/`SetPos`), 16-byte aligned, every
out-of-line member the original has, its two weak copies (`__defctor__`, `PostMultiply`)
and a few inferred getters. Moving a file rewrites the views' `m[4][4]`, `m[16]` and
forward/position spellings. Of 148 units with their own view, 53 kept their match and were
moved, and 3 more after renaming their stripped extab labels; 87 still have their own CVector3 view, which the header's `game/CVector3.h`
redefines; 5 lost or need code changes (`research/notes/shared-headers.md`,
`research/layout/header_moves/CMatrix.tsv`). The alignment is the original's (it realigns
the stack for local matrices); player_ctor's CPlayerObject view, with matrices at 8-aligned
offsets, is the one conflict. After the CVector3 base form, a re-run over the 92 units still on their own
view moved 31 more; 54 of the rest still have their own CVector3 view.

ISceneNode, 2026-11-12: `include/game/ISceneNode.h` with its bases (IDestructible, ISubject,
IObserver: the chain the original's RTTI gives) and the original's vtable order. Every
ISceneNode virtual is weak in the original, so the header defines them inline (Moh2.cpp's
matched bodies). Of 179 units with their own view, 54 still have their own CVector3 or
CMatrix view; of the other 125, 12 kept their match and were moved. 63 lost only because
MWCC emits a weak copy of each inline virtual a unit calls, which the unit configs didn't
list. The original's objects had those copies too (the linker kept Moh2.cpp's), so the
configs now list them as `weak_duplicates`: 53 of the 63 kept their match, 52 were moved (and compartment_ctor after renaming its stripped extab labels)
(`research/notes/shared-headers.md`). After the CVector3 base form and the CMatrix re-run, 8 more
moved (4 with their weak copies listed); 38 still have their own CVector3 or CMatrix view.

CScreen, 2026-11-13: `include/game/CScreen.h` (with CScreenFieldMode) and the value types it
holds, `CColor.h` and `CRect.h`. The virtuals are in the original's table order with its names
(GetHeight, GetWidthScale; two pure slots named by CScreenFieldMode's table: Rebuild, SetDraw).
GetHeightScale and GetWidthScale are inline (weak in the original), everything else out of line
in Screen.cpp. Of 38 units with their own view, 29 kept their match and were moved (3 with the
weak GetWidthScale copy listed); 5 still have their own CVector3 view, 4 need code changes
(`research/notes/shared-headers.md`).

CCamera, 2026-11-13: `include/game/CCamera.h` with its base `IMovingSceneNode.h` (all inline:
weak in the original) and `CPlane.h`. Of 14 units with their own view only 2 moved: 9 still
have their own CVector3 or ISceneNode view, camera_dtor keeps its view to keep the table off,
2 need code changes.

CInputManager, 2026-11-13: `include/game/CInputManager.h` with `CDeviceManager.h`. The device
manager is a member at +0, not the base two views made it (the weak destructor destroys it as a
member). 20 of 23 units with their own view moved; 3 call the device manager as a base (same
code, different source) and stay on their views.

CScene, 2026-11-13: `include/game/CScene.h`: every member function the original has, the fields
the views agree on, and the dwi containers as byte ranges until they get a header (their types
are in the mangled names). It includes no other header, so units still on their own ISceneNode
or CVector3 view can use it. 74 of 94 units moved (4 after renaming stripped labels); the 10
scene units that use the containers by member wait for a dwi header.

CPlayerObject, 2026-11-13: `include/game/CPlayerObject.h`, 0x12d0 bytes, the original's 72-slot
table with its weak members inline, the agreed fields and flag bits, and byte ranges for the
embedded objects that have no header yet. 41 of 103 units moved; 40 still have their own
CVector3/CMatrix/ISceneNode view, player_dtor keeps the table off, the rest need code changes.
With this the boot-path classes of this step (ISceneNode, CScene, CPlayerObject, CCamera,
CScreen, CInputManager) all have a shared header.

dwi containers, 2026-11-13: `include/game/DwiContainers.h` (list, vector, fast_vec and their
iterators, min/max) and `include/game/DwiHashMap.h` (hash_map with the prime table every including
file emits). 54 of 114 units moved (13 with relabelled stripped labels or listed weak copies);
the fragments that place the weak template copies by explicit class instantiation stay on their
views (instantiating the whole header changes them).
CScene's containers are now typed (with DwiContainers.h included first; ISceneNode.h gained
SExtent for the extent lists): scene and objcreate_nonvis moved; the other scene units use
invented helpers in their CScene views (Matches, FindExclusion, ProcessCommands, InvolvesBullet).

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
