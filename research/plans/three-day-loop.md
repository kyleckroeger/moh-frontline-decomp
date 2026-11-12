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

- Verified: 88.08% (1,245,996 bytes) at 39b69cd3, +26,724 this run. Landing commits: 31.
- Lanes (2026-11-12 01:12): geometry, airender and systems lanes dry;
  all four slots now run cross-lane work: scripting 9th agent (MechFollowPath, session-8
  tricks), late2 (SweepExtents 7, AIStartCoverPointWalkToStored 3), struct1 (far-off
  non-scripting drafts), sweep2 (Mul/FPR tricks on geometry knots). Usage limit stopped all
  agents 00:10-00:40 on 11-12; resumed with SendMessage. Largest landing:
  TriggerEnterAndLeaveMessages, 3,480 (72ffb0fc). Data step 2 by data1: undefined 197 -> 82.
- Waiting on the maintainer (agents continue meanwhile; all reversible):
  1. **`.bss` size views.** A fragment defines its file's `.bss` objects in original order
     with exact sizes and ports with `--pool .bss`: plain views, container views with their
     constructors removed, and 12-byte placeholders for the compiler's destructor-chain
     records, all marked inferred, no `__sinit` compiled. Not listed in docs/Remaining.md;
     adds no storage the original lacks. Find them: `git log --grep '.bss pooled'`.
  2. **Hand-trimmed manifests.** `buddy_destroy.json` and `debug_prof_register.json` had
     their extab/extabindex slices cut by hand where port_unit kept foreign records (5ef1c047
     since fixes one case in port_unit). Both pass the full build.
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
     (drop them if multiplayer-last should cover data too); should `_eti_init_info` and the
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
