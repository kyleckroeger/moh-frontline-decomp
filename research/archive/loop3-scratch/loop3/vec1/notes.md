# vec1 (2026-11-12): POD-vector lever outside the geometry lane
Tools: d.sh (diff rows with context), probe.py <cpp> (compile with the RenderSystem template and dump), gen_pin.py (pin
probes), hc_rs.py <src> <passes> (RenderSystem index/field hill climb; ~0.4 s per compile).

LANDED: CThrownObject::Init 856 B, 9b775aa0 (NEW thrown_obj_init fragment, .sdata2 pooled): ctor-less local CVector3 +
plain Length body (t3.cpp). thrown_obj_cleanup stays on the shared header (coordinator).

Looked at / parked:
- CStaticObject::OnCollision (3 = 1 real row, point.x reload f7 vs f8): impact-style CVector3 (with/without pair
  operator=), Sub/-= members, operator- by value, dx locals, point declared/assigned/reference: 3 or worse. No signal.
- PlayerScreenFlash (39): bytes, not FPR; CColor without operator=, aggregate-init BSArgColor, named to/from locals: 39-96.
- CScene::Draw (49): no FPR signal; GPR (camera+192 address hoisted before the screen call, frame 416 vs 432). Not worked.
- EndDraw (11): CColor byte copy; sweep1 did 100 forms. Not worked.
- ConstrainVelocity / DrawNode: sweep2's (geometry), views already ctor-less. Skipped.
- UpdateBoundingBox: alias-free pin via ((float*)this)[i] operator[] = 101 (same as the declined alias form; ubb_a.cpp).
  Ctor-less view (Set instead of the 3-arg ctor) 703 - keep the ctors. Left = FPR colouring in the second block (rows 443+).
- RenderSystem: alias-free pin 943 -> 711 structural (rs_m1.cpp: index spelling on the 18 vectors, CMatrix33 view
  (inferred name) for the 3x3 rotation), hill climb 696 (rs_hc_best.cpp). Remaining hunks spread over setup + loop.
- RenderSystem cont.: colour ranges as 'birthRange.Sub(CVector3(c1.r, c1.g, c1.b), birth)' (3-arg ctor temp; the orig's
  b,g,r conversion order for c1/c3 suggests ctor args) 696 -> 671 (k1.cpp); setup-block statement-order climb
  (hc_ord.py) 644 = BEST: vec1/rs_ord_best.cpp (real 975). Remaining: setup scheduling + loop body. Still no declined forms
  in it apart from the inferred CMatrix33 3x3 view name (question: is an inferred 3x3 class OK, or should it be CMatrix?).
Nothing held, nothing running.
