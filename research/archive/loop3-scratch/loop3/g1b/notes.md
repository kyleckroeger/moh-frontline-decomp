# g1b (2026-11-13, day-2 group 1, rows g1 didn't reach)

LANDED 16d5b52a: animmorph_dtor += CAnimMorph::Draw (424 B, prepended at 0x800653c0; unit onto include/game/CVector3.h).
  Key: the shared header's REFERENCE operator= + the accepted Assign inline copying through a local
  ('inline void Assign(CVector3& d, const CVector3& s) { CVector3 t = s; d = t; }'). The dead stack copy at the
  orig's last slot is then the inline local, created before left's second half is stored (by-value op= creates its
  return temp after the whole assignment: left.zw store came first, 3 lines). By-value Assign param: 12.
  Probes: am_*.cpp, h_*.cpp, merged.cpp.

No change (all stay at index score; nothing committed):
- CMatrix::Rotate (landed BuildRot body: 24 here, 21 with fpr2's body): Rotate-side forms (const params, direct
  PSMultMatrix, axis/angle copies) and the shared CVector3 header (base-class copy ctor) all inert. Pure FPR
  permutation (orig 1.0f f8 / ax f12 / ay f9 / s f5 / ty f6; ours f6/f5/f8/f12/f13). rot/t.py = scorer.
- DMClusterSkelTraverseHeirarchy (31): params stay the LOWEST callee regs under: block scope for scratch/depth/a/b/c,
  a/b/c declared in the loop, a/b/c as by-value params of an inline holding the loop body, 'register' on either
  group or on the params. dmc/.
- PROFILE_UPDATE (82): six min/max as 'short&' references: worse (130 vs 124 sbs). Orig keeps hist + the six
  pointers in VOLATILE r12/r7..r11 and the inner temps/300 count in callee r25-r27/r30; ours the reverse. pu/.
- BIFunc_MechFollowPath: inspected only (scripting lane's 1000+ arg-read forms cover it).
- CVolBox::TestCollision(CVolCapsule) (102): new detail: after the 'code > m_result' update the orig tests
  'cmpwi r30,2' (result, the callee copy) where ours tests r3 (code). Raise() inline with by-value code, no code
  local, split 'if (flag)' blocks: no (split ifs restructure, 185). bcp/.
- PATHI_eventstart: inspected only (scalar-replaced saved-field temp ranks; no rule fits).
- BIFunc_GetObjInfo (16, ArgObject accepted per coordinator): filter r27 vs ARG(2) offset CSE temp r26 swap.
  Inert: ArgObject param orders (6), body spellings (9; most collapse to 1500+), ArgNode/ArgObject also in cases
  29/30, ArgInt/ArgFloat siblings for ARG(1)/ARG(3), target statement at each of 10 init positions (49-51).
  Note: case 3's 'else filter = 0;' is load-bearing (without it 1534: the shared li with iresult's store breaks).
  varmap: isFloat r31, self r30, stack r29, target r28, basic r25, node r24 (degree order, not decl order). goi/.
- CStaticObjectFactory::Init (138): ours count r31 / i-IV r23.. vs orig count r26, IV r27, r r28: a +4 rotation
  with count on top in ours. 'int count' at function top (before/after r): inert. init/.
- DrawBulletStats: not reached (multiplayer, last).
Nothing held.
