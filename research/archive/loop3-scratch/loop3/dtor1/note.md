
## An inline's locals: reversed declaration order when expanded (dtor1, 2026-11-13; probes scratch/loop3/dtor1/p, pr.py, v1.sh)
MWCC 1.3.2 numbers the locals of an EXPANDED inline in the reverse of their declaration order,
relative to the same function compiled out of line. So a weak out-of-line copy and its inlined
copies in the same file colour two locals opposite ways whenever the locals sit directly in that
function's body. Probe (dwi::list<Info>, `~list` loop with `node`/`next`, `reserve` destroying a
local list; target out-of-line node r6/next r5, inlined node r5/next r4):
- locals in `~list` itself: next-first gives the out-of-line copy right and the inlined one
  swapped (a, h, i, l), node-first the opposite (b, c, d, g, k). Scoped/for/while/helper-for-the-
  three-stores forms only move which side is right; '-inline deferred,auto' is the same (da, db).
- locals in an inline that `~list` calls (`~list() { clear(); ... }`): BOTH copies are expanded
  inlines, so one declaration order gives both (f: node first; f2: next scoped inside the loop;
  f3: with `node->~list_node<T>()`). next-first in clear() is wrong on both (e). Depth does not
  re-flip: clear() calling clear2() behaves like clear() (m BOTH, m2 neither).
- Not the cause (ruled out): another TU's weak copy (each file record holds its own ~list<X>; the
  orders reproduce in a one-file probe), different flags (every list/reserve fragment pair has the
  same flags: alarm/pumhandler/world_object/world_volume '-inline auto', bsutil deferred,auto), or a
  degree/pressure effect (rank1's threshold): same 4 values, same live ranges in both copies.
- Recipe: when an out-of-line copy and an inlined copy of one function need opposite local orders,
  move the locals into an inline the function calls (a real member such as clear(), or a single-use
  inline helper, accepted form) and write them in the INLINED copy's order. bsutil's
  DoWeOwnThisMemory: out-of-line (owned, base, offset) and the copy in
  BSUtilFreeObjectInstanceMemory both match with an inline OwnsElement declaring offset, base, owned
  (bsu/var2.py: only the f-b-o declaration order, any assignment order). alarm: all 11 functions
  with `~list() { clear(); ... }`. include/game/DwiContainers.h NOT changed: its ~list through
  clear() keeps all 105 header units' code exact (vh.py), but renumbers the compiler's @N data
  labels, so scene_dtor's stripped_objects '@177' no longer names one object and reconstruct
  fails; changing it needs the manifests that name @N labels updated in the same commit.
- world_volume after the fix: all 27 functions exact but the weak copies' ORDER still differs
  (original ~CWorldVolume, ~list, ctor, reserve; ours ~CWorldVolume, ctor, reserve, ~list with
  reserve in-class, wv/c2.cpp). Out-of-class/inline/weak ~list and deferred+reversed sources
  (wv/rc*.cpp, TestCollisionWithSubVolumes then gets inlined) did not place ~list before the ctor.
- pumhandler: the clear() form makes the list fragments' pair exact in one compile (pumhandler_list's
  ~list<QueuedSubtitle> and pumhandler_list_reserve, pw/pumhandler_lr.cpp); the whole-file draft is
  not written yet. world_object (callee registers: the loop calls ~CCompartment): with clear() the
  out-of-line ~list<CCompartment> is exact and reserve's node/next keep the right relative order, but
  both sit one register lower (r26/r25 for r27/r26) whatever the clear() spelling (pw/wov.py, 7
  forms): the second inline level seems to add one ranked value. Open.
