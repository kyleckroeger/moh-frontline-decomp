# Lane agents for the three-day loop: brief and protocol

Four agents share the main checkout `/home/kyle/workspace/moh-frontline`, one per lane. The
coordinator (the `/loop` session, see `three-day-loop.md`) launches them with:

> Read research/plans/loop-agents.md and follow it as agent NAME on lane LANE.

## Brief (read in this order)

1. This file, all of it.
2. `research/plans/three-day-loop.md`: goal, hard rules (they apply to you), method.
3. `scratch/loop3/NAME/notes.md`: your predecessor's handoff (drafts, scores, parks). Continue
   from it. Create it if missing.
4. `scratch/loop3/TRICKS.md` (cross-lane tricks; append yours), `research/notes/matching-tricks.md`,
   `research/notes/register-tools.md`, and `docs/Remaining.md` (accepted and declined forms).
5. Your lane's rows of `research/plans/loop_queue.tsv` (column `area`, see Lanes). Before
   working a function, claim it (below) and grep its name in `research/log/*` and `research/notes/*`.

Work your lane top-down, skipping rows with a `note` (blocked). Matched in isolation -> land
(protocol below) -> next. Stuck after about 45 minutes -> park it in notes.md (draft path,
best score, what was tried) and move on.

Keep your context lean: pipe long output through head/tail/grep, never cat asm dumps or big
files whole. Stop after about 3 hours of work, or 8 landings, or a full hour with no
landing and no improved draft, whichever comes first: update notes.md, release anything you
hold, and end with a final message listing landings (commit hashes and bytes), best WIPs
(paths, scores), parks with reasons, new tricks and any policy questions. The coordinator
then starts a fresh agent on your lane, which reads your notes.

The partial linked build helps: every open function has a draft that is logically
consistent with the original (`research/drafts/`, `check_draft.py`). Start from it, not from
scratch; `check_draft.py` tells you whether a change kept the logic (calls, constants,
globals, offsets, access sizes).

## Lanes

| NAME | LANE (queue `area` values) | First |
| --- | --- | --- |
| geometry | Collision, Animation | |
| scripting | Scripting and level objects | |
| airender | AI, Rendering | the CCompartment/CStaticMesh Draw drafts fixed 11-11 come late in the list |
| systems | Audio, Shell and HUD, System, Video, Other (not Multiplayer) | Phase A: the code-exact drafts (CParticleSystemManager::Draw and thunks, RCMP_SYSTEM dtor, CBulletDecalManager thunks) |

Multiplayer (priority 3) is no one's lane in this run.

## Claims

Append `NAME function` to `scratch/loop3/CLAIMS.txt` before working a function; skip
functions someone else claimed. A whole-file merge claims every function of the file it lands.

## Shared-checkout protocol

**Workspace.** Your private directory is `scratch/loop3/NAME/`. All drafts, probes,
permuter runs and notes go there; copy older WIPs in before editing them. Outside the lock,
NEVER write to `config/`, `src/`, `include/`, `docs/`, `tools/`, `tests/`, `progress/`,
`research/` or `build/`, and never run `tools/port_unit.py`, `tools/reconstruct.py`,
`research/tools/one_unit.py`, `research/tools/land.sh`, `research/tools/addunit.py` or
`scratch/game/try.sh`. Safe outside the lock (temp dirs only): `research/tools/check_draft.py
<name>`, `score_draft.py`, `sbs.py`, `varmap.py`, `scratch/fic/ss.py`, `scratch/coll/sbs.py`,
`scratch/game/fast.py`, `scratch/perm/autoperm.py`, the Ghidra MCP tools.

**Memory (the real limit: WSL has 10 GB + 8 GB swap, shared by everything).**
- Every new or untested script: `(ulimit -v 3000000; timeout 900 python3 ...)`.
- Permuter: at most 2 jobs, one permuter per agent at a time, and stop it when you park.
- Ghidra: use the already-running Ghidra MCP tools. Don't start a headless Ghidra/JVM
  unless you hold `research/tools/landlock.sh` (it doubles as the JVM slot), and then give
  it `-Xmx2g` at most.
- Before starting anything heavy, check `free -m`: if "available" is under 2500 MB, wait.
- Never `pkill -f`; use `pgrep` and `kill <pid>`. Kill your permuter processes when you stop.

**Landing (one agent at a time).**
1. Finish matching first, then `sh research/tools/landlock.sh acquire NAME` (waits up to 20 min).
   After a release, wait about 60 s before acquiring again so other agents get a turn
   (2026-11-13: back-to-back merges starved the 5 s pollers).
2. `git status --porcelain` must show nothing but your own work. If someone else's files are
   there, release and report it; don't touch them.
3. `tools/port_unit.py ... --category reconstructed_game` (game code), check
   `research/tools/poolvals.py <unit>`, `research/tools/addunit.py <unit>`,
   `python3 research/tools/one_unit.py <unit>` (read the text: it exits 0 even on FAIL).
4. Remove the landed function's draft and its `research/drafts/index.tsv` row, append one line
   to `research/log/autopilot.md` (date, `[loop NAME]`, function, bytes, trick used).
5. `sh research/tools/land.sh "<subject>" "<body>"`, then `git push`.
6. `sh research/tools/landlock.sh release NAME`. ALWAYS release, even on failure: first revert
   your partial changes (`git checkout -- <your files>`, remove your untracked files).

Keep work in progress under scratch/ until you land it: land.sh adds all of research/, so
untracked research/ files are swept into whoever lands next (2026-11-13, vt1). Never amend, rebase, reset or force-push. Commit only through land.sh. Never decide policy:
a function that needs a declined form or a gap function is parked with a question.
