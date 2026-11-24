# Matching plan: the last 94 functions (2026-11-17)

This is the plan for matching everything that is left, in occasional sessions while the PC port is
the main project. It replaces the stop rule of endgame.md's W3 (one time-boxed pass): when the
maintainer gives the decomp a session, the session works this queue. Port requests still come
first (endgame.md W4: a draft the port implicates jumps the queue).

## When and how far (maintainer, 2026-11-17)

This plan is for the spare decomp sessions once the PC port is the main project (after W1 and the
rename tool). The milestone is the register, order and near groups: about 79 KB, up to about 96.5%.
When that milestone is reached, or the queue stops yielding, stop and reassess with the maintainer
before going on to the structural group and the decisions. Expect steady small gains rather than a
quick jump: nearly every row has earlier attempts, so the lever is new compiler rules applied
across a whole group.

## Where it stands

Verified: 1,285,620 / 1,414,572 code bytes (90.88%). Left: 94 functions, about 129 KB, all with a
draft in `research/drafts/` (index.tsv). The drafts already link into the pure-source image (two
static initialisers compile only inside their file); this plan is about making them byte-exact.

`research/tools/mismatch_kinds.py` compiles every draft and sorts it by what kind of mismatch is
left (the measurement is in `matching_kinds.tsv`; run it again at the start of any session where
drafts changed). With registers blanked out:

| Group | Functions | KB | What's wrong | First tools |
| --- | ---: | ---: | --- | --- |
| not_code | 4 | 1.9 | nothing in the code; blocked by where it can land | merges, unit layout |
| register | 29 | 45.3 | identical instructions, other registers | varmap.py, declaration order, const copies, permuter |
| order | 14 | 12.4 | the same instructions in another order | statement order, inline boundaries, opswap tools |
| near | 20 | 21.1 | at most 4 instructions extra and 4 missing | one expression form, a copy, a branch form |
| structural | 23 | 36.7 | more than that | redraft the part that differs from Ghidra + sbs.py |
| multiplayer | 4 | 8.2 | (by name; last, after everything else) | |

The register group is the surprise: 45 KB of drafts already have the original's instructions,
including TestCDBVolBox::TestCollision\<CTriangle\> (3.9 KB, 449 lines that differ only by register)
and BIFunc_GetObjInfo (6.2 KB, 16 lines). They need the register-allocation rules, nothing else.

## The queue: `matching_queue.tsv`

One row per function, in working order: group (cheapest first), then by differing lines. Columns:
n, group, kind, size, real (differing lines), blind (differing lines with registers blanked),
status, function, draft, note. Statuses:

- `open`: not tried under this plan.
- `parked`: tried and stopped; the note says what was tried, the best score and where the work is.
- `decision`: the only known form is declined (docs/Remaining.md "Declined"), or the user set it
  aside. Not worked on until the maintainer decides; see "Decisions needed" below.
- `landed`: verified and pushed (commit in the note).

