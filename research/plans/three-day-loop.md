# Three-day matching loop (2026-11-12 to 2026-11-14)

The maintainer is away for three 12-hour work days. Claude runs unattended on `/loop` to
match as much of the remaining code as possible, byte for byte, with four parallel lane
agents. Usage is not a concern; WSL memory is. This file is the coordinator's instructions;
the agents' brief and protocol are in `loop-agents.md`.

Start command (the maintainer runs it):

```
/loop Continue the three-day matching loop: read research/plans/three-day-loop.md and coordinate
```

## Goal

Raise the verified figure (86.19%, 1,219,272 / 1,414,572 bytes at the start) by landing
exact functions. Priority 1 first (collision, scripting, AI, animation), then priority 2,
multiplayer not at all. Every landing keeps the complete image identical.

## State (the coordinator updates this every wake-up, under the landing lock)

- 2026-11-13 ~19:00: verified 1,282,440 bytes (90.66%). Landed since 17:25: list<BuddyInfo> range ctor
  360 (44540024), buddy.cpp tail unit incl. __sinit_buddy_cpp 224 (5ed2ccfa); link 54/102/45 (3576e509).
  Rules found: BuddyState partial scalarisation (dead whole-struct read), `i * K` induction variables,
  commutative operand order (lighter operand first). Agent running: opswap (operand-order sweep).
  **Coordinator rulings for the user (reversible):** (a) the dead whole-struct read is accepted, spelled
  `inline void Check(BuddyState) {}` / `Check(state);` (a compiled-out debug check); (b) two near-identical
  helpers differing only in store order (SNDI_precalcaztospkrvol) declined, one helper must serve both
  branches (now at 4 lines); (c) CPartBin::Link `position = CVector3(lightPos);` (unnamed temporary,
  scores 0) declined as the same steering copy chain as the declined named form (draft at 4).
  Blocked: BIFunc_CreateBuddyDataStructures (0 by score) needs AddBuddy (6) in the same unit.
- 2026-11-13 ~17:25: late1/late2 matching agents on scratch/loop3/day2/late{1,2}.tsv (the 100
  unlanded drafts). Landed so far: CFontDriver::EndDraw 108 (8240e679), TRKSuppAccessFile 708
  (c37a3c4e). Verified about 1,281,856 bytes (90.62%).
