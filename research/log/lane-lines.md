# lines agent notes (2026-10-23)
Scorer: ./sc.sh {cvb,vbl,fln,fic} <src> [v]   (wraps scratch/fic/ss.py with the right template/range)
var.py <which> <base> <variants.py> : substitution variants (2 jobs); rsearch.py <which> <gen.py> N outdir : random generator search.
pre.py <src> <fn> <start> <n>: prefix diff + stack-store slot list (ours vs orig) - great for temp/slot layout work.

## CVolFiber::TestCollision(CLine3) 0x800c9668
fln_BEST.cpp (= fln/a6.cpp): raw 54 / struct 4 (was 97 struct / f4, 48 w_refparam_outer).
Keys found:
- IMPLICIT CVector3 copy ctor (union) + 'CVector3 operator=(const CVector3& o) { d.pair[0] = o.d.pair[0]; d.pair[1] = o.d.pair[1]; return *this; }'
  (returns BY VALUE: the dead return copy = orig's dead GetDirection copies at 72/56; reads stay on n/l at 248/232).
- p21/p43/p13/n via inline SDR(CVector3& r, const CVector3& a, const CVector3& b) (const refs): getter temps read in the
  source-order temp region and the getters' inner return temps dead at the frame bottom (orig's 24/40 dead start copies).
- contact.m_vector1 = pa AFTER the cross product (orig stores it last).
Left: pure FPR colouring in 4 spots (start p21/p43 loads, p43 degeneracy eps/fabs, pb sums at 0x2ec, cross n/l regs).
Invariant: decl orders (random 400), cross operand/statement orders, eps as named local/const.

## CCDBVolBox::TestCollision(CLine3) 0x8006efc0
cvb_BEST.cpp (= cvb/a2.cpp): raw 62 / struct 6 (was 100). Keys: in Clip compute e (offset dot) BEFORE d (dir dot) -> 65;
'const CVector3* a = &axis; e = a->Dot(offset); d = a->Dot(dir);' -> 62. Colouring: orig gives axis.x a LOW reg (f2), ours high.
Invariant: decl orders, Dot operand orders, by-ref axis, named component locals, setup order (rs1/rs2/rs3 searches).

## CVolFiber::TestCollision(CVector3) 0x800c9ba4 (832, exact gap fiber_testfiber..fiber_ivolume; untried before)
fpt_BEST.cpp (= fpt/a4.cpp; a5 = same with Vec() helper for the permuter): raw 20 / struct 0 after ~10 compiles.
Decoded: diff = point - start (SDR), dir = GetDirection() (by-value op=), t = dir.Dot(diff) BEFORE distSq (eval order fixed
the opening colouring), dist = sqrtf(distSq), miss unless |t - dist| <= 0.001; length = cached CLine3::GetLength()
(mutable m_length +64/m_lengthSq +68, flags +72/+73, nested GetLengthSq); miss unless 0 <= t <= length; hit: result 2,
time = length > 0.001 ? t/length : 0, contact sep = t, v0 = Cross(dir, axis) with axis (1,0,0) if |x|<|y| else (0,1,0) if
|y|<|z| else (0,0,1) (fabs results as DOUBLE locals 'double ay = fabs(y); double ax = fabs(x);' -> 37->20), v1 = point, index 0.
Left: FPR colouring in branches 2/3 of the axis pick (dir.x reload low in orig). Permuter perm_fpt running.
TRICK: when FPR colouring is off and decl order is invariant, try reordering independent COMPUTATIONS (statement order)
- evaluation order changes vreg numbering (fpt t before distSq; cvb e before d), while declaration order often does not.

## LANDED 67d1988a NEW fiber_point (CVolFiber::TestCollision(CVector3), 832). Source: fpt/final.cpp. Eps is 1e-12f (fast.py hid it).

## Big new trick (2026-10-23 evening): ACCESS PATH changes FPR colouring
CVector3Data union with a named struct 'struct { float x, y, z, w; } c;' next to v[4]: writing some component accesses as
'n.d.c.z' instead of 'n.d.v[2]' changes CW's commutative-operand canonicalisation/vreg order without changing the code shape.
- fln: cross product with c.x/c.y/c.z (or just c.z) 39 -> 18 raw (fln_BEST.cpp = fln/b1.cpp, struct 0).
- fic: random c/v per site in FindIntersection 372 -> 252; hill.py (per-site flip hill climb) running.
- cvb Dot: invariant (all 64 combos same).
Also: 'double x21 = fabs(p21.x);' (double local for a fabs result) fixed fln's opening colouring (54 -> 41); pa0 likewise (-> 39).
fln left: p43 degeneracy check (fabs f10 / eps f0 swapped) and pb/pa sums at 0x2ec.

## LANDED 2477e6a1 fiber_line + cdbgeom_volbox_line + box_line (67.03%). Sources: lines/final/*.cpp
KEY: '#undef fabs\ninline double fabs(double x) { return __fabs(x); }' (MSL inline fabs) + CVector3 x/y/z named view
(all d.c.x access). These two fixed ALL remaining colouring. Apply first to every FP park.
## Overnight: coordinator says continue: lane = collision/geometry, then untried_now.txt rows (claim in CLAIMS.txt).
## LANDED 5d54458c IsBoxVisible, 0121e6d1 CCDBVolBox plane+sphere (67.73%).
TRICK: reverse-order products (row2,row1,row0) = 3-float ctor CVector3 local(a.Dot(v), b.Dot(v), c.Dot(v)) (args right to left).
Parks rescored with fd/c: GetIntersection 98->67 raw (struct 54), others unchanged (Case0/Face use operator[] indices).
## WIP CVolBox::TestCollision(const CVolSphere&) 0x800c3210 (1472, untried; own unit box_sphere): bsp_BEST.cpp = 202 raw / 41 struct.
Decoded fully: OSGetTick() at entry/exit (profiling, result unused); GetCenter() (out of line) diff; local = 3-float ctor of the
three axis dots; per-axis d = |local_i| - half_i: outside -> distSq += d*d, flag: sign via int-bit inline Sign (x = r; return x),
normal += CVector3(s*d,0,0)-style, closest_i = s*half_i; else axis0 'maxDist = d' (unconditional), axes1/2 'else if (d > maxDist)';
separation = (outside ? sqrtf(distSq) : maxDist) - GetRadius(); codes 1/3/2; hit: rotate normal (normalize) & closest(+centre).
Left: orig keeps m_axes in f0-f8 across the v0 Normalize (ours reloads: stores to contact kill the loads), separation in f27 stored
early, local store order, frame 240 vs 224.
## WIP CVolCapsule::GetExtents (park 74): cex/a1.cpp 33 raw / 6 struct (ref helper 'MinOf(float& a, float b) { if (b < a) a = b; }').
Left: orig min/max shape 'fcmpo a,b; bge L1; b L2; L1: fmr a,b' (the kept-a arm coalesced); ternary/if-else forms give an extra fmr or a CR bool.
## WIP CVolBox::GetExtents (park 90): bge/a3.cpp 54 raw / 2 struct (inline fabs, c-path accesses, shuffled product order zx,yx,xz,zz,zy,yz,xx,yy,xy).
## FindIntersection: fd makes 6 callee FPRs (orig 7) -> keep macro fabs there; best still fic/d1.cpp 251 (rs4 equal 251 at v7).
## bsp (CVolBox sphere) now 19 raw / 0 struct (bsp_BEST.cpp). Keys since: 9 axis floats hoisted in the hit block (no axis reload
across Normalize), DECLARATION ORDER READ OFF THE ORIG REGISTER NUMBERS (sort orig regs low->high = decl order:
zx,yx,xx,zy,yy,xy,zz,yz,xz) 45 -> 19; contact.m_separation set first; normal zeroed z,y,x; Sign: 'x = r; return x;'.
Left: local.y/local.z/a0 rotate (f7,f6,f5 orig vs f6,f5,f7). Permuter perm_bsp (base 95) running from bsp/e4.cpp (C-friendly).
TRICK: reading decl order from orig register numbers works directly for hoisted named float locals.
## LANDED 959bfe91 CCDBTriangle anim test (inline-helper slots trick). 68.29%.
## WIP GetNearestPoint 0x8006c800 (964; contiguous after cdbgeom_sphere): gnp_BEST.cpp = 15 raw / 0 struct (was 176).
Keys: final point via scaled temps (unfused 'float sx = s*e0x; result.x = v0.x + sx;' -> struct 0), edges/diff as CVector3
(scalarised) not floats (50->37), diff components computed y,x,z (->15). Left: b0/b1 order + dz reg. Permuter perm_gnp running.
## WIP CVolBox sphere 19 raw (bsp_BEST.cpp).
## LANDED 87760f48 fiber_extents (first compile with fiber_line view).
## WIP CVolBox::TestVisibility 0x800c14cc (280, untried; between box GetExtents and box_testtriangle; needs local static
remapCodes$890 in .data {0x20,0x10,2,1,4,8}): btv_BEST.cpp = 7 raw (2 struct = reloc only). Keys: halves hoisted before the
loop (decl h0,h1,h2; assign h2,h1,h0), normal components as named floats nx,ny,nz in the loop, fabs results a0..a2 locals,
mask = *code at loop top. Left: radius/distance regs (f4/f5 orig). Permuter perm_btv (base 40) running.
## LANDED a325b868 box_visibility (68.49%). TRICK: an inline returning float (Radius(a0,a1,a2,h0,h1,h2)) changed the
final colouring (result temp) when decl orders could not.
## Parked this round: brb TestCDBBranchNode<CVolBox> (76/70: frame copy slot order robust to every form), cex (33/6 coalescing),
bge 54/2, bsp 19, gnp 15.
## LANDED 164cd229 cdbgeom_triangle_line (68.81%). TRICK: an inline int Compare(separation) fixed PARAMETER register order.
## LANDED cacaa685 capsule_capsule (68.99%). Keys: CLine3 inline empty dtor + --weak-duplicate __dt__6CLine3Fv (extab action), 'a > b ? b : a' min.
## WIP CVolBox::TestCollision(const CPlane&) 0x800c15ec (2292): 8 corners gray-code walk (corner += a twice), per corner a
by-value projection inline; contact holds best (sep at +32 updated). bpl/ drafts.
## PARKED bpl (CVolBox plane, 2292): structure understood (corner = m_center copy in memory; -= a,b,c; 8 gray-code corners:
a+,b+,a-,c+,b-,a+,b+ each as 'corner += v; corner += v;'; per corner three by-value copies of corner (A 256.. used by the
distance dot, B 384.. used by '+= p', C 128.. dead) and a shared point at 416 (Scale(n,-d) then += p), contact (432) holds
v0 = -n, v1 = point copy when better, sep). Best draft bpl/a2.cpp 448/319 (frame 768 vs 672). 11 callee FPRs.

# HANDOFF (2026-10-24, lines agent wrapping up)
Landings this session: 67d1988a fiber_point, 2477e6a1 fiber_line+cdbgeom_volbox_line+box_line, 5d54458c IsBoxVisible,
0121e6d1 CCDBVolBox plane+sphere, 959bfe91 CCDBTriangle anim test, 87760f48 fiber_extents, a325b868 box_visibility,
164cd229 cdbgeom_triangle_line, cacaa685 capsule_capsule (64.x -> 68.99%). Final sources: scratch/par/lines/final/.
Tools here: sc.sh <key> <src> [v] (keys: cvb vbl fln fic fpt ibv cvp vbs btv bge cex brb tan gnp ctl ccap bpl mrot vn bsp),
var.py (substitution variants), rsearch.py (random gens), hill.py/minim.py (c/v access sites), dhill.py (decl order),
lhill.py (line order), xf.py (add inline fabs / x,y,z view), tryall.sh (base/fd/c/both).
TRICKS (most valuable first): MSL inline fabs 'inline double fabs(double x){return __fabs(x);}'; x/y/z struct view in the
CVector3 union instead of v[]; 3-float ctor args evaluate right-to-left (reverse-order products); inline returning a value
(Compare/Radius) fixes colouring/param registers; decl order read off orig register numbers; mirrored stack slots = body in an
inline helper; 'a > b ? b : a' min shape; CLine3 inline dtor + --weak-duplicate __dt__6CLine3Fv for extab cleanup actions;
statement (evaluation) order matters where decl order does not.
Best WIPs (score raw/struct):
- GetNearestPoint 0x8006c800 (contiguous after cdbgeom_sphere): gnp_BEST.cpp 15/0 (b0/b1 order, dz reg left).
- CVolBox::TestCollision(CVolSphere) 0x800c3210: bsp_BEST.cpp 19/0 (local.y/z vs a0 regs).
- CVolBox::GetExtents 0x800c13b4: bge/a3.cpp 54/2. CVolCapsule::GetExtents: cex/a1.cpp 33/6 (min/max coalescing).
- CMatrix::Rotate 0x800904e8: mrot/a0.cpp 21/6 (old park). CVector3::Normalize 0x8006c224: vn/a0.cpp 25/0 (length in f1 orig).
- CVolBox plane 0x800c15ec: bpl/a2.cpp 448/319 (structure notes above). TestCDBBranchNode<CVolBox>: brb/f1.cpp 76/70.
- FindIntersection: fic_BEST.cpp 251/2 (keep macro fabs there). 
Claims (CLAIMS.txt, lines): the parks listed in parks.txt, GetNearestPoint, CVolBox sphere/plane/TestVisibility/GetExtents,
CCDBTriangle line/anim, capsule-capsule, fiber GetExtents, Rotate, Normalize. Release any you don't continue.
No permuters running; lock not held.

