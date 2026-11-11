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
| `notes/buddy.md`, `notes/oom-guard.md`, `notes/runtime-libraries.md`, others | Topic notes |
| `drafts/` | The best draft of each open function, listed in `drafts/index.tsv` |
| `data/` | Original data tables written as source, each checked byte for byte by `tools/check_data.py` (PortReadiness step 2) |
| `layout/` | `layout_lint.py`'s output: `conflicts.tsv` (views that disagree), `classes.tsv` (per class: views, sizes, members all views agree on); the raw layouts go to `build/layout/views.json` |
| `tools/` | Working scripts, tested from the repository root |
| `tools/reference/` | Older scripts, kept for their method; logs and notes refer to them. They read local data that isn't in the repo, so they won't run as they are. |

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
| `bs_disasm.py <elf> <level.viv> <class>`, `--opcodes`, `--verify <viv>...` | Disassemble GameCube behaviour scripts (`.cbs`/`.sin`). Opcodes come from `BSInitMachine`'s code and built-in names from `BIFunc_` symbols. Its output is game data: don't commit it. |
| `dolphin_boot.py prepare/run/bisect/diff` | Boot a linked ELF in Dolphin from WSL, compare traces, and bisect crashing or diverging drafts (below) |
| `input_script.py <script.txt> <out.cpp>` | Build the input probe (`research/probes/input_probe.cpp`) from a text input script (`research/probes/scripts/`) |
| `unowned_data.py [--list KIND]` | Data symbols no accepted unit owns, by kind |
| `vtables.py [Class]`, `--json` | Each class's virtual table from the original's relocations, slot by slot |
| `layout_lint.py`, `--class NAME`, `--check FILE SETTINGS`, `--reuse` | Compare every file's view of each class (from `-sym on` debug output): members, vtable pointer, bases. Writes `layout/`. `--check` lints one new or edited draft against the stored views in seconds; run it before trusting a draft's class view. |
| `source_symbols.py [--jobs N]`, `--missing SECTION` | Which of the original's named symbols the units, drafts and `data/` would define in a pure-source link: defined, missing or duplicate, by section (PortReadiness 2.4's figure until that link exists). Writes `layout/source_symbols.tsv`. |
| `check_data.py [FILE]` | Compile the tables in `data/` and compare each with the original symbol: section, size, bytes and relocation targets. Tested (`tests/test_data_tables.py`). |

Run untested scripts under `ulimit -v` and `timeout`: one bad script once took the
whole machine down (`notes/oom-guard.md`).

## Linked build

`research/tools/link_drafts.py` links every draft into `build/linked/Moh2RelGC_linked.elf` by patching
the original image, and writes the linked figure. Use `--exclude NAME` to leave functions out
when bisecting a crash. How it works and its known risks are in
[docs/Remaining.md](../docs/Remaining.md#two-progress-figures).

`research/tools/dolphin_boot.py` runs it. `prepare` copies the player's ISO into a
Windows-side work directory and writes a logging Dolphin profile; `run` writes an ELF after
the last file on that disc, points the FST entry at it, boots it and prints one JSON line
(exception, SRR0, LR, logged stack, movies played); `bisect` restores patched functions to
their original first instruction, halving until it finds the draft behind a crash. A run
takes about a minute. The control image for a bisect is the linked image with every patch
restored, so its memory layout is the same as the linked build's.

```sh
python3 research/tools/dolphin_boot.py prepare --iso "<Frontline ISO>" --dolphin "<Dolphin.exe>" --work /mnt/c/.../mohf-dolphin
python3 research/tools/dolphin_boot.py run --work /mnt/c/.../mohf-dolphin build/linked/Moh2RelGC_linked.elf
```

Dolphin's movie playback shows thin vertical stripes in this setup for the original ELF too,
so they aren't a draft bug.

### Scripted play

`research/probes/input_probe.cpp` replaces the pad driver's per-frame update in a test
image: controller 1 follows a fixed script, and every few updates it prints a trace line
(player position, yaw, pitch, weapon and a hash of the player object) with `OSReport`.
Build one with `input_script.py`, link it with `link_drafts.py --probe FILE --output ELF`,
and run it with `dolphin_boot.py run`. The run's log is saved as `<work>/<tag>.log`.

For a fair comparison, the control is the drafted image with every draft patch restored
(`dolphin_boot.Linked(...).build(set())`): same memory layout, same probe, original code.
`dolphin_boot.py diff A.log B.log` finds the first differing trace line, and
`bisect --reference CONTROL.log` treats any difference as a failure.

`prepare` gives Dolphin 64 MB of MEM1. The drafts' code segment takes about 510 KB from the
game's heap, and with the usual 24 MB, Mission 1 runs out while loading and crashes in
`CScene::AddNode_now` on a null node. That's the test setup, not a draft bug.
`research/probes/scripts/m1_opening.txt` reaches Omaha's beach from boot; the player has
control from about update 8,300.
