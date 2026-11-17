# Endgame plan: the last 10% and the hand-off to the PC port (2026-11-16)

The decomp has done its cheap work. From here, match progress will be slow. This plan
changes what "done" means for this repository. The goal is now a decomp the PC port
(`moh-frontline-pc`) can build on and trust. 100% matching is no longer the goal. Matching
continues only where it pays for the port, or within a fixed time box.

## Where we are

| Measure | Now | For the port |
| --- | --- | --- |
| Verified matching code | 1,282,440 / 1,414,572 B (90.66%) | Nice to have; not the gate |
| Code with source | 100% (96 unverified functions all have drafts: 5 code-exact, 89 consistent, 2 differs) | Have it |
| Drafts proven in play (Dolphin, identical to control) | 91 / 96 (not: 3 multiplayer, YUV_WriteRow, RCMP_SYSTEM dtor) | Proven on **all** levels, not only the routes run so far |
| Pure-source link | 25 undefined, all vtables; 101 fragment-local, 45 duplicate | Zero undefined, boots, replays |
| One declaration per class | Shared headers: CVector3, CMatrix, ISceneNode, CScreen, CCamera, CInputManager, CScene, CPlayerObject (+ boot path). About 790 files still declare their own views | One header per class the port compiles, 64-bit clean |

The unverified bytes break down like this (research/notes/loop-2026-11-13.md):
- 0-2 lines off: 20.7 KB in 13 functions
- 3-10 lines off: 21.5 KB in 24 functions
- 11-60 lines off: 38.1 KB in 28 functions
- over 60 lines off: 40.5 KB in 23 functions
- multiplayer: 11.1 KB
- unidentified: 172 B

The last loop day (11-13) added 1.4 KB, so the near-miss rate has dropped a lot.

## What the port actually needs from here, ranked

1. **Behaviour it can trust.** Every function the port compiles must act like the original. A
   draft that is "consistent" but wrong costs far more in the port, where native bugs mix in,
   than here, where Dolphin replays it against the control. This is the decomp's most valuable
   remaining output.
2. **Complete source.** No original bytes and no undefined symbols. Each vtable must come from
   a class declaration.
3. **One declaration per class, and 64-bit clean.** The port can't compile about 790 differing
   views. The port's own notes list the first ones it hits: SDK structs declared at GameCube
   sizes (OSThread as `char[784]`, OSMessageQueue, OSAlarm, DVDFileInfo/DVDCommandBlock,
   GXTexObj, EA's `_AXVPB` view), pointers passed as `unsigned long` (ARQ/DMA records; they
   truncate on Windows), and DVDFileInfo's extra Aurora field. **This is the biggest
   workstream, and most of it is port work done upstream.**
4. **PC hooks.** Examples: `CScreen::Wait`'s spin on `g_bSwap` needs
   `pcOSWaitForInterrupt()`, and dvdStatus_check's IPL-font screen must be skipped. These
   live in the port repo.
5. **Format and engine knowledge.** The script VM (bs_disasm.py) and the loaders' formats
   (`.lfc`, `.mvd`, `.skl`, `.dmf`, `.emt`, `.sin`, ...) go in docs/ as they are recovered.

Point 2 changes how to treat vtables. **GameCube vtable bytes mean nothing on PC:** clang
generates the tables from the class declarations. For the port, the 25 undefined tables are
solved by step 3's headers, not by fragment merges. Merges matter only for the GameCube
pure-source image (W1), which is the port's oracle.

## Workstreams

### W1. The pure-source GameCube image: the oracle (first)

This is a GameCube image built only from this repo's source, with no original bytes. It boots
in Dolphin and replays every route identically to the original. Once it does, any divergence
the port shows is a port bug, never a decomp bug. That is the hand-off point.