# SESSION 2 (2026-10-24, new lines agent)
## LANDED ead57cdc cdbgeom_vector += CVector3::Normalize (returns float length -> f1; 'x *= 1.0f/length').
## WIP CMatrix::Orthonormalize 0x8008f1e8 (688, between matrix_endian and matrix_ps; old park 174 off): orth_BEST.cpp = 68 raw / 0 struct (sc key orth).
Keys: base scratch/pool/orthonormalize_wip.cpp (99/3); the extra x reloads (forward.x / up.x read again after the Dot) go away when
the vector is read through a reference: 'const CVector3& fw = forward; dot = fw.Dot(right); f.x = fw.x - px' (same for up) -> struct 0;
operand/statement order random search (og1/og2/og3.py) 97 -> 68. Left: {right.y, right.z, forward.x} colours rotate (orig f6,f7,f5;
ours f5,f6,f7) and everything after follows. Invariant: access paths (hill2), Normalize forms (ret float etc.), dot term orders.
## BuildRot 0x80090980 (brt key; brt/oh2.cpp 19/0 from 24 via x/y/z union view + one operand swap). c/tx colour swap left. Rotate = same inline (mrot).
## gnp/bsp: tried xf c/fd, access hill, dot order random (gn12.py), Normalize-return, fabs order: no gain (15 / 19).
## WIP CVolBox::TestCollision(const CVolCapsule&) 0x800c290c (2308; directly before bsp 0x800c3210, after box_dispatch): bcp_BEST.cpp 252 raw / 57 struct (sc key bcp), from 491/399 in ~40 compiles.
Decoded: OSGetTick() at entry only; start/dir copies (start decl first: start@208 dir@192); r = CONTACT_EPSILON + radius;
3x inline AxisSeparates(axis, half, dir, diff, r) (dd = dir.Dot(axis), ds = diff.Dot(axis); |dd|>1e-5f ? (dd>0 ? (ds>0 ? ds-r>h : r+(ds+dd)<-h)
: (ds>0 ? (ds+dd)-r>h : ds+r<-h)) : |ds|-r>h) each -> 'if (flag && m_result<1) m_result=1; result=1' (single exit, goto for the
projection reject); GetProjection(m_center, p, b2) (t param shares b2's slot!), n = p - centre as floats (y,x,z), if lenSq > r*r:
normalize, proj = sum fabs((h_i*a_i).n); reject if len-proj > r. Then dist = inline SegmentBoxDistance(sqrtf(inline
SegmentBoxSqrDistance)) = Eberly segment/box: BoxLineSqrDistance; lt>=0 ? (lt<=1 ? copy (no null checks) : end point + 'if (t) *t=1'
+return) : start point + 'if (t) *t=0' +return (returns inside keep the null checks like the orig); the extra sqrt wrapper inline fixed
the temp-slot order. code = inline Compare(sep); contact: box point unfused temps, capsule point via inline Evaluate(line,t,x,y,z)
with named start/dir copies, normal in memory + Normalize, v0 = normal, sep first, v1 = tmp (cp + (-radius)*n), index 0.
Left: radius reload before the sqrt (ours CSEs it), sqrtf 'fmr' for lenSq (colour), fabs block operand order, schedule nits.
## bcp update: bcp_BEST.cpp (= bcp/g2.cpp) 132 raw / 13 struct. Further keys: AxisSeparates(float dd, float ds, half, r) called with
dir.Dot(m_axes[i]) / diff.Dot(m_axes[i]) (arg form fixed dd operand order); start/dir copies from capsule.m_axis directly with
'const CLine3& line' declared AFTER them; 'if (nx*nx+ny*ny+nz*nz > r*r) { lenSq = same; len = sqrtf(lenSq) ...' (CSE'd, removes
the sqrtf fmr); separation inline SegmentBoxDistance(..., const float& radius) { sq = ...; rr = radius; return sqrtf(sq) - rr; }
(radius load placement); projection terms 'h * a.c * n.c'; contact v1 assigned before v0; statement orders from dmh.py
(dependency-safe move hill; gens bg3.py contact, bg4.py projection, bg5.py opening).
Left (struct): opening schedule of axis test 1 (dir.x load vs dir pair1 store; |dd| fabs position), 0x760/0x77c in the contact.
TOOL: dmh.py <key> <gen.py defining base,a,b,units,defs(u),uses(u)> <out>.
## Session 2 status (end): landed ead57cdc (Normalize). Best WIPs: bcp_BEST.cpp 132/13 (box-capsule), orth_BEST.cpp 68/0
(Orthonormalize), brt/oh2.cpp 19/0 (BuildRot), gnp 15/0 and bsp 19/0 unchanged (many invariants tried: SDR helpers, refs, Dot
orientation, value-returning inlines, decl/statement hills). _matrix_rot_yzx 0x800f4e98 (1016, dmcluster local, before
dmcluster_init) looked at: rows copied, CVector3 temporaries with many dead stores, SetRight/SetFront/SetUp calls - not started.
# SESSION 3 (2026-10-24, new lines agent)
Tools added: rd.py <key> <src> (side-by-side RAW diff rows, orig | ours), ours.sh <key> <src> <hexstart> <hexend> (our asm window).
New sc keys: quat (CQuaternion::Multiply), asv (AddSubVolume), wvr (RemoveSubVolume(const&)).
## LANDED 68a105dd world_volume_remove += AddSubVolume (source final/world_volume_remove.cpp). Key: one 'head' local shared by push_back + UpdateExtents(head).
## perm_orth running (harness orth_h.cpp: refs -> pointers, same score 68/0); permuter base 620.
## brt dmh (statement order, brt_dm.py): no gain from 19.
## bcp: code/result variants (r30 cmpwi at 0x6bc) all invariant; SetResult inline invariant.
## WIP CQuaternion::Multiply 0x8009174c (164, append to quaternion): quat_BEST.cpp 29 raw / 0 struct (was 47/12). Keys: x = nx + ax + bx sums (b2 form) + dmh statement order (ny before nx, by before bx) -> f31 save appears. Invariant: decl split/order, all product operand flips (rs2), local copies, q ref/ptr.
## LANDED 433bcad1 NEW world_volume_list_reserve (list<const CCDBObject*>::reserve). Key: fresh sentinel address via '(list_node<T>*)((char*)this + 16)' assigned after tail, 'head' declared at ctor top. Applying to SRequest/SExtent reserves next.
## orth: 3-float ctor / Set(x,y,z) for u (args right-to-left) 68 -> 21 (orth_BEST.cpp; orth/s1.cpp = Set form for permuter). Permuter junk 'fx = fx;' -> 18.
## csg CheckContacts (csg/a0.cpp 12/0): result r28 vs SR pointers order; decl/type/helper variants invariant; permuter perm_csg running.
## reserves SRequest (lrq/b1.cpp 22/2: + lis/addi reloc noise) and SExtent (lrx/b1.cpp 14/0): head trick gives the right structure; left = head r6 vs added r7 swap (head takes node's colour; the SExtent T() temp lives in r6-r9). Invariant: head/tail/added decl positions, head assignment position, alloc_node forms, copy-loop statement orders, 8 cast forms (&m_free+1 and (char*)&m_head get CSE'd).
## PARK CCDBObject::Init 0x8007133c (784): cdi/rs_best.cpp 97 raw / 79 struct (old b0 102/97). Loops fine (addi sched only); extents block needs 4 callee FPRs (orig f28-f31: 0.5 double const, size.z, 2 fabs temps) and computes row-1 halves first; named h-locals give 3. cdi_rs.py/cdi_rs2.py random gens.
## BuildRot: brt_BEST.cpp (= brt/fl_best.cpp) 2 raw / 0 struct. Keys: 'txy = t * axis.x * axis.y', 'tyz = t * axis.y * axis.z' (re-associated, CSE with tx/ty) 19 -> 8, flip.py operand hill -> 2 (sx = s * x). Left: txy/tyz operand order (tx,y / ty,z). perm_brt2 running. TOOL flip.py <key> <src> <out> <fn-substr>.
## LANDED ee48b5da cdbobject_leaf += GetDistanceFromPlane (Sq() inline helper fixed the reg swap). TRICK: x*x -> inline Sq(x).
## WIP CObstacleAvoidance::DetermineNewObjectivePosition 0x80053738 (1276, untried; would be its own fragment unit before RunEnvironmentCollisionChecks/ObstacleAvoidance): dno_BEST.cpp 136 raw / 34 struct (first draft 499). Decoded fully (state switch 'case 3: default:' first, case 4, case 0; FindLine inline returning 5; IsClear/HasClearance/ShouldAvoid bool inlines with li 1/li 0 forms; tail search low/line(high reuses line)). Left: duplicate 'bge' in IsClear, 'mr' pair for 'low = next = line - 1' (orig computes r0 then two mr), tail mask first test uses neg/or/srwi. + beq (blocked[2]) and per-iteration 'add r9,r30,r6' entry base.
## WIP CBulletDecalManager::AddBulletDecal 0x8008385c (288, untried; PREPEND to decal_reset): abd/d1.cpp 14 raw / 2 struct (= colouring: orig contact r31, this r30, decal r29; ours this r31, contact r30). Keys: 'BulletDecal* d = bin.m_decals;' hoisted before 'if (decal == d)', loop with 'next' local; surface type = signed 11-bit bitfield at +12 of the contact's +36 pointer. Invariant: contact const/accessor/assign-in-if/early return/self alias; inline helper (member or static, any param order) gives decal r31 (32). perm_abd running (harness abd/h.cpp).
# HANDOFF (session 3 end, stopped by coordinator)
Landed: 68a105dd (AddSubVolume), 433bcad1 (list<const CCDBObject*>::reserve), ee48b5da (GetDistanceFromPlane).
READY TO LAND: AddBulletDecal abd_BEST.cpp (= abd/p_b.cpp) 0/0 - prepend to decal_reset; key = redundant 'decal = &m_decals[m_active & 511];' re-assigned after the bin reference (permuter find). Check rawcmp/one_unit.
Best WIPs: orth_BEST 21/0, brt_BEST 2/0, quat_BEST 29/0, dno_BEST 136/34, lrx/b1 14/0 + lrq/b1 22/2 (reserves), csg/a0 12/0, cdi/rs_best 97/79, bcp_BEST 132/13, gnp 15/0, bsp 19/0, mrot 21 (reloc-only struct).
No permuters/climbers running; lock not held.
# SESSION 4 (2026-10-24, new lines agent)
## LANDED 8819a84c decal_reset += AddBulletDecal (70.29%).
# SESSION 5 (2026-10-25, new lines agent)
## WIP IsPointInTrigger 0x80044d3c (908; own NEW unit before TriggerActivateMessages; sc key ipt, template proxtrig_leafs.json): ipt_BEST.cpp 39 raw / 7 struct (first draft 219).
Decoded: core type short@44 switch (0 sphere, 1 cylinder (radius/top/bottom), 2 oriented box via CMatrix from core->rotation@28 + SetPos), sizes shorts @46/48/50 '/ 16.0f' (gives conv*0.0625 operand order; '* 0.0625f' puts the literal first), halves '/ 16.0f / 2.0f', box d2 upper bound reuses hy (orig bug).
Keys: center = 3-float ctor CVector3 before the switch (z,y,x loads); 'bool result' single exit; CVector3 copy ctor ': d(o.d) {}' (SetPos arg built in place) + operator= returning BY VALUE (dead pos copy at 32);
box in static inline ToLocal(m, point, d0&, d1&, d2&) with 'CVector3 pos; pos = m.GetRow(3);' and 'const CVector3& rK = m.GetRow(K)' temps (inline slot order pos16 dead32 r0 48 r1 64 r2 80); cylinder decl order via 720-perm grid.
## ipt now 19 raw (5 are reloc-only) / 1 struct: ipt_BEST.cpp. Further keys: all case-0/1 floats (dy,dx,dz,radius,bottom,top) declared at FUNCTION scope (C style) fixed the cylinder colouring; ToLocal ends with 'Set3(d0, d1, d2, m.GetRow(0).Dot(dx,dy,dz), m.GetRow(1).Dot(..), m.GetRow(2).Dot(..))' (args right to left = orig copy schedule r0/r2/r1 + slots) 34 -> 26; permuter: sphere 'dx = ...' before 'dy = ...' -> 19.
Left: box FPR colouring only (dx f31 orig vs hx; hz f1/d2 f4; py/r1p1 f2/f3). perm_ipt running (harness ipt/h6.cpp: V3() for the SetPos arg, center.Set()).
## TriggerActivateMessages (old park 21): pta/m.cpp 13 raw / 3 struct ('for (i = 0; i < count; i++, messages++)'). Left: 'li r30,0; mr r31,r30' zero sharing (every copy/cast/helper form const-propagates), entry/bsObj/i callee order (decl order, 512 access-site combos invariant).
## LANDED a4233e3e NEW proxtrig_point (IsPointInTrigger, 71.40%). Source lines/final/proxtrig_point.cpp. Last key: rows passed as temporaries into an inline 'Dots(d0,d1,d2, const CVector3& r0, r1, r2, dx,dy,dz)' (copies before the dots, slots r0<r1<r2) + halves 'Set3(hz, hy, hx, ...)'.
## Next: TriggerActivateMessages (pta/m.cpp 13/3) could append to proxtrig_point (0x800450c8 directly after). _matrix_rot_yzx 0x800f4e98 (1016, static, prepend to dmcluster_init) claimed, asm in mry.s, decoded: angles 2pi*(float)a/65536 (sin y,z,x then cos), rows copied (right/front/up), rot about z (r,u), x (f,r'), y; SetRight/SetFront/SetUp by value; many dead temp stores (136/104/120 slots) - not started.
## LANDED d5a99ee8 Screen_fieldmode += GetTMViewToScreen (first compile 8 -> 0 with '/ 2.0f'). Next contiguous: PutDisp 0x8001b7e8.. GetDepthBufferMax 0x8001b840 .. SetClear (small CScreen getters) up to Screen_cscreen_dtor 0x8001be68.
## (wasted) CScreen getters after GetTMViewToScreen are already in unit 'Screen' (0x8001b7e8..): always check coverage of neighbours before drafting. Screen.cpp 0x8001b544..0x8001be68 now contiguous.
## LANDED 40e37af2 projectilebullet_commit += Penetrate (6 compiles). TRICK: arg copy slot lowest = call through an inline wrapper taking const&.
## LANDED 5e82d211 dmcluster_init += DMClusterSkelTraverse (old 1-store park; 'CMatrixStack* stack = _dmeshStack;' local before the RotateZ call).
# SESSION 6 (2026-10-25 evening, new lines agent)
## LANDED 4aa6607d decal_reset += InitBulletDecalFromCollision (72.21%). Source final/decal_reset.cpp. sc key ibd.
TRICK: 'mulli rX,6; slwi rX,3' index = array of 8-byte pairs indexed 'base[bin * 6 + k]' (via an int local), not a 48-byte struct (mulli 48).
## dsh (DMClusterSkelTraverseHeirarchy 0x800f53c0, 604): dsh/b3.cpp 49/22 (from h0 54/26). Keys: TransformPoint through
'static inline void TransformByTop(CVector3& p) { _dmeshStack->Top().TransformPoint(p, p); }' (arg temp slot lowest, keeps the schedule).
dsh/v1 r2 ('bone->z = 0.0f' via the local) 48/20. Left: GPR colouring = orig params r26-r29 ABOVE scratch/depth/a/b/c (ours params lowest
r21-r24); also FLT_MAX const regs and depth++ schedule follow from it. Invariant: inline Traverse() with all 720 param orders (worse, 89),
nested block, scratch++ forms, index IVs, a/b/c types/struct copy, Fill helpers, depth++ placement.
## LANDED 42a2b998 dmcluster += DMClusterSynthesizeMatrices (72.37%). Source final/dmcluster.cpp (sc key dcs).
TRICKS: operator= call for a local copy with NO ctor/InitClass check = inline copy ctor '{ *this = other; }'; 'slwi 16; srawi 16' = '(d << 16) >> 16' wrap (not a short cast, which gives extsh).
## perm_dsh running (base 300 = r2h.cpp, 2 jobs, timeout 2h).
## dsh: dsh/hoist_best.cpp 38/15: all locals hoisted C-style + PARAM COPIES ('orient = orient_' etc., unnatural) in hill order
count,bone,minimum,c,b,matrices,orient,maximum,scratch,depth,a (dsh/hoist.py <iters> copy <order> <seed>). Fixed depth++/orient/matrices/a;
left: min/max always lowest (r21/r22 vs orig r28/r29), b/c regs, scratch split into two vregs, FLT_MAX const reg, the 'mr r0,r3' copy
artifact. perm_dsh (base 300 from r2h) found nothing in 16k iterations. PARKED.
## LANDED b9f4538e dmgeom += DMGeomBuildPartList (old park 24 -> 0). TRICK: volatile-reg swap between a user pointer and a
strength-reduced IV: write the pointer as an indexed access 'part[n]' (becomes an IV created later -> lower reg); user increments
that must schedule after the IV updates go in the for-increment clause ('n++, parts++, count++').
## LANDED 83185a4f dmesh += Draw/BindTextures/BindSkeleton/Init (2056 B, folds dmesh_cpart_dtor). 72.59%.
TRICK: a register COPY of a placement-new result ('mr r7,r10', later uses through the copy) = 'T* part = (T*)(void*)bin;' declared right after the new.
## LANDED d7c9e46c NEW propdat_load (LoadPropertyData, old park). 72.68%. Key: loop in a static inline with hoisted pointer+counter locals
(the inline removes an 'addi r0; mr r30,r0' copy of the array address). GOTCHA: check whether weak neighbours already have their own
unit (propdat_endian_header) -> --file-duplicate; drop static header tables another unit of the file owns.
## pta (TriggerActivateMessages): per-trigger body in a static inline SendMessages() with own count/messages locals gives the
shared zero 'li r24,0; mr r25,r24' (pta/s1.cpp 27/2) but the inline locals take the LOWEST callee regs (orig r30/r31 highest);
ref-output Clear() and cast forms const-propagate (13/3 as before). Parked.
## ebl/ebv (weak EndianSwap(BPDLight&)/(BPDLightVolume&) 0x80044168/0x800443b4): ebl/a0.cpp 5/2 + 3/3. field18 swap in orig is
strictly sequential reusing r4 (a WAR chain); an own inner helper with a separate 'd' temp (v2 z3) gets 3/2. Parked.
## WIP RunEnvironmentCollisionChecks 0x80053c34 (1744, prepend to ObstacleAvoidance; sc key rec): rec/g4.cpp = 1 instruction off
(raw 3 = 2 reloc + 1): orig has a DEAD 'b end' right after the state switch dispatch (0x48c) that ours never emits; tried case 0 /
default / order permutations, empty statements, braces, case 1/2 split. Everything else matches. Keys: CAIFilterRealPosition and
Vector3 aligned(8) (16-byte slots), named 'offset' temp '(offset = forward) *= -0.5f', CopyPosition (z,y,x loads) for starts and the
grid position, 'f_rv3ObstacleAvoidanceLines[m_line].x' direct (indexed lfsx), owner flags read via inline GetFlags() (reload),
switch case order 1/2 then 3. perm_rec running (harness rec/gh.cpp, base 2870 = harness mismatch?).
## ulp (UpdateLightingParams 0x800f2ce4, prepend staticmesh_cmshmatbin_dtor; sc key ulp): ulp/a0.cpp 28/16 = only the best-light
store block FPRs (orig colour products f2/f1/f0, dir y/z f4/f3; ours f4/f3/f2 and f1/f0). Invariant: 720 hoisted decl orders of
r,g,b,x,y,z, Set3 helpers, product order, store order (worse). Parked.
## alc (list<AlarmInfo> range ctor 0x8005a270): old draft alc/a0.cpp 31/12; porting the lrx ctor body is worse (52). Parked.
## lrx (SExtent reserve): head declared right after 'm_owned = true;' (lrx/c2.cpp) 2 raw / 1 struct: head now CSE'd with &m_head
(stw r0) instead of the orig's fresh 'addi r7,r1,40' before the copy loop. a5/b5 (head after the loop) 3/2.
# HANDOFF (session 6 end)
Landed: 4aa6607d decal_reset += InitBulletDecalFromCollision; 42a2b998 dmcluster += DMClusterSynthesizeMatrices; b9f4538e dmgeom +=
DMGeomBuildPartList; 83185a4f dmesh += Draw/BindTextures/BindSkeleton/Init (folds dmesh_cpart_dtor); d7c9e46c NEW propdat_load
(LoadPropertyData). 71.96 -> 72.68%.
Best WIPs: rec/g4.cpp RunEnvironmentCollisionChecks 1 dead 'b' off (raw 3 incl. 2 reloc); dsh/hoist_best.cpp 38/15; lrx/c2.cpp 2/1;
ebl/a0.cpp 5/2 + 3/3 (EndianSwap BPDLight/Volume); pta/s1.cpp 27/2 (shared zero right, callee order wrong); ulp/a0.cpp 28/16.
Cosmetic: src/game/propdat_load.cpp header comment line 4 is over-long (rewrap at the next landing that touches it).
No permuters running; lock not held.
# SESSION 7 (2026-10-25 night, new lines agent)
## LANDED 2e6ee18f ObstacleAvoidance += RunEnvironmentCollisionChecks (73.20%). Source final/ObstacleAvoidance.cpp. Dead 'b' fix:
'case 0: if (distance == -1.0f) break; break;' (found by scanning the ELF for consecutive identical 'b' and reading matched
BIFunc_SetPlayerCollision: its dead b is a case whose copy 'id = none' coalesces away late).
## LANDED 2955b3cc NEW scene_list_reserve (SExtent reserve). Key: head = end().m_node with end() = list_iter((list_node<T>*)((char*)this + 16)). (73.38%)
## LANDED 4024703b NEW world_object_request_reserve (SRequest reserve, same end() trick; 73.57%). Coordinator tip (pos = &m_head local + placement new(node) list_node<T>(value), from AddNodeExtents 69e74f86) arrived after both reserves landed - keep for alc (list<AlarmInfo> range ctor).
## LANDED ff74409b NEW alarm_list_ctor (list<AlarmInfo> range ctor; 73.78%). Keys: element-wise AlarmInfo copy ctor, loop in inline link_range(end, first, last) (param order mattered: end first), free-list set-up in inline init_free() -> shared zero. Every function-level shared-zero spelling swapped the param callee regs; the inline fixed both.
## brt (BuildRot): brt/s1.cpp 1 raw / 0 struct: 'float tyz = Mul(ty, axis.d.c.z);' (static inline Mul(a,b) {return a*b;}) fixed tyz; txy (fmuls tx,y orig vs y,tx) resists Mul/access-path/flip/line-order hills.
## LANDED 029e8fe5 cdbgeom_triangle_line += CCDBTriangle::TestCollision(CVolBox) (3844 B, 74.20%). Draft lines/tbx/a7.cpp (sc key tbx; tln = line test). TRICK: argument evaluation order of float args (CW right-to-left) fixed by taking them into locals in the orig's computation order (q0 then q1). Next big one in lane: CCDBVolBox::TestCollision(CTriangle) 0x8006e0b0 (3856, no calls, between cdbgeom_triangle_ctor and cdbgeom_volbox_line) - probably the same SAT with everything inlined.
## Looked at CCDBVolBox::TestCollision(CTriangle) 0x8006e0b0: different algorithm (Eberly box/triangle, edge axes via scalar triple products, Min/Max by reference with addi r4,r1,X picks, inline normalise with frsqrte Newton steps), no contact, 18 callee FPRs - not started (asm in lines/tbx/vbt.s).
## LANDED ab48b0bc csg_volume_line += CheckContacts (value-returning inline Result(best) for the result reg; 74.43%).
## quat/gnp: Mul()/Dot()/Id() value-returning inline wraps per statement: no gain (quat 29, gnp 15).
# HANDOFF (session 7 end)
Landed: 2e6ee18f RunEnvironmentCollisionChecks; 2955b3cc SExtent reserve; 4024703b SRequest reserve; ff74409b list<AlarmInfo> range
ctor; 029e8fe5 CCDBTriangle::TestCollision(CVolBox) (3844 B); ab48b0bc CCSGVolume::CheckContacts. 72.68 -> 74.43% (incl. others' landings).
Best WIPs: brt/s1.cpp BuildRot 1/0 (txy operand order); gnp_BEST 15/0; bsp_BEST 19/0; quat_BEST 29/0; orth_BEST 21/0;
dsh/hoist_best 38/15; pta/m.cpp 13/3; ebl/a0 5/2+3/3; mrot/a0 21/6 (Rotate's inlined BuildRot schedules differently from BuildRot).
Big untouched: CCDBVolBox::TestCollision(CTriangle) 0x8006e0b0 (3856, all inline, 18 callee FPRs; asm lines/tbx/vbt.s),
TestCDBBranchNodeCollision<CLine3> 0x800701d4 (2696).
No permuters running; lock not held.
# SESSION 8 (2026-10-25 late, new lines agent)
Tool: scr.sh <key> <src> v = sc.sh with RAW (register-level) diffs (ssr.py).
## LANDED 528601ae matrix_translate += BuildRot (74.61%). Key: 'float txy = t * axis.x; txy *= axis.y;' (compound) keeps tx as the FIRST fmuls operand; 't*x*y' / 'y*(t*x)' put the CSE'd t*x second. Mul()/v[1] hacks no longer needed.
## ebl/ebv insight: the field18 (Light) / planeCount (Volume) "sequential swap" is a SCHEDULING-WINDOW boundary counted from the
function start (removing field00 moves the anomaly to the next field at the same position). The 'float& f = value;' local in
ChangeEndian(float&) costs one window unit per float. Exact matches exist but are non-uniform: Light = first 2 float fields
through a no-ref overload (ebl/k2.cpp idea, raw 0), Volume = first 3 or 4. Unused/used refs at function level cost nothing;
uniform helper forms (cA/cB/cP x eA..eG, ebl/v7.py) give only the two states 5/2 (ref) or 8/4 (no ref). Parked; needs a natural
form whose per-field cost differs between int and float fields.
## LANDED aed4ce61 proxtrig_point += TriggerActivateMessages (74.90%). Source final/proxtrig_point.cpp. Inline SendMessages with
default-argument params 'unsigned int* messages = 0, unsigned int count = 0' (shared zero + high callee regs), flag test
'(*trigger & 0x8000) >> 15' (index into r0). Inline locals = lowest regs; inline PARAMS initialised from constant args = shared li/mr.
## blc (list<BuddyInfo> range ctor 0x80058988, 360; sc key blc; claimed): blc/q3.cpp 5 raw / 5 struct from the alarm_list_ctor scaffold.
Keys: ctor init list 'm_count(0), m_capacity(capacity), m_pool(alloc), m_free(0), m_head(&m_head, &m_head, T()), m_owned(true)' +
init_free() keeping only the free-list loop (params callee order then right); BuddyInfo() body through 'BuddyInfo* self = this;'
keeps the T() temp in memory (frame 48). Left: orig stores only the BODY fields (path/moving/active at 16/20/24) of the temp,
ours also soldier/progress (8/12). Base-class split, self in copy ctor, pointer reads: same. Plain body = everything SROA'd.
## gnp: b0-before-b1 dots (p01324) give the orig fmuls order but 35 raw; lhill from there 30 (best stays 15). bsp: fabs/diff/local forms no gain.
- ulp: already landed by bigfiles fee75de3 (same key: colour read into locals, products at the stores). Check config ownership before reopening old WIPs!
## orth: natural uniform 'dot * comp' products (orth8/o5.cpp) tie BEST at 21/0 (operands now all orig order; colouring only); lhill no gain.
## quat: read-locals-first (bigfiles' trick) quat8/po1.cpp 33/0, lhill running (best still quat_BEST 29/0). mrot: axis locals worse (45/25).
- quat lhill from po1: 29 (ties quat_BEST). 
# HANDOFF (session 8 end)
Landed: 528601ae matrix_translate += BuildRot (74.61%); aed4ce61 proxtrig_point += TriggerActivateMessages (74.90%).
Best WIPs: blc/q3.cpp list<BuddyInfo> range ctor 5/5 (temp stores soldier/progress too); ebl/a0 5/2 + 3/3 (window-boundary
analysis above; non-uniform k2 variant matches); gnp_BEST 15/0; bsp_BEST 19/0; orth_BEST / orth8/o5 21/0; quat_BEST 29/0;
mrot/a0 21/6; bge/a3 54/2. Tools: scr.sh (raw-register diffs), dump.py (our disassembly). No processes running; lock not held.
# SESSION 9 (2026-10-26, new lines agent)
## blc: blc/n3.cpp 3 raw / 3 struct (was 5/5). Key: split the body fields into a nested 'struct BuddyState {path, moving, active}'
with the self-trick ctor + user copy ctor (moving, active, path), BuddyInfo() : soldier(0), progress(0), state(BuddyState()) {} and an
implicit BuddyInfo copy ctor -> the soldier/progress temp stores vanish (address-taking is per OBJECT; the separate state temp keeps
its stores). Left: orig temp sits at sp+16 (8 unused bytes at sp+8), ours at sp+8. Unused address-taken locals get no slot (tested);
temp-of-temp, placement new, dtors, by-value params: no.
## LANDED 9dcc9c21 NEW pumhandler_list_ctor (list<QueuedSubtitle> range ctor; 75.36%): blc/q3 scaffold matched at once (qsc/a0.cpp).
## blc: partial SROA exists (QueuedSubtitle temp: scalars in regs, text array on the stack at temp+12), so orig BuddyInfo temp = sp+8 with only path/moving/active memory-resident; char/bool array members, member address-taking, nested ctor: all-or-nothing. Still 3/3 (n3).
## vbt = CCDBVolBox::TestCollision(const CTriangle&, CCollision&, bool) 0x8006e0b0 (3856; claimed): STRUCT 0 in vbt/a3.cpp (raw 527 = FPR colouring only).
Decoded: the tbx SAT from the box side, everything inline, collision/flag unused, returns Compare(best). D = v0 - centre, E0, E1;
box axes: Separate(h_i, D.A_i, E0.A_i, E1.A_i) (axis 0 dots in order D,E0,E1; axes 1/2 D,E1,E0 -> slot store order);
normal = E0 x E1, Unitize (sqrtf volatile), dots in order nd, a2n?, (p0231 permutation of nd,a0n,a1n,a2n), r = |a0n|h0+|a1n|h1+|a2n|h2;
9 cross axes Eberly-style: p0 = (A_i x E_j).D, p1 = +-a_in, r from |A.E| abs values computed ONCE at first use (b1e0.. locals,
reused later: this was the 86 -> 20 key), E2 terms as differences (c02 = c01 - c00, a1e2 = a1e1 - a1e0).
Colouring: orig D=f30-28, E0=f27-25, E1=f24-22, best f19, b1e1 f31; ours D/E0.x in volatile f10-f13 (shift). Declaring all scalars
at the top changes colouring a lot (raw 1252, best->f19) = lever; ch.py (choice-template hill climber, {{a||b}} / {{#grp:a||b}})
on vbt/t2.cpp (decl placement knobs) running -> vbt/ch2_best.cpp. Tools: regmap.py (orig->ours FPR map), bd.sh (blind diffs).
# SESSION 10 (2026-10-27, new lines agent)
## LANDED e7064024 light_dtor += CLight::SetDefaultLight (old park 54 fast; sc key sdl; source final/light_dtor.cpp). 75.70%.
Key: 'r = luma + 1.402 * (cr - 128.0f)' CSE'd offsets instead of 'cb -= 128.0f;' in-place (1 compile, 36 -> 0).
## cex (CVolCapsule::GetExtents): STRUCT 0 in cex2/m1.cpp (raw 20 = radius f6 vs bx colour swap). Key: the orig's
'bge L1; b L2; L1: fmr' = then-arm re-assigning a CSE'd value: 'if (ax < bx) ax = a.d.v[0] - radius; else ax = bx;' (ternary,
Min()/MinOf inlines, if/else stores all give an extra fmr or no b). Radius placement/form variants (cex2/v1-3.py) no gain.
perm_cex running (1h, base 115).
## LANDED eea430f1 box_linedist += Case0 (sc keys cz0 / c00 / bls / fce; source final/box_linedist.cpp). Key: by-ref Extent(i, float&) overload.
## LANDED b75dab00 box_linedist += Face (first compile with the by-ref Extent). sc key fce (range fixed to ..0x800c11d8).
## cex m2.cpp 9 raw / 0 struct: permuter hint 'if (ax < b.d.v[0] - radius) ax = a.d.v[0] - radius; else ax = bx;' (x of block 1 only). perm_cex2 running from m2.
## mry (_matrix_rot_yzx 0x800f4e98, 1016): NEW structure (mry/d3..d8, gm2_best.cpp 145 raw / 53 struct, same instruction mix as orig
except 1 extra callee FPR). Decoded: rows 'CVector3 right; right = m.GetRight();' with BY-VALUE getters + op= returning by value
(getter temps 184/168/152 are read, the dead op= copies 88/72/56); newRight/newFront/newUp declared after the rows, initialised
componentwise (x,y,z) (slot order 136/120/104 needs decl newRight,newFront,newUp); c (never stored) componentwise + compound;
compound args as explicit 'CVector3(s * v.x, ...)' temps: any INLINE-RETURNED temp (operator*, Mul, friend) passed to -=/+= makes
CW reload x components afterwards (3 extra lfs). Init via 'CVector3 a = v * s' is copied (not elided) when a is modified later.
Left: colouring (orig rows in f28/f13..f9, trig sy f29 sz f26 sx f30 cy f31 cz f27; ours one more callee reg). Trig decl order invariant.
## vbt KEY (2026-10-27): D/E0/E1 as NAMED FLOATS ('float dx = v0.x - c.x; ...' then diff.d.c.x = dx ...) gives the orig's
f30..f22 for all nine (vbt/n0.cpp, raw 579 / struct 35: frame needs one more callee FPR; best f31 vs orig f19; orig keeps
m_axes[0].x/.z in f21/f20 for the A0 cross axes). Hoisted-all n1.cpp. ch2.py (my scratchpad copy of ch.py) running on
vbt/t3.cpp (42 decl-placement knobs) -> vbt/t3_best.cpp. TRICK candidate: CVector3 components that should sit in callee regs
are named float locals in the original (SROA'd struct fields colour like temps).
## pgi (CPlane::GetIntersection 0x800910ac, 664, between matstack and plane_ctor; sc key pgi, tmpl scratch/game/tmpl_inc.json):
pgi/b1.cpp 50 raw / 4 struct (from old 76/2 draft): MSL inline fabs, 'CVector3 dir; dir = line.GetDir();' with op= by value
(orig slot 80 read, dead copy 64; tail reads dir.x from 80), return type 'unsigned char' gives the orig's final clrlwi r3.
Left: two opening load/fmadds swaps + |den| colour (orig f8 = n.x's reg).
## vbt: vbt/m0.cpp = 518 raw / 0 struct (NEW BEST; was ch2_best 523): n2 (all floats hoisted, b1e1 declared first, best after gz,
D/E named floats) + MACRO fabs (no MSL inline). With macro fabs D goes volatile again; ch2.py on vbt/t4.cpp (decl hoist knobs
from m0) running -> vbt/t4_best.cpp.
## cex: block compare-operand grids (cex2/v5.py, v6.py): m2 (9/0) stays best.
## HandleExplosion (0x800756cc, 736; sc key hex; claimed): hex/b0.cpp 56 raw / 6 struct ('int index;' hoisted). Left: index r7 vs
data(m_data) r6 swap in the first loop + SwapTexture inline block GPR colouring. Find forms/data local/const ref invariant.
Permuter harness can't parse fast_vec templates / ctor-style locals.
## vbt: decl-order hill (dhill2.py, vbt/dg_m1.py) -> vbt/dg_m1_best.cpp 508 raw / 0 struct (NEW BEST). Gains are tiny per swap;
the real problem is CW's colouring order of long-lived floats (D/E callee only with named floats + inline fabs, which breaks the rest).
# HANDOFF (session 10 end)
Landed: e7064024 light_dtor += SetDefaultLight; eea430f1 box_linedist += Case0; b75dab00 box_linedist += Face (75.66 -> ~76.2%).
Best WIPs: cex2/m2.cpp 9/0 (capsule GetExtents); mry/gm2_best.cpp 145/53 (structure decoded, colouring left; harness h_perm2.cpp);
vbt/dg_m1_best.cpp 508/0; pgi/b1.cpp 50/4; hex/b0.cpp 56/6 (HandleExplosion). Unchanged: gnp 15, bsp 19, bge 54/2, orth 21, quat 29.
Tools added: probe.sh (dump all functions of a probe file), memops.py (r1 float mem-op sequence), od.py (orig disasm), var2.py/dmh2.py/
dhill2.py/ch2.py (scratchpad-path copies). No processes running; lock not held.
# SESSION 11 (2026-10-27, new lines agent)
## tel = TriggerEnterAndLeaveMessages 0x800451ec (3480; append to proxtrig_point; sc key tel; claimed). NOT untried: AUTOPILOT
2026-10-22 log has the decode (scratch/pt/, prior best scored here: pt/BEST 172/156). Mine: tel/a14.cpp raw 126 / struct 88
(a11 151/80, a12 177/76 by ss.py; ~30 of struct are lis/addi reloc noise). Keys: ProximityDataView {prevMask, mask, prevLeaf, leaf};
core struct offsets id@0 (int: switch with ONE case gives lis/addi/cmpw beq;b), a0..a3 shorts @52, kind@72, enter/leave/enterAct/leaveAct
@88/92/96/100; Enter = if (core->enabled) {msgs; loop; climb} else switch(id); merge loop 'for(;;){if (newT==newEnd || oldT==oldEnd) break;'
+ '*newT > *oldT' + equal case 'unsigned newIn/oldIn' locals; merge locals declared first (newT, oldT, newEnd, oldEnd, newBit, oldBit,
newMask, oldMask) then assigned -> exact regs; E/M sites via 'trigger->core' pointer, loop + climb via g_pTriggerObjects[index] (a14).
STILL OPEN (same as 10-22): Enter's first core read 'addi rX,trig,4; lwz rX,0(rX)'. Tried (all fold to lwz 4): array/pointer/ref-local
per site (3^5 enum tel/acc2.py), by-ref inline params (IsEnabled, GetEnterMessages, whole-body EnterCore(MOH_core_Struct*&)), ref-returning
accessor, struct copy, switch(enabled), pc=&core. A by-ref param deref in a later sibling block DOES give addi+lwz 0 (a9) but is not
value-CSE'd with direct loads. Acquire: a0/a1 as int locals + a2/a3 in the initializer gives the orig's lha order (rest is RA-driven).
- tel: unified 'HandleTrigger(value, entering, activate, object)' (tel/f0.cpp) scores identically to separate Enter/Leave (CW const-props
  the flag) - either shape is plausible. Cast/char*-offset core reads also fold. tel/g0.cpp (ref-local core for E/M/S/A) raw 116 but
  shifts the Acquire set slot (sp+40 vs 56) - not better. BEST: tel/a14.cpp 126/88.
# HANDOFF (session 11 end): no landings. Best WIP tel/a14.cpp (TriggerEnterAndLeaveMessages). Lock not held, nothing running.
# SESSION 12 (2026-10-27 evening, new lines agent)
## LANDED ae41f603 plane_ctor += CPlane::GetIntersection (pgi 50 -> 0 in 3 compiles; source final/plane_ctor.cpp).
Key: |den| / |num| tests as 'if (Abs(denominator) > kEpsilon * line.GetLength())' with 'inline float Abs(float x) { return fabs(x); }'
(MSL inline double fabs inside) - the value-returning float inline in the condition gives the frsp temp the orig's f8; named
local / (float)fabs / assignment-in-condition do not. eps = 0.001745328f. port_unit: --like box_line.json (math.h include dir)
--original-file plane.cpp --file-index 5476 --pool .sdata2. scr.sh now has a pgi key.
## LANDED e6461006 NEW world_volume_ctor (CWorldVolume ctor 0x800cc29c; was 16 off since 10-08). Source final/world_volume_ctor.cpp.
Key: '(this+4)+16' head address kept unfolded via 'list* self = this; m_head.m_prev = &self->m_head;' (the BuddyInfo self trick);
extab: declare '~list();' (member cleanup action 0x800cc1f4) - port_unit's extab size 28 is short, linked 36 is right.
## brb (TestCDBBranchNode<CVolBox>): orig slots storage 136, vec 112, plane 80, frame 16 (plane NOT read back: Dist uses node fields);
ours storage 144, frame 80, vec 48, plane 16. Tried aligned(16) frame, plane temp/decl split, frame dtor/copy ctor, GetFrame(),
no CPlane dtor (vec then drops to 16): the frame copy always lands above vec. brb/s/*.cpp.
## cex: decl-order perms of block 1 (720, cex3/perm.py): all 9 (invariant).
## __sinit_collision_cpp 0x800c7d44 (408; parked 10-14 at 58): MATCHED in ~25 compiles (sin/z/z3b.cpp; sbs_sinit diff 0).
Keys: Reset loop with TWO counters, the slot pre-decremented in the index and the free-list one decremented first:
'int j = m_capacity; for (int i = m_capacity; i > 0;) { --j; *(P*)&m_memory[--i] = m_free; m_free = &m_memory[j]; }'
(both strength-reduced -> two offset IVs, unroll x4); Init 'memory = !storage ? alloc : storage' (bne to the storage arm).
Landing: sinit name follows the source basename -> unit collision_contacts RENAMED to collision (src/game/collision.cpp),
--file-duplicate the CFactory dtor, --weak-duplicate __ct__12SClsnContactFv, --pool .sdata2 --pool .rodata.
## LANDED 9610dc34 collision.cpp += __sinit_collision_cpp (see above). LANDED 6d571ed3 staticmesh_animdraw += CStaticAnimMesh::Init
(key: loop + rebase in an inline member Relocate(int base); 'li r31,0; mr r30,r31' counter/offset = loop inside an inline).
## rcmp DECODER ctor: still 4 (vt in r6, zero li hoisted above the vt store); 15 forms in rcmp/ (init list, self, IDATA ctor forms, op=).
## LANDED 4a56a79a bsutil_size += BSUtilObjectInstanceMemoryAllocator::Init (same trick: loop in inline member with params).
## WIP PatchUpAllPropertyData 0x8004372c (2620; claimed; append to propdat_core): pup_BEST.cpp (= pup/h1.cpp) raw 56 / struct 6
(score: ssr.py config/GMFE69/propdat_core.json <src> PatchUpAllPropertyData__FP9BPDHeader 0x8004372c 0x80044168).
Prefix = propdat_core.cpp with offsetPtr and EndianSwap(BPDLight&)/(BPDLightVolume&) DECLARED only (orig calls the weak copies after it).
Keys: header-level offsetPtr(..., (int)header) but nested ones with 'int base = ToInt(header);' (inline int ToInt(void* p) {return (int)p;})
after cursor - a plain 'int base = (int)header' gets CSE'd with every (int)header (r25 everywhere, copy at entry); char*/long base gives
per-section conversion copies. trigger declared first; '...n++, trigger++)' increment; light header loop C written inline with
'EndianSwap(light->field78[j], true)' (shallower inline -> right stack slot); case 14 through inline helper(light, (int)cursor);
switch case bodies order: shorts, playerPath, fog. Left: base r26 vs section offset IV r25 swap, plane loop volatile rotation.
## LANDED d0edc14c propdat_core += PatchUpAllPropertyData (2620). Final source final/propdat_core.cpp. chr.py = knob hill climber
(raw score; env T/FN/S/E for other functions). Knob search found the leads (++i etc. were noise, minimised away by pup/minim.py).
## ebl/ebv SOLVED (weak EndianSwap(BPDLight&) 0x80044168 / (BPDLightVolume&) 0x800443b4, 588 each): the "window" anomaly was
three-float VECTOR members swapped through an inline 'EndianSwap(PropVector3&)' whose members go 'EndianSwap(v.x, true)' (no ref
local): Light = {int, vec3, vec3, 3 floats, ulong, int}; Volume = {vec3, 3 floats, ...}. Both emitted as weak copies after
PatchUpAllPropertyData in propdat_core (inline definitions in that unit). Source final/propdat_core.cpp.
## WIP CAnimObject::ConstrainVelocity 0x80093e64 (1268; claimed; append to anim_obj_collide): acv_BEST.cpp (= acv/dh_best.cpp) raw 35 / struct 3
(ssr.py config/GMFE69/anim_obj_collide.json <src> ConstrainVelocity__11CAnimObjectFv 0x80093e64 0x80094358). From old 153/40.
Keys: whole body in 'if (size != 0 && !m_flag2450) {...}'; found = first != end bool (orig 'subf/subf/or/srwi.'), found = second != end;
pointers/floats declared at the top (begin,end,first,second,it; bx,by,bz,best,best2,b,a,zero); final update unfused via
float temps sx=b*bx, tx=px*a, dx=(sx+tx)*1.05f. Left: (1) 'beq body; b end' for the flag test (ours bne; 12 guard forms tried),
(2) orig loads m_data into r3 then copies to begin r31 AND it r26 (ours loads into begin) - 'for (it = m_collisions.m_data; ...)'
in loop 1 fixes that copy (acv/m2.cpp struct 2) but rotates all GPRs, (3) solve-block FPR colouring (decl-order hill 42->35).
# HANDOFF (session 12 end, 2026-10-27 ~19:50)
Landed: ae41f603 plane_ctor += CPlane::GetIntersection; e6461006 NEW world_volume_ctor; 9610dc34 collision.cpp += __sinit_collision_cpp
(collision_contacts renamed collision); 6d571ed3 staticmesh_animdraw += CStaticAnimMesh::Init; 4a56a79a bsutil_size += Init;
d0edc14c propdat_core += PatchUpAllPropertyData; 18dd751a propdat_core += weak EndianSwap(BPDLight&)/(BPDLightVolume&). (~77.6 -> ~79.1%)
Best WIPs: acv_BEST.cpp (CAnimObject::ConstrainVelocity) 35/3; cex2/m2 9/0 (knob search over compare forms: no gain);
quat 29/0 (helper-form knobs no gain); brb (vec slot order) and bge (vector-helper forms worse) unchanged; rcmp DECODER ctor 4.
Released: DECODER ctor, CStaticMesh::InitClass (uses the staticmesh.cpp .bss block base -> whole-file (e) route only).
Tools: chr.py (knob hill climber on raw; env T/FN/S/E), pup/minim.py pattern (minimise junk knobs), uncov.py <name-substrings>
(uncovered functions by name), ext.py (extab bytes/relocs of a compile), scr.sh has pgi. Lock not held; nothing running.
Protocol slip: once used 'pkill -f' on my own shell wait-loop (matched only that loop).
# SESSION 13 (2026-10-29, new lines agent)
## acv (ConstrainVelocity) 35 -> 25 raw / 0 struct: acv2/w4.cpp (perm harness acv2/ph.cpp, permuter acv2/perm running -j2, log acv2/perm.log).
Keys: guard = 'if (m_collisions.m_size == 0 || m_flag2450) return;' (gives the orig 'beq body; b end'); 'end = m_collisions.m_data + m_size'
(not begin + size) + pointer decl order it,begin,end,first,second (acv2/p/096) fixes the r3 copy + all GPRs; in the solve compute
b BEFORE a in source (fixes the numerator FPRs). Left: dc/bd FPR swap (f11/f8) and the final 1.05 block colouring.
Invariant: decl order (swap hill acv2/dh.py), statement order of the final block (acv2/fo.py, explicit reversed orders), the dc..det
statement order, inline Solve/Comb/Upd forms, k local.
## cdi (CCDBObject::Init 0x8007133c, 784): 97/79 -> 50 raw / 0 struct = cdi_BEST.cpp (= cdi3/best.cpp). Keys: MSL inline double fabs +
'x = sx * (0.5f * (float)fabs(r0.x)) + sy * (0.5f * (float)fabs(r1.x)) + sz * (0.5 * fabs(r2.x))' fully parenthesised (row 2 double),
px/py/pz locals for the min (orig hoists them), q* reloads for the max. Left: FPR colouring only (orig sz in f30, 0.5d f31, sx f12, sy f9).
Gens: cdi3/gen.py (decl/stmt order) gen2.py (+operand-order knobs: no gain from 50). Natural 'd->m_position.x - x' form = struct 79.
## NEW (never tried, >6000 B so missing from untried lists): CVolBox::TestCollision(const CVolBox&) 0x800c37d0 (6200, prepend to box_ivolume;
claimed) and CCDBTriangle::TestCollision(const CVolCapsule&) 0x8006a71c (6920, claimed). Box-box drafted: bbx/ (s.sh <src> [v|b] scorer, b = blind diff).
Decoded: Gottschalk OBB SAT: d.Sub(other.GetCenter(), GetCenter()); depth FLT_MAX, axis 0; a0..b2 = GetAxis copies; r.DotDotDot -> T (A·d),
U (B·d), rows R_i = (B0,B1,B2)·A_i, r.Abs(r) -> |R_i|; 6 face tests BoxBoxTest(extent, proj, n, depth, sign, axis) (box_dtor static) and
9 edge tests 'proj != 0 && !BoxBoxTest(ra + rb, ...)' (ra/rb named, computed before the test); FAIL macro; code = Compare(depth);
if (flag) { if (code > m_result) m_result = code; if (code == 2) { sep = depth; switch (axis) 1..15 contact; m_object = 0; AddContact } }.
Contact: faces Scale(axis, sign) + other/this m_matrix.TransformPoint(v1, p(±h*Sign(R))); edges Cross+Normalize+*=sign, closest-point
solve with den = 1 - R_ij^2 (case 13 uses R10^2: original bug). bbx/a6.cpp = 557 raw / 104 struct, same length; left = which floats spill.
## bbx status: bbx_BEST.cpp (= bbx/g0.cpp) raw 230 / struct 91, same length; whole structure incl. all 16 switch cases verified (blind diff
clean after 0x9b4 except jump-table relocs). Keys: face tests also via ra/rb locals, top-declared floats (decl order from cs2 search),
A02-first read of the row-0 Abs. LEFT = FPR spill SET: orig spills ux,uy,uz,A02,A12,A21 (slots 516..496) and colours the rest in strict
definition order (T f31-29, R0 f28-26, A00 f25, A01 f24, R1 f23-21, A10 f20, A11 f19, R2 f18-16, A20 f15, A22 f14); ours always keeps A02
and spills A20 (invariant to decl order, read order, q/proj/ra decl placement, Dot3 inline, permuter 1h = junk only).
Untried ideas: different h access (local half copies / GetHalf), CVector3 rows with float-wise op= (tried: not scalarised), splitting the
function's contact switch into an inline.
PROTOCOL SLIP (session 13): used 'pkill -f'/'pgrep -f <pattern>' that matched my own shell twice (self-kill, no other damage). Use '[p]attern'.
## bbx BREAKTHROUGH: spill set fixed by the ETEST statement order 'ra = ...; rb = ...; proj = ...;' (ra, rb BEFORE proj) -> struct 11
(bbx/b0.cpp = natural decl order; vmap.py <src> prints our reg->variable map + variable-level diff vs orig). Spill choice = cost (use count)
then live-range ties, so statement order inside the tests decides it. Left: prologue 'addi r31/r30' scheduled after the centre loads
(5 lines; a6.cpp had it right), FPR colouring (orig = strict definition order). cs3.py (decl order + row-read perms) running -> cs3_best_21.cpp.
## LANDED 5a8cfe1c box_ivolume += CVolBox::TestCollision(const CVolBox&) (6200). Source final/box_ivolume.cpp. Final keys: ETEST order
ra, rb, proj (spill set); per-case '{ float qy = ...; float den = ...; }' scopes (colouring became strict definition order);
'd.Sub(mb.GetRow(3), ma.GetRow(3))' (prologue addi schedule); BoxBoxTest defined at the end + --file-duplicate (static from box_dtor).
TOOL: bbx/vmap.py maps our/orig callee FPRs to variable names from the load pattern -> variable-level diff (great for RA puzzles).
## tcap (CCDBTriangle::TestCollision(const CVolCapsule&) 0x8006a71c, 6920; append to cdbgeom): delegated 10-29 ~08:00 to a helper
subagent (works in scratch/par/lines/tcap/, log tcap/LOG.md). Eberly segment-triangle sqr distance (parallel case first) + capsule contact.
## session 13 misc: bsp statement/interleave orders of local/a0..a2 (bsp3/): no gain (19). orth up-block 6-product permutations x dot order
(orth8/perm6.py): running/no gain so far. acv block-scoped solve temps (acv2/bs*.cpp): worse.
# HANDOFF (session 13, 2026-10-29 ~08:30)
Landed: 5a8cfe1c box_ivolume += CVolBox::TestCollision(const CVolBox&) (6200; 79.53 -> 80.64% incl. others).
Best WIPs: acv2/w4.cpp ConstrainVelocity 25/0 (was 35/3); cdi_BEST.cpp CCDBObject::Init 50/0 (was 97/79); bsp 19/0, gnp 15/0, orth 21/0 unchanged.
tcap (6920) delegated to a helper subagent working in tcap/ (see tcap/LOG.md when it ends). Claims: ConstrainVelocity, box-box (landed), tcap.
Tools: bbx/vmap.py (variable-level FPR diff), bbx/s.sh, bbx/cs*.py (decl/read-order searches), cdi3/gen*.py, acv2/v.sh, acv2/dh.py.
No permuters running (acv2/cdi3/bbx permuters killed).
## cdi: random statement/decl search cdi3/rs.py -> 49 (cdi_BEST.cpp updated). 11:00 resumed after usage limit; tcap helper resumed (was raw 28 / struct 0, tcap/LOG.md).
## tcap helper result: tcap_BEST.cpp (= tcap/d15.cpp, fn tcap/fn15Q2.inc) raw 28 / struct 0, len+frame equal; NOT landed. Left = volatile
FPR choice in the contact-point block only (0x1988-0x1a30: fS f5 orig vs f3 ours, fT f3 vs f10, fR f7 vs f11). Full decode + landing flags
(--weak-duplicate __dt__6CLine3Fv --pool .sdata2, append to cdbgeom) in tcap/LOG.md. Score: tcap/s.sh <draft> [v|b].
# SESSION 14 (2026-10-29, new lines agent)
## usp: UISStackProcess 0x80085a8c (8236, never tried; claimed) - bytecode VM, giant switch. Dir usp/: cmp.py <src> [cases] [-b]
(per-case diff vs orig using both jump tables; -b = register-blind), case.sh <op> (orig asm of a case), var.py (text-variant tester,
RAW=1 for raw), reorder.py (reorder switch cases to orig layout order = source order). Template t.json = UIStudio_visibility flags
(deferred inline, GC1.3.2). New unit needed between UISEvent (ends 0x80085a8c) and UIStudio_visibility (0x80087ab8).
Progress: a37.cpp blind 34 (incl. 8+? reloc-only lines in 0x4c/0x5b/0x5c), raw 354. Keys found:
- 'UISParam_t* sp = stack->sp;' BEFORE the opcode fetch (store to pc then blocks CSE with pops); POP = (*--stack->sp), PUSH = 2 stmts.
- byte reads as statements 'x = *stack->pc; stack->pc++;' (not *pc++); LONG = 4 'unsigned long bN' temps (pushes) but
  'unsigned char' temps for the counts in cases 99/100/101 (rlwinm/rlwimi vs slwi/or); SHORT = inline returning short.
- offsets: 'POP().i + (long)screen->data' (integer add, POP first); 'offset += stack->sp[slot].i' then use offset.
- group (UISThreadGroupInfoT 16 B at sp+28): values computed into temps BEFORE the field stores.
- compares written 'sp[-1] OP sp[-2]'; case order = orig layout; inner array switches start with case 97/98;
  0x66 loop counter 'int i' (long dims/total) gives the orig unfolded 'li; cmpw'.
- frame: locals declared group, bits (float bits for 0x11), arg, value.
Open: case 0x6a (orig has its own 'b' = non-empty body that vanishes), volatile colouring in several cases, 0x51/0x5b/0x65/0x66 callee regs.
- 10-29 ~13:00: usp/a50.cpp = everything matched except case 0x6a (orig keeps a lone 'b loop' block for the empty case; ours
  retargets the table entry to the loop test; 30+ empty-body forms tried) + reloc-only pseudo diffs. More keys: C-style decls at the
  top of a case fix colouring (vreg order = declaration order; 'long i;' first in 98, dims before total in 102-104, n before
  count in 0x51, b1 declared before b0 in ReadShort); 'mask = bit >= 0 ? 1 << bit : 0xffffffff'; inline
  UISScrAddr(screen, offset) = '(unsigned char*)screen->data + offset' for popped offsets (0x58, 0x6 with all args popped to locals).
- a51.cpp: 0x6a solved by a cross-member union self-assign 'sp[-1].p = (void*)sp[-1].i;' (CW drops it late, after the jump-table retarget). Now only reloc pseudo-diffs left.
## LANDED 69b87862 NEW UISStack (UISStackProcess, 8236; final/UISStack.c). File record is UISStack.c (the local .bss uisFormatStringStack told it).
## session 14 misc: TestCDBBranchNodeCollision<CLine3> 0x800701d4 looked at (Ghidra lbr/g.c, asm scratch/cdb/line_branch.s): SLocalFrame stack
(fast_vec over 20x80 __construct_array), line split by node plane into front/back CLine3 parts (CLine3(a,b) = start,end,dir=b-a,flags 0),
inline CLine3 leaf test (profile bin 25, CCDBVolBox(leaf box).TestCollision(line) then TriGroups with TestCollisionWithTriGroup, max result,
3 = stop). Dozens of CVector3 stfd/lfs stack temps (incl. dead double stores) -> released, low odds. acv final-block decl order: invariant.
# HANDOFF (session 14, 2026-10-29 ~12:20)
Landed: 69b87862 NEW UISStack (UISStackProcess 8236, whole file UISStack.c) -> 81.77%.
Best WIPs unchanged: tcap_BEST 28/0, acv2/w4 25/0, cdi_BEST 49/0, bsp 19/0, gnp 15/0, orth 21/0, bcp 132/13.
Tools: usp/cmp.py (per-switch-case diff via both jump tables), usp/dperm.py (declaration-permutation search for one case/block).
Claims: UISStackProcess (landed); TestCDBBranchNodeCollision<CLine3> released. Lock not held, nothing running.
# SESSION 15 (2026-10-29 afternoon, new lines agent)
## LANDED d1ad5980 UIStudio_active += UISProcessEvent (parked 15 since 10-14): param TYPE brute force (upe/ty.py: event/p1/p2/end/i over
ulong/uint/long/int) -> 'long event' 16 -> 9, then 'i' declared before 'end' -> 0. extern "C" fns: param types are free (no mangling).
Deferred file: the new function goes AFTER the existing one in source (port_unit "Inconsistent address for .text" = wrong order).
## UISStringFormat 0x8008a774 (648; parked 10-16 at 116): usf/ -> 0. Keys: all locals hoisted, decl order hill (usf/dh.py) 'text, more,
length, i, p, n, k, q' fixed every register; then per-case statement order (orig scheduling is REVERSED vs ours: 'k++; more=false;
n += 2;' in f, 'n++; more=false; k++;' in c/d/x); then NO p/q pointers at all: 'for (i...; i++) if (text[i] == '%')' and
'while (more && i++ < length) switch (text[i])' - CW's strength-reduced IVs give the orig 'i, p, q' increment order. Blind-diff trap:
identical case bodies 'c'/'d' swapped in layout (only branch targets differ; one_unit FAIL "bytes differ") -> case 'd' before 'c'.
## ISirenStartup (IStudio, 380): isu/ type knob search (member types + extern "C" UISGetMemSize/UISInit param types) 40 -> 26.
## ISirenStartup (IStudio, 380; parked 10-14 'param regs only'): isu/ks.py random type-knob search -> 0; minimal = the extern "C"
PROTOTYPES of UISGetMemSize/UISInit with 'unsigned long' params (were int; caller-side arg types change CW's callee order).
## CreateWeaponByCRC: type-knob search (wfc/ks.py: lookup/crc/key/i types, compare cast/order) no gain (17). UISExecuteFnc: stack copy local forms no gain (3).
## cex4: statement-order perms (720 each) of both GetExtents blocks: invariant (9). a/b decl swaps worse.
## acv3/fo.py <iters> <seed> <V>: random topo order of the final block WITH velocity reads as locals (V&2), k local (V&1),
split sums (V&4). Running V=2,6.
## decal Init__NotTheVirtualOne (decal.cpp tail blocker): dci/ - name read forms (pointer arith, &entries[i], (unsigned) casts, 11 forms) never give the orig's 'li r28,8' IV (+4 lwzx); released.
## session 15 no-gain tries (after the 3rd landing): AddAmo cast/type/local forms (2), SNDtimeremaining tk (body types, 5),
CreateWeaponByCRC (17), UISExecuteFnc stack-copy forms (3), cex4 statement perms (9), acv3 topo orders incl. velocity locals (25),
bsp4 dot/abs order 36 combos (19), decal Init name IV (released), CalculateLevelStats (bigfiles' slm.cpp, only location r30 vs r29:
tk types + location decl/direct forms no gain - bigfiles' claim, not touched in src).
TOOLS: tk.py <tmpl> <src> <fn> <start> <end> <iters> [seed] [--body-only], env FN_LINE='decl-line-prefix|...' = random hill climb over
int/long/unsigned int/unsigned long tokens in the function body(s) and extern "C" blocks (ISirenStartup-style fixes).
cdefs.py <tmpl> <src> <fn> <start> <end> = callee-FPR definition sequence orig vs ours. batch.py <parks.txt> = template + base score per park.
# HANDOFF (session 15, 2026-10-29 ~16:00)
Landed: d1ad5980 UIStudio_active += UISProcessEvent (316); 6250d629 UISUtils_actionptr += UISStringFormat (648);
01d272f1 IStudio += ISirenStartup (380). Best WIPs unchanged: tcap_BEST 28/0, acv2/w4 25/0, cdi_BEST 49/0, bsp 19/0, gnp 15/0, cex2/m2 9/0.
Claims: UISProcessEvent/UISStringFormat/ISirenStartup landed; CreateWeaponByCRC and decal Init released. Lock not held; nothing running.
# SESSION 16 (2026-10-29 ~16:15, new lines agent)
## tk sweep (sw/sweep.py <name-prefixes>, uses sw/parks.txt + batch logs): HandleExplosion, Draw CScene, ResetShell, DrawTextCentered,
SetFinishedLevelData, BuddyCheckpoints, ThrownObject Init, Add_MPPlayerKilled, DoTargetCheck, InputManager Update: NO gain (few knobs).
sw/dump.py <tmpl> <src> <fn> <start> <end> [lo hi] = side-by-side orig|ours with relocation names.
## coverstart (AIStartCoverPointWalkToStored) cvs/: 24 more flag forms (inline wrappers w/ bool/int params, BSTopB, BSPush inline, copies): no.
## ResetShell rs/: only 0/1 constant regs swapped (orig 0->r3, 1->r4) before the inlined SetGameType switch; 20 forms (param/member types,
flag order, bitfield types, zero local, ClearFlags/ResetGame inlines, SetGameType before flags): invariant 27.
## brb TestCDBBranchNode<CVolBox> 0x800724f0 (claimed s16): 76 -> 50 raw / 26 struct = brb_BEST.cpp (= brb3/c2_best.cpp). KEY: the box
GetDistanceFromPlane inline copies the frame as FOUR NAMED 'CVector3 rN = box.m_frame.m_row[N];' locals (not one CVolBoxFrame copy, not an
array) -> ALL stack slots now right (frame 16..79, plane 80, vec 112, storage 136). 'distance = plane.Dist(r3)' AFTER radius = 50.
Left = FP scheduling/colouring of the 3 dots (orig starts dot1, interleaves dot0/dot2). chr.py knob runs (brb2/tmpl1, brb3/tmpl2, brb4/tmpl:
dot impl orders, Dot arg order, term order/assoc, d-locals perms, row decl perms, Dist spellings, halfSize ptr, frame ref) no gain past 50.
NOTE chr.py writes chr_tmp.cpp next to <out>: never run two chr.py with outputs in the same dir.
## PlayerScreenFlash (bsbifunc_death tmpl, 0x80030658 388) psf/mk*.py: base draft 77/68. Orig slots: from built 16 -> word copy 20 -> bytes to
m_from; to built 8 -> 12. 'Set(CColor from, CColor to, float)' by value + user member-wise operator= (C1) + Set(BSArgColor(..1), BSArgColor(..5), dur)
(psf mk.py variant C1/SETV/B2) gives EXACTLY those slots, but CW evaluates args right to left (to's loads first) and needs r29 (len 99 vs 97).
Named v1..v8 locals + CColor(v..) temps: len 97 but pairs swapped. 54-combo grid mk5.py: best struct 55. Not landed.
## brb UPDATE: 50 -> 25 raw / 4 struct = brb_BEST.cpp (= brb5/best.cpp). Second key = the CCDBVolBox plane trick (0121e6d1):
'CVector3 proj(BDot(r0, n), BDot(r1, n), BDot(r2, n));' (3-float ctor, args evaluated right to left) + 'width/depth/height = (float)fabs(proj.d.v[i])'
+ radius 'h0 * width + h1 * depth + height * h2', distance = plane.Dist(r3) LAST. chr template brb5/tmpl.cpp (3 seeds) plateau 25/4.
Left: copy-load order (orig lfd row2.xy(48) before row1.z(40)) + f3/f4, f5/f7, f7/f8 colour pairs. Invariant: row decl perms, frame ref,
CVector3 user copy ctors (pair/d/reversed), const ref n, row ctor-init.
## mrot (CMatrix::Rotate): landed-BuildRot body = 31 (worse); chr position/commutation hill mrot3/ = 21 again; 'one' local, t -= c, sc[2],
cc/ss copies, axis copy: 21+. Left = (1.0,ty) and (s,x) FPR pair swaps.
## brb UPDATE 2: 25 -> 8 raw / 4 struct = brb_BEST.cpp (= brb6/z.cpp). Third key: r3 (centre row) copied as its own named local declared
AFTER radius, right before 'distance = plane.Dist(r3)'; radius 'width * h0 + depth * h1 + height * h2'. ALL FP colouring now right.
Left ONLY: copy-load order in the first block (orig lfd row2.xy(48) BEFORE lfd row1.z(40); ours r1.z first) -> regs f1/f0 vs f0/f2 there.
Exhaustive brb6/ex.py (row perms x proj arg order x fabs perms x Dot impl, 144): all 8/4. Caller forms (pop/plane order, side decl), n ref,
r0/r1/r2 assign/comma decls, Project() by-value inline (95+): no.
## LANDED 59930c8b cdbobject_trigroups += TestCDBBranchNodeCollision<CVolBox> (740). Final key: 'CVector3 r0, r1, r2;' then assignments in
REVERSE order (r2, r1, r0) -> 0 (decl order = slots, assignment order = copy schedule). Source final/cdbobject_trigroups.cpp.
Port: port_unit ... --like itself --original-file cdbobject.cpp --file-index 3304 --weak-duplicate __dt__6CPlaneFv --weak-duplicate
'__dt__Q23dwi27fast_vec<PC13CDBBranchNode>Fv' (NO --pool: keeps the owned .sdata2 section). 82.44%.
## gnp 15 -> 13/0 (gnp_BEST.cpp = gnp16/best_hoist.cpp): dots hoisted 'float a01, a00, b1, b0, a11;' + assignments a01, b1, a00, b0, a11
(gnp16/ex.py: all 120 orders, plain vs hoisted). Hoisted decl order is inert (120 perms), only assignment order matters. Left: b0/b1
y-product order at 0x74 (orig b0 first) + regs. chr on e0/e1/diff/dots perms: 13.
TOOL hz.py <sc-key> <src> <first-line-substr> <n> [iters]: hoist the 'float x = e;' decls of an n-line block and search
dependency-respecting statement orders (exhaustive <= 5040). PlayerScreenFlash: named-copy forms (copies elided, len 75): no.
## session 16 more no-gain: hz.py on orth (dot/products blocks, 21), quat (10 stmts, 29), mrot (13 stmts, 21);
UISExecuteFnc frame forms (sp++ post-inc, ternary, 'sp = frame + 1', screen copy): 3; cex 3-float ctor min/max forms: worse (20+).
# HANDOFF (session 16, 2026-10-29 ~17:35)
Landed: 59930c8b cdbobject_trigroups += TestCDBBranchNodeCollision<CVolBox> (740) -> 82.44% at the time.
Best WIPs: gnp_BEST 13/0 (was 15), tcap_BEST 28/0, acv2/w4 25/0, cdi_BEST 49/0, bsp 19/0, orth 21/0, quat 29/0, mrot 21, cex2/m2 9/0,
PlayerScreenFlash psf/ (slots exact via C1/SETV/B2 but arg evaluation order; best struct 50-55), ResetShell rs/ 27 (0/1 const regs).
Tools: hz.py (hoist + dependency-respecting statement orders), sw/sweep.py (tk over parks), sw/dump.py (side-by-side with reloc names).
Claims: TestCDBBranchNode<CVolBox> landed; tk sweep released. Lock not held; nothing running.
## PlayerScreenFlash UPDATE: inferred inline with SWAPPED params 'Set(CColor to, CColor from, float)' called as
Set(BSArgColor(..5), BSArgColor(..1), dur) -> from's loads first like the orig, 51/32 (C1 user op=, psf/p51.cpp) / 40/37 (POD, psf/p40.cpp);
but the pair slots follow PARAMETER position (from pair at 8/12, orig 16/20). Separate SetFrom/SetTo by-value inlines: worse (77+).
# SESSION 17 (2026-10-29 ~17:45, new lines agent; task: whole-file merge near collision/geometry)
## Survey (fileopen_all.py = bigfiles/wf/fileopen.py listing ALL open fns; filefns.py <file.cpp> = fns + owner unit + data ranges;
basescan2.py < allopen.txt = data-base/guard refs of each open fn -> baseopen.txt). Lane files: box/capsule/cdbobject/cdbgeom opens are
pure regalloc parks (no base/guard refs) -> merging does not help them. compartment.cpp: Link (guard prevVertFormat$1091 only), Draw
(.bss base + g_DefaultHW$827 guard), InitClass (.bss base + bReady$730/init$731), Init (.bss base), __sinit (.bss) are data-blocked;
DrawNode (raw 80 FP) / HandleExplosion (53) regalloc. ANY unit owning the .bss block must contain __sinit and every base user ->
Draw..end (Draw, DrawNode, HandleExplosion, InitClass, Init, sinit = 7.3 KB new) and needs Init's GXInitTexObjCI shape-in-r4 knot
(soldier s19 / bigfiles CStaticMesh::Init 103). gx/ probes: image expression forms (5) x width/height forms (3), locals/copies (10),
16-bit bitfields: shape always coalesces with the LAST use (height, r6) - the orig's IR evaluates image after width/height. Unsolved.
## CCPTMatBin::Link 0x80073bd4 (1504, never drafted; twin of bigfiles' landed CMSHMatBin::Link b9369c22): cpt/ (sc key cml,
tmpl cpt/tmpl.json = compartment_render + dolphin-sdk include) -> 0 in 6 compiles. Keys: 'unsigned long next' + 'int command = next'
(cmpwi), first word 'next = t->m_data[0]; index++;', node = (CPTDrawNode*)next after the command block re-reads next = data[index++],
light list node->m_owner(+0)->m_lights(+92) {next, CLight*}, colour '(u8)light->m_color[k]' (the (int) cast mis-schedules one fctiwz),
GetPosition = ISceneNode virtual slot 15 (vtable +64), PSMTXMultVec into an 'eye' CVector3, GXLoadNrmMtxImm(.., i * 3) with NO
separate id counter (a separate 'id3 += 3' IV puts the reset loop bound in r26), reset 'for (j = 0; j < i * 3; j += 3)', ambient
{128,128,128,255}/material {255,...} literals, 'static int prevVertFormat = 0;' declared inside 'if (list)' (guard mid-function).
Port: port_unit compartment_render ... --like staticmesh_render.json --category reconstructed_game --original-file compartment.cpp
--file-index 3422 --pool .sdata --pool .sdata2 -> VERIFIED.
## LANDED 627aafc1 compartment_render += CCPTMatBin::Link (1504) -> 83.66%. Source final/compartment_render.cpp. Compartment tail claim released.
## lb/ TestCDBBranchNodeCollision<CLine3> 0x800701d4 (2696; claimed s17; sc key clb; tmpl cdbobject_line.json, append to cdbobject_line):
first draft from cdbobject_line.cpp views -> lb/f1.cpp raw 384 / reg 140 / stk 176 (lb/ns.py <src..> = raw / regs-masked / regs+stack-
offset-masked counts via fast.parse; V=1 prints the masked diff; lb/dd.sh <src> = side by side). Frame size now exact (2960).
Keys so far: SLocalFrame with INLINE ctor/dtor (out-of-line weak copies come from __construct_array refs; the orig never calls them for
the named frames), 'int result = 1;' before the first OSGetTick, 'start.m_node = &root;' before 'start.m_line = line;', back()+pop_back(),
CPlane(const CDBPlane&) : m_normal(p.m_normal), m_d(p.m_d) (CVector3(const CDBVector&) ctor), Dist(const CVector3&), split branch
FIRST ('if (d1 < 0.0f) {split} else {front.SetSE(start, end)}' on the d0 >= 0 side; '(d1 >= 0) split else back = cur.m_line' on the
other), lines filled by SetSE(CVector3, CVector3) by value (no CLine3 temporaries), leaf test = inline CLine3 specialization of
TestCDBLeafNodeCollision (profile bin 25: 'OSGetTick(); time -= OSGetTick(); ... time += OSGetTick(); count++; OSGetTick();'),
operators both const& with the 3-float ctor (lb/grid.py knob grid). Left: the split block's temps: orig copies start (208) and dir
(224) by value and computes t*dir into the slot 288 that then holds mid (x first: member-wise, not the right-to-left 3-float ctor).
- s17: coordinator rule: no '#pragma dont_inline' in new game-code landings (Waiting-on-user 5(b)); none used by lines.
## lb UPDATE (s17 end): TestCDBBranchNodeCollision<CLine3> -> lb_BEST.cpp (= lb/f6.cpp) = 2 instruction lines off (fast.parse 6 incl. moves;
everything else exact incl. ALL stack slots, length 674). Final keys: split point 'CVector3 mid(t, cur.m_line.m_dir); mid += cur.m_line.m_start;'
via an inline ctor CVector3(float t, CVector3 v) {x = t * v.x ...} (by-value v = the dir copy; mid is a NAMED local so it is allocated
with the named locals right after the plane) + 'operator+=(CVector3 o)' by value (the start copy); leaf result 'if (r > result &&
(result = r) == 3) break;' (compare after the move); Dist(const CVector3&) with GetStart()/GetEnd() by-value temps.
LEFT ONLY: __construct_array arg setup registers: orig 'lis r4,ct; lis r3,dt; addi r5,r3,dt; li r6,80; addi r4,r4,ct; addi r3,r1,1216',
ours 'lis r3,ct; lis r5,dt; addi r4,r3,ct; li r6,80; addi r3,r1,1216; addi r5,r5,dt'. Probes ca/probe*.py: the orig style appears when the
array address is an early vreg (array as a member at a NONZERO offset of an object built by an inline ctor, e.g. 'struct E { E() {} int n;
F s[20]; }'), but then the address stays in a callee reg (w1/w2: r17, frame +16) while the orig rematerialises 'addi r3,r1,1216'.
Invariant: decl order/comma decls/&storage[0]/pointer local, SLocalFrame ctor/dtor inline/implicit/out-of-line, flags (deferred, 1.3),
wrapper struct at offset 0. Port plan: append to cdbobject_line (cdbobject_fastvec_push/frame units follow; SLocalFrame ctor/dtor weak
copies need --weak-duplicate or are the next units' functions -> fold cdbobject_fastvec_push + _frame into the unit).
# HANDOFF (session 17, 2026-10-29 evening)
Landed: 627aafc1 compartment_render += CCPTMatBin::Link (1504) -> 83.66%. Lock released; tree clean; nothing running.
Best WIP: lb_BEST.cpp TestCDBBranchNodeCollision<CLine3> (2696) 2 lines off (construct_array arg regs only; claim kept). Others unchanged
(gnp_BEST 13/0, cdi_BEST 50/0, tcap 28/0, acv2/w4 25/0, bsp 19/0, orth 21, quat 29, mrot 21, cex2/m2 9/0).
compartment.cpp tail merge released: needs the Init GXInitTexObjCI shape-in-r4 knot + DrawNode + HandleExplosion (all in one .bss-owning unit).
Twin scan tool: twins.py (open fns vs matched fns by mnemonic/callee cosine) - top unmatched twins are all known parks/claims.
# SESSION 18 (2026-10-29 evening, new lines agent)
## lb __construct_array regs (s18/): orig extab (0x8000b7a0, 228 B) == ours byte-for-byte (excl. relocs) -> storage IS a plain local
array (0x85 destroy-local-array action at 1216) and start a separate local (1136): NO wrapper struct possible. Binary survey (all 60
__construct_array calls, s18 notes): orig style ('lis r4 ct; lis r3 dt; addi r5,r3') appears ONLY for member arrays at nonzero offset
in (inline) ctors (CPlayerObject CPlane[10], CAnimatedVolume CVolBox[24]) where arr = 'addi r3,rThis,off'; every local-array call
(CAnimatedVolume::TestCollision(CLine3) CCollision[24], landed) is OUR style. CLine3 branch is the only r1-based orig-style one.
Probe s18/pv.py <variants file, '====' separated> prints style per call. No flip from: implicit/explicit ctor/dtor of SLocalFrame and
CLine3 (16 combos), ctor/dtor decl order, out-of-class inline defs before/after the fn, result/OSGetTick forms (g1..g6), const/register
arrays, inline helper or template static holding the array, aligned(16) variants, __construct_array prototypes in scope,
compilers 1.3/1.3.2r/2.0 (all OURS on the minimal local probe). Member-at-offset-0 wrapper -> 'this' folded to r1 and OURS.
## CPartBin::Link 0x800f5930 (1492; prepend to dmesh_bins; took over stale bigfiles claim; sc key cpl): cpl/r1.cpp raw 77 / struct 5
(from bigfiles pl11a 113/61). Keys: light pos = THREE stores (72,56,88) via 'CVector3 position; CVector3 lightPos; lightPos =
lighting->m_lightPos[i]; CVector3 tmp = lightPos; position = tmp;' (position declared first -> slot 88), (u8) casts on ALL colour
conversions (fctiwz schedule, like CCPTMatBin), 'lighting = &mesh->m_lighting' declared before geometry and matrices =
lighting->m_matrices. Left: callee-reg permutation (orig this r23/tag r24/packet r25/mask r29/lighting r31/count r26; ours +1/r31/r29/r23).
- cpl UPDATE: rs-score (cpl/rs.sh = reg-relevant mismatched lines) r1 67 -> dLMK 54 ('DMeshLightingView* lighting;' then 'int mask = 0;'
  declared at fn top after fogColor, mesh local in loop... see cpl/dLMK.cpp) -> cpl/q/sjpq_1.cpp 47 (C-style, 'int slot;' at fn top).
  Perm dirs cpl/perm (base r1) and cpl/perm2 (base dLMK). Left: params r24-26 vs r23-25 (count/rowptr in r23 vs orig r26),
  parts loop p/off regs, light-loop FP regs (orig colours f3..f0, pos f4/f3; ours f5..f2 / f1,f0).
- cpl UPDATE 2: 47 -> 33 = cpl/b33.cpp (= cpl/pm/0412356.cpp): 'float red = lighting->m_lightColor[i][0]; if (red != 0.0f || ...)' and the
  light block statement order color.r, lightPos = .., color.g, color.b, tmp, color.a, position = tmp (TRICKS 181: later-created FP temps
  get lower FPRs). Light loop FP now exact. Left: params+1 / count-rowptr r23 (orig r26) / parts loop p,off,j,slot regs. cpl/hc.py
  (decl-move hill climb) found nothing from 47; cpl/rgen.py random top-declaration sets running.
- s18: coordinator 21:30: user decisions -> '#pragma dont_inline' now allowed in game code; register devices still declined.
- cpl s18 no-gain at 33: rgen random top-decl sets (400), p int/u16, j u32/u8, part = m_parts + p, no part local (56),
  ternary/if-else GXCallDisplayList (63/67), count local removed (39) / int count (41), index inline (42), ++p.
  perm2 (base dLMK) and perm3 (base b33, permuter base score 190) only find illegit type hacks (char p, long long size/index).
# HANDOFF (session 18, 2026-10-29 late)
Landed: none. Lock never taken; tree untouched; no permuters/background jobs running (perm2 stopped, perm3 timed out).
Best WIPs: cpl_BEST.cpp (= cpl/b33.cpp) CPartBin::Link 0x800f5930 (1492, prepend to dmesh_bins, sc key cpl, score with cpl/rs.sh)
= 33 register lines, structure/stack/FP exact: params r24-26 vs orig r23-25, count/light-row IV r23 vs r26, parts loop p/off/j/slot.
Port when matched: --like dmesh_bins / --pool .sdata2 (0.0f @883, @@199/@@315/@@709 refs) --category reconstructed_game.
lb_BEST.cpp unchanged (2 lines, construct_array regs; s18 findings above say a plain local array never gives the orig style).
Claims kept: TestCDBBranchNodeCollision<CLine3>, Link__8CPartBin (takeover noted in CLAIMS.txt).
# SESSION 19 (2026-10-29 late night, new lines agent; task: twins.py + data-base whole-file merges; <=1 fresh idea each for lb/cpl)
## Survey refreshed: s19_files.txt (fileopen_all), s19_allopen.txt, s19_base.txt (basescan2), s19_twins.txt (twins.py). Lane files still have
NO data-base-blocked opens (box/capsule/cdbgeom/cdbobject/matrix/quaternion = pure colouring parks). Twins top list = known parks/claims
(list<BuddyInfo> range ctor = buddy partial-SROA knot; RotateRel soldier; Orthonormalize/box GetExtents/capsule GetExtents/bpl/gnp lines).
## lb fresh idea (s19/t1.txt via s18/pv.py): SLocalFrame as a CLASS TEMPLATE / member template class / local class inside the template fn
(6 variants): all OURS style. lb stays 2 lines.
## cpl fresh idea: reuse the light-loop 'i' as the parts-loop counter (s19/cpl/i1.cpp): count/row/p now SHARE one register class like the
orig (orig {count,row,p}=r26, ours r23) but params stay r24-26 (orig r23-25) and 'off' starts as 'mr r22,r23' (copy of i=0) -> still 33.
Variants (i decl at top/u32 i/part ptr) 33-40. Permuter s19/cpl/perm (base i1, 385) ran 40 min: nothing.
## ChooseCoverPoint (claimed 22:4x, fresh look): no new idea beyond parks s20 (this/type/from rotation); released.
## vbt (CCDBVolBox::TestCollision(CTriangle) 3856): natural named-float draft with MACRO fabs = s19/v0.cpp 523/0. Inline MSL fabs ONLY at
the Separate '(float)fabs(d) < (float)fabs(best)' sites: v0 -> 515 (s19/fx_0100.cpp), dg_m1_best -> 499/0 (s19/dgd.cpp = NEW BEST, tiny).
Inline fabs at p/n/b sites drops one callee FPR (frame 320 vs 336, struct 35). Orig callee FPRs: dx f30 dy f29 dz f28 fx f27 fy f26
fz f25 gx f24 gy f23 gz f22 (STRICT definition order), f31 = a (float)fabs(a1.e) b-term (used by later tests), f21/f20 = m_axes[0].x/.z
then cross results, best f19. Toy (s19/toy/gen.py): with decl order = def order named floats colour first-fit f0..f13 then f31 down in
DECLARATION order; reversed DEFINITION order breaks that (a0 f13, a1..a17 f31..f15) -> colouring order is not simply decl or def order.
## tcap 28 -> 25 (s19/tc/e.cpp, NEW BEST; copy of tcap/d15.cpp): contact block 'SetSum(point, sv, m_vertices[0]);' (sv FIRST; the old
'SetSum(point, m_vertices[0], sv)' = 28) -> the base loads now get f2/f1/f0 like the orig. Operand order of the SROA'd-vs-load sum DOES
matter here (helper's 'CW canonicalises' does not hold for this). Left (25, same block): s f3 (orig f5), products f5/f4/f3 (orig f3/f4/f6),
sums f9/f8/f7 (orig reuse f5/f4/f6), fT f10 (f3), fR f11 (f7). point+=tv / SetSum(point,point,tv) same; diff 'rv + start' worse (29).
Score: sh tcap/s.sh <src> (0.4 s/compile). Random search s19/tc/rs.py <N> <seed> (component forms x interleavings).
## bpl (CVolBox::TestCollision(CPlane) 0x800c15ec, 2292) structure re-read (s19/bpl/): orig 'point' is ONE out-param object at 416
written in place (Scale products stored, then += p sums stored) and copied to contact.m_vector1 (448) on a hit; separation at 464.
Draft b1.cpp: 'float Project2(CVector3 p, CVector3& out) const { float distance = GetDistance(p); out.Scale(m_normal, -distance);
out += p; return distance; }' + step 'float distance = plane.Project2(corner, point); if (distance < contact.m_separation) {...}'
-> each step now has the orig's shape (A copy for the dot, B copy for the sum, product/sum stores to point) but the orig has a THIRD
per-step corner copy C (128/112/96..., 16 B each, never read; frame 672 vs ours 528, len 573 vs 553). Dead calls/pass-through by-value
inlines are removed whole by CW (no copy). b3 (GetDistance(corner) at the step + Project computing it again) gives frame 672 and 3
copies but computes the dot twice. grid.py (by-value/ref GetDistance/Project/Dot/+= x outer TestCorner): best 435 raw (g_00101) but
still 2 copies. Open question: what source makes a dead, unread by-value copy survive. (a2 was 448/319.)
## bpl UPDATE: 448 -> 239 raw (s19/bpl/best239.cpp = g3_X7P3.cpp; frame 672 + length 573 EXACT; with stack offsets and FPR numbers
masked only ONE line differs (the @CONTACT_EPSILON reloc)). Plane helpers: 'float Dist3(CVector3 p) const { return n.x*p.x+... -
m_distance; }', 'float GetDistance(CVector3 p) const { return Dist3(p); }', 'float Project2(const CVector3& p, CVector3& out) const
{ CVector3 q = p; float distance = GetDistance(q); out.Scale(m_normal, -distance); out.AddV(q); return distance; }' (AddV(CVector3 o)
by value), point declared BEFORE corner (corner 400 / point 416 like the orig). Left: (1) stack-slot FAMILIES of the three per-step
corner copies: orig sum-read 384..272 (step -16), dot-read 256..144, dead 128..16; ours sum-read q 384.., dead GetDistance.p
interleaved 368.., dot-read Dist3.p 128... Observed CW rule (s19/bpl/grid*.py): slots are allocated in families top-down:
F0 = depth-1 inline by-value params, F1 = depth-1 locals + depth-2 params (interleaved per call site), F2 = depth-2 locals +
depth-3 params. A dead copy is only KEPT when it is the middle of a by-value copy chain (its read is a copy forwarded to the next
param); an unread copy or a dead call is deleted whole. So the orig's 'dead copy in the deepest family' is not yet explained.
(2) FPR colour shift in the prologue: orig a=(f7,f8,f9) b=(f10..f12) c=(f13,f31,f30), center.zw f2; ours everything +1
(center.zw f3, c uses f31,f30,f29). Decl/statement order of a/b/c/corner/point: invariant (c1-c5).
- bpl colour shift is invariant to: a/b/c decl/statement order (c1-c5), componentwise/Scale/h-local forms (m0-m3), a/b/c as NINE
  named floats with componentwise corner updates (nf1): all 239. Family probes: g4_Y4P0 (GetDistance(CVector3 p){CVector3 q = p;
  p = q; ...}) puts the dead copy q in the BOTTOM family (128/136) like the orig, but rewrites p (extra stores, len 585).
