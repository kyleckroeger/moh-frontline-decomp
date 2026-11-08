# oom-guard

Agent working notes, kept as written; dated claims may be out of date.


2026-10-17: WSL froze and rebooted 5 times in a row because scratch/gmw/tree.py enumerated a full int32 range into a set. The user saw "terminal freezes, memory spikes, crashes wsl". Each new session resumed from AUTOPILOT.md and re-ran the same script. Fixed (it now uses interval lists).

**Why:** WSL has 11 GB plus 8 GB swap. A runaway process stalls the whole VM before the OOM killer acts, so the session dies without leaving any clue in the transcript.
**How to apply:** run any new or untested Python/analysis script as `(ulimit -v 2000000; timeout 120 python3 ...)`. If WSL crashed, check the last tool call of the previous transcripts (~/.claude/projects/.../*.jsonl) before re-running anything. Related: [[autopilot-loop]]
