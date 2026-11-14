# big2 (2026-11-12): UpdateAnimation__14CSoldierObjectFf, follow-up on big1's 16-line draft

No improvement: still 16 (d<0=5, cross=11). base.cpp = big1/t1.cpp = research/drafts/UpdateAnimation.800acfb8.cpp.
Tools: var2.py (big1 var.py + cross colour table), who2.py (finds the cross block by pattern, works when length changes),
d.py <src> a b (side-by-side lines a..b), scv.py <src> <compiler versions...>, scd.py (deferred/RTTI flags), g1-g3.py grids.

Analysis (cross knot): final schedule is identical, only colours differ. Assuming CW colours highest vreg first with the lowest
free FPR, the colouring order is orig rx,dz,dx,ry,dy vs ours dz,dx,ry,dy,rx (rz takes f2 in both), i.e. right.x is the LAST-created
load in the orig and the FIRST in ours. The d<0 knot is the same kind of thing: orig colours dir.x (block-local) before front.x
(cross-block CSE'd value), ours colours front.x first.

Inert (cross stays 11, rx=f8 every time):
- Rising Sun forms (src/script/adjust_aim.cpp has exactly 'Cross(right, direction, up); Normalize(right); Cross(up, right, direction)'
  with a temp-result static inline Cross and a Normalize with 'inverse' local): both crosses, either one, member temp-result form.
- Cross2 as separate inline: 6 statement orders x 3 forms (direct, CVector3 temp, float temps) (g1.py); Set(x,y,z) arg form;
  float* / p[i] array access; pointer params; right.CrossTo(dir, up) out-param members.
- Hoisting float decls (blink/t/scale/e) in all positions/orders (g2.py, 95 compiles): cross never moves.
- Compiler: -inline deferred,auto, -RTTI on, versions 1.3.2r / 2.0: identical.
- Removing the first cross (diagnostic) still rx=f8.
Worse: return-by-value Cross (frame changes), CVector3(0,0,1) temp for the first cross (orig stores up), dir/right/up as an
array or struct (frame changes), up-Normalize Length term orders (move cross colours but never to the orig's).
d<0 inert: mixed Dot call forms in cond/block, operator*(v, k) forms, (-2*d)*v orderings, front *= k (adds stores),
setup statement orders (g3.py: current order best), scale via member/free inline helpers.

Parked at 16. Also tried: the whole g_bConstraint block as an inline member ConstrainDir(dir) -> CW does not inline it (-189 lines). Next ideas not tried:
whole if (g_bConstraint) block (e.g. a static inline helper holding the constraint with dir passed by reference).
