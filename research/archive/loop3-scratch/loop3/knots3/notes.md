# knots3 (2026-11-12): knots2's vector-locals / in-place recipes on the parked FP extents and collision knots

Result: no landing, no draft improved. All nine are brchk-clean (branch targets and lengths equal). Scores here are
`real` from research/tools/score.py (tool: knots3/sc.py KEY file...). Nothing running; no draft files touched.

Tools: knots3/sc.py (real score by index.tsv key); knots3/thc.py KEY file a b seed N [split] = generic order hill climb
of lines a..b (identifier def/use deps; split=1 hoists 'T x = e;' decls as free tokens). bge/hc.py, orth/hc.py, orth/dhc.py
(decl-at-top order climb), fab/g.py, rot/g.py (scores Rotate inline AND landed out-of-line BuildRot together).

| function | draft | without wrappers | recipe results |
| --- | --- | --- | --- |
| CVolBox::GetExtents | 37 | 37 | half-extent locals / basis-row locals (hc 600 its x2): 38+; min via 'q = c.x; q -= ..' 60-62; 'qx = c.x - xx' identical |
| CMatrix::Orthonormalize | 7 (Mul, identity 'fw/uv' pointers) | 17 (Mul -> a*b) | up/right/f components as locals 19-175; u via locals+in-place 101-106; 'float r = a; r *= b;' instead of Mul 60; decl-at-top order climb (600): 17 flat; statement climb: 17 flat |
| CAnimObject::ConstrainVelocity | 14 (Mul) | 24 | velocity comps as locals 24 (identical); 'vx -= dx; m.x = vx' 41; in-place 'r = a; r *= b' 39; split order climb 500: flat |
| _find_aim_blend_values | 5 (chained 'blend[0] = blend[1] = ... = 0') | - | difference locals inside TriangleBlend, all 720 orders x k position: 113 every one |
| CMatrix::Rotate | 14 (index) | 24 with the LANDED BuildRot body | axis.x/y/z locals in BuildRot (18 placements): Rotate 41-88 and out-of-line BuildRot 24-75 (landed is exact): dead |
| CCDBTriangle::TestCollision(CVolCapsule) | 7 (mid4 final, being committed by mid4) | 7 | sum via locals / in-place 18-21; e0 comp locals 7; 'float s = fS' 7; Set(x,y,z) inline 21-24; X()/Y()/Z() getters 7. Only MulV (declined) gives 0 (mid4/tc/l_tv.cpp) |
| CVolBox::TestCollision(CVolSphere) | 13 | 13 | diff as floats 13; local via floats 19; center comps locals 13; statement/decl climb 135-156: flat. Knot: local.y f7 <-> a0 f5 swap |
| CVolBox::TestCollision(CVolCapsule) | 102 | - | 'result == 2' / result-only forms identical; BoxPoint before Evaluate 101-134 |
| CVolBox::TestCollision(CPlane) | 52 (MulV everywhere) | - | not tried beyond reading: MulV + doubled 'corner op v' forms, far from landable |

Reading: the recipes help where a reloaded vector's in-place sum reuses the load register (CCDBObject::Init). In these
knots the mismatches are product/temporary colourings that only value-returning inline temps (Mul/MulV) reproduce;
locals or in-place compound statements do not. Rotate's index score (14) is from a hill-climbed BuildRot body; with the
landed body it is 24, so Rotate is further than the index shows.
