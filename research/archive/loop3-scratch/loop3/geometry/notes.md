# geometry lane notes

## Agent 1 (2026-11-11 evening)

### Parks
- CVolCapsule::GetExtents 0x800c555c (304, claim released): draft = research/drafts/GetExtents.800c555c.cpp (score_draft 9, fast 18).
  Left: block-1 x pair colours (orig sx-r f4 / ex-r f5, ours f5/f4) + block-2 start.x load reg (f2 vs f1).
  Tried (scratch/loop3/geometry/cex/): all 720 definition orders x 5 x-compare forms of block 1 (gen.py b1; best 15 fast
  but loses the 'b' of the bge/b/fmr shape = worse), unrolled loop forms (106-110), lo/hi CVector3 + if/else stores (32),
  't = a; if (b < t) t = b' (104). Earlier: 2 h of permuter (lane-lines s16).
- CVolBox::TestCollision(const CVolSphere&) 0x800c3210 (1472, claim released): draft research/drafts/TestCollision.800c3210.cpp
  (score_draft 13, fast 28). Only diff: local.y dot result and a0 = |local.x| swap FPRs (orig y f7 / a0 f5, ours f5 / f7), plus
  the local store order (orig x,y,z; ours z,y,x). Tried (scratch/loop3/geometry/bsp/): local member orders x 6 abs orders x
  Abs inline x decl orders (gen.py), named float dots in all decl/def/store orders (gen2.py, worse), all dependency-safe
  interleavings of the 3 dots + 3 fabs (gen3.py), float-init forms, diff via Sub/operator-/component orders, DotDotDot + Abs
  (58-236), decomp-permuter 40 min (base 65, nothing; many type-map failures from opaque structs).
- CQuaternion::Multiply: vector (Cross/Dot/Scale helper) forms 68-121 vs draft 56 (fast). Not pursued.
- TestCollision(CVolCapsule) tcap 0x8006a71c: contact block point/tv/rv forms (168 variants, tcap/gen.py): none below the draft.

### Landed
- 9aa74087 cdbgeom_sphere += CCDBTriangle::GetNearestPoint (964): dots as inline CVector3::Dot calls, order b0, a01, a00, b1, a11.
- aim_cb 0x8005e9ec (fast 122): dot products as Dot()/free-inline calls in all 6 orders = identical or worse (gen.py); pitch block
  forms (cf/su order, x/y/z sum order, Set(cf+su), vector up -= (f-su)*t, b in xyz order; aim/gen2.py) none below 122.
- CVolCapsule::GetExtents with the shared CVector3 header (whole fragment): 143 (worse).
- CCDBObject::Init: looked only (13-FPR colouring in the extents block); not retried.
- CVolBox::GetExtents 0x800c13b4 (280): IMPROVED draft 60 -> 45 real lines = scratch/loop3/geometry/bge/BEST45.cpp (fast 62 vs 116):
  the nine |h*basis| floats declared in one 'float a, b, ...;' line in decl order [xx,xy,zz,yx,yy,zx,yz,zy,xz] and assigned in
  order [zx,yy,xx,yz,zz,xy,yx,xz,zy] (found by random order search bge/gen.py + hill climb bge/hc.py; 9! space, plateau 62).
  Left: product/fabs/frsp schedule (orig computes the x column first and stores min.x early). Macro fabs (no inline wrapper)
  base 94 but random orders only reach 74. Vector Scale/Abs/Sub forms 88-144. research/drafts copy NOT updated (needs lock).
- _find_aim_blend_values 0x8005e494 (576, APPEND to anim_callback, which ends at 0x8005e494): IMPROVED 94 -> 5 real lines
  = scratch/loop3/geometry/fab/BEST5.cpp. Key: the barycentric inline written with a 2D cross helper
  'inline float Cross2(float ax, float ay, float bx, float by) { return ax * by - bx * ay; }' and no difference locals:
  'float k = -0.5f; wa = k * Cross2(bx - x, by - y, cx - x, cy - y); wb = k * Cross2(cx - x, cy - y, -x, -y);
  wc = k * Cross2(-x, -y, bx - x, by - y);' (named difference locals = 54-116). Left: case 3 (x <= y, -x < y) colours
  pbx (-1 - x) and pay (-y) swapped (orig f8/f7). Inert: Cross2 param order, TriangleBlend param order (720), statement order,
  k vs literal, compare spellings, early return vs else, init chain forms. Permuter: fab/perm (base 30). CHECK the -0.5 pool
  constant with one_unit before landing (scorers ignore .sdata2 values).
