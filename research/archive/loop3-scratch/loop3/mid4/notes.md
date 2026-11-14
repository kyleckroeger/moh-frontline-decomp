# mid4 notes (2026-11-12)

Rank list: scratch/loop3/mid4/rank.tsv (rank.py rebuilds it: bytes / (1 + claims + notes mentions); multiplayer,
queue-note, pending/ and State questions excluded at the bottom). The attempt count misses lane-parks.md and
older names (quick_sort, AnimStStartState had long histories), so read research/log/lane-parks.md too.

## Draft improvements (committed together, see the commit at the end of this file)
- TestCollision CCDBTriangle(CVolCapsule) 0x8006a71c: 10 (MulV value wrapper, declined) -> 7 without it: named float
  locals for the sv/tv edge products (tc/final.cpp). Left: fS f3 vs orig f5 (products f4/f6/f7 vs f3/f4/f6). Inert:
  decl placement of the locals (11 places), 's = fS' copies, operand order, ctor CVector3(float,float,float) (21-29).
- GetClosestParameter 0x800516b4: 94 -> 32 (gcp/b4.cpp): plain x,y,z order in Position/Derivative/SecondDerivative,
  num/den and both distances, and a per-loop 'float u' for the scan loop. Left: scan-loop FPR colours (orig m_step f4,
  p f5-f7, bestDistance f8, end f9, t f10). dwi::min forms 37-38; decl-order hill climb inert.
- AnimStStartState 0x800676d8: 10 -> 10 workaround-free (ss/final.cpp): '_St' accessor + identity 'o = owner' removed;
  'AnimStateSlotView* slots = owner->slots;' in the loop + '(unsigned short)id == slots[k].state'. Left: id clrlwi r3 vs
  orig r0 (state loads r0 vs r3); id types, cur copy, compare orders inert.
- BIFunc_GetObjInfo 0x8002c258: 16 -> 16 without the declined chained 'iresult = (int)(filter = 0)' (goi/final.cpp).
  STILL a workaround: ArgObject inline only steers (plain ARG(2) = 194); header comment added. Left: filter r27 vs the
  ARG(2) offset r26 swap; filter decl position inert.
- ChooseCoverPointByType 0x8005ca88: mid3's plain 'enemy.y - origin.y' cleanup (20, same).

## Parked / tried (this run)
- ResetShell (18): zero r4/type r3 swap (orig r3/r4). Param types x6, const ref, this->, type local, flags bool/0u/int
  bitfield, gameType/index unsigned, SetGameType before/between flag stores, switch(type) split: inert or worse. The
  landed CShellMenu ctor has the same r4/r3 as ours, so the orig ResetShell differs in something not yet seen.
- ChooseCoverPointByType (20): GetDistanceSquaredXYZReal-shaped inline for 'distance' fixes the B-block loads but swaps
  threatDistance/distance f31/f30 (33); heading DirectionView + Normalize CSE form 47; 720 decl orders inert.
- FindIntersection (323): one structural line = first-block sqrt result not coalesced with fDiscr (extra fmr);
  fRoot = sqrtf(fDiscr), fInv first, fDiscr only: inert/worse. Normalising the mixed d.v[i]/d.c.x spellings 323 -> 338
  (spellings steer). Old permuter 40 min.
- CPathNodeList ctor (99): structure exact; callee rank: orig pool base r31 > this r30 > map r29 > r28; ours this r31,
  base r28. initlist, 'self' const copy, count vs m_count, node pointer in loop 1, total position: inert/worse.
- quick_sort_<UNodePair> (6): tail 'next' r27 vs orig r29: per-loop i in insertion_sort 22, no prev 173, tail over mid 6,
  fin helper 44.
- TestCDBBranchNodeCollision<CLine3>, AllocateStaticObject, PATHI_eventstart: not worked (exhaustive notes / layout-blocked /
  deep colouring, see scratch/par/lines/notes.md s18, scripting notes, lane-parks.md).

## Tools
- rank.py (rank.tsv); per-function dirs have env.sh + s*.py specs for scratch/loop3/mid3/tools/v.py; d.sh/blk.sh in cc/.

## Commit
- 01ad83c1 (pushed): the five draft updates above. Verified 1,256,876 bytes after it.