1. **DONE 2026-11-16:** source_link has 0 undefined (research/data_stopgap/vtables.cpp,
   gen_vtable_stopgap.py). The 25 undefined vtables were **decision D1**. Recommended: emit them as a marked
   stopgap. A generated `research/data/vtables.cpp` would take every slot from
   `research/layout/vtables.tsv` (the relocations give each target exactly). It would be used
   only by `source_link.py` and never by verified units. Each line is deleted once its file
   merges. This reverses the 11-12 coordinator ruling, which chose merges only. Merges still
   happen where they come for free (see W3).
2. **DONE 2026-11-17 (research/tools/pure_link.py):** a real link of the pure-source image (statics
   merged per file, duplicates weakened, .bss in each file's layout, start-up tables built).
   Fix the 45 duplicates and the fragment-local reach-ins that a real link rejects. Most are
   weak copies and file statics; source_link.tsv lists them.
3. Boot the pure-source image and replay route_menu, m1_opening and route_1_2 against the
   original. **2026-11-17:** it boots like the original and its route 1_2 trace matches into level
   1_2, then fails at the first subtitle pop-up (research/notes/pure-link.md).
4. Widen coverage. Replay every existing level route (2_1, 3_1, 4_1, 4_2, 5_1, 5_4, 6_1,
   carry_1_3; research/movies/) on the all-drafts image. Then add a short route for each level
   that has none, so every draft the campaign reaches is proven somewhere. Fix or bisect what
   diverges.

Exit: the pure-source image replays every level route identically, and every non-multiplayer
draft is proven.

### W2. Shared headers for what the port compiles (largest; in parallel with W1)

The strategy report's upstream-first rule stays: headers move here, gated by the bytes.

1. **SDK struct views first** (the port's first link blockers; DONE 2026-11-17 for the 17 surveyed files,
   research/notes/sdk-views.md). Game and EA files take their
   OS, DVD, GX, ARQ and AX structs from `include/dolphin-sdk` instead of sized byte arrays.
   Pointers passed as `unsigned long` become a pointer type or a project `uintptr_t` typedef
   that is 32-bit on GameCube, so the bytes are unchanged.
2. **A 64-bit hazard inventory** (DONE 2026-11-17: moh-frontline-pc tools/hazards64.py, pc/hazards64.tsv,
   589 rows at the pinned decomp), as a tool, not by eye. It is a port tool, so it lives in
   `moh-frontline-pc` and reads this source. It lists casts between pointers and 32-bit
   integers, structs that mirror disk formats (they must keep GameCube layout: `BE<T>` and
   offset pointers on the port side), and `sizeof`-dependent arithmetic. The fixes it leads
   to land here when they keep the bytes.
3. **Classes in port order.** Start with the boot and first-level path, ranked by how many
   files declare views (`layout_lint.py`, `research/layout/classes.tsv`). Use the
   `try_header.py` loop that CVector3/CMatrix/ISceneNode used. A file whose match breaks keeps
   a guarded view, an accepted transitional form, and is listed.

Exit: every class the port compiles has one header, the lint is clean, and the hazard list is
empty or every entry is owned by the port.

### W3. Matching, time-boxed and targeted

**2026-11-17:** for sessions the maintainer gives the decomp while the port is the main project, the
plan for matching everything left is [matching.md](matching.md) (a resumable queue sorted by kind of
mismatch). It replaces the stop rule below; the rest of this section is the 11-16 reasoning.

1. **One pass over the 37 drafts within 10 lines** (42 KB, up to about 93.6%). Use the full
   rule set from loop3-tricks and the matching-rule files. Order: drafts on the port's first
   path, then drafts whose match unblocks a merge.
   - CPartBin::Link (4 lines): dmesh's 5 tables.
   - CStaticMesh::DrawNode (5): staticmesh's 3.
   - AddAmo (2): player_weapon_object.
   - TestCDBBranchNodeCollision\<CLine3\> (1): cdbobject whole-file.
   compartment (DrawNode, 58 lines) and particlesystem (UpdateBoundingBox, 56) are not
   near-misses; their 9 tables stay in W1's stopgap.
   **Stop rule:** stop when a week of work lands under 3 KB, or when the pass ends.
2. **Over 10 lines (78.6 KB): not pursued as matching.** These drafts stay as the source. W1
   proves them. They are matched only if the port traces a bug to one, or one is on a path
   that W1 shows diverging.
3. **Multiplayer (11.1 KB):** out of scope; the PC port omits it. Its drafts stay as they are.
4. **Declined forms (D2):** UpdateTargetPosition (2.5 KB), InitMultiplayer (1.2 KB) and
   STREAM_kill (0.7 KB) are exact in a declined form. Accepting them is 4.4 KB for one
   decision.
5. **Merges** happen as files become fully exact. They also remove W1's stopgap tables one by
   one.

Realistic end state: **about 92-94% verified**, 100% source, everything else proven in play.
A literal 100% isn't the goal: 172 bytes lie outside any function, and the hardest
register-allocation misses may never match.

### W4. Port support (continuous)

- Keep `progress/GMFE69.port_manifest.json` current. It is the only file the port reads, and
  `land.sh` already regenerates it. Add a `proven` status to it per function.
- No `port` branch here. TARGET_PC hooks live in the port repo (see "Which repository holds
  what").
- Bug loop. When the port finds a divergence, replay the same input on the GameCube
  pure-source image. Same divergence: the bug is in the decomp; bisect the drafts here and fix
  the draft. No divergence: the bug is in the port.
- Write format docs as the port's loaders need them, not before.

## Sequence

1. **Now (decomp-led):** W1.1-W1.3, W2.1 and the W3.1 pass, in parallel. W2.1 matters most to
   the port; W1 matters most to trust.
2. **Hand-off (port-led), when W1 exits and W2.1 is done:** the port becomes the main project.
   Decomp work then runs on demand: W2.3 class by class as the port needs each one, W4's bug
   loop, and W3 only for drafts the port implicates.
3. **Maintenance:** the decomp keeps `main` byte-identical and its CI green, and the port
   keeps syncing it with `tools/sync_decomp.py`. Matching continues opportunistically, for
   example when a compiler rule found in one function closes others.

## Which repository holds what (maintainer, 2026-11-16)

Work done only for the PC port goes in `moh-frontline-pc`, not here. This repository keeps what
serves the decomp itself: matching, proofs, the pure-source GameCube image, shared headers that
keep the bytes, data in source, format knowledge and the port manifest. So:
- **TARGET_PC hooks** (CScreen::Wait, the dvdStatus_check skip) live in the port repo as
  overrides or patches applied to the pinned decomp. There is no `port` branch here.
- **The 64-bit hazard inventory** (W2.2) is a port tool. It reads the decomp's source and
  writes into the port repo.
- **W2.1 SDK struct views and the uintptr-style typedefs stay here.** They are source fixes
  that keep the GameCube bytes, and they help both repositories.
- **W4's bug loop:** the port finds a divergence; if the same replay diverges on the GameCube
  image, the draft is fixed here, otherwise the fix goes in the port.

## Decisions for the maintainer (answered 2026-11-16: "go with your recommendations on all of them")

- **D1: yes.** The 25 remaining vtables are emitted from a generated stopgap data file, used only
  by the pure-source link and deleted per file as merges land.
- **D2: accepted.** UpdateTargetPosition (2,516 B) and STREAM_kill (664 B) landed 2026-11-16 as
  their own fragments. InitMultiplayer can't land as a fragment (see docs/Remaining.md).
- **D3: yes.** W3.1 is a single pass over the drafts within 10 lines, stopping when a week lands
  under 3 KB.
- **D4: start the port now.** The port begins its headless build (its Phase 2) on draft source
  in parallel with W1. W2.1 (SDK struct views) is the decomp's first priority, because the port
  needs it to link.
- **AX reference (port repo, open since 11-11): option (a).** Use public format docs plus the
  SDK's CPU-side AX code. Most of Frontline's sound goes through EA's CPU mixer, so an inexact
  AX mainly affects the ADPCM banks.
