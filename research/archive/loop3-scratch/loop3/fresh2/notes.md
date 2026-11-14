# fresh2 (2026-11-12): fresh1's untouched.tsv worked from the bottom (most-worked end)

## Landed
- 00b269c6 capsule_plane += CVolCapsule::GetExtents (304). Key: per block copy start, take its 3 terms, THEN copy end
  (interleaving of copy and use; 8-way interleave search cex/v2.py). In the real unit (shared CVector3 header, cast copy
  ctor) the getters add an 'addi r6,r3,40' CSE: copy members 'CVector3 a = m_axis.m_start;'. Lesson: score the draft
  INSIDE the target unit source before landing; header differences change the result.

## Parked (this session)
- CVolBox::TestCollision(CVolSphere) 20 rd: local x/y/z store order x fabs order (36) inert. Landing would prepend to
  box_ivolume, whose Sign is an out-of-line static defined after box-box, but box-sphere inlines Sign: whole-file question.
- CVolBox::GetExtents 45: two-phase (products then fabs) and orig product/conversion orders 50-88. FP schedule.
- AddAmo 2: can/type locals, w local, CanAddAmo spellings: all 2 (IR canonical).
- CStaticMesh::DrawNode 4 real: 72 dot/sum/d orders, one-expression, float locals, no n ref, dots-then-fabs: inert.
- GetPeekLocation 6 -> 3 real: member 'CoverPointTypes operator|(const CoverPointTypes&) const'
  instead of the friend gives the orig mask registers (draft updated). Left: the '4' temp's load is scheduled after
  or(1|2) (orig: all four loads first). Mask-first ordering fixes the schedule but moves the .sdata temps (ff must be
  created first: orig @766 ff, @767 1, @768 2, @770 4). Tools: gpl/rel.py (SDA reloc -> .sdata offset per load) and
  gpl/at.py (@-number gaps). @-numbers of const-int& temps are a fingerprint: orig 0x51/0xa2 are consecutive (@803/@804),
  ours 4 apart; 'if/else' costs ~3 numbers, '?:' none, precomputed temps none (probe2.py). Probably inline helper
  definitions (numbered at definition), so no codegen lever found.
- CMatrix::Rotate: the draft's inline BuildRot body differs from the LANDED out-of-line BuildRot (matrix_translate.cpp);
  with the landed body Rotate is 31 rd (draft 21). Landing the draft would be the q8 per-fragment-view shape. Also:
  Rotate belongs at the END of matrix_rotate (deferred-inline unit, ends at 0x800904e8), not matrix_translate.
- CSoldierObject::Draw 22 rd: RemoveWeapons' pointer IV r28 vs orig r24 (call/flag order, nested if, unsigned i,
  weapon local, landed for-init body: inert).
- SNDPKTPLAY_start 2: orig IV init 'addi r7,r29,0; add r7,base,r7' = base + (slot*128 + 2i); every form groups the base
  first (loudestvoice(voice) 8, slot-index inline 24, byte-offset 20).

## Draft updates (committed)
- 0c9e12e9 CCDBTriangle::TestCollision(CVolCapsule) 25 -> 10: SetSum(point, m_vertices[0], sv) (vertex FIRST: orig
  fadds vert,sv) + MulV on the sv/tv products field-wise; rv keeps Scale. Left: fR f7 vs tv f8-f10 (orig loads fR
  before the tv products in allocation order); inert to statement order (prepass scheduling), fR locals, diff forms.
  Scratch: tcap/k_tv_scale.cpp.  Also GetPeekLocation 6 -> 3 (member operator|).
- 54d3c9c8 CStaticObject::OnCollision 21 -> 3: motion as plain copies then compound subtractions in x,y,z (the 3-line
  form other agents had in scratch was never committed). Left: orig reloads point.x into its own register.
- (landing) ConstrainVelocity 16 -> 14 (per-component Mul solved block, all sums 's + t'); CVolBox::TestCollision
  (CVolCapsule) 108 -> 102 (n defs x,y,z). NOTE: CW orders commutative fmuls operands by colouring, not spelling;
  'n*(h*a)' vs '(h*a)*n' both inert.
- Parked further: DisplayHint (480 decl orders x 4 assignment forms), CQuaternion::Multiply (decoded layout w at +0).
- (landing) PublishEvent 46 -> 3 = late3's scratch pe0.cpp that was never committed. LESSON: scratch bests can be far
  ahead of research/drafts; GetInitialBuddyOrders has scratch/loop3/scripting/chk/GI_BEST7.cpp = 7 real vs draft 27
  (bigger restructure: GiveBuddyOrders(BuddyInfo* const), DumpBuddyVector helper; not committed by me - check its forms).
