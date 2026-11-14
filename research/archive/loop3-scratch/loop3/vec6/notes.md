# vec6 notes (2026-11-12, continuing vec4's CVolBox::TestCollision(CPlane))

Scorer: tcp/run.py (rd, register-exact line diff). Variant harness tcp/vh.py (BASE=file env), batches b1..b12.py,
hill climb tcp/hc.py (seed iters [state file]; tokens P products, S corner subtractions, D point, C corner,
K contact decl, N negation fields, F first Project2), parse.py (cpp -> state), nat.py (score named orders).

## CVolBox::TestCollision(CPlane) 0x800c15ec (2292): 85 -> 56 rd (check_draft: consistent, 53 lines)
tcp/BEST56.cpp, landed as the research draft in 99c836c2 (pushed). The lever: STATEMENT ORDER of the negated normal relative to the corner fields, which vec4's
searches never moved together (Negate was fixed, contact decl fixed). Order now:
  products a, b, c.x, c.y; point; corner = m_center; contact;
  corner.x -= a.x, b.x, c.x;  contact.m_vector0.Negate(plane.m_normal) [body x,y,z];
  c.z product;  corner.y -= a,b,c;  corner.z -= a,b,c;  first Project2.
SubV no longer needed (all plain '-='); Negate body x,y,z (x,z,y = 60 here); MulV on products still needed.
Hill climbs (4 seeds x 3-4k from 56, 2 random restarts) all stop at 56.
Left (all in the first block, lines 60-161): the orig's first-distance/separation is volatile f6 (shared with the
GetDistance accumulator) and corner.x final is callee f29; ours swaps them (sep f29, corner.x f6), which drags
corner.z-mid / -normal / the first GetDistance's p loads into other regs.
Inert/worse on the 56 base: separation in a local (398+), Negate after the loop / in the if (530-630), Negate via
CPlane member / pointer / by-value (by-value 635), GetDistance term orders (only x,y,z), Dot()/local forms,
Project2/GetDistance by const ref (576), 'first = corner' copy, named cx temp, view variants (union order, no aligned,
user pair operator=, double struct) all 56, a/b/c decl orders, contact decl at top, product forms per field.

## CMatrix::Orthonormalize: not touched (out of time; no idea beyond vec4's list).
