# opswap (2026-11-13): automated operand-order / statement-split sweep

Tools (all here):
- opsweep.py FUNC [--draft f] [--out d] [--pairs] [--cmp|--cmponly] [--split|--splitonly] [--nomember]
  Tokenises the draft, finds every binary commutative operator (* + & | ^) in function bodies,
  builds one variant per operator with its two operands swapped; with --cmp also mirrors
  comparisons (a < b -> b > a); with --split rewrites 'v = L op R;' into 'v = L; v op= R;' and,
  for commutative op, 'v = R; v op= L;' (v not used in L/R). --nomember skips edits inside
  class/struct bodies (shared inline members: changing them is the declined "different bodies
  for the same inline"). Scores (score.real + |len diff|, difflib) per variant, greedy-combines the
  improving ones, second pass over neutral ones; --pairs tries every pair of singles.
- iter.sh FUNC: rounds of swap+split(+nomember) from the index draft until no gain.
- climb.py FUNC draft L1 L2 out: hill-climb single-line moves in a line range.
- chk.py FUNC files...: check_draft.compare on variant files.

Results: see the final report section below.

## Run 2026-11-13 (commit c99a8c85, no landing)
- Swap sweep (79 rows, singles+greedy; pairs on the 30 rows <= 11): nearly inert. Pairs: nothing.
- Split sweep (iter.sh over all rows): applied FindIntersection 320->266, MemoryCardBootUpPortCheck
  1028->958, DrawBulletStats 140->119, GetClosestWaypointByType 94->88, PATHI_eventstart 195->191,
  aim_cb 59->57 (swap). Proven marks of these six are stale (re-prove).
- Not applied (bests in it/<func>/r*/): PROFILE_UPDATE 81 (+ cmp mirror 74: v/pu2.cpp, steering),
  CCDBVolBox::TestCollision 461, TestCollision(CVolCapsule) 100, DMMorph 251, iSNDrandom 79,
  CheckLineCollision 6 (v/clc1.cpp; GetTypes-body question), UpdateAnimation 14 (shared Cross, declined).
- Gotcha: validate.py must put research/tools before scratch/game on sys.path (two fast.py modules;
  scratch/game's lacks what some drafts need -> spurious no_compile).
- Next: the split sweep could be extended to member/array lvalues ('a.x = L op R') and to
  'return L op R' (via a named local); not done.
