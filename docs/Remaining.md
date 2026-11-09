# Remaining work and priorities

This file records what is left and in what order it gets done. The figures are
generated from `progress/GMFE69.snapshot.json` by `tools/sync_docs.py`; the rest
is kept by hand and changes in the same commit as the decision or work behind it.

## Why the order matters

The matching source is meant to be the base of a native PC port without
multiplayer. Work is ordered by what that port needs, not by what is cheapest
to match.

1. **Collision, scripting, AI and animation.** The game cannot run correctly
   without them.
2. **Everything else** that is not multiplayer: rendering, shell and HUD, audio,
   video and system code. The port is likely to replace some of it anyway (GX
   rendering, audio, memory cards, movie playback, debug tools).
3. **Multiplayer, last.** It is decompiled for the completeness of the project,
   not for the port.

A cheaper function in a later group does not jump the queue.

## What is left

<!-- sync_docs:status -->
Verified: **1,219,272 / 1,414,572 code bytes (86.19%)**. Left: about 195.1 KB in 170 functions across 64 source files.

| Priority | Area | Left | Main files |
| ---: | --- | ---: | --- |
| 1 | Scripting and level objects | 58.8 KB | `bsbifunc.cpp`, `objcreate.cpp`, `buddy.cpp`, `staticobjfactory.cpp`, `proxtrig.cpp`, `bsfile.cpp` |
| 1 | Collision | 25.2 KB | `cdbgeom.cpp`, `box.cpp`, `cdbobject.cpp`, `capsule.cpp`, `matrix.cpp`, `quaternion.cpp` |
| 1 | AI | 21.1 KB | `Soldier_object.cpp`, `AISplinePath.cpp`, `AIObject.cpp`, `AIFilter.cpp`, `CoverPoint.cpp`, `PathNodeList.cpp` |
| 1 | Animation | 15.7 KB | `anim_callback.cpp`, `animst.cpp`, `skelblend.cpp`, `animtran.cpp`, `animmorph.cpp`, `dmcluster.cpp` |
| 2 | Rendering | 22.5 KB | `particlesystem.cpp`, `compartment.cpp`, `scene.cpp`, `staticmesh.cpp`, `font.cpp`, `decal.cpp` |
| 2 | Audio | 16.2 KB | `path.c`, `salloc.c`, `soundschedule.cpp`, `sound.cpp`, `smixer.c`, `spktplay.c` |
| 2 | Shell and HUD | 15.8 KB | `ShellMenu.cpp`, `pumhandler.cpp`, `UserInterface.cpp`, `popup.cpp` |
| 2 | System | 7.1 KB | `debug_prof.cpp`, `toilet.cpp`, `input.cpp`, `aramalloc.cpp` |
| 2 | Video | 4.3 KB | `rcmp_main.cpp`, `getpic.cpp`, `hwvideodisplaylist.cpp`, `swvideodisplaylist.cpp`, `rcmpbase.cpp` |
| 3 | Multiplayer (by name) | 8.4 KB | `BIFunc_UpdateFragCount`, `DrawFragCount`, `InitMultiplayer`, `BIFunc_MPAddPlayerHealth` |

Priority 1 is 120.8 KB and would take the project to 94.7% at most.

Partial-source sweep: 5 code-exact, not yet linked (5.7 KB), 89 consistent (119.7 KB), 8 flagged as differing (11.2 KB); 68 functions (58.5 KB) have no draft yet. These are checker results, not proof of behaviour, and earn no progress credit.
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
because a change under `src/` needs a full rebuild. Partial source earns no progress
credit, and 86.19% won't move because of it.

`research/tools/check_draft.py` compares each draft with the original by content:
calls in order, floating constants, strings and tables, globals, field offsets. It
writes `research/drafts/status.tsv`. A similarity score is never proof of correctness,
and "consistent" doesn't prove behaviour; the real test is running swept functions
in Dolphin or the port. Work order: functions with no draft in priority 1, then drafts
flagged as differing, then the other areas, then multiplayer. Anything that happens
to match exactly still lands the normal way.

### Two progress figures, after the sweep

The verified figure stays the headline and its rules don't change: bytes that match
exactly in the complete-image comparison. After the sweep, a second figure will report
**linked** bytes: exact bytes plus partial source that compiles and links into a working,
non-matching build. It needs its own build, since partial functions differ in size and
can't sit at the original addresses, and the two figures are always reported separately.
Linked doesn't mean correct; only runtime testing shows that.

## Known blockers in priority 1

- **`buddy.cpp`:** `DestroyBuddyDataStructures` needs the file's `.bss`, and so
  `__sinit_buddy_cpp`, which is stuck on the shape of a `BuddyInfo` temporary.
- **`UpdateTargetPosition`:** matches only with a form that was declined (below).
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

**Declined:**
- Devices that add storage, slots or code only to steer registers: one-element
  local arrays, padding arrays, chained `x(copy = y)` assignments, aliases
  such as the `STREAM_kill` one, `(int)(filter=0)` and the
  `staticobjfactory` offset spelling.
- A reconstructed removed function for `InitMultiplayer`, and a duplicate inline
  for `UpdateTargetPosition`.
- Invented dead functions with made-up logic to hold orphan strings.
- Identity wrappers such as `RetV`: useful for discovery, never landed.
- Using the block header outside `ShellMenu.cpp`, unless the maintainer agrees.

**On hold:** gap functions, where a unit compiles an unmatched function of its
file and strips it, leaving its bytes uncovered, to hold the file's data layout.
