# Remaining work and priorities

This file records what is left and in what order it gets done. The figures are
generated from `progress/GMFE69.snapshot.json` by `tools/sync_docs.py`; the rest
is kept by hand and changes in the same commit as the decision or work behind it.

## Why the order matters

The matching source is meant to be the base of a native PC port without
multiplayer. Work is ordered by what that port needs, not by what is cheapest
to match.

From 2026-11-16 the plan for the rest of the project, and for the hand-off to the PC port, is
[research/plans/endgame.md](../research/plans/endgame.md).

1. **Collision, scripting, AI and animation.** The game cannot run correctly
   without them.
2. **Everything else** that is not multiplayer: rendering, shell and HUD, audio,
   video and system code. The port is likely to replace some of it anyway (GX
   rendering, audio, memory cards, movie playback, debug tools).
3. **Multiplayer, last.** It is decompiled for the completeness of the project,
   not for the port.

A cheaper function in a later group does not jump the queue.

Matching is no longer the only work. [PortReadiness.md](PortReadiness.md) is the plan
for what the port needs beyond it: drafts proven in play, all data in source, and one
declaration per class. It sets the overall order; these priorities apply within it.

## What is left

<!-- sync_docs:status -->
Verified: **1,305,184 / 1,414,572 code bytes (92.27%)**. Left: about 109.2 KB in 70 functions across 43 source files.

| Priority | Area | Left | Main files |
| ---: | --- | ---: | --- |
| 1 | Scripting and level objects | 28.2 KB | `bsbifunc.cpp`, `staticobjfactory.cpp`, `bsfile.cpp`, `objcreate.cpp`, `explosion.cpp`, `proxtrig.cpp` |
| 1 | Collision | 13.2 KB | `box.cpp`, `cdbgeom.cpp`, `cdbobject.cpp`, `capsule.cpp`, `matrix.cpp`, `quaternion.cpp` |
| 1 | AI | 12.8 KB | `Soldier_object.cpp`, `AISplinePath.cpp`, `AIFilter.cpp`, `PathNodeList.cpp`, `CoverPoint.cpp` |
| 1 | Animation | 12.5 KB | `anim_callback.cpp`, `skelblend.cpp`, `animtran.cpp`, `dmcluster.cpp`, `dmesh.cpp`, `animmorph.cpp` |
| 2 | Rendering | 13.0 KB | `particlesystem.cpp`, `compartment.cpp`, `scene.cpp`, `staticmesh.cpp`, `font.cpp` |
| 2 | Shell and HUD | 9.5 KB | `ShellMenu.cpp`, `UserInterface.cpp`, `popup.cpp` |
| 2 | Audio | 8.0 KB | `path.c`, `salloc.c`, `smixer.c`, `spktplay.c` |
| 2 | System | 3.1 KB | `toilet.cpp`, `debug_prof.cpp`, `aramalloc.cpp` |
| 2 | Video | 0.6 KB | `swvideodisplaylist.cpp` |
| 3 | Multiplayer (by name) | 8.4 KB | `BIFunc_UpdateFragCount`, `DrawFragCount`, `InitMultiplayer`, `BIFunc_MPAddPlayerHealth` |

Priority 1 is 66.7 KB and would take the project to 97.0% at most.

Partial-source sweep: 1 code-exact, not yet linked (1.2 KB), 67 consistent (102.2 KB), 2 flagged as differing (5.8 KB); 0 functions (0.0 KB) have no draft yet. These are checker results, not proof of behaviour, and earn no progress credit.

Linked: **1,414,400 of 1,414,572 code bytes (99.99%)** are exact or partial source that links into a non-matching build (170 partial functions). Linked doesn't mean correct, but that build plays scripted input routes in Dolphin, with 79 of the 87 drafts proven in play (`docs/Dolphin.md`).
<!-- /sync_docs:status -->

All of it is game and engine C/C++, and most of those files are already partly
matched. Areas are a judgement call based on file and function names, and other
functions may turn out to be partly multiplayer; those move to priority 3 when
they are identified.

