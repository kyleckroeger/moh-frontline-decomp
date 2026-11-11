# Three-day matching loop (2026-11-12 to 2026-11-14)

The maintainer is away for three 12-hour work days. Claude runs unattended on `/loop` to
match as much of the remaining code as possible, byte for byte. This file is the loop's
instructions: read it at the start of every iteration, and keep **State** current.

Start command (the maintainer runs it):

```
/loop Continue the three-day matching loop: read research/plans/three-day-loop.md and work the queue
```

## Goal

Raise the verified figure (86.19%, 1,219,272 / 1,414,572 bytes at the start) by landing
exact functions. Priority 1 first (collision, scripting, AI, animation), then priority 2,
multiplayer last. Every landing keeps the complete image identical.

## State (the loop updates this after every landing or park)

- Verified: 86.19% (1,219,272 bytes) at 7c716368. Landings this run: 0.
- Current item: none yet; start with Phase A.
- Last push: 7c716368.

## Hard rules

1. **Land only with `research/tools/land.sh "<subject>" "<body>"`.** It runs the complete
   build, the snapshot, the port manifest, doc sync and the tests, then commits. If it fails,
   fix or revert; never commit around it.
2. **Push after every successful landing** (`git push`), so a WSL update or crash loses
   nothing that was verified. Push nothing else: experiments stay in `scratch/`.
3. **Source forms.** Only those docs/Remaining.md accepts. Declined forms (storage or code
   added only to steer registers, aliases, identity wrappers, invented functions) are never
   landed, even if they match. Gap functions stay on HOLD (ShellMenu.cpp pilot only). Don't
   invent types, names or file boundaries; mark inferred views in comments.
4. **No Dolphin runs, no parallel agents, no workflows.** Static checks only.
5. **Resources.** Run untested scripts under `ulimit -v 6000000` and `timeout`. Build with
   limited parallelism (`--jobs 4`, `ninja -j4`). A full `tools/reconstruct.py` takes about
   2 minutes; don't run two at once.
6. **Git.** No destructive commands (no reset --hard, no force push, no rebase). Never
   touch `orig/` or the disc image. Commits carry the Co-Authored-By trailer (land.sh adds it).
7. **Time box.** At most two iterations (about 45 minutes) on one function. Then park it:
   one log line with the draft path, best score and what was tried, and move on.
8. **Search history first.** Before working a function, `grep -n <name> research/log/*
   research/notes/*`. Many have had several attempts; don't repeat one.
9. **Keep the repo current** (docs-stay-current): each landing's commit carries its log line
   in `research/log/autopilot.md`, its `research/drafts/` cleanup (remove the draft and its
   index.tsv row once the function is in a unit), and doc sync.
10. **The shared header.** New units that use CVector3 should include `game/CVector3.h`
    when that keeps the match; otherwise a local view is fine (see PortReadiness step 3).

## Method for one function

1. Read its draft (`research/plans/loop_queue.tsv` columns `best_draft`, `draft`) and the
   history (rule 8).
2. `python3 research/tools/check_draft.py <name>` for the logic status;
   `score_draft.py` / `sbs.py` for differing lines; `varmap.py` for register colouring.
3. Try the tricks in `research/notes/matching-tricks.md` and `research/notes/register-tools.md`
   first: declaration order, inline depth, accessor inlines, const copies.
4. When the function matches in isolation, make it a unit (`tools/port_unit.py`, pass
   `--category reconstructed_game` for game code; check pool values with `poolvals.py`),
   register it (`addunit.py`), verify it (`one_unit.py <unit>`), then land with land.sh.
5. If it can't match without a declined form or a decision, park it (rule 7) and note why.

## Queue

`research/plans/loop_queue.tsv` ranks all 170 unfinished functions: priority first, then
code-exact drafts, then fewest differing lines (best score from `scratch/solo/gb/real.tsv`,
2026-11-08), then size. The 7 known blockers are at the end with a note; skip them.

### Phase A (day 1 morning): the code-exact drafts

15 drafts are byte-exact in isolation and need only a unit (6.4 KB). Skip
UpdateTargetPosition, InitMultiplayer, STREAM_kill, BIFunc_UpdateBuddyCheckpoints and
DestroyBuddyDataStructures (blocked: see notes). Land the rest: CParticleSystemManager::Draw
and its five `@32@` thunks, `RCMP::RCMP_SYSTEM::~RCMP_SYSTEM`, the three
CBulletDecalManager `@36@` thunks. If one needs the file's data block or a whole-file
merge, park it with the reason.

### Phase B (days 1-2): priority 1 near-misses

Queue ranks 1-120 (priority 1): collision, scripting, AI, animation, fewest differing lines
first. Expect most of the time here. Watch for whole-file opportunities: when several
functions of one file are close, a merged unit can land them together.

### Phase C (days 2-3): priority 2

Rendering, audio, shell/HUD, system and video, same method. The drafts fixed on
2026-11-11 (CCompartment::Draw, CStaticMesh::Draw and the five padded views) are
logically right now and are good candidates.

### Phase D (only when A-C are exhausted or all parked)

Safe secondary work that changes no verified source:
- Initialised data as source (PortReadiness step 2.2): tables in `research/data/`,
  checked by `check_data.py`, for the 181 undefined data symbols (`source_link.py --list
  undefined`).
- Re-run `research/tools/layout_lint.py --reuse` so `research/layout/` reflects the
  11-11 view fixes.

Not in this run: multiplayer (priority 3), moving more units onto shared headers (each move
is a maintainer decision), CMatrix/ISceneNode headers, Dolphin.

## Stop and wait (don't push through these)

Stop scheduling work and write a note at the top of **State** if:
- the complete build stops being identical and a revert doesn't fix it;
- tests fail for a reason unrelated to the current function;
- a landing would need a declined form, a gap function or a policy change;
- disk, memory or tool failures repeat.

Otherwise keep going until the maintainer is back, parking anything stuck.

## Iteration pacing

Work to a natural checkpoint (a landing, a park, or about 30 minutes of progress), update
**State** and the log, then `ScheduleWakeup` 60 s with the same prompt. If a long build or
search runs in the background, wait for its notification instead of polling.

## End of run

On the last iteration (or when the maintainer returns), write a summary at the top of
**State**: landings with bytes and commits, the new figure, parks with reasons, and
anything waiting for a decision.
