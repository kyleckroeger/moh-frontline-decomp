
## rules2 (2026-11-13; scratch/loop3/rules2, probes p/c1-c4.cpp)
- Commutative operand order is NOT source order in general. CW 1.3.2 puts the lighter operand first:
  leaf (variable, parameter, loaded member) < expression tree < call result. 'h*a*x' emits
  'fmuls d,x,(h*a)' whichever way it is written; 'h*a*sq(x)' emits 'fmuls d,(h*a),sq'; 'sq(x)*h' emits
  'fmuls d,h,sq'. Two leaves keep SOURCE order ('x*ha' and 'ha*x' differ). Reading the original:
  'fmuls d,(tree),(value)' means that value was itself a tree (e.g. 'nx * inv' written inline, CSE'd)
  or a call result, not a variable; 'fmuls d,k,mem' where ours has 'fmuls d,mem,k' is a plain source
  swap. UpdateBoundingBox's moving block: scalar-first spellings ('k * sa.x', 'age * vx', 'a2 * sa.x',
  'ex + ax') give the orig operand order on every FP op, leaving a pure FPR permutation (real 56 -> 60,
  not applied). CVolBox::TestCollision(CVolCapsule): the orig's '(h*axis.c) * n' product-first order
  comes back with '(nx * inv)' inline (tc9) or scaled half-axis variables (tc11), but recoloured (worse).
- Strength-reduced IV temporaries rank BELOW the parameters (DMClusterSkelTraverseHeirarchy:
  '_scratchMats[k]' with k eliminated puts the pointer at r21 under all params). The orig there has
  scratch/depth/a/b/c below the params, but depth/a/b/c cannot be IVs: still open.
- SNDI_precalcaztospkrvol with ONE helper for both branches: a branch-local copy of the argument that
  must be created first ('{ int d = azimuth - az[n-1]; pan(.., d, ..); }') reproduces what the second
  helper's swapped parameters did (14 -> 4, same as the declined two-helper form).
- ds2.py (rules2): the one-line move sweep generalised to any line range (inner-block declaration
  blocks, statement runs). UpdateBoundingBox's 20 inner float declarations: inert (base and operand-fixed).
