# rules2 (2026-11-13): late2c rows + rule sweep

Tools here: ds2.py (one-line move sweep over ANY block of lines L1..L2, not only leading decls),
probe.py (compile a toy and print its asm), sbsfull.py (side by side incl. equal lines),
pcg*.py (SNDI single-helper generators), tcgen.py, aimgen.py.

## Rows (all brchk 0 branch diffs)
- DMClusterSkelTraverseHeirarchy (31): dsweep inert (64 difflib). New datum: an index-addressed
  '_scratchMats[k]' (k eliminated by strength reduction) puts the SR pointer BELOW the params (r21),
  so compiler IV temps rank below params; the orig's scratch/depth/a/b/c are all below the params,
  but depth/a/b/c cannot be IVs. dm5.cpp 75. Open.
- UpdateBoundingBox (56): ds2 over the moving block's 20 float decls inert (112). Operand-order fix:
  ubb_all.cpp (scalar-first spellings 'k * sa.x', 'age * vx', 'a2 * sa.x', 'ex + ax') gives the
  orig operand order on every FP op of the block; what is left is a pure FPR permutation (orig t f8,
  t2 f10, sa.z f31; ours t f13, t2 f31). Real 60 (draft 56), not applied. ds2 on it: see below.
- TestCollision(CVolCapsule) (102): orig multiplies '(h*axis.c) * n' with the PRODUCT first; ours
  puts n first whatever the source order (tc1 = same code). Normalised components written inline as
  '(nx * inv)' (tc9) or scaled half-axes as variables (tc11) give the orig operand order but recolour
  (234-276 difflib vs 194). Not applied.
- aim_cb (59), _AnimTranDoTransition (185), _matrix_rot_yzx (109): dsweep inert; natural-order
  PitchRotate forms worse (142-206 vs 118). Not applied.

## Rule: commutative operand order (probes p/c1-c4.cpp)
CW 1.3.2 orders the two operands of a commutative FP op by weight, not by source:
leaf (variable, parameter, loaded member) < expression tree < call result. 'h*a*x' emits
'fmuls d,x,(h*a)'; 'h*a*sq(x)' emits 'fmuls d,(h*a),sq'; 'sq(x)*h' emits 'fmuls d,h,sq'.
Two leaves keep SOURCE order ('x*ha' vs 'ha*x' differ). So an orig 'fmuls d,(tree),(value)' means
that value was itself a tree/CSE (e.g. 'nx * inv' written inline) or a call result, and an orig
'fmuls d,k,mem' vs ours 'fmuls d,mem,k' is fixed by swapping the source operands.

## SNDI_precalcaztospkrvol (coordinator: one helper only)
ONE helper 'pan(p, q, x, y) { *p = shape(x*127/(y+x)); *q = shape(y*127/(y+x)); }' for both
branches, branch 2 as '{ int d = azimuth - az[n-1]; pan(&row[0], &row[n-1], d, 0x10000 - azimuth + az[0]); }'
= real 4 (difflib 6), the same as the declined two-helper form. Without the 'd' local: 14.
Left: branch 2's sum takes r31 (shared with the second quotient), orig r30. Tried: 24 param orders,
5-arg helper with the sum passed in (46+), 8 helper bodies, 8 branch-2 local spellings: no 0.
Draft p/pc5_l3.c.

## Index/IV rule sweep (step 2)
- SNDPKTPLAY_start (2): loudest loop: 'v[i + 2]' on '(short*)&voices[slot]' (spk_c.cpp) gives the
  orig 'lha r0,4(r7)' but not the 'addi r7,r29,0' IV copy (5 difflib, length 202). Integer-offset
  and array-index spellings 5-47. Not applied.
- quick_sort_<UNodePair> (6): tail loop forms 12-112 (next r27 vs r29). YUV_WriteRow: scheduling.
- No other small draft has a per-iteration '+= K' accumulator next to pointer IVs (grep).
