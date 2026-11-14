# fpr2 (2026-11-12): FPR-order pass with value inlines
Tools: s.py (python3 s.py cv|ubb SRC [1] -> (real, lendiff) [+ diff rows]); rot.py (both(body, helpers) -> (Rotate, BuildRot)
real scores; Rotate compiled from ../unlockC/rot/landed.cpp, BuildRot from src/game/matrix_translate.cpp); rv/rc2.py
(climb: order x {a*b,b*a,MulV,Mul,compound,t-chain} x hoisted decl order x t-form x s/c decl order, BuildRot kept exact).

LANDED b9f2cf08: anim_obj_collide += CAnimObject::ConstrainVelocity (1,268 B). Fix from unlockC's 2: swap which y product
is CREATED first ('sy = Mul(b, bv.y)' where 'ty' was, 'ty = Mul(py, a)' where 'sy' was, 'uy = sy + ty'). Schedule already
matched; CW colours by creation order. (cvv.py/cvw.py: sum/dot inlines Lin/Add/AddR/Dot2 were all worse, 5-8.)

CMatrix::Rotate (landed BuildRot exact): 24 -> 21 (rv/best1.txt body + rv/h.txt helpers; rc2 seeds 1 and 2, 2500 its each
plateau at 21). Not committed: research/drafts/Rotate (14) assumes a BuildRot body that is 32 off out of line. Left: the
1.0f constant f6 vs orig f8 (held to the m[3][3] store) and s in f31 (orig f5); schedule identical. Inert: OneMinus/SubV/
'-c + 1.0f' for t; AddV/SubV value inlines in the 12 assignments (both functions); CVector3 Scale for t*axis breaks
BuildRot (27).

UpdateBoundingBox (56): inert/worse: t/age via Ticks()/TicksC() inlines, t2 via Sq/SqC/Mul2, k via Twice/Mul2 (432 combos,
min 56; ubbv.py); no t local (expression or Tk() at each use: 736, no CSE of the division); per-axis Pos(t, t2, v, a)/Vel
inlines (283-720); saz/svz assignment position (all 56: the copies coalesce). Parked; vec5's Kin family (64, exact callee
set) is still the most promising lever.
Nothing running, nothing held.
