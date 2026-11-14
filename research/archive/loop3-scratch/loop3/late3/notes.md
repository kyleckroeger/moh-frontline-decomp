# late3, 2026-11-12

Task: apply late2's list-iterator-object form (b9dfe832 SweepExtents) to parked list-walk register swaps.
Scorer: ../scripting/tools/sc2.py <tmpl> <func> <addr> <size> <src...>; side by side: ../scripting/tools/dump.py.

## BIFunc_GetInitialBuddyOrders (7, unchanged)
- GI_BEST7 (= gi0.cpp) ALREADY walks the ready list with dwi::list_iter (begin()/end()/operator*/++), as does the landed
  DumpReadyBuddies in src/game/buddy_head.cpp. So the iterator form is not new here.
- late2's iterator shape (operator!= by value, default ctor, no (0) init) gi1-gi3: 7. Post-increment (RS operator++(int)),
  for-init iterator, while loop (gv_*.cpp): 7. DumpReady written out in GI (iterator or raw node, gi_w*.cpp): 16.
- Orig DumpReady in GI: node r30 (= GI soldier / GiveBuddyOrders buddy / DumpAttached buddies reg), soldier r26 (= DumpAttached
  soldier). Ours swaps them. Died/UBP copies match with the same inline, so the cause is GI's own colouring context.
- Permuter on GI body (DumpReady kept inline): perm_gi/ (base 50 = 7 lines), see result below.

## UpdateProximityTriggerStatus (9, unchanged)
- Intrusive BSGO_Basic::m_next list, not dwi::list. A GOIter struct (ctor/copy/operator*/++/bool) for the German loop, in
  for-init / function-top / before-i positions, and a plain struct {p} walk (up_a-e.cpp): all 9. German user stays r29.

## CSoldierObject::Draw (8, unchanged)
- m_weapons[9] array, the weapon IV is compiler-made (orig r24 = later lwz r24,48(r3) local; ours r28 = later loop IV).
  Pointer walk w++ (11), struct iterator w.p (9), 'CWeapon*& w = m_weapons[i]' (8). sv_*.cpp.

## PublishEvent (3, unchanged)
- Already a hash_map iterator. Iterator shape (by-value !=, no copy ctor -> 132, operator==): 3/46.
  The orig loads loop 2's callback before the slot compare; any named callback in loop 2 (18 decl-form x position grid pg_*,
  SItem* item at top, shared callback at top pt_*) gives 46 (it/event/subject rotation); references/pointer/it-> paths: 3.

## SweepAllExtents (193, released, not a loop knot)
- Orig keeps the first half's '-OSGetTick()' values (neg r31/r29/r30, live across the sort/sweep calls) but drops the adds and
  the whole second half's arithmetic; ours drops everything (array or scalar times). Looks like DCE timing, not iterator.
- Permuter result: perm_gi 72k iterations (-j2, ~40 min, ~60% permuter failures from undefined-struct type info): no
  improvement over base 50. Killed. GI parked at 7 (GI_BEST7 unchanged).

## Conclusion
The iterator-object trick does not transfer: GI and PublishEvent already use iterators; UPTS and CSoldierObject::Draw
walk intrusive lists / arrays where struct iterator forms are inert. No landing this session.