Many of these functions have drafts in [`research/drafts/`](../research/README.md).
A draft is only proven by the complete build, because the near-miss scorer ignores
small-data constants. For the port, a logically correct draft may be usable before
it matches. Earlier attempts at each function are in `research/log/`: search there
before retrying one.

## Partial-source sweep

As of 2026-11-08 the main goal is logically correct source for every remaining
function, with exact matching second. Exact matching had slowed to a crawl, and the
port needs complete, correct logic more than identical bytes. The approach follows
[Rising Sun's partial-source workflow](https://github.com/lifewillbeokay/moh-rising-sun/blob/main/docs/Progress.md#partial-source-workflow),
including its three separate states:

- **Partial:** the whole function compiles and is compared, and the differences are
  known. The original bytes stay in the verified image.
- **Exact, unlinked:** every instruction matches, but it hasn't passed the complete
  build. Near-misses in this state may depend on layout devices or declined forms.
- **Linked and verified:** it passed the complete-image build and earns progress.

Readable source is useful even with a few differences left: its behaviour and types
can be read and reviewed, and a swapped pair of register moves shouldn't keep it
unpublished. So attempts are time-boxed, and the work is published with its
differences measured rather than left in `scratch/`.

Unlike Rising Sun, partial source lives in `research/drafts/` for now, not `src/`,
because a change under `src/` needs a full rebuild. Partial source earns no credit in
the verified figure, and the verified figure won't move because of it.

`research/tools/check_draft.py` compares each draft with the original by content:
calls, floating constants, strings and tables, globals, field offsets. It
writes `research/drafts/status.tsv`. A similarity score is never proof of correctness,
and "consistent" doesn't prove behaviour; the real test is running swept functions
in Dolphin or the port. Every function now has a draft. Drafts still flagged differ in
load counts or one table word, and each is listed in `research/drafts/status.tsv`.
Anything that happens to match exactly still lands the normal way.

### Two progress figures

The verified figure stays the headline and its rules don't change: bytes that match
exactly in the complete-image comparison. The second figure, **linked** bytes, adds
partial source that links into a non-matching build. The two are always reported
separately, and only the first is proof of anything exact.

`research/tools/link_drafts.py` measures it and writes `progress/GMFE69.linked.json`. Run
it after changing drafts, then `tools/sync_docs.py`. A complete relink isn't needed:

1. Each draft is compiled with small data and pooling turned off. Its data can't stay
   in range of the original small-data registers, and pooled globals are reached from
   a section anchor that would leave the draft using its own copies.
2. Its symbols resolve against the original image. Anything else the draft defines
   also resolves to the original, except the functions being linked and statics that
   exist only in the draft. Where a name has several original addresses (file-local
   statics), the one the original function refers to, or nearest to, is used.
3. The compiled code and data become a new load segment at the old start of the arena,
   and both of `OSInit`'s arena assignments (`__ArenaLo`, then `_stack_addr`) move past it.
   The program headers stay at the start of the file, because the game's disc loader
   reads every segment as empty if they move.
4. The first instruction of each original function becomes a branch to its draft.

The result is `build/linked/Moh2RelGC_linked.elf`, which the boot loader loads as
`Moh2RelGC.elf` from a disc image. **Runtime result (Dolphin 2609, 2026-11-09): with
all 170 drafts it boots, plays the three opening movies and reaches the main menu.**
Since then the linked builds have played scripted input routes, and 89 of the 94 drafts in
`research/drafts/status.tsv` are proven in play (2026-11-17; `docs/Dolphin.md`). Two findings got it there: the pooling problem above, and
`__sinit_compartment_cpp`'s draft constructing its render bins with the virtual table
pointer at `+0` instead of `+0x1c`. A bisect over the patched functions found that one,
so the same method applies to later crashes.

Known risks:

- `OSInit` uses the arena start from the boot information block when that's set; the
  new segment assumes it isn't, which held in Dolphin.
- Exceptions can't unwind through a draft, because its exception tables are dropped.
- Drafts are only as right as their inferred layouts. To find a bad one, rebuild with
  `--exclude` and bisect; a function left out runs the original code.

## Known blockers in priority 1

- **`buddy.cpp`:** `DestroyBuddyDataStructures` and `__sinit_buddy_cpp` have landed
  (`config/GMFE69/buddy.json`), and `BIFunc_GetInitialBuddyOrders` is in buddy_head (2026-11-17).
  Left: `AddBuddy` and `BIFunc_AddBuddy` (the `BuddyInfo` partial-scalar knot), and
  `BIFunc_CreateBuddyDataStructures`, which lands with an earlier `.bss` user such as
  `AddBuddy` (`research/plans/matching.md`, `research/notes/buddy.md`).
- **`bsbifunc.cpp` built-ins:** many reach the file's shared `.bss` block, so they
  cannot land alone.
- **Register near-misses:** many have had repeated attempts. Check the history of
  a function before retrying it.

## Source forms

Every landing must stay byte-exact in the complete-image comparison. Beyond that,
the maintainer has decided which forms are acceptable.

**Accepted:**
- Named copies with no added storage (`T* const self = this`, typed parameter
  copies, an `int i = 0` index local).
- Single-use inline helpers, byte-offset loops and inferred constructors.
- `#pragma dont_inline` in game code, and per-field `volatile` on shared EA data.
- Linker-stripped stubs for classes attested by the original.
- The extab padding patch ([Progress.md](Progress.md)).
- `src/game/ShellMenu_block.h`, for `ShellMenu.cpp` only: the block's strings and
  tables copied from the original into a discarded function with no logic
  ([Dolphin.md](Dolphin.md)).
- Added 2026-11-12 (maintainer delegated these to the coordinator's recommendations; see
  the State section of research/plans/three-day-loop.md, questions 1-17):
  - `.bss` size views: a fragment defines its file's `.bss` objects in original order with
    exact sizes, container views without constructors and 12-byte destructor-chain
    placeholders, all marked inferred, no `__sinit` (commits tagged "(.bss pooled)").
  - `#pragma auto_inline off` / `reset`, like `dont_inline`, in game and EA code.
  - The block-header form (strings and tables copied into a discarded function with no
    logic) for any file whose `...rodata.0` head holds strings only stripped functions used:
    rcmp_main, UserInterface, pumhandler and bsbifunc so far.
  - Small value-returning inline helpers (products, differences) where the original's
    register colouring shows a return temporary, single-purpose helpers whose parameter order
    reproduces the original, and two inline copies of a block when the original's two
    copies compile differently. Names are inferred and marked so; real names (CVector3::Sub,
    Scale, Add, Dot, ...) are preferred wherever the original's symbols give one.
  - Redundant tests, redundant zeroing and dead `else` code when the original's instructions
    show them (the compiler cannot create them).
  - Constant initialisation chains (`a = b = c = K` with a literal K), and a store chain into a
    member plus a local the code goes on using (`extra = m_messages[i].m_extra = entry[0];`,
    PopUpMessageHandler ctor, 78daa000). Copy chains that only steer registers stay declined.
  - A member `operator[]` used once to keep a local in memory, and an inferred 3x3 matrix view.
  - Inline helpers whose boundaries are read from the original's stack-slot groups, forwarding
    overloads, and the reference-returning `dwi::min`/`dwi::max`.
  - An identity statement standing in for a compiled-out statement in an otherwise empty
    `case`, with a comment saying so.
  - A second-level assigning inline (CAnimMorph::Draw's `Assign`) when the original's last
    stack slot holds its dead `operator=` temporary; and per-line spelling choices found by
    search (mixed `[i]` and `.x`).
  - A view-based pointer-cast field spelling (LevelField) in place of byte offsets.
  - The shared CVector3 header's base-class copy constructor (`CVector3 : CVector3Base`).
  - Added 2026-11-13 (coordinator ruling): buddy.cpp's empty by-value inline `Check(BuddyState)`
    called first in `BuddyInfo`'s constructor. It reads like a compiled-out debug check; its dead
    whole-struct read makes CW keep only the state part of a `BuddyInfo` temporary in memory, as the
    original does. It replaces the earlier `BuddyState unused = state;` spelling in every buddy unit.

**Accepted by the maintainer (2026-11-16, endgame plan D2), were declined:** `STREAM_kill`'s
`void**` header alias in the handle check (src/ea/stream_kill.cpp) and `UpdateTargetPosition`'s
inline filter step, a second body for the filter beside CalculateTargetPosition's setup
(src/game/AIObject_updatetarget.cpp). Both landed as their own fragments, because the same
spellings in the neighbouring fragment break functions that are already exact there.
`InitMultiplayer`'s form is accepted too, but it can't land as a fragment. Its code reaches
its strings at +0x274 from player.cpp's shared `.rodata` base, and its draft got there only
with a scoring-harness padding array. It waits for a player.cpp merge (multiplayer, last).

**Declined:**
- Devices that add storage, slots or code only to steer registers: one-element
  local arrays, padding arrays, chained `x(copy = y)` assignments, `(int)(filter=0)` and
  the `staticobjfactory` offset spelling.
- (Moved to accepted 2026-11-16, above: the `STREAM_kill` alias, `UpdateTargetPosition`'s
  duplicate inline and `InitMultiplayer`'s form.)
- Invented dead functions with made-up logic to hold orphan strings.
- Identity wrappers such as `RetV`: useful for discovery, never landed.
- Using the block header outside the files listed above.
- Dead branches with logic inside a real function to place a string (TLT_LoadLevelContentsFile).
- Hand-written vtable stores after placement new.
- Different bodies for the same inline in different fragments (DrawTextCentered, FlagAsOccupied).
- An identity `Copy()` to keep dead stack copies (DoTargetCheck), and a by-value `operator=`
  only to move slots (PlayerScreenFlash).

**Accepted by the maintainer (2026-11-13), was on hold:** gap functions with logic, where a unit
compiles a function of its file that the original linker stripped, to reproduce the file's data
layout or weak-copy placement (light.cpp's discarded CAnimLightManager helper, world_volume.cpp's
Probe()). Write the function as plausible source with the logic its references require; its bytes
are stripped, as in the original. (Strings-only block headers are accepted above.)

**Accepted (coordinator ruling 2026-11-13, landed 2026-11-16):** a single fragment that holds its
class's key function may turn on RTTI and own its file's virtual table and RTTI records (.data,
.sdata, .rodata, .sdata2) without merging its neighbours, when byte-exact. It is a stepping stone
toward the whole-file merge (first: cdbobject_dtor.cpp owns `__vt__10CCDBObject`).

**Rulings of the 2026-11-17/18 matching run, accepted by the maintainer (2026-11-24):**
- Accepted: a single-use typed-read inline over an argument slot (`ReadObject(int* p) { return
  *(BSObjectView**)p; }`, BIFunc_GetObjInfo), as a single-use inline helper; it lands only as
  part of a byte-exact form.
- Accepted: a class `operator new` returning through a named local for one class while other
  classes keep the direct return (BSGO_Tank vs BSGO_Soldier/Destructable): different classes, so
  not two bodies of one inline. Single-use helpers returning through a named local
  (`AllocHier`, `TankOf`, CreateRootHierObject).
- Accepted: buddy.cpp's ready-list dump copy differing from GetInitialBuddyOrders' (two inline
  copies when the original's copies compile differently).
- Accepted: respelling an inline body inside a landed unit, when that unit stays byte-exact and
  every user of the inline takes the new body.
- Declined: an identity-conversion inline used only to keep a parameter copy alive (`KeepU`,
  identity wrapper family), and a macro accessor where a sibling fragment of the same file has
  the body as an inline (different bodies per fragment).
- Tooling: cadmic/mwcc-debugger is not installed; the ptrace tracers in research/tools/alloc/
  cover the need.

**Accepted by the maintainer (2026-11-13):** `axDspSlave`/`axDspSlaveLength` (SDK DSP microcode,
6,624 B .data + 2 B .sdata) are transcribed into `research/data/` as data, not left platform-owned.
