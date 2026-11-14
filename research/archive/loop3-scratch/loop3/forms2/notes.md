# forms2 (2026-11-12): CAnimMorph::Draw Assign draft, RenderSystem RS_BEST514 draft, Rotate fpr2 draft
Tools: var.py/var2.py/var3.py (replacement grids on am_c2/am3), am_s.py (ohc statement-order spec), cd.py
(check_draft compare() on a scratch file), brf.py (brchk on one function + file), mkrot.py (Rotate draft builder).

- CAnimMorph::Draw: am3.cpp = research draft, real 3, consistent (Assign inline, inferred, ruling a). Last 3 lines:
  orig stores left.zw (stfd f3,144) after the op= temp (24/32); ours stores it first. Inert at 3: Assign spellings
  (member Set, operator=(), (void), pointer, arg order, non-const/ref aliases), op= bodies (pair halves, double casts,
  return other, CVector3Pair& alias), copy-ctor bodies (pair halves, double casts), 7 left/right statement orders,
  ohc statement climb over the whole TransformPoint..dx block (2 seeds, plateau 3). Worse: pair halves reversed (9-10),
  Assign by value (70), op= via named temp (71-73), member-wise copy ctor (120). Parked at 3.
- RenderSystem: rs514.cpp (= vec3/RS_BEST514 + header + CMatrix33 inferred comment) real 835 vs 1064. check_draft:
  'differs' for BOTH old and new, same extras (0.0f x3, 1.0f x4, 8-byte access at +0x8) -> pre-existing, not new.
  brchk: old draft 6 branch-target diffs, new 1 (277: b +4400 vs +4364).
- Rotate: rot21.cpp = draft with fpr2 rv/best1 body + MulV (Mul from h.txt unused, left out); 21, consistent,
  BuildRot out of line stays 0 (fpr2 rot.py). 0 branch diffs.
