# zero3 notes (2026-11-12): shared-zero rule + loop-index reuse on parked constant/index register drafts
Harness: ../mid3/tools/v.py with per-dir env.sh + s*.py specs (rs/, qs/, pn/, cfo/). scan2.py = all drafts (real <= 120),
aligned 'li rA,C' (C in 0,1,-1,2) whose register differs from orig (scan2.out). No landing, no draft change.
- ResetShell (18) rs/: zero r4/one r3 vs orig r3/r4 around SetGameType(1). Inert (all 18): enum param, default arg
  'SetGameType()', local copy 't = type', flag clear order, bool/int setter inlines for the two flags (SetF40/SetF20,
  one SetFlags(b): tested only to probe the vreg-order theory, would be steering helpers anyway), function-level 'int i'
  for the ResetPlayer loop, const param. Worse: index store before suffix 19, m_gameType store after the switch 160,
  flags after SetGameType 159. So the zero is not made a variable by any of these; theory "inline param var gets
  colour before the CSE'd constant" is not what decides it.
- quick_sort_<UNodePair> (6) qs/: tail next r27 vs orig r29 (orig: next takes first's/insertion loop-2 i's register).
  Inert 6: function-level i declared in else-scope, SGI 'next = last; --next', last-copy local in uli, T value local,
  T(*i), j copy, while loop, written-out uli body in the tail. Worse: first as tail iterator 34, function-top i 34,
  fin2 helper 44, no mid 84, mid before sort_ 9, 'last != i' 7.
- CPathNodeList ctor (99) pn/: function-level i reused by both outer loops 99 (inert), at the top 110, shared k 99.
  The diff is a pure rotation: orig .rodata pool base r31 > this r30 > map bases; ours this first.
- CheckForObstructions (25, claimed + released) cfo/: sign ternary forms 25-26, swapped if 25. Same pattern: orig sign r31
  > this r30, ours this r31 > ... > sign r28.
- Pattern across PathNodeList, CheckForObstructions, SetFinishedLevelData (orig i r29 > this r28): a later-defined value
  ranks ABOVE 'this' in the original; ours always gives 'this' r31. Not a shared-constant problem; decl order, copies,
  ternaries, reuse do not touch it. Open question for whoever studies callee ranks next.
- scan2 hits (others): GetObjInfo, DrawFragCount (MP, hold), DMClusterSkelTraverseHeirarchy (-1 depth = callee rank,
  params lowest), decal Init, SNDVOICEI_alloc, CreateDumpObject, MIX_audioslice, PATHI_seeknextnode, iSNDrandom,
  PROFILE_UPDATE, random, GetClosestWaypointByType, DoTargetCheck: colouring/rank, not shared constants.
