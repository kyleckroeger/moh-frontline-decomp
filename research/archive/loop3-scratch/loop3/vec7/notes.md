# vec7 notes (2026-11-12, CVolBox::TestCollision(CPlane) 0x800c15ec, continuing vec6)

Result: no landing, stays 56 rd (research draft = vec6 BEST56, 99c836c2). ~80 min.
Tools (vec7/tcp/): who.py FILE [a b] = value-register mismatch table (orig>ours pairs, opcode-aligned) over the first
block; w1..w8.py variant batches (BASE BEST56); hc2.py = vec6's hill climb + 'V' (vector1 copy) token + STEP1's six
'corner.x += a.x' field ops as tokens (after F) with +=/'x = x + a' forms. vec7/perm/ = permuter set up on BEST56
(base 1380, fixed prefix/base generation; ran ~3.6k iterations -j2: nothing below base).

The knot is ONE swap: orig corner.x final = f29 / separation (+ GetDistance accumulator) = f6; ours the reverse.
who.py on BEST56: f27>f6:11 f6>f24:7 f29>f28:7 ... f29>f6:3 f6>f29:3. NOTHING tried moved the f29>f6/f6>f29 pair:
- separation forms: Project2 'float distance; distance = ...', GetDistance via local sum / Dot (56), 'float k = -d' (56),
  float& alias (56), comma form (56), Project3 out-param distance into contact (286-382), Project4 = Proj(p,out,GetDistance(p))
  first only (56) / all (155), Proj(corner, point, plane.GetDistance(corner)) first (89), function-level 'float distance'
  reused by every step x 3 positions x 3 first-step forms (all 56).
- corner helpers: Sub3(float&, a, b, c) per field / x only, param orders (56); MinCorner(corner,a,b,c) (148, moves corner.x
  to callee f23 but a/b shift down one reg and sep stays callee); per-field index helper (755+).
- Negate positions after the first Project2 / inside STEP1 (394-486); before F (89).
- STEP1 field-wise orders (pairs/sequences, 6 perms each: 56-74), mixed operator/field forms (56).
- decl order of a,b,c,point,corner,contact at the top (61 perms: all 56), corner(m_center) / pair-wise copy (56).
- hc2 (2 seeds x 4k): 55 by junk knobs (hc2_best_1.cpp, 'x = x + a' mixes), key pair unchanged; not committed.
Read: sep/corner.x colouring is insensitive to every source form of the separation; the pair is decided elsewhere
(maybe the store-forwarded corner.x value's web, created by CSE). Next idea if anyone retries: probe what CW coalesces
corner.x final with (orig keeps f29 -> f27 -> f28 for corner.x, +a, +a; ours f6 -> f6 -> f27: ours coalesces the first add),
e.g. forms of STEP1 that read corner.x into a separate temp, or a GetDistance that reads the stored corner (by const ref) only
in the FIRST call while keeping the copy count. Otherwise park: 56 lines in one block, logic exact.
