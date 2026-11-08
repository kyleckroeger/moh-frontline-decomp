# tooling agent notes (2026-10-29 evening)

Private mirror: scratch/par/tooling/m (copies of tools/tests/config/src/docs; include/orig/build/tools symlinked).
Develop + port_unit/one_unit there without the lock; copy into the real tree only under the lock.

## TASK 1: label_functions (size-0 NOTYPE assembly labels)
- Manifest key `label_functions`: [{name,address,size,binding}]; compiled FUNC must be NAMED AFTER the label
  at its start (no invented wrapper names): sources renamed from the parks drafts
  (targsupp: `asm u32 TRKAccessFile(...)` + entries; __exception: `__declspec(section ".init") asm void
  gTRKInterruptVectorTable(void)` ... `entry gTRKInterruptVectorTableEnd`; gc_idct already named right).
- project_build: check_label_functions (validate_units), check_linked_labels (verify_unit), covered_functions
  for tiling/progress/report/strip roots. code_map gaps subtract accepted intervals. progress_report counts them.
- port_unit: text_layout keeps a function whose original is a label; draft writes label_functions.
- scratch/one_unit.py: roots/retain include label_functions.
- Test: tests/test_label_functions.py (15 tests).
- All three verified in the mirror (one_unit VERIFIED).

## TASK 2: linker_stripped (Screen.cpp stub classes)
- Finding: per-object data stripping already existed (stripped_objects, linked_offset/size via port_unit kept_span);
  what was missing was an explicit stub list + port_unit excluding stub vtables from kept_span (the PAL vtable
  points at kept CScreenFieldMode code, so the automatic rule kept it).
- Manifest key `linker_stripped: {functions: [...], objects: [...]}` (compiled names). data_strip.linker_stripped()
  expands: listed objects + each listed function's extabindex entry + the extab entry it names + each listed
  __RTTI__ record's base list (@N object its word 1 points at). Type-name strings are NOT touched (they survive).
  strip_objects moves them to the section tails; fails if one lands inside the kept slice; fails if a listed
  name is not exactly one compiled function/object. validate_units: functions must be discarded, no name in target.
- port_unit --linker-stripped NAME (repeatable): pre-moves, excludes stubs from kept_span/match.
- BUG in bigfiles' WHOLE_rt88.cpp: _Capture's counter is post-increment in the original
  ('sprintf(..., _captureNum++)', orig addi r0,r5,1 / stw r0; draft had '_captureNum++; sprintf(.., _captureNum)').
  ss.py did not flag it (2 words). Fixed in the landed source.
- Tests: tests/test_linker_stripped.py (9, skip until Screen built).

## Landings (2026-10-29)
386ae88b tooling label_functions; ae388e6e targsupp (32); 5be760aa gc_idct (824); 6f25244c __exception (7988, .init);
676e2ae1 tooling linker_stripped; 6a1b2043 Screen whole file (+1696). 83.67% -> 84.76% (incl. others' landings in between).

## Notes / questions for the user
- Label functions must be NAMED after the size-0 label at their start (no wrapper names); parks' drafts were renamed
  (targsupp: TRKAccessFile with 3 entries; __exception: gTRKInterruptVectorTable ... entry gTRKInterruptVectorTableEnd).
  Extra LOCAL labels (gc_idct's 'entry static PS_IDCT_col', absent in the original) are allowed; extra GLOBAL labels are not.
- scratch/check_bounds.py prints 5 pre-existing lines (ssysreal, smixfram, susercb, slinklst, dolphin_trk .init+.text span);
  none from these landings. Not investigated.
- Mirror scratch/par/tooling/m is now stale; delete or re-copy before reuse.
