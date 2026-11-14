# real2 (2026-11-12): pass over unlanded drafts with invented vector helpers -> the original's CVector3 members
Scorer: scratch/loop3/names1/vr.py KEY base.cpp variants.py. Variant files here: bp.py bs.py bc.py ge.py ubb.py tb.py oc.py dc.py cfo.py.
LANDED: CVolBox::TestCollision(CPlane) 52 -> 0, 4e2dff8f (box_testtriangle += 2292). Key: a.Scale(axis, half) x3, then
  contact.m_vector0.Negate(normal) BEFORE 'corner = m_center; corner.Sub(corner, a/b/c)' (after: 149), corner walk Add/Sub(corner, v).
Renamed, score unchanged (draft commit after 4e2dff8f): box-sphere 13, box-capsule 102, UpdateBoundingBox 101, TestCDBBranchNodeCollision<CLine3> 1,
  CStaticObject::OnCollision 3, death_cb 5, CheckForObstructions 25.
Not improved / not kept:
- CVolBox::GetExtents (37): Scale+Abs+Sub/Add forms 53-96. Orig max side is center.x + xx, then yx + (..), zx + (..) (operand order flips after the first add).
- box-capsule: n = p - center as CVector3 (Sub, Scale(n, inv)) 127-560; Evaluate/BoxPoint as Scale/Add 175-197.
- UpdateBoundingBox: Mul stays load-bearing (acc.Scale + start.Add: 103); vel as vector 735; m_center.Scale(m_center, 0.5f) 104 (orig is x *= 0.5); m_min = lo; Add 172.
- box-sphere: local Dot order x,y,z 29 (z,y,x is 13); Abs member same 13 (kept).
- OnCollision: motion.Sub(point, local) 33; motion = point copy + per-component 192.
Skipped: GenerateTestSplinePath (partial draft, unscored; obvious Sub/Scale/Add spots in the tridiagonal sweep), RenderSystem (q10),
  CheckLineCollision (CAIFilterRealPosition, not CVector3), real1/undo1 holds (ConstrainVelocity, CCDBTriangle-capsule, FindIntersection, CCDBVolBox-triangle).
