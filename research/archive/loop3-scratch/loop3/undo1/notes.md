# undo1 notes (2026-11-12)
Tools: fv.py (region-replace variants, fast.py score), fw.py (whole-source variants, fast), fs.py (whole-source, score.real).

## Landed
- bac20c93 Natural form: CVolCapsule::GetExtents without SubV/AddV (cap/): per block 'CVector3 start = m_axis.m_start;
  CVector3 s; s.x = start.x - radius; ...; CVector3 end = ...; CVector3 e; ...; min.x = s.x < e.x ? s.x : e.x'.
  Fields of a local CVector3 behave like copy-defined temps. Ctor form 32 (ctor args evaluated z before y).
- AISplinePath_segment += GetClosestParameter (1056, prepend, port_unit --pool .sdata2 --original-file AISplinePath.cpp
  --file-index 2053): 'float end = dwi::min(a, b);' (ref-returning min, body 'a < b ? a : b'; SGI body 4-6).

## Parked
- CFont::DrawTextCenteredF Mul (font/): every plain/compound/reassigned/global-direct product x all decl orders of the
  5 locals: best 2 real lines; fmuls operand order is vw-first whenever the product's dest shares viewWidth's register.
  The bottom/right/top/left decl order is real (stack slots); `float viewX;` early decl sets viewX/width colours.
- TestCollision CCDBTriangle(CVolCapsule) (7, tc/): topo statement orders x300 >= 14; pre-declared product locals
  (subsets x 5 places) >= 10 (fixes py/pz, swaps fS/px); operator forms, Scale, field forms 42-56.
- FindIntersection (fi/): draft 323/2 -> 320/0 (fi/final.cpp): natural 'if (fabs(x) >= fabs(y))' (fixes the structural
  fmr), Eberly x-then-y orders in fA/fB/fC and the parallel block, 'fRoot = sqrtf(fDiscr)'. Uniform d.c.x spelling 343/0:
  the mixed spellings still steer 23 lines. Named kP/kD component locals inert. Deep FPR colouring left.
