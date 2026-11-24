# Lane agents for the structural group (2026-11-24)

The structural group of research/plans/matching.md (queue rows 68-87, about 37 KB) plus the
pre-allocation scheduling rows the last run left (36, 37, 43). Four agents share the main checkout
`/home/kyle/workspace/moh-frontline`. The coordinator launches each with:

> Read research/plans/structural-agents.md and follow it as agent NAME on lane LANE.

**Everything in research/plans/matching-agents.md applies** (brief, time box, claims,
shared-checkout protocol, memory rules, landing steps, stop conditions, never deciding policy),
with these changes:

- The run directory is `scratch/loop5/` (not loop4): your directory `scratch/loop5/NAME/`,
  `scratch/loop5/CLAIMS.txt`, `scratch/loop5/TRICKS.md`. Read `scratch/loop4/TRICKS.md` too (the
  last run's rules) and grep `scratch/loop4/*/notes.md` for your functions.
- Commit log lines use `[match NAME]` as before.
- Read `research/notes/matching-rules.md` and `research/notes/matching-triage-2026-11-18.tsv`
  (the allocator verdict per row) before starting.

## Tools from the last run

`research/tools/alloc/` (README there): `tr.py` dumps MWCC's real register-allocation graph;
`nmap.py` says whether the original's colours are reachable on our graph (STUCK = the structure
or pre-allocation order differs, which is exactly what this group must change). The patched
compilers it needs still exist in `scratch/loop4/`. tr.py copies last.o to a dead scratchpad path
at its end and writes a fixed name: give your own copy a private output path before using it.

## Lanes

| NAME | Lane | Rows (in order) | Notes |
| --- | --- | --- | --- |
| sched | scheduler | 70 GetPeekLocation, 69 PublishEvent, then 36, 37, 43 | All are scheduling, not allocation. First build a scheduler probe on the tr.py model: find MWCC 1.3.2's instruction scheduler(s) in mwcceppc.exe (the post-allocation call is at VA 0x433f7e, see alloc/README.md; look for the pre-allocation one too) and dump each block's input order, dependency edges and priorities, so the order the original needs can be read off instead of guessed. GetPeekLocation: the '4' mask load (`CoverPointTypes(1) \| 2 \| 4`) is scheduled after `or(1\|2)` in ours, before it in the original; ~77 forms tried in loop3 (research/archive/loop3-scratch). PublishEvent: original loads the callback (lwz r12,12(r29)) before the slot compare. The tool goes in research/tools/alloc/ when it works. |
| small | small structural | 72 CheckLineCollision, 71 DrawNode, 68 AllocateStaticObject + 81 Init (same file, staticobjfactory.cpp; land together if needed), 73 ResetShell, 74 DMClusterSkelTraverseHeirarchy | CheckLineCollision shares GetTypes() with GetPeekLocation (lane sched): coordinate through TRICKS.md, keep the inline body identical in both. |
| mid | mid structural | 76 CreateDumpObject, 77 PROFILE_UPDATE, 79 CPathNodeList ctor, 75 SNDVOICEI_alloc, 85 BSFileReadClass | Mostly STUCK register verdicts with small blind scores: find the structural difference nmap.py points at. |
| big | non-register structural | 78 BIFunc_MechFollowPath, 80 DrawBulletStats, 82 SweepAllExtents, 83 CProfileInitClass ctor, 84 DMMorphBlendDisplayLists, then 86 RenderSystem / 87 MemoryCardBootUpPortCheck | Blind scores dominate: redraft the part that differs side by side with Ghidra (sbs.py). Split the two big ones into inline candidates and match parts first (matching.md "structural"). |

Rows 88-90 (decision) and 91-94 (multiplayer) are no one's lane.
