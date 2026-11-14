# sweep2 notes (2026-11-12, FP compound / Mul-inline sweep over geometry knots)

Tool: tools/rd.py <tmpl json> <src> <mangled> <addr> <size> [-v] = register-exact difflib line diff
(counts relocation-only lines too: lbz/lwz r13 SDA and lis/addi pairs show as diffs; subtract them).

## Results
- CVolCapsule::GetExtents 0x800c555c: IMPROVED 9 -> 4 (score.real) = cex/BEST4.cpp. Both blocks written uniformly as
  'float ax = SubV(a.d.v[0], radius); ... ax = ax < bx ? ax : bx; min.d.v[0] = ax;' with a value-returning
  'inline float SubV(float a, float b) { return a - b; }' (AddV for the max block) on ALL six terms. Block 1 (min) is now
  exact. Left: block 2 (max) a.x load f1 vs orig f2 and b's last lfd 40(r3) f2 vs orig f1 (copy-load colouring).
  Inert for block 2: 3^6 AddV/AddC/plain per term x 2 compare forms, all 720 decl orders, per-axis interleaved
  decl+select (45+), Min/Max inlines (75+), a/b copy order swap (12). research/drafts copy not updated.
- _find_aim_blend_values: stays 5. Compound Cross2 bodies / Mul(k, ...) all break fmadd fusion (69-99).
  NEW clue: a value-returning 'NegS(y)' (return -a) for wc's -y fixes case 3 (block 6 = 0) but breaks the
  CSE with the compare's -y in cases 1/4 (32 total). Neg in compares too: no help. Extra inline level per case: inert.
- CVolBox::TestCollision(CVolSphere): stays (rd 20). Compound Dot bodies (worse), Abs/AbsF3 inline forms x orders: inert.
- CVolBox::GetExtents: Mul()/MulV() products (both operand orders) 41-78 vs 41: inert (schedule knot, not operand order).
- CQuaternion::Multiply: 3^6 Mul/MulV/plain on the six cross-scale products: 27 (from 28); 3000 random statement
  order x form samples: 27. Not kept.
- CStaticMesh::DrawNode: per-axis Dot bodies (7 forms ^3, incl. compound r += and z-first) and in-place
  'double f = Dot(); f = fabs(f);' forms x 6 orders: inert at 4 real.
- CThrownObject::Init: Length() bodies (Sq/MulC/SqC/local copies per term, LengthSq/Dot helpers, compound sums): 16 best
  (= draft), x/z load colour swap in all three Length blocks unchanged.
- CAnimObject::ConstrainVelocity 0x80093e64: IMPROVED 24 -> 16 (score.real) = cvel/BEST16.cpp. The tail 'dx = ux * 1.05f' compiled
  as 'fmuls fN,const,u'; 'Mul(u, 1.05f)' (compound-copy inline) gives the orig's in-place 'fmuls fU,fU,const' (m_131.cpp = 17
  with only that change). Then a hill climb over solved-block statement order x Mul forms (g2.py/g5.py) reached 16. Left: the
  colouring of the six products/sums in the solved block, and dc (f11 orig) vs bd (f8) in the det block (Mul/compound/
  operand-order forms of dc, bd, det: inert). Vector-operator forms: 180+.
- CStaticObject::OnCollision: SubC/SubV/Cp value inlines per motion component x 6 orders x split: stays 3 real.
- CMatrix::Rotate: hill climbs over BuildRot order x Mul/MulV x operand swaps (~5k): 14 -> 13 real (rot/k_20.cpp, not kept).
  Knot = the 1.0f constant (t = 1 - c, reused for m[3][3]) in f8 (orig) vs f6; 't = 1.0f; t -= c', 'one' local, '-c + 1': inert.
- aim_cb, _matrix_rot_yzx (rd 145), TestCollision(CVolCapsule): looked only, too far for this trick.

## Handoff (sweep2, ended ~2.9 h)
No landings. Drafts updated under the lock (research/drafts + index.tsv): GetExtents 800c555c 9 -> 4, ConstrainVelocity 24 -> 16.
Nothing running, no claims held. Best next try: ConstrainVelocity solved block (statement order + Mul at 16) with the permuter;
capsule GetExtents max-block copy-load colouring (4).

## POD CVector3 lever (coordinator request, after handoff)
Tried the no-constructor view (fields + pair-copy operator=, values built field-wise) on the geometry knots:
- CThrownObject::Init: 16 -> 16 (POD + pair operator=, assignment after declaration, implicit copy); with a plain Length body 36.
  The original copies frame vectors through stack (lfd/stfd then lfs), so it is NOT the register-kept shape.
- CStaticMesh::DrawNode: 11 rd / 4 real, unchanged in 3 views. _matrix_rot_yzx: 145, unchanged.
- aim_cb: 67 -> 255 (structural 4 -> 240), field-wise operators. CVolBox::TestCollision(CVolSphere): 20 -> 174 (Make()).
- ConstrainVelocity and capsule GetExtents views already have no constructors.
Conclusion: these originals keep vectors in stack copies (address-taken / ctor-split forms); the lever does not apply here.
