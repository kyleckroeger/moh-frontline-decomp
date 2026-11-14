# vec4 notes (2026-11-12, continuing vec2's two geometry drafts)

Scorers: tcp/run.py, orth/run.py (rd = register-exact line diff, sweep2/tools/rd.py).

## CVolBox::TestCollision(CPlane) 0x800c15ec (2292): 149 -> 85 real (draft updated)
tcp/BEST85.cpp (= research/drafts/TestCollision.800c15ec.cpp).
1. 'inline float MulV(float a, float b) { return a * b; }' on ALL NINE half-axis products written field-wise
   ('a.d.c.x = MulV(m_half[0], m_axes[0].d.c.x);'): a=(f7,f8,f9), b=(f10..f12), c.x=f13, c.y/c.z=f31/f30 now as
   orig (154 -> 89). The compound Mul on all = 366, plain products field-wise = 154 (same as Scale()).
2. 'corner.d.c.y = SubV(corner.d.c.y, a.d.c.y)' (87), Negate member body in x,z,y field order (85).
Left: final corner.x lands in volatile f6 (orig callee f29) while -normal.y takes f29 (orig -normal = f21,f22,f23);
knock-on m_half[0]/m_half[2] load regs and the first GetDistance register block.
Inert: coordinate descent over 5 product forms x 3 subtraction forms per field (hc2.py); random hill climb over
statement order x forms (hc3/hc4, ~5.5k steps); all 120 decl orders of a,b,c,point,corner; Negate position (4) x
body form (-x, NegV, compound NegC, 0-x) x 6 orders; Scale/+=/-= body orders (g11, 1296); MulV/AddV/Dot forms in
GetDistance/Project2/Scale (v7, 150); 'first = corner' copy (permuter's only hint); c unnamed (413, recomputes).
Worse: in-place negate of a copy (311), operator- (524), Scale(-1.0f) (213), Negate before the products (153).
Permuter (tcp/perm, -j2, ~6k iterations, base 2010): only 'c.z product moved after the b subtractions' (rd 88) and
'copy corner before the first Project2' (rd 85 = same). Stopped.
Next ideas (untried): what makes -normal an early vreg (no-Negate probe puts the normal loads in f30/f31/f29:
the normal's colouring is pressure-driven); a Negate that reads the normal through the GetDistance loads
(e.g. Project2 returning distance and negating from the same const ref) or contact initialised after the loop.

## CMatrix::Orthonormalize 0x8008f1e8 (688): stays 7
Inert at 7: full 4^6 grid of product forms (plain a*b, b*a, Mul(a,b), Mul(b,a)) on fz,fx,fy,ry,rx,rz (g10);
MulV/SubV/SubC/compound forms on the u subtractions (v2); dotRight as accumulator/explicit/const/late/top decl
(v1, v3, v10: 1-3 top-declared floats in any order); free DotV; Dot term order x operand-swap for each dot (v5);
Normalize scale order x Length term order (v6, 36); product inlined at its use, 1-2 at a time x 3 forms (v7);
Prod3(x,y,z,v,k) helper (v8); dotRight unnamed / CSE'd (15-16, g12 grid on it: 15).