- bpl FAMILIES SOLVED in s19/bpl/z3.cpp (= best_fam.cpp): 'static CVector3 Id(CVector3 v) { CVector3 q = v; return q; }' +
  'float GetDistance(CVector3 r) const { return n.r - m_distance; }' + Project2(CVector3 p, out) { float distance =
  GetDistance(Id(p)); out.Scale(m_normal, -distance); out += p; return distance; } -> copies exactly like the orig (dot-read 256..,
  sum-read 384.., dead q 128..). But z3 needs one callee FPR fewer (no f21 save: frame 656, len 569) and the first steps schedule
  differently. Metric: s19/bpl/lcs.py <abs src...> = LCS misses raw / with FPR+slot masked (best239: 222 / 0; z3: 225 / 35).
  The Id() helper is invented - if this ever lands it needs a natural spelling (POLICY: 'invented dead function' was declined
  for ShellMenu; an identity copy helper is similar). grid6.py (helper/param/copy combos, lcs metric) running.
- bpl grid6 (helper/param/copy combos, lcs metric): best LCS-miss 219 ('const CVector3& p' + local q + GetDistance(q) by value + out.AddV(q),
  no Id) vs best239's 222 - marginal; not kept as a file.
## vbt SA (s19/vsa.py <iters> <seed> [base]: annealing over hoisted-decl subset+order): seed1 from dgd 499 -> 498; seed2 from natural
fx_0100 515 -> 495 = s19/vbt_best495.cpp (NEW BEST, tiny).
# HANDOFF (session 19, 2026-10-29 night)
Landed: none. Lock never taken; no processes left running (SAs/permuter stopped). Tree: config/GMFE69/AIObject_storecover.json +
src/game/AIObject_storecover.cpp are modified by ANOTHER agent (not lines) - left untouched.
Best WIPs: bpl_BEST.cpp (= s19/bpl/best239.cpp) CVolBox::TestCollision(CPlane) 448 -> 239 raw, frame+length exact, 0 lines with FPRs+slots
masked; s19/bpl/z3.cpp has the orig's copy-slot families (Id helper) but one callee FPR fewer. tcap_BEST.cpp (= s19/tc/e.cpp) 28 -> 25.
vbt s19/vbt_best495.cpp 508 -> 495. lb (2) / cpl (33) unchanged after one fresh idea each. Claims: bpl (new), lb, cpl kept; CCP released.
