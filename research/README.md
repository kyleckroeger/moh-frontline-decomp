# Research

Everything the project has learned that isn't accepted source: working notes,
unfinished drafts and the scripts behind them. Nothing here is built or verified,
and none of it earns progress credit. For the current plan, start with
[Remaining.md](../docs/Remaining.md); for the settled method, [Game.md](../docs/Game.md).

New findings go here, or into the right doc, in the same commit as the work.

## Before you start on a function

Search the logs first. Most open functions have earlier attempts, and repeating
them is the most common way to waste time:

```sh
grep -n "GetPeekLocation" research/log/* research/notes/*
```

## What is here

| Path | Contents |
| --- | --- |
| `log/autopilot.md` | The main work log and decisions, oldest first. Append new entries at the end. |
| `log/lane-*.md` | Logs of the parallel agents, by lane of work |
| `log/parallel-backlog.md` | The parallel agents' work queues, as they stood |
| `notes/matching-tricks.md` | Source forms that fixed specific compiler output. Try these on a near-miss. |
| `notes/game-code.md`, `notes/register-tools.md` | Compiler behaviour and register-allocation findings |
| `notes/autopilot-loop.md`, `notes/parallel-*.md`, `notes/parallel-agents.md` | How the agent runs were organised |
| `notes/agent-pipeline.md`, `notes/opencode-v2-quirks.md` | A retired worker setup, kept as history |
| `notes/buddy.md`, `notes/oom-guard.md`, `notes/runtime-libraries.md`, others | Topic notes |
| `drafts/` | The best draft of each open function, listed in `drafts/index.tsv` |
| `tools/` | Working scripts, tested from the repository root |
| `tools/reference/` | Older scripts, kept for their method. They read local data that isn't in the repo, so they won't run as they are. |

## Reading the notes

These are working notes, kept as written:

- **They contain mistakes.** Later entries correct earlier ones, so read to the end.
- **"The user" means the project maintainer.** Dated decisions in the logs are
  summarised in [Remaining.md](../docs/Remaining.md), which wins if they differ.
- **Paths under `scratch/` are local.** That folder is ignored by git, so most of
  those files are not available. It holds bulk experiments; the best draft of each
  open function and the working scripts have been copied here.

## Drafts

`drafts/index.tsv` gives each draft's differing lines, size, address, function, the
manifest to take compiler settings from, and its path. The line counts were measured
when the drafts were collected. `drafts/status.tsv` holds the latest checker result for
each, and [Remaining.md](../docs/Remaining.md#partial-source-sweep) explains the sweep.

- **A low score isn't proof.** The scorer ignores small-data constants: three drafts
  that scored 0 had wrong floats. Only `one_unit.py` and `tools/reconstruct.py` prove
  a draft.
- **Some use declined forms.** `UpdateTargetPosition` and `InitMultiplayer` score 0
  only because of forms that were declined; see [Remaining.md](../docs/Remaining.md).
- **Types are inferred.** Class views in a draft are what that function needed, not
  recovered declarations.
- **Not every open function has a draft here.** Functions with attempts but no scored
  draft only appear in the logs.

## Working tools

Run these from the repository root, after `tools/setup.py` and `tools/audit.py`.

| Script | Use |
| --- | --- |
| `check_draft.py <name>...`, `--all` | Compare drafts with the original by content (calls, constants, strings, globals, offsets). `--all` rewrites `drafts/status.tsv`. Use this first. |
| `score_draft.py <name>...` | Differing lines of matching drafts against the original |
| `sbs.py <manifest> <draft> <function> <start> <end>` | Side-by-side original and draft, marking register differences |
| `varmap.py` | Which variable sits in which register (uses `-sym on` debug output) |
| `one_unit.py <unit>` | Compile, link and verify one unit |
| `poolvals.py <unit>` | Print a unit's pool constants. Check these after `port_unit.py`. |
| `addunit.py <unit>...` | Register a unit in `project.json` |
| `land.sh "<subject>" [body]` | Full rebuild, snapshot, doc sync and tests, then commit |
| `block_files.py` | Per file, which open functions share a data block and can't land alone |
| `genh.py <output>` | Regenerate `src/game/ShellMenu_block.h` from the original's bytes |

Run untested scripts under `ulimit -v` and `timeout`: one bad script once took the
whole machine down (`notes/oom-guard.md`).
