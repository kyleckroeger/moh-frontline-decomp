# split2 (2026-11-13): extended statement-split sweep + declaration sweep

Tools (scratch/loop3/split2):
- split2.py FUNC [--draft f] [--out d] [--nomember]  (wraps opswap/opsweep.py, split-only). Kinds:
  split@k (simple var, every split point of a top-level same-precedence chain), split@km (member/array/deref
  target: a.x, p->f, arr[i], *p), splitR/splitRm (commutative last op reversed), splitT@k ('return chain' via a
  typed 'ret_' local split at k), splitV ('return E' via a named local), splitC (collapse 'T v = E; return v;' /
  'v op= E; return v;' into the return). --list FILE prints the variants.
- iter.sh FUNC (rounds from the index draft), iterd.sh FUNC DRAFT TAG (rounds from any draft), all.sh (rows.txt).
- autods.py FUNC DRAFT [OUT]: automatic declaration sweep (ds2 rule) over every run of >= 2 consecutive declaration
  lines outside class bodies, greedy until no gain (difflib score), prints real before/after. ds1.sh FUNC wrapper.
- unitcmp.py JSON SRC... : per-function diff counts of a landed unit's source variants (shared-inline checks).

## Results (83 rows: opswap's 79 + _find_aim_blend_values, AddBuddy, BIFunc_AddBuddy, GetInitialBuddyOrders)
- Split kinds, round-1 singles over all rows (n / compile-fail / better / same):
  split@ 335/16/4/147, split@m 229/13/2/65, splitR 177/33/2/50, splitRm 155/14/1/49,
  splitT@ 15/4/0/6, splitV 86/1/0/82, splitC 6/0/0/1. Member/array splits are the only new kind with gains;
  return-through-local, collapse and chain-position splits are inert on every row.
- Kept in scratch, not applied: _matrix_rot_yzx 109 -> 105 (it/_matrix_rot_yzx__FR7CMatrixlll/r2: six scattered
  't.d.c.x = s; t.d.c.x *= v.x' member splits in the hand-inlined scale blocks; a non-uniform pattern the original's
  inline could not produce), DrawBulletStats 119 -> 118, FindIntersection 266 (lendiff only),
  TestCollision(CVolCapsule) 102 -> 100 and PROFILE_UPDATE 82 -> 81 (both = opswap's), CheckLineCollision 10 -> 6
  (opswap's GetTypes question).
- Declaration sweep (autods): applied PATHI_eventstart 191 -> 160 (findleader: 'int j;' before 'PATHTRACK* track;'
  = the landed path_service.c copy's order), PATHI_seeknextnode 73 -> 63 (choosebranch: 'int next = -1;' after
  track; landed path_fadevolume.c's PATHI_enternode stays 0 with either order, checked with unitcmp.py),
  _AnimTranDoTransition 185 -> 177 (FindTransition: found, key, i). All check_draft consistent, same conversion
  counts. Split sweep on top of them: inert.
- AddBuddy 6 -> 0 and BIFunc_AddBuddy 22 -> 16 via GiveBuddyOrders' x/y/z order ('float x = 0.0f; float y;
  float z; y = x; z = x;'): the same shared-inline trade-off buddy2 found -- buddy_head.cpp with that order puts
  landed UpdateBuddyCheckpoints and UpdateBuddyProgress at 14 each (bud/buddy_head_xyz.cpp). Not applied.
- Real-scored ds (REAL=1 autods.py, dsr/): applied CCDBVolBox::TestCollision(CTriangle) 463 -> 449 (5bb2d2b8). Kept:
  aim_cb 57 -> 55 (dsr/aim_cb*), PROFILE_UPDATE 82 -> 81, _unpack_skel_frame 157 -> 154 (dsr/).
- Commits: d6c3334d, 5bb2d2b8 (notes/drafts only, no landing). Stale proof marks: PATHI_eventstart,
  PATHI_seeknextnode, _AnimTranDoTransition, TestCollision(CTriangle) -> re-prove.
