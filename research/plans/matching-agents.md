# Lane agents for the matching milestone (2026-11-17)

The milestone is matching.md's register, order and near groups (about 79 KB, up to about 96.5%).
Four agents share the main checkout `/home/kyle/workspace/moh-frontline`, one per lane. The
coordinator (a `/loop` session) launches each with:

> Read research/plans/matching-agents.md and follow it as agent NAME on lane LANE.

The shared-checkout protocol is the one the three-day loop used (`loop-agents.md`, sections
"Claims" and "Shared-checkout protocol"), with the paths below. It is repeated here so this
file stands alone.

## Brief (read in this order)

1. This file, all of it.
2. `research/plans/matching.md`: the plan, its per-group levers, clusters and decisions.
3. `scratch/loop4/NAME/notes.md`: your predecessor's handoff (drafts, scores, parks). Continue
   from it. Create it if missing.
4. `scratch/loop4/TRICKS.md` (cross-lane rules found in this run; append yours),
   `research/notes/matching-rules.md` (if it exists), `research/notes/loop3-tricks-2026-11-12.md`
   (grep it, it is long), `research/notes/matching-tricks.md`, `research/notes/register-tools.md`,
   and `docs/Remaining.md` (accepted and declined forms).
5. Your lane's rows of `research/plans/matching_queue.tsv` (see Lanes). Before working a function,
   claim it (below) and grep its name in `research/log/autopilot.md`, `research/notes/*` and its
   draft's header comment: most rows have earlier attempts; don't repeat them.

Work your lane in row order, skipping `decision` and `landed` rows. Start from the draft in
`research/drafts/` (copy it into your directory); `research/tools/check_draft.py <name>` tells you
whether a change kept the logic. Time box: about 90 minutes per function (matching.md step 3).
Stuck -> park it in your notes.md (draft path, best score, what was tried) and move on.

**New rules are the lever.** When a function closes because of a compiler behaviour (not a
one-off spelling), write the rule in `scratch/loop4/TRICKS.md` at once and try it on the other
rows of your lane before going on. Read TRICKS.md again whenever you start a new row: another lane
may have found the rule your row needs.

Keep your context lean: pipe long output through head/tail/grep, never cat asm dumps or big
files whole. Stop after about 3 hours of work, or 6 landings, or a full hour with no landing and
no improved draft, whichever comes first: update notes.md, release anything you hold, kill your
permuter processes, and end with a final message listing landings (commit hashes and bytes),
best WIPs (paths, real/blind scores), parks with reasons, new rules and any policy questions.
The coordinator then starts a fresh agent on your lane, which reads your notes.

## Lanes

| NAME | Queue rows | Notes |
| --- | --- | --- |
| regA | register rows 5-18, plus the buddy cluster (3, 24) | buddy: research/notes/buddy.md; AddBuddy (13) unblocks CreateBuddyDataStructures (3), they land together |
| regB | register rows 19-32 (not 24) | the big ones (27-32) may be one variable spilling: fix the first differing register and rescore |
| order | row 34 first, then rows 1-2 (the rcmpbase.cpp tail lands as one), then 35-46 | `_matrix_rot_yzx` is probably one loop shape; opswap tools in scratch/loop3/opswap |
| near | rows 48-66 | diff the blanked sequences to find the one construct |

Structural (68-90), multiplayer (91-94) and `__sinit_compartment_cpp` (4) are no one's lane in
this run. A lane that runs dry stops; the coordinator decides what it does next.

## Claims

Append `NAME function` to `scratch/loop4/CLAIMS.txt` before working a function; skip
functions someone else claimed. A whole-file merge claims every function of the file it lands.

## Shared-checkout protocol

**Workspace.** Your private directory is `scratch/loop4/NAME/`. All drafts, probes, permuter runs
and notes go there. Outside the lock, NEVER write to `config/`, `src/`, `include/`, `docs/`,
`tools/`, `tests/`, `progress/`, `research/` or `build/`, and never run `tools/port_unit.py`,
`tools/reconstruct.py`, `research/tools/one_unit.py`, `research/tools/land.sh`,
`research/tools/addunit.py`, ninja or `scratch/game/try.sh`. Safe outside the lock (temp dirs
only): `research/tools/check_draft.py`, `score_draft.py`, `sbs.py`, `varmap.py`,
`research/tools/mismatch_kinds.py` on single drafts, `scratch/fic/ss.py`, `scratch/coll/sbs.py`,
`scratch/game/fast.py`, `scratch/perm/autoperm.py`, the Ghidra MCP tools. If a tool turns out to
write outside a temp dir, stop using it outside the lock and note it in TRICKS.md.

**Memory (WSL has 10 GB + 8 GB swap, shared by everything).**
- Every new or untested script: `(ulimit -v 3000000; timeout 900 python3 ...)`.
- Permuter: at most 2 jobs, one permuter per agent at a time; stop it when you park.
- Ghidra: use the running Ghidra MCP tools. Don't start a headless Ghidra/JVM.
- Before anything heavy, check `free -m`: if "available" is under 2500 MB, wait.
- Never `pkill -f`; use `pgrep` and `kill <pid>`.
- Builds: `ninja -j4` at most (only under the lock).

**Landing (one agent at a time).**
1. Finish matching first, then `sh research/tools/landlock.sh acquire NAME` (waits up to 20 min).
   After a release, wait about 60 s before acquiring again so other agents get a turn.
2. `git status --porcelain` must show nothing but your own work. If someone else's files are
   there, release and report it; don't touch them.
3. `tools/port_unit.py ... --category reconstructed_game` (game code), check
   `research/tools/poolvals.py <unit>`, `research/tools/addunit.py <unit>`,
   `python3 research/tools/one_unit.py <unit>` (read the text: it exits 0 even on FAIL).
4. Remove the landed function's draft and its `research/drafts/index.tsv` row; set its row in
   `research/plans/matching_queue.tsv` to `landed` with the trick in the note; append one line to
   `research/log/autopilot.md` (date, `[match NAME]`, function, bytes, trick). A new compiler rule
   also goes into `research/notes/matching-rules.md` (create it on first use) in the same commit.
5. `sh research/tools/land.sh "<subject>" "<body>"`, then `git push`.
6. `sh research/tools/landlock.sh release NAME`. ALWAYS release, even on failure: first revert
   your partial changes (`git checkout -- <your files>`, remove your untracked files).

Parking a row needs no lock: keep it in your notes.md; the coordinator copies parks into the
queue's `parked` status at quiet moments.

Keep work in progress under scratch/ until you land it: land.sh adds all of research/, so
untracked research/ files are swept into whoever lands next. Never amend, rebase, reset or
force-push. Commit only through land.sh. Never decide policy: a function that needs a declined
form (docs/Remaining.md "Declined") or a gap function is parked with a question.
