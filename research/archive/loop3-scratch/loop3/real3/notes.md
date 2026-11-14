# real3 (2026-11-12): member-call / declaration order pass over real-member drafts
Tool: ohc.py (order hill climb) + gspec.py (generic spec from a block file) + reblock.py; variant files *.py with ../names1/vr.py.
Draft commit (land.sh): ConstrainVelocity 14 -> 6 (cv_b1.cpp), CCDBTriangle-capsule 7 -> 5 (tc_b1.cpp). No landings.
- TestCDBBranchNodeCollision<CLine3> (1): only __construct_array arg regs, not vector code; lane-lines s17/s18 probes exhaustive. Not worked.
- OnCollision CStaticObject (3): one colour (point.x reload f7 vs f8). motion Sub/copy+Sub/-=/Set forms x before/after away, decl
  positions, Normalize returning float, whole-block order climb: all >= 3 (Sub 33, copy ctor 347).
- death_cb (5): loop-init order of kept/located loops (comma/for/split forms) inert; no vector code.
- box-sphere (13): Dot statement permutations (z,y,x best), DotDotDot 93, Set/3-float ctor 19 (store order right, y/abs.x colours
  swap), Abs variants 90-326, decl/statement climb (ctor and base) inert.
- GetExtents (37): direct fabs expressions 108 (no CSE), component-major/vector-major float locals 55-56, Scale/Abs members with
  in-place / shared temp / per-axis temps 53-60, order climb 52. Max side '+ operand flip' is CW's normal output (our per-float
  draft emits it too), not a clue.
- ConstrainVelocity: now 6, all in the Mul solved block. p as CVector3 (Scale/Dot) with operator or Scale/Add/*= forms 40-243.
  Mul solved-block climb reaches 3 but needs Mul (declined) -> not committed.
- CCDBTriangle-capsule: now 5 (fS load vs first product colour). Scale for sv/tv 21-28, per-component sv without float locals 28.
- box-capsule (102): n-block decl/statement climb inert. Not further worked.