- CMatrix::Rotate: random dependency-safe orders of the inlined BuildRot temps (+ split decls, + 'tyy = ty * axis.y'):
  nothing below the draft (rot/gen.py, ~5k compiles). Orig reads: tyy = ty*ay, tyz before sx, 1.0f const in f8.
- CVolCapsule::GetExtents: random split decl/def orders for both blocks (cex/gen2.py, struct-0 filter): best 9 = draft.
- CVolBox::TestCollision(CVolCapsule) bcp: random dependency-safe orders of the inline BoxPoint statements + call order (bcp/gen.py, ~300 tries): 206 -> 200 fast, not worth keeping. Diffs spread over 5 regions.
- _matrix_rot_yzx: vector-operator forms (binary / compound, v*s vs s*v) 450-465 fast vs draft 220: the draft's explicit
  temps are right; not pursued further.

### Handoff (agent 1, ended ~22:45)
Landed: 9aa74087 GetNearestPoint (964 B). Drafts pushed a3ccaa34: fab 94 -> 5, bge 60 -> 45 (research/drafts updated).
Best next tries: fab case-3 colour swap (5 lines, see above); bge schedule (45). Everything else in the lane is a long-parked
colouring knot (see research/log/lane-lines.md, lane-parks.md, lane-soldier.md before retrying).
Nothing running; no claims held; lock released.

## Agent 2 (2026-11-11 late evening)
No landings, no improved drafts (stopped on the 1-hour-without-progress rule). Nothing running, no claims, lock never taken.
- _find_aim_blend_values (fab2/): block-level scorer fab2/blk.py (per-blr-block diff counts; case 3 = block 6, the only bad one).
  Inert for block 6 (stays 5, colour f7/f8 of (bx-x) and -y): -x/-y passed as TriangleBlend params (both orders); results via
  named locals 'float a,b,c' in all 6 statement orders, split decl or not; 'Cross2(...) * k'; outputs as pointer+indices or 3 pointers;
  case-3 compare as 'y > -x', named 'nx = -x' (const or not), '1.0f < y', if/else instead of early return, '-x / -y'.
  NO combination of per-weight Cross2 operand forms (4^3) x statement order (6) zeroes block 6 even if other blocks break
  (fab2/g5.py), and a general 3-vertex TriBlend3 with the origin as (0,0) in any rotation is worse (20). Conclusion: the case-3
  tie is not reachable from the inline body or the case-3 statement; something outside (vreg numbering from earlier code?) decides it.
- CVolBox::GetExtents (bge2/): the orig fabs AND frsp order is column-major (xx,yx,zx,xy,yy,zy,xz,yz,zz); fmuls order
  xx,xy,yx,yy,zx,zy,xz,yz,zz is load-driven. Tried: products first then fabs in place (row/col/load orders x decl orders: 110-122),
  column-major assignments with a 400-step decl-order hill climb (best 80), operand flips of each product (62-64), min/max store
  orders (84-124), direct expressions without locals with fabs/(float)fabs/__fabs (163-173). BEST45 (fast 62) stays best.
- Layout recipes vs the lane: scanned every Collision/Animation queue function's orig relocations (scratchpad lr.py, same idea
  as research/tools/block_refs.py): NONE references a '...bss.0' / '...rodata.0' / '...data.0' block; only SDA21 '@N' pool
  constants (and death_cb's ADDR16 '@2135/@2136' strings). The .bss-pooled / .rodata-head / linker-stripped recipes unblock
  nothing in this lane: every remaining geometry function is a colouring/scheduling knot (see lane-lines.md, lane-parks.md).
  TestCDBBranchNodeCollision<CLine3> (2 lines, __construct_array arg regs) remains the biggest near-miss; s18 notes there are exhaustive.
