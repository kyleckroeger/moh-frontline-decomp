# big1 (2026-11-12): UpdateAnimation__14CSoldierObjectFf (3828 B)

Scorer: python3 scratch/loop3/big1/sc.py <src> -> real lines + per-knot counts (d<0 480-520, cross 600-640,
conv 700-780). var.py base.cpp vars.py runs replacement batches; who.py prints value->register tables.

- Start: research draft 59 real, fresh1 v7 58. brchk: len 957/957, 0 branch diffs (logic/branches fine).
- No parameter-copy workarounds in this function (only 'SkeletonView* skeleton = m_skeleton', which is needed: 901 without).
- conv knot (39 lines) SOLVED by grid.py (288 combos of the angle blend): targets as 'short dx = (int)(...) >> 8;'
  BEFORE 'short* angles = m_boneAngles[bone];', 'int x = angles[0]...', then per component
  'dx = dx - x; short sx = dx * t; angles[0] = sx + x;' -> g1.cpp 19.
- d<0 knot: 'if (flat.Dot(front) < 0.0f) { flat += (flat.Dot(front) * -2.0f) * front; ...}' (no d/k locals, Dot twice,
  CW CSEs it) puts k in f2 like the orig -> t1.cpp 16. Left: front.x f3 (orig f6) / dir.x f5 (orig f3).
- cross knot (up.Cross(right, dir), 11 lines): value->reg orig rx f4 dz f5 dx f6 ry f7 dy f8 rz f2, ours (t1) dz f4 dx f5
  ry f6 dy f7 rx f8 (rx coloured last instead of first). Inert: Cross statement order (breaks first cross), explicit
  call-site statements in all 6 orders, temps, by-value args, Normalize forms (scale local, return length, LengthSq),
  up init forms, declaration forms. Product operand swaps at the call site reach 12 total (cross 7) but change the
  fmuls operand order (not the original's); named load locals at the call site (p23/p29/p36: 720 + 556 orders/subsets)
  also stop at cross 7. Not solved.
- d<0 knot after 'twice': front.x f3 (orig f6), dir.x f5 (orig f3); orig colours all three front loads AFTER the
  block-local dir.x/flat reloads. Inert: Dot term orders/operand swaps (p24 48 forms), Reflect inlines, MulV, named
  front locals, scale forms (inline, local, order), operator* bodies, decl positions of scale/e, 'd' variable forms.
- LANDED 421241fe (drafts-only). Parked at 16: see knot notes above.
