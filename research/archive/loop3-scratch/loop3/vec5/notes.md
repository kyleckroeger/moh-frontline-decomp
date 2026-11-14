# vec5 (2026-11-12): UpdateBoundingBox + RenderSystem, continuing vec3
Tools (copied from vec3, paths fixed): sc.py, rw.py, rw2.py (rw + per-line toggles sa.d.v[2]<->saz, sv.d.v[2]<->svz,
Mul(t2, sa.d.v, i)<->Mul2), rw3.py (rw2 over SEVERAL blocks: extra 'start|||end' marker args, e.g. an inline's body
and the caller block), hcd.py, who.py (prints which FPR holds t/sax..svz and the k/t2/a2 products in UBB's block 2,
next to the orig), loc.py (2^6 sweep of 'float svx = sv.d.v[0]' style locals), probe.py/probe2.py (tiny colouring probes).

## UpdateBoundingBox: 58 -> 56 real, structural 0 (vec5/UBB_BEST56.cpp, alias-free, no new forms)
Target colouring (orig, block 2): k f3, sa.x f4, t f8, t2 f10, sv.x f11, sv.y f12, sa.y f13 | sa.z f31, sv.z f30, a2 f29.
- UBB_BEST56 = BEST58 + 'float svz; float saz;' locals assigned right after GetSystemAcceleration ('svz = sv.d.v[2];
  saz = sa.d.v[2];', uses in block 2 through them) + hcd decl move (svz after t2). Gets sv.z f30 and a2 f29 right;
  t2 still takes f31 where the orig has sa.z (sa.z sits in f10).
- Whack-a-mole seen everywhere in the outer-locals family: whichever of t / t2 is a DECLARED local of the function
  overflows into f31/f30. Writing t2 as '(t * t)' everywhere (CSE temp) makes t2 volatile but then t takes f31; locals for
  sv.z/sa.z declared before t make t volatile but t2 takes f31. 'const float t', a scoped t, a local t passed to an inline:
  all 72.
- NEW FAMILY (vec5/UBB_KIN64.cpp, 64 real, struct 0): an inline 'Kin(CVector3& p, float& vx, float& vy, float& vz,
  const CVector3& v, const CVector3& a, float t)' computing start (t v + t^2 a) and the velocity (2t a + v), called with
  the EXPRESSION 'Kin(start, vx, vy, vz, sv, sa, (m_updateTicks - m_startTicks) / 60.0f)', no t local. This is the only
  form found that gives EXACTLY the orig callee set (sa.z f31, sv.z f30, a2 f29) with t, t2, k volatile. Left: the
  volatile permutation (sa.x f11 vs f4, t f9 vs f8, t2 f8 vs f10, sv.x f7 vs f11, sv.y/sa.y swapped) and the prologue
  (orig keeps updateTicks in f4, t numerator f3, 60.0 in f2; ours f3/f2/f1). Kin's 'k = Mul2(2.0f, t)' is load-bearing
  (plain '2.0f * t' 106). Kin param order inert; vel as a CVector3 adds a 4th callee FPR (struct 12); caller decl
  order matters ('float vz;' must come after ex/ey/ez: 12 -> 0). rw3 over Kin body + caller: 5000 iterations stuck at 64.
  A per-axis 'Adv(p, v, a, t, t2)' inline for the end point (inl_M.cpp) gives ALL six loads in their orig registers
  (sa.x aside) but struct 6; random walks over Kin+Adv+caller reach struct 0 only at 84+.
- Next: the K family's volatile order. Ideas not yet tried: t computed inside Kin from both tick members
  (Kin(..., m_updateTicks, m_startTicks)), age passed the same way to an end-point inline taking the 3 axes, a Kin that
  also writes the end point. Permuter is impractical (the lever lives in inline boundaries the permuter flattens).

## RenderSystem: not worked beyond probes (vec3/RS_BEST514.cpp still best)
- Can only land with the whole particlesystem file (rodata/bss block base, autopilot.md 2026-11-02), so it is not a
  single-function landing anyway.
- Frame 1488 vs 1504: scalar temp vectors (force/offset via a named temp, acc hoisted, axis declared with right/up) are
  scalarized and leave the frame at 1488; the missing 16 bytes need an address-taken (pinned) object.
Nothing held, nothing running.