- Verified: 89.20% (1,261,812 bytes) at 9da917fa, +42,540 this run. Landing commits: 71.
- **For the user, next time (decided by the coordinator, reversible):** CAnimMorph::Draw (16d5b52a)
  landed with Assign's body as `CVector3 t = s; d = t;` (the dead copy is Assign's own local,
  in the original's last stack slot); kept as the accepted Assign form, not the declined
  DoTargetCheck `Copy()`. GetObjInfo's ArgObject helper ruled an accepted single-purpose inline.
  STREAM_kill's by-reference handle declined (it would be a second body for a shared inline).
  Vtables (vt1, 91da456a): the 64 undefined global vtables are kept off on purpose by the
  fragments that define their key functions; the coordinator chose fragment merges (a unit
  owns its file's vtable bytes once the file's fragments merge), not vtables emitted from
  research/data/, since that matches the original's structure.
  Boot-path headers (hdr5): CScreen, CCamera, CInputManager, CScene, CPlayerObject landed (166
  units). Accepted: CScene.h declares VisitAllNodesInVolume only under `#ifdef GAME_ISCENENODE_H`
  as a transitional guard until the remaining CScene units take ISceneNode.h; and
  bsbifunc_playerammo's stale TriggerScriptEvent weak copy was dropped from weak_duplicates
  (CPlayerObject now overrides it).
  AddBuddy (6 lines) is exact only with x,y,z declared in a different order inside the shared
  GiveBuddyOrders inline, which breaks landed UpdateBuddyCheckpoints/Progress; all three are in
  buddy.cpp, so they must share one body (kept declined). CPartBin::Link's draft relies on a
  steering copy chain (`CVector3 tmp = lightPos; position = tmp;`): unlandable as written.
  Merges (merge3): a merged whole-file unit may keep a file-private CVector3/CMatrix view,
  marked inferred, where the shared header's copy doesn't keep the match (plan rule 10). Where
  an inline's out-of-line weak copy and its inlined copies need different local orders
  (bsutil's DoWeOwnThisMemory, world_volume's dwi::list destructor), the file stays split
  until one body serves both; no second body.
  Dolphin step 1 (dol1, e60f4d6b): input scripts and movies replay deterministically; route
  m1_opening proves 49 drafts in play. The SweepAllExtents draft got an unused volatile array to
  restore the original's 32-byte frame (stale-stack reads elsewhere depend on it); that is a
  declined device for a landing, so it marks what the landing must reproduce naturally (the
  original's dead -OSGetTick() values). RenderSystem still changes particle frames: unproven.
  (Later: dol2 fixed RenderSystem's register clobber and __sinit_compartment_cpp's crash; 79
  of 100 drafts proven.) merge6 rulings: light.cpp stays split (the only trigger that places
  CAnimLightManager's weak vtable is a discarded helper doing `delete (CAnimLightManager*)p`,
  which is a gap function with logic, still on hold). rcmp_main's unexplained 4-byte .sbss gap
  may be filled with an inferred size-view static (an object the original has, from code the
  linker removed), the accepted .bss-view form.
- **OPEN QUESTION for the user (not decided by the coordinator):** gap functions with logic.
  Two whole-file merges are fully matched except for where one weak copy is emitted, and only a
  compiled-then-stripped function between two real ones fixes it: light.cpp (a discarded helper
  that deletes a CAnimLightManager) and world_volume.cpp (a Probe() with a local dwi::list
  between ~CWorldVolume and the constructor; the original's extab label gaps suggest a real
  dead-stripped function there). Its bytes would be stripped, not left uncovered. Accept for
  weak-copy placement, or keep on hold? Drafts: scratch/loop3/merge5/lt/,
  scratch/loop3/merge11/wv/gap1.cpp.
- **Plan for 2026-11-13 onward (user, 11-13):** today (11-13) is matching again: exact matching
  is never wasted (it is the end goal and replaces drafts that may be wrong). Four agents each
  take a quarter of research/drafts/index.tsv (scratch/loop3/day2/group1-4.tsv, balanced by
  size) and re-attack every row with the full rule set found on 11-12 (research/notes/
  loop3-tricks-2026-11-12.md) and the forms accepted on 11-12 (docs/Remaining.md). After
  today, the port plan (docs/PortReadiness.md): vtables from the original, boot-path class
  headers (CScene, CPlayerObject, CCamera, CScreen, CInputManager), a pure-source link worked
  to zero undefined, then step 1 in Dolphin (input movies, frame hashes, routes from 1_1).
  The Dolphin limit is lifted (user, 11-13).
- **Paused again 2026-11-12 ~22:40.** After the 21:57 update: __sinit_pumhandler_cpp 276
  (fac4d7fa). Verified 1,280,616 bytes (90.53%), +61,344 this run. The buddy.cpp temporary
  knot (__sinit_buddy_cpp, list<BuddyInfo> range ctor) was re-studied by sinit2 and is still
  open (scratch/loop3/sinit2/notes.md: the original splits the BuddyInfo temp, keeping only its
  state part in memory with typed stores). No agent has a fresh lead; restart with the /loop
  command when one appears.
- **21:57 update:** since the resume, also landed UserInterface::Init 1,160 (349761a7),
  ConstrainVelocity 1,268 (b9f2cf08), PopUpMessageHandler ctor 988 (78daa000). Verified
  1,280,340 bytes (90.51%), +61,068 this run. One agent (sinit1, __sinit_pumhandler_cpp); other
  slots empty, no fresh lead. Rulings since the decisions are in docs/Remaining.md.
- **Resumed 2026-11-12 ~21:30** after the decisions (docs/Remaining.md, 2026-11-12 additions).
  The unlock agents landed Play 1,868, BSInitBIFuncs 1,228 (strings-only block headers),
  PATH_event 1,884, Update_Picture_Buffers 904 (getpic.cpp complete), PopUpMessageHandler::Loop
  2,308, CCDBTriangle::TestCollision(CVolCapsule) 6,920; collisionvolume_ctor onto the shared
  headers. Next leads for agents: UserInterface::Init (1,160; coordinator ruling: one real
  inline declaring `char path[20] = ""`, used by the block holder and Init, may carry the
  template; scratch/loop3/unlockB2/notes.md), ConstrainVelocity (2 lines with Mul),
  UpdateBoundingBox (56, block-2 FPR knot), the PopUpMessageHandler ctor (76, this/pool-base
  swap), CMatrix::Rotate (23 with the landed BuildRot), and any draft the new accepted forms
  help. Agents must read docs/Remaining.md "Source forms" first: forms accepted on 11-12 may now
  land.
- **Run paused by the coordinator, 2026-11-12 ~10:50.** Matching ran dry: every remaining
  draft waits on a question below or has been worked repeatedly with no new lead, and the
  secondary header work (CVector3/CMatrix/ISceneNode, 8c84b851..9da917fa) has taken every unit
  that verifies. Restart with the /loop command at the top of this file after answering
  questions; answers to q1, q5, q9, q14 and q15 alone unlock about 10.8 KB of code-exact drafts.
  Summary of the run (11-11 20:40 to 11-12 10:50): +42,540 verified bytes (86.19% -> 89.20%)
  over 67 landing commits; 17 scratch bests moved into research/drafts; 152 data symbols
  defined (undefined 197 -> 82); shared CMatrix.h (87 units), ISceneNode.h (75) and a revised
  CVector3.h (+46 units); tools: brchk.py, extab_records.py, port_unit kept_span fix; rules found
  (scratch/loop3/TRICKS.md): shared zero, member destructors in ctors, real CVector3 members,
  stack-slot inline groups, callee rank; independent audit in
  research/notes/loop-audit-2026-11-12.md with its free fixes applied. Usage limit stopped all
  agents 00:10-00:40 and 03:50-05:40.
- **Decided 2026-11-12 (maintainer: "I will go with whatever you recommend"):** questions
  1, 2, 4, 5, 6, 10, 12, 13, 14, 15, 16 and 17 accepted; 3, 7, 8, 11 and the PlayerScreenFlash
  part of 16 declined; 9 accepted narrowly (strings-only block headers). Recorded in
  docs/Remaining.md "Source forms". The original questions follow for the record.
- Questions as asked:
  1. **`.bss` size views.** A fragment defines its file's `.bss` objects in original order
     with exact sizes and ports with `--pool .bss`: plain views, container views with their
     constructors removed, and 12-byte placeholders for the compiler's destructor-chain
     records, all marked inferred, no `__sinit` compiled. Not listed in docs/Remaining.md;
     adds no storage the original lacks. Find them: `git log --grep '.bss pooled'`.
  2. **Hand-trimmed manifests: largely answered by tooling.** 9d94f2c9 (agent tool1) makes
     port_unit decide extab/extabindex ownership from relocations, so it now drafts the trims
     agents had made by hand (8 of the 9 hand-trimmed units regenerate exactly; 102 of 105
     changed drafts across all units match their committed manifests). Left for you: three
     manifests (scene_cmppairs, impact_dtor, AIObject_visionseek) list a trailing dropped
     record in stripped_objects where the tool trims it; same bytes, style only. attachobject_ctor
     and matstack can't be regenerated from their manifest arguments (unrelated, pre-existing).
  3. **Dead branch inside a real function** to place a string the layout proves was created
     there (TLT_LoadLevelContentsFile, 1,060 bytes, code exact). Parked; not used.
  4. **Pointer-cast field spelling** for CStaticObjectFactory::Init's variant motion name:
     `((const char**)&((LevelFileInfo*)def)->fields[i * 3 + 7])[1]` with a LevelField view
     (as in AnimObjFactory_head.cpp), replacing the declined `(char*)def + (i*24 + 0x38)`.
     Plain member spellings lose the original's separate +4 induction variable. Init is
     parked on registers anyway; this only matters if they are solved.
  5. **`#pragma auto_inline off` / `reset`** around PATH_pause in `src/ea/path_stop.c`. With
     it PATH_event (1,884 bytes) matches, plus all 6 other functions in the merged unit
     (scratch/loop3/rs2/pev/PATH_event_unit_autoinline.c). The accepted `dont_inline` also
     stops selectstate inlining inside PATH_pause (83 lines off). Not on the accepted list
     (which names dont_inline in game code), so parked, ready to land on a yes.
  6. **Data step 2 (agent data1: d5d1282e, 18f0a9a4, d0f84b89; undefined 197 -> 82).**
     Review: files inferred from section neighbours (marked); MPD/gFileSysOpts/FONTcurrentdriverext
     put in init/unassigned.cpp against the map's guess; six plain MP float constants included
     (drop them if multiplayer-last should cover data too). Related: e959464b landed
     CShellMenu::MoveStatsIntoProperAreaForIstudio (236), which packs multiplayer stats for
     iStudio but is queued under Shell/HUD; should `_eti_init_info` and the
     vtables stay out of step 2; should research/data/init/ merge with bss/ later.
  7. **Placement-new knot** (CreateRootHierObject 3,416 B at 1 line, CreateDestructable at 1):
     CW always null-checks placement new and keeps a redundant beq; no natural form gives both
     the original's `mr.` and the register ranking (scratch/loop3/pnew1/notes.md). The only
     untried form writes the two vtable pointers by hand after `if (box)`. The coordinator
     expects that to be declined (code written only to steer output), so it was not tried.
  8. **Per-fragment inline views that differ.** CFont::DrawTextCentered (416 B) is code exact
     only if its fragment's view of the inline DrawText declares scaleY before scaleX, while the
     landed DrawTextCenteredF and font_dtor need scaleX first (scratch/loop3/sweep1/
     DTC_BEST0.cpp). FlagAsOccupied has the same shape. Treated as the declined "duplicate
     inline" form, so not landed. Also: late1's SweepExtents writes an inline's loop out in the
     caller; the coordinator accepted that as ordinary source (adds nothing).
  9. **Gap functions (on HOLD) now gate code-exact work:** OPTION_PARSING_PLAYER::Play (1,868)
     and UserInterface::Init (1,160) are code exact (drafts 39b69cd3) but their `...rodata.0`
     offsets run past strings only stripped functions used. BSInitBIFuncs (1,228) is the same,
     with an unlanded multiplayer function in the head. The PopUpMessageHandler constructor
     (structural 1, relocation only) is the same: 188 bytes of unreferenced templates ahead.
  10. **Vector locals kept in memory.** CParticleSystem::RenderSystem (4,532; structural
     1069 -> 943, every stack slot right) and UpdateBoundingBox need 18 vector locals and a
     matrix kept in memory with dead stores. MWCC 1.3.2 only does that when a named pointer or
     reference is bound to them, an alias form that was declined. Is there an accepted form?
     vec1 found one that adds no code: a member `float& operator[](int)` used once on the local
     (and `float* operator[](int)` on an inferred 3x3 view class, `CMatrix33`). The coordinator
     let agents use it (RenderSystem structural 943 -> 644 alias-free); confirm, and whether an
     inferred 3x3 class is acceptable if Rising Sun has no real name for it.
  11. **Dead stack copies the original visibly keeps.** DoTargetCheck is 2 lines off with a
     natural form (bool return, attested by bsbifunc_schedules.cpp). The rest is two dead copies
     of the other node's position. They survive only through a declined identity `Copy()` or a
     constructor that writes through a cast of `this` (which changes landed AIFilter_queries).
     Rising Sun has the same copies from a dead `a = otherPosition`. Is there an accepted form?
  12. **Duplicated condition, landed (fc2de588).** DetermineNewObjectivePosition (1,276) matches
     only with IsClear testing the height-1 distance twice (`d < a[1] && d < a[1] && ...`),
     following earlier landed redundant tests (GiveEnemyGrenadeWeapon, AddBulletDecal). Revert
     fc2de588 if you see it as code written only to steer output. Same class, not landed yet:
     GetClosestWaypointByType's draft (structural 0, d5baeeb8) re-zeroes `found = 0;` after a
     loop to get the original's second `li r3,0`.
  13. **Constant chains.** Does the chained-assignment decline cover `a = b = c = 3.4e38f`?
     The DMClusterSkelTraverseHeirarchy draft uses one; it is treated as declined until you say.
  14. **Audit of this run's landings: research/notes/loop-audit-2026-11-12.md.** An
     independent reviewer rates 7 landings likely declined and 7 needing a look. The
     load-bearing ones are coordinator approvals it disagrees with: the second WriteVCD copy in
     CCompartment::Init (2,628), the Mul/SubV/AddV value wrappers (DrawTextCenteredF,
     CVolCapsule::GetExtents), DumpBuddyVector in BuddyDied, debug_prof's `(void)0`, and local
     CVector3 views that the shared header would also match. Its "not needed" items were
     cleaned up during the run (agent clean1). From 06:40 on 11-12, agents no longer land forms
     of the wrapper and duplicate-inline kind. natural1 replaced three load-bearing forms with
     natural ones (1976c05b: GetPalette inline, `bin = new (bin) CCPTMatBin(base)`, which you
     may still read as a write-back; cf37912d: debug_prof's `(void)0` gone). Still load-bearing:
     WriteSwapVCD (4 lines without), DumpBuddyVector (28), Mul (2); SubV/AddV removed by undo1
     (bac20c93). knots3 found
     the floating-point knots left in the geometry drafts (GetExtents, Orthonormalize,
     ConstrainVelocity, the capsule tests) are all products that only a value-returning inline's
     return temp reproduces; plain locals, in-place statements and getters colour differently.
     So the original probably did use small value-returning inlines there; the open part is
     only their names and signatures. Held back under that rule: Update_Picture_Buffers
     (904 B) is code exact with an invented `third_plane(height, width, data)` inline whose
     parameter order sets the registers; on a yes, getpic.cpp becomes one complete unit (recipe
     in scratch/loop3/knots1/notes.md, draft research/drafts/Update_Picture_Buffers.80100a30.cpp).
  15. **Empty switch case.** PopUpMessageHandler::Loop (2,308 B) is code exact (draft, 7dcdeb04)
     except that the original keeps a separate empty `case 8`. Only the identity statement
     `message = (PopUpMessage*)(void*)message;` reproduces it; every empty form drops one
     instruction. Acceptable, or is there an accepted form?
  16. **Inline helpers read from stack-slot groups.** SoundSchedule::Update (1,864, a842943c)
     landed with five single-use inlines (NodeAzimuth/NodeDistance, PositionAzimuth/
     PositionDistance, ObjectSpeed). Their boundaries are read from the original's stack-slot
     groups (one block per inline expansion), so they reflect real structure, but the names are
     inferred and the Node*/Position* pairs have similar bodies with different inputs. Also: is
     the `GiveBuddyOrders(vec_iter&)` forwarding overload (needed by GetInitialBuddyOrders, 7
     lines) an inline helper or a forwarding wrapper? Also inferred: `dwi::min` (reference-
     returning, body `a < b ? a : b`) in GetClosestParameter (1,056, 1b0b4dc8); the original's
     pointer select between two stack slots is what that inline compiles to. And PlayerScreenFlash goes 39 -> 34 with a
     CColor `operator=` taking its argument by value (slots1 judged it not natural; uncommitted).
  17. **Shared headers (secondary work, research/notes/shared-headers.md).** CMatrix.h and
     ISceneNode.h landed (8881a8a4, 306aa460, b9bf12de, ac0e8fe7, 4e927077): 53 + 3 units on
     CMatrix, 12 + 53 on ISceneNode, complete image identical. The coordinator ruled that the
     weak copies of ISceneNode's inline virtuals go in `weak_duplicates` (every original unit that
     called them emitted them too). Open: collisionvolume_ctor is ready but would break
     tests/test_data_strip.py's `['.data']` fixture assertion; player_ctor's 8-aligned matrices
     vs CMatrix's 16-byte alignment; the GetTranslation/SetAxes helpers. cv1 (d8cf803c) changed
     CVector3.h's copy constructor to a base-class form (`CVector3 : CVector3Base`, aligned(8)):
     one header then fits both groups of units (166 of 168 kept, 42 more moved, all 210 verify).
     The base-class spelling is this project's choice; the original may have spelled it
     differently. Two units changed source (not code) to take the header: AIObject_mmgpoint and
     bsbifunc_waypointinfo.
- **Code exact, waiting on an answer above (bytes):** PATH_event 1,884 (q5), Play 1,868 (q9),
  BSInitBIFuncs 1,228 (q9), UserInterface::Init 1,160 (q9), TLT_LoadLevelContentsFile 1,060
  (q3), Update_Picture_Buffers 904 (q14), DrawTextCentered 416 (q8), PopUpMessageHandler::Loop 2,308 (q15). Total 10,828.
## Hard rules (for the coordinator and every agent)

1. **Land only with `research/tools/land.sh`**, holding `research/tools/landlock.sh`. It
   runs the complete build, snapshot, port manifest, doc sync and tests, then commits.
   On failure, revert and release; never commit around it.
2. **Push after every successful landing** (inside the lock), so a WSL update or crash loses
   nothing verified. Experiments stay in `scratch/` and are never pushed.
3. **Source forms**: only what docs/Remaining.md accepts. Declined forms are never landed,
   even when they match. Gap functions stay on HOLD (ShellMenu.cpp pilot only). Don't invent
   types, names or file boundaries; mark inferred views in comments.
4. **No Dolphin runs. No workflows.** Exactly four lane agents (below), no more.
5. **Memory.** WSL has 10 GB + 8 GB swap. Untested scripts run under `ulimit -v` and
   `timeout`; permuters at most 2 jobs each; one Ghidra JVM at a time (the MCP server
   already runs one); nothing heavy starts while `free -m` shows under 2,500 MB available.
   A full `tools/reconstruct.py` takes about 2 minutes and runs only under the lock.
6. **Git.** No reset, rebase, amend or force push. Never touch `orig/` or the disc image.
7. **Time box.** About 45 minutes per function, then park it in the lane's notes.
8. **History first.** grep a function in `research/log/*` and `research/notes/*` before trying it.
9. **Keep the repo current.** Each landing's commit carries its log line, its draft removal
   and doc sync (land.sh runs sync_docs).
10. **Shared header.** New units use `game/CVector3.h` when that keeps the match; otherwise
    a local view is fine.

## Lanes and queue

`research/plans/loop_queue.tsv` ranks all 170 unfinished functions: priority, code-exact
drafts, fewest differing lines (best scores from `scratch/solo/gb/real.tsv`, 2026-11-08),
size. Rows with a `note` are blocked (declined form, buddy.cpp knot, user said not now);
nobody works them.

| Agent | Area | Functions | Bytes |
| --- | --- | ---: | ---: |
| geometry | Collision, Animation | 27 | 40.9 KB |
| scripting | Scripting and level objects | 46 | 57.5 KB |
| airender | AI, Rendering | 40 | 38.1 KB |
| systems | Audio, Shell/HUD, System, Video, Other; first the code-exact drafts | 48 | 42.8 KB |

(Unblocked functions only; the 4 multiplayer functions belong to no lane.)

When a lane runs dry, its next agent takes the unclaimed tail of the largest remaining lane.
When every lane is dry or parked: secondary work that touches no verified source
(initialised data tables for the 181 undefined data symbols, `source_link.py --list
undefined`; `layout_lint.py --reuse`), one agent at a time.

## Coordinator procedure (each wake-up)

1. **Memory.** `free -m`. Under 2,500 MB available: SendMessage every agent to stop its
   permuter and pause heavy work. Under 1,500 MB: find the biggest process (`ps -eo
   pid,rss,cmd --sort=-rss | head`) and stop it (a runaway script or permuter: kill its PID;
   an agent: TaskStop, then relaunch it later).
2. **Lock.** `sh research/tools/landlock.sh status`. Held more than 30 minutes by an agent
   that has finished or died: check `git status`; if the tree is clean and its commit exists,
   remove `scratch/loop3/LANDLOCK`; if it left partial changes, revert only its files.
3. **Agents.** Any lane without a running agent gets a fresh one (Agent tool, background,
   general-purpose): "Read research/plans/loop-agents.md and follow it as agent NAME on lane
   LANE." An agent stopped by a usage limit is resumed with SendMessage after the reset
   rather than relaunched. A lane whose last two agents landed nothing and improved nothing
   is retired; its agent slot moves to the largest remaining lane.
4. **State.** Under the lock: update the State section above (figure from the latest
   land.sh, landings per lane, parks, questions), commit it, push, release.
5. **Schedule.** `ScheduleWakeup` 1,800 s. Agent completions wake the coordinator earlier.

## Stop and wait (don't push through these)

Stop launching agents and write the reason at the top of State if:
- the complete build stops being identical and a revert doesn't fix it;
- tests fail for a reason unrelated to any landing;
- memory runs out twice in a row despite the limits;
- an agent reports a policy question whose answer blocks its whole lane (park the lane,
  keep the others going).

## End of run

On the last wake-up (or when the maintainer returns): ask agents to wrap up, then write a
summary at the top of State: landings with bytes and commits, the new figure, parks with
reasons, and the open questions.
