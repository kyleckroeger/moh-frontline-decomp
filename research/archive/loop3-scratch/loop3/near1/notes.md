# near1 (2026-11-12): near-miss list from fresh1

## Landed
- 5d836f39 supxaf += SFILTER_unpackxaf (172): 'int total = 0; total += decoded;' (sibling xalf shape).
- 4ef46f60 drafts: ShellInit switch case order (string pool/blocks), TRKSuppAccessFile 162 -> 151 (Pikmin support.c shapes).
- 6b511f5e draft: PopUpMessageHandler::Loop 43 -> 27 (pl/d3.cpp; per-loop index locals, IV order, int messageTop).
  Left: FPR colouring top/bottom f26/f25 vs ours f30/f29 (sy loads swap), shrink r25/r24, multiplayer fsubs block, 459-464.

## Parked (forms tried in CLAIMS.txt)
- death_cb (4 fast): IV-init order in posture/location filter loops; for-init, while, continue, per-loop j, idx locals,
  INFO forms, counter reuse, pointer src: inert or worse.
- quick_sort_ (6 sbs): tail 'next' r27 vs orig r29 (shares insertion i/first's reg in orig); hand-written tail with next
  declared at 5 places, separate j, mid moves, fin helpers: inert/worse.
- CAnimMorph::Draw (9): orig's dead operator= temp is the LAST stack slot = 2nd-level inline (Assign wrapper, declined).
  Putting the whole span calc into an inline moves all its locals low (38). No natural form found.
- CheckLineCollision (22 sbs): types r19 (shares m_flags load's reg) vs r27, mask/flags r4/r5. operator|(const
  CoverPointTypes&) bodies, mask placement, types decl positions: all exactly inert.
- DMClusterSkelTraverseHeirarchy (42 sbs): params always rank below all declared locals in ours (orig: count, bone >
  params > scratch, depth, a, b, c). In-loop a/b/c decls, init-decls, 15 param-copy subsets: none better.
  Note: the draft's 'low.d.v[0] = low.d.v[1] = ... = 3.4e38f' is a chained assignment; check policy before landing.

## brchk sweep (all 119 drafts)
No same-length draft has branch-offset differences. .c drafts: brchk's SUFFIX=c is a tool bug (compile as C++).
Unequal-length: tools/brchk2.py (alignment) - no logic bugs; SoundSchedule::Update bool form (li r0,0 before the
compare) and PopUp ctor/TRK/RenderSystem differences are codegen shape, not logic.
- 6b511f5e also: research/tools/brchk.py compiles .c drafts as C++ (fixes the 3 COMPILEFAILs).