Before trying a function, grep the log and notes for it (`research/log/autopilot.md`,
`research/notes/loop3-tricks-2026-11-12.md`, the draft's own header comment): most have earlier
attempts, and repeating one wastes the session.

## How a session runs

1. Read the State section below. `git pull`; run `research/tools/mismatch_kinds.py` if drafts
   changed since the last run.
2. Take the first `open` row (or a port-implicated draft, which comes first). For a cluster, take
   the whole cluster (see Clusters).
3. Time box: about 90 minutes per function. No landing by then: set it `parked`, write what was
   tried, the best score and the scratch path in the note, and take the next row. A function parked
   twice drops to the end of its group until a new rule turns up.
4. Land with `research/tools/land.sh` (full verify), never a partial. Update the draft index, the
   queue row and the State section in the same commit, and push.
5. **When a landing comes from a rule that is new** (a compiler behaviour, not a one-off spelling),
   write it in `research/notes/matching-rules.md` (create it on first use) and try it on every
   other row of the same group before going on. This is where most progress has come from: one
   rule closing several functions.
6. End of session: write two lines in State (what landed, what's next).

Use `ninja -j4`, `ulimit -v` and one Dolphin at a time, as always.

## Per group

**not_code (n 1-4).** No instruction differs; they can't land where they are:
- The rcmpbase.cpp tail: RCMP_SYSTEM's weak destructor and `__sinit_rcmpbase_cpp` must join
  rcmpbase.cpp together, after the RCMP::DECODER constructor (row 34, order group, 2 lines: the
  vtable address and the zero swap r5 and r6). Do row 34 first; the three then land as one.
- BIFunc_CreateBuddyDataStructures: matches by score, but as its own fragment the pooled .rodata
  base loads before .bss. It lands inside a unit with an earlier .bss user, e.g. with AddBuddy
  (row 13).
- `__sinit_compartment_cpp`: compiles only inside compartment.cpp; lands with that file's merge.

**register (29).** Start every one with `varmap.py` (which named variable sits in which
register) and the declaration-order rule: earlier-declared callee-saved locals get the higher
register in 85-91% of pairs. Then the levers in loop3-tricks: a value-returning inline written
directly, a const named copy to break a tie, inline boundaries read from stack slots, the
callee-register rank. Then the permuter (scratch/perm, autoperm.py) overnight on what's left.
Big ones (TestCollision\<CTriangle\> 449 lines, AnimTranDoTransition 177, PATHI_eventstart 160) may be
one variable spilling everything after it: fix the first differing register and score again.

**order (14).** Scheduling and block layout: statement order, where an inline begins and ends,
loop shape (`for` vs `while`, the test at the top or bottom), `if` vs `?:`. The opswap tools
(scratch/loop3/opswap) sweep operand swaps. `_matrix_rot_yzx` (77 lines) is probably one loop
shape.

**near (20).** One construct differs. Diff the blanked sequences (mismatch_kinds.py's rows) to
find it, then try the spellings the rule files list for that construct.

**structural (23).** Redraft the part that differs, side by side with Ghidra
(`sbs.py`), not the whole function. The largest (MemoryCardBootUpPortCheck 5 KB,
RenderSystem 4.5 KB) are best split: match the function's parts as separate inline candidates
first.

**multiplayer (4).** After everything else (the user's rule: multiplayer last).

## Clusters (land together)

- rcmpbase.cpp tail: rows 34, 1, 2.
- buddy.cpp: AddBuddy, BIFunc_CreateBuddyDataStructures, BIFunc_AddBuddy,
  BIFunc_GetInitialBuddyOrders (the BuddyInfo partial-scalar knot; research/notes/buddy.md).
- Whole-file merges become possible as files go exact; each also removes a table from W1's
  vtable stopgap (research/data_stopgap).

## Decisions needed (status `decision`)

These rows' only known matching forms use devices the maintainer declined (docs/Remaining.md):
FlagAsOccupied and DrawTextCentered (a different inline body per fragment), TLT_LoadLevelContentsFile
(a dead branch placing a string), DoTargetCheck (an identity Copy()), BIFunc_PlayerScreenFlash (a
by-value operator= to move slots). GenerateTestSplinePath was set aside by the user (2026-11-02).
Each gets a fresh try for an accepted form first. If none is found, they are collected and asked
about together, once, near the end; not one at a time.

## What 100% needs beyond the queue

- 172 bytes lie outside any function (alignment and padding): they come with the merges.
- Everything here verified, then the files whose fragments are all exact merged into whole
  units.
- Realistic: the register and order groups and most of near are within reach (about 79 KB, up to
  about 96.5%). Structural and the decisions decide the rest.

## State

- 2026-11-17: plan written; queue seeded from mismatch_kinds.py (4 not_code, 29 register, 14 order,
  20 near, 23 structural, 4 multiplayer; 6 rows need a decision). Next session in this repo:
  W1 and the rename tool first (overnight-2026-11-17.md Decisions), then this queue from row 34
  (the rcmpbase tail).
- 2026-11-18: parallel matching run (research/plans/matching-agents.md, 2026-11-17 20:45 to
  2026-11-18 14:20, a usage-limit stall 01:50-13:42): 90.88% -> 92.10%, 21 functions landed
  (rcmpbase.cpp and buddy.cpp whole-file merges, decal.cpp merge, CreateRootHierObject, the
  animst pair, CInputManager::Update, quick_sort_, CSoldierObject::Draw, ...). The lever was
  reading MWCC's register allocator itself (research/tools/alloc/, rules in matching-rules.md).
  Every other register/order/near row is now `parked`: nmap.py shows 18 of 54 open rows STUCK
  (the original's colours are impossible on our graph: pre-allocation order or structure
  differs), 28 not a colouring difference, and the few feasible ones blocked by known
  constraints. Next: reassess with the maintainer (structural group, the pre-allocation
  scheduler probe for rows 36/37/43, the queued policy questions).
- 2026-11-24: structural run (research/plans/structural-agents.md, 4 lanes, 11 agents): 92.10% ->
  92.38% (1,306,824), 5 landed: GetPeekLocation 740, SFILTER_rsf 780, CProfileInitClass ctor 836
  (debug_prof.cpp tail merge + gap function), CheckLineCollision 1,184, BIFunc_MechFollowPath 456.
  New exact tools in research/tools/alloc/: sched.py/ssim.py/post.py (both scheduler passes),
  sim2.py/fit.py/fitF*.py (simplify + index fitter). Drafts improved: BSFileReadClass 209,
  MemoryCardBootUpPortCheck 197, DrawBulletStats 37, DMClusterSkel 6, DMMorph 222. Every other
  structural row parked with the simulators' target in scratch/loop5/<lane>/notes.md. Leads: the
  copy-coalescing rule (temps-only merge, 0x57a1f0) swept over the parked register rows; the IR
  dead-code pass for SweepAllExtents; RenderSystem split into inlines.
