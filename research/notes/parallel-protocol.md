# Parallel agent protocol (2026-10-23)

Several agents share the main checkout <repo> at the same time. Follow these rules exactly.

## Workspace
- Your private dir is scratch/par/<NAME>/. Put ALL drafts, probes, permuter dirs and notes there.
  You may READ anything in scratch/ (older WIPs, tools); copy files into your dir before editing them.
- Outside the lock, NEVER write to config/, src/, include/, docs/, tools/, tests/, progress/, build/, or scratch/AUTOPILOT.md,
  and never run port_unit.py, reconstruct.py, one_unit.py, land.sh, ninja, or scratch/game/try.sh (it writes a fixed probe unit to config/).
- Matching outside the lock: scratch/fic/ss.py, scratch/coll/sbs.py, scratch/game/fast.py, scratch/perm/autoperm.py.
  These use temp dirs and are safe to run in parallel.
- Resources: 8 cores and 10 GB RAM are shared by 4 agents. Run the permuter with at most 2 jobs, and only one permuter at a time per agent.
  Run new or untested scripts as `(ulimit -v 2000000; timeout 600 python3 ...)`. Never `pkill -f`; use pgrep and kill <pid>.
  If a build is needed: ninja -j2.

## Landing (one agent at a time)
1. `sh scratch/par/lock.sh acquire <NAME>`. This waits for up to 15 minutes.
2. `git status --porcelain`: it must be clean apart from your own files. `git pull` is not needed: everyone shares one checkout.
   If someone else's stray files are there, release the lock and report it; do not touch their files.
3. Run port_unit, then poolvals/one_unit (read the output text: one_unit exits 0 even on FAIL), then scratch/land.sh "<subject>".
4. Append ONE log line for the landing to the Log at the end of scratch/AUTOPILOT.md (only the Log; leave the header to the coordinator).
5. `sh scratch/par/lock.sh release <NAME>`. ALWAYS release, even on failure; revert your partial config/src changes first
   (`git checkout -- <your files>`, and remove your untracked files).
Keep the lock as short as possible: finish matching before you acquire it.
Do NOT push. Do NOT amend or rebase. Only commit through land.sh.

## Reporting
Keep scratch/par/<NAME>/notes.md current: what you tried, best scores, draft paths, and new tricks.
When you stop, your final message must list landings (commit hashes), the best WIPs with their paths and scores, and any user-policy questions.
Never decide policy items (see "Waiting on the user" and the Decisions in AUTOPILOT.md); write them up as questions.
