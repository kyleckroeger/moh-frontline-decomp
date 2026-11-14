# late1 (register-rotation recipe sweep), 2026-11-11/12

## CreateDestructable: handed to pnew1 (claim released). Findings, all from systems5's cd_o4.cpp (1) / cd_i6.cpp (3):
- Tools here: rm.py/d.py (T=t.json), v.py (base = cd_o4.cpp, H_OLD header / B_OLD box block), run1..run9.py = variant batches; w_*.cpp outputs.
- The CCV recipe (core declared first, assigned after the const copies) makes CD WORSE: 93-109 (run1), 43 best (r4 + cast).
- Class-scope 'static operator new' for CVolBox + 'box = new CVolBox': same as plain placement (box r30), 102-124 (run2).
- Box-block forms (run3/5/6/9): copy-then-new 'box=(CVolBox*)mem; new (mem) CVolBox;' = 3 (cmplwi r3,0 BEFORE mr r28,r3,
  so no 'mr.' peephole). Inline NewBox(CVolBox*&)/NewBox(CVolBox**), '(CVolBox*)(IVolume*)new', IVolume* + cast = 3
  (cmplwi + late mr, like i6). 'if ((box=(CVolBox*)mem) != 0) new (mem)...', goto skip, static_cast, '!= 0': all 1 (double beq).
  'if (mem) { box=..; new (mem) ..} else box=0;' = 6. 'box->CVolBox::CVolBox()' compiles but builds a temporary (25).
  'throw()' on placement operator new: no change (CW null-checks either way).
- 'CVolBox* const box = new (...) CVolBox;' declared at its use: box goes LOWEST (r26, below core/object) for every one of
  the 120 header orders of {param, trigger, xyz, core, object} (run8: best 47). So a CONST new-result local follows declaration
  order (late = low); the non-const one ranks just under the first new-temp. The needed slot (below trigger, above core) is
  only reachable if box is declared between them, which a const can't be.

## BIFunc_GetInitialBuddyOrders (7): parked, released
- gi.cpp = scripting chk/GI_BEST7.cpp. gv.py/g1-g3.py. Recipe on GI's top-level soldier (declared first, const copies of
  stack/caller, assigned late, every order): 24 diff lines (worse, adds a mr). DumpReady: soldier before/after node, direct
  'node.m_node->m_data.soldier' read: inert (14 diff = 7 real). Raw list_node pointer + const end / generic DumpBuddyList(list&)
  with a const list pointer: 60-92 (worse).

## Not callee-saved rotations (checked, not worked): AddAmo (volatile r4/r5 load), CThrownObject::Init (volatile FPRs),
  CStaticObject::OnCollision (FPR per scripting notes), CVolBox::TestCollision(CVolSphere) (volatile FPRs),
  SNDVOICEI_alloc (mixed volatile + callee, 53).

## CSoldierObject::Draw (8): parked. sdv.py/sdv2.py: const self/context copies at top or in RemoveWeapons, weapon temp, pointer
  walk, i at top, while form: IV stays r28 (orig r24); self at top is worse (this moves).

## CStaticObjectFactory::Init (138): parked. sofv.py/sofv2.py on scripting's BEST_w1.cpp: each pool new[] into a late 'T* const pN'
  (via helper or direct new), non-const, count declared first + late assign, template ptr declared first + late assign: all 138.

## UpdateProximityTriggerStatus (w_ccpy 12 real): parked. upv.py/upv2.py: separate german var (x6 decl orders, for/while/const
  inner copy) 31 diff; while/do/for-without-step/cast/second inline without the const copy: 24-27 (same).

## BIFunc_AIStartCoverPointWalkToStored (7 real): best c_i3.cpp = 3 diff lines: 'bool const flag = **stack;' gives the orig's
  registers (flag r31, stack r30) but the bool conversion lands directly in r31 ('srwi r31,r0,31'); orig converts into r0 and
  copies ('srwi r0,r0,31; mr r31,r0'). Every non-const form keeps the old swap (12); every const form (BSArgB, !!, ?:, int/uchar
  const + bool, const inline params, const copy of a non-const flag) = 3. Permuter permcw/ (base 105 there) found 90 only.

## SweepExtents: 39 -> 7 real (score.real), best sx_inl_hl.cpp (= s_qforscope.cpp with a for-scoped it). Claimed.
- sv.py harness (rm.py diff count; real.py for score.real). Steps: (1) const pointer copies of the three reference params
  declared after 'active' ('list<...>* const c_extents = &extents;' etc., uses through them): sceneNode right, 39 -> 31.
  (2) IsExcluded 'lo' declared before 'hi': 29. (3) Pairs' loop written OUT of the inline, directly in SweepExtents' start
  branch (IsExcluded stays inline): all volatile regs right; then 'hi' before 'lo' again: 7 real.
- Left: node (orig r31) vs the CSE'd active.m_data temp (orig r30, 'mr r30,r28', reused by push_back's stwx); ours swapped.
  Inert: it at function/if/for scope, it first, node first, direct m_data reads for it/end, named 'data' local (const or
  not), node assigned before the for / before UseBuffer. sv10.py: 1529 of 5040 decl orders of the 7 top locals, best still 14 diff (7 real); stopped.
- Intermediate: Pairs(..., ISceneNode**& it) with it an outer local fixed node vs sceneNode too (25 real) - superseded.
