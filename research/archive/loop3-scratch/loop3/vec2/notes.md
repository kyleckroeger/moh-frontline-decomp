# vec2 notes (2026-11-12, POD CVector3 lever on the geometry lane)

Scorer: scratch/loop3/sweep2/tools/rd.py (register-exact line diff), per-dir run.py wrappers.

## Symptom check (coordinator: apply the lever only where the orig shows it)
- _find_aim_blend_values: leaf, no stack vectors. V2-struct forms 70-249 (vs 5). Skipped.
- CQuaternion::Multiply: leaf, no calls; vector-op POD form 102 (vs 28). Skipped.
- CMatrix::Rotate: f31 = the angle param across calls, axis reloaded from its by-value stack copy. Skipped.
- CVolBox::GetExtents: leaf, no stack. Skipped.
- CVolBox::TestCollision(CVolCapsule): orig keeps start/end copies in stack (lfd/stfd then lfs) = opposite
  shape; ctor-less view 145 -> 199 (with or without pair operator=). Skipped.
- sweep2 (released) did CThrownObject::Init, capsule GetExtents, TestCollision(CVolSphere), DrawNode,
  ConstrainVelocity, aim_cb, _matrix_rot_yzx: inert or worse.

## CMatrix::Orthonormalize 0x8008f1e8 (688, lands by prepending to matrix_ps.cpp): 21 -> 7
Found by scanning the geometry rows for 'fmadds x,x' on stack-loaded fields (Orthonormalize: 7 such).
orth/BEST7.cpp. Two changes to the up (third) Gram-Schmidt step:
1. build u field-wise as 'up - right*dotRight', then subtract the forward part IN PLACE
   ('u.d.c.x -= fx;' ...): 21 -> 17. (Field-wise 'up - r - f' in one expression = 21; Set/ctor same.)
2. Mul(a, b) compound-copy inline (sweep2 trick) on fz, ry, rz: 17 -> 7.
Ctors in the view don't matter here (7 either way); the landed matrix_ps view is already ctor-less.
Inert at 7: statement order (600 random + 1500-step hill climb over order x Mul forms x Dot operand
order), product operand order, in-place order, Dot spelled as DotV/member. Worse: DotC compound, inline
Mul at use sites (15+), up modified in place (115), second Set (18).
Left: dotRight's final fmadds lands in f5 (orig f4) and the fz product schedules one slot early.
Permuter (orth/perm, -j2, ~10k iterations): only 'u.z -= Mul(dotForward, f.z)' inline (fixes the fz slot but rd 12);
Mul grid on top of it (g9, 729 forms): 12 best. Stopped. Draft updated (research/drafts, score.real 7).

## CAnimObject::ConstrainVelocity: permuter from sweep2 BEST16 (cv/perm, -j2, ~20k iterations, 25 min)
Base 140 -> 130 only: 'uz = sz + tz' (cv/BEST_p130.cpp); rd stays 17. Stopped.

## CVolBox::TestCollision(CPlane) 0x800c15ec (2292; coordinator lead: orig 11 callee FPRs vs ours 10): 547 -> 149 score.real
tcp/BEST154.cpp (rd 154). 1. ctor-less view (drop 'CVector3() {}' and the pair copy ctor): rd 375 -> 279 (pair operator= same).
2. 'contact.m_separation = plane.Project2(corner, point); contact.m_vector1 = point;' (orig copies 416 -> 448): 279 -> 154.
The 'Id()' copy helper in Project2 is no longer needed (same 154); dropped. Instruction streams now align 1:1;
left = colouring: orig a=(f7,f8,f9) b=(f10..f12) c.x volatile f13, c.y/c.z f31/f30, corner.x f29; ours shifts a/b up one and
puts c in f31/f30/f29. Inert: 500-step hill climb over decl/Scale/-= order, Scale operand order, corner copy-init vs assign;
Project2 'k = -distance' / 'q = p' forms. Draft updated. Next: Negate/Scale as natural operators, the STEP macro written out.
