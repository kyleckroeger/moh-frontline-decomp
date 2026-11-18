# Matching rules found in the matching-plan run (2026-11-17)

Compiler behaviours that closed a queue row; each was tried on the other rows of its group.

- **An empty member destructor changes a constructor's registers.** A class member whose type has
  `~T() {}` (even empty and inlined) makes CW build the constructor's code differently: in
  RCMP::DECODER's constructor the vtable address and the shared zero got one register (r5, the
  vtable stored before `li r5,0`) only once CODEC_IDATA had its destructor; without it the zero was
  hoisted above the vtable store into r5 and the vtable went to r6. Check other units' weak copies
  (here avplayer's `__dt__Q24RCMP11CODEC_IDATAFv`) for destructors a class view is missing.
- **A weak inline function lands where its first referencing function is emitted.** CW emits a
  referenced inline/weak function right after the function that first references it (queued in
  reference order); a reference made only by `__sinit` puts it after `__sinit`. If the original has
  the weak copy before `__sinit` and no kept function references it, a stripped function did: use
  an accepted gap function (`--linker-stripped`). A `delete p` through a virtual destructor counts
  as a reference. Such a gap function also makes its class's vtable and RTTI emit at that point;
  with `-inline auto` that put RCMP_SYSTEM's records before DECODER's, and `-inline deferred,auto`
  (source last to first) gave the original's order (rcmpbase.cpp).
- **A `.bss` destructor-chain record numbered far below the file's first extab label** (`@106` vs
  `@173`) means the global was defined near the top of the file.
- **A class `operator new` that returns through a named local puts the new-expression's result in
  the highest callee register.** `void* p = alloc(...); return p;` instead of `return alloc(...);`
  (same code wherever the result is only returned): in RCMP::MOHFL_VD_create the GCHW_VD pointer
  moved from r29 to r31 and the constructor's inlined parameters fell into the original's order
  (17 -> 0, with the frame height declared before the width). swvideodisplaylist_dtor's
  GCSW_VD_create, which shares VIDEO_DRIVER's operator new, is unchanged by it. Of the other near
  rows only MOHFL had a class operator new.
- **A constructor inlined into its only caller can still be a linker-stripped function.** If a
  weak copy (VIDEO_DRIVER's destructor) sits between the previous function and the caller, define
  the constructor out of line just before the caller: it emits there, pulls the weak copy after
  it, and `strip_unused` drops it (accepted gap function; hwvideodisplaylist.cpp).
- **Register priority comes from Chaitin sweep levels, not only declaration order** (regA,
  BIFunc_GetInitialBuddyOrders). CW's allocator (described in the mwcc-debugger README,
  github.com/cadmic/mwcc-debugger): phase 1 sweeps the variables repeatedly and removes any with
  at most 28 remaining neighbours (r0/r3-r12 count as neighbours for values live across calls;
  coalesced copies stay in the graph as neighbours); a variable removed in a later sweep gets a
  higher priority than every variable of an earlier sweep. Phase 2 assigns by priority: the lowest
  numbered callee register already in use that no assigned neighbour holds, else a new one below
  (calibrated: `int a = g(); int b = g(); h(); k(a, b); int c = g(); ...` gives a r31, b r30, c r30).
  Declaration order therefore ranks locals only inside one sweep; a busy caller can push some of an
  inline's locals into a later sweep and invert their rank. Lever: put the loop body in an inline
  with parameters (the entry and slot) and declare the locals inside it; GI's ready-list dump went
  from the soldier ranked above the node (any of 900+ declaration orders, a 23k-iteration permuter)
  to the original order. Re-sweep the float declaration orders of the other inline blocks after it.
