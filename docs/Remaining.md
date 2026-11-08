# Remaining work and priorities

Snapshot: 2026-11-08, **1,219,272 / 1,414,572 code bytes (86.19%)**. This file
records what is left and in what order it gets done. Figures here are a dated
survey; `progress/GMFE69.snapshot.json` is the verified count.

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

About 195 KB in 172 functions across 70 source files, all game and engine C/C++.
Most of those files are already partly matched. Areas are a judgement call based
on file and function names.

| Priority | Area | Left | Main files |
| ---: | --- | ---: | --- |
| 1 | Scripting and level objects | 58.8 KB | `bsbifunc.cpp`, `objcreate.cpp`, `buddy.cpp`, `staticobjfactory.cpp`, `proxtrig.cpp`, `bsfile.cpp`, `explosion.cpp`, `impact.cpp` |
| 1 | Collision | 25.2 KB | `cdbgeom.cpp`, `box.cpp`, `cdbobject.cpp`, `capsule.cpp`, `matrix.cpp` |
| 1 | AI | 21.1 KB | `Soldier_object.cpp`, `AISplinePath.cpp`, `AIObject.cpp`, `AIFilter.cpp`, `CoverPoint.cpp`, `PathNodeList.cpp` |
| 1 | Animation | 15.7 KB | `anim_callback.cpp`, `animst.cpp`, `skelblend.cpp`, `animtran.cpp`, `animmorph.cpp`, `dmcluster.cpp` |
| 2 | Rendering | 22.5 KB | `particlesystem.cpp`, `compartment.cpp`, `scene.cpp`, `staticmesh.cpp`, `font.cpp` |
| 2 | Shell and HUD | 15.8 KB | `ShellMenu.cpp`, `UserInterface.cpp`, `pumhandler.cpp` |
| 2 | Audio | 16.3 KB | `path.c`, `salloc.c`, `soundschedule.cpp`, `sound.cpp`, `smixer.c` |
| 2 | System | 7.1 KB | `debug_prof.cpp`, `toilet.cpp`, `input.cpp`, `aramalloc.cpp` |
| 2 | Video | 4.3 KB | `rcmp_main.cpp`, `getpic.cpp`, `hwvideodisplaylist.cpp` |
| 3 | Multiplayer (by name) | 8.4 KB | `BIFunc_UpdateFragCount`, `DrawFragCount`, `InitMultiplayer`, `BIFunc_MPAddPlayerHealth` |

Priority 1 finishes at about 94.7% at most. Other functions may turn out to be
partly multiplayer; those move to priority 3 when they are identified.

About 137 KB of the total has a working draft outside the repository, and about
61 KB is within 30 differing lines. Those counts come from a scorer that ignores
small-data constants, so a draft is only proven by the complete build. For the
port, a logically correct draft may be usable before it matches.

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
