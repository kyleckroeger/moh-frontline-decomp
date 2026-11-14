# late2 (day2 run, 2026-11-13) - work list scratch/loop3/day2/late2.tsv

## LANDED 8240e679 font_driver += CFontDriver::EndDraw (108, 11 -> 0)
- inline 'CColor GetColor() const { unsigned int w = *(unsigned int*)&color; return *(CColor*)&w; }' on shared CColor.h (ed/v7).
- StartDraw/Draw already belong to font_draw; port_unit --like a scratch copy of font_driver.json with 'include' added
  (drop the adapted_from key it writes for a template outside config/).

## Time-boxed (no gain)
- buddy BuddyInfo temp knot (list range ctor 3, CreateBuddyDataStructures 66, BIFunc_AddBuddy 207): orig keeps the
  BuddyInfo temp/local's path/moving/active in memory (+8..+20, ctor-body order moving, active, path) while soldier/progress
  are scalars (never stored; zeros/regs forwarded across calls). Toys (toy/t*.cpp): CW 1.3.2 keeps a local struct fully in
  memory (any self/&member/address escape, or no user copy ctor = block copy) or fully scalar (user copy ctor, plain ctor);
  never partial (A-L, N, Q). Only a SEPARATE BuddyState local plus a BuddyInfo(soldier, progress, const BuddyState&) temp
  (toy tM) gives the orig shape, but the list ctor's dead stores sit at temp+8 with 8 unused bytes below, i.e. inside a
  20-byte BuddyInfo. Not resolved. bl/ (list ctor 40+ variants), cb/c1 (CreateBuddy with b0's BuddyState: 75).
- AddBuddy 6: moving the 'UsingBuddy' dump into a single-use inline (ab/h1) fixes the inlined GiveBuddyOrders' FPRs
  (f29/f30/f31) but shifts its GPRs one lower (17). Decl orders/init chains (36 forms) inert.
- SweepAllExtents 192: orig keeps DEAD 'neg' of the first three OSGetTick starts in r29-r31 (stops and second-half
  resumes vanish). Scalars/array/static/timer class/register/empty-inline use/if(0): CW removes every neg (sa/, toy1-2).
- Scroll 6: Reset's inlined copy schedules differently (out-of-line Reset exact). Setter forms (256 combos), call forms inert.
- Update (CInputManager) 3: Both(): orig computes the hi shift amount first but the lo term first; body orders, uchar
  params, arg spellings, locals inert (up/).
- AnimStStartState 10: 'slots[k].state == id' fixes the cmplw operand order but the field/candidate regs stay r0/r3
  swapped (as/, 18); helper param orders give 10 or 18 only.
- GetInitialBuddyOrders 7 (DumpReadyBuddies decl perms inert), SNDtimeremaining 3 (decl perms, gs local, EA flags inert),
  quick_sort 6, GetPeekLocation 3 (mask/operator| forms), OnCollision 3, SFILTER_rsf 5, iSNDrandom 80, EndDraw done.
- Skipped: declined/held (UpdateTargetPosition, STREAM_kill, DoTargetCheck, PlayerScreenFlash, CreateRootHierObject and
  CreateDestructable = Q7 placement new, RCMP dtor waits on DECODER ctor (5+ agents)), multiplayer (BIFunc_UpdateFragCount,
  DrawBulletStats), CProfileInitClass (debug_prof .rodata gap, on hold).
