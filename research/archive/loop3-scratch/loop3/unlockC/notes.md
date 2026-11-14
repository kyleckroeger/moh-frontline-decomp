# unlockC (2026-11-12): accepted MulV/Mul value products + operator[] pin
Landed: 62be44ba cdbgeom += CCDBTriangle::TestCollision(CVolCapsule) 0x8006a71c (6,920 B), MulV on sv products (tc/b.cpp).
Drafts (a437cf73): ConstrainVelocity 6 -> 2 (cv/draft.cpp; left sy/ty compute order + f6/f8 swap; ohc climbs of solved
block, all 720 top-decl orders, Mul/MulV/plain per product, Push() helper with 24 param orders x 4 bodies (best 6): flat).
UpdateBoundingBox 101 -> 56 (pending UBB_BEST56 promoted; Kin taking tick members: inert at 64).
CMatrix::Rotate: rot/rc.py climbs BuildRot order x {a*b,b*a,MulV,Mul,compound} with landed BuildRot kept exact: 24 -> 23 only.
Item 5: no other drafts depend only on Mul/MulV/SubV (DrawTextCentered is declined: per-fragment inline views).
Nothing running, nothing held.
