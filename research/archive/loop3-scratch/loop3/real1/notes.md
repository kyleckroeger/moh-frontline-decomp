# real1 notes
- 3a9096c2: matrix_ps += CMatrix::Orthonormalize (688, prepended, 0x8008f1e8), ctor-less view with Scale/Sub/Dot/Normalize members.
- 7a325e0f: SetDifference -> CVector3::Sub, free Dot -> CVector3::Dot in 13 units (objects identical) and 4 drafts.
- Tools: xform.py (helper -> member rewrite), cmp.py (compile old/new source, compare objdump -dr -s).
- Left: three-argument SetSum in the CCDBTriangle-capsule draft (no two-argument form; could be point = v0; point.Add(sv), untested).
