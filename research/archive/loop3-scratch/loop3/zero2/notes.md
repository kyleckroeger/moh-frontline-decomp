# zero2 notes (2026-11-12): natural-form cleanup of inferred shared-zero helpers (zero1's rule)
Tools: vtest.py (copy of audit1's, work dirs here), scratch/game/fast.py + research/tools/sbs.py. Variants in var/.
- LANDED SNDBANKI_playpatch: markvoices inline dropped (var/sp_final.c): function-level i reused by loops 1,3,4.
  Matrix var/spA_*.c: compaction loop must keep its own index (000=18), retry loop must use the shared i (110=2).
- Kept (fast lines of best plain form): PickUp 68 (pl_health v1), PATHI fadein: fadetrack(to,0,0,time) 6 (target
  const-folds), target reuse 11, index-as-flag 4 (unnatural); SendMessages 50; init_free alarm 9 / pumhandler 11
  (orders/chained inert); FindClasses -1: 2 (li+li).
