# late1 (day2, scratch/loop3/day2/late1.tsv), 2026-11-13

Tools here: sc.py (json func files... [-v]: difflib count + side-by-side diff; SUFFIX=c for C),
dsweep.py (one-declaration move sweep), tsweep.py (single integer-type substitution sweep in the
function body + prototypes/inlines), flagx.py (-inline variants), perm.py (full decl permutations),
real.py (score.real for a row + candidate files), batch*.sh/out (runs over the list).

## Landed
- TRKSuppAccessFile 0x80135324 (708) c37a3c4e: Pikmin's local order ('done' after 'error').
  The .c draft had been scored as C++ (brchk/score default), which hid it behind a false u8 diff.

## Sweeps over all non-MP rows (no other hit)
- dsweep (each leading local moved to every slot): only tiny gains, NOT applied (would reset Dolphin
  proven marks for 1-3 lines): PROFILE_UPDATE 82 -> 81 real ('short* min2;' to slot 5),
  _unpack_skel_frame 157 -> 154 ('SkelFrameRef_t* ref;' to slot 8), CCDBVolBox::TestCollision
  463 -> 461 ('float fy;' -> 16, 'float best' -> 9). Files out_*.dsw here.
- flagx (-inline auto/deferred/deferred,auto/on/noauto/all/level=1): nothing better than the
  current flags on any row.
- tsweep: see batch3b.out (SNDPKTPLAY_start, PublishEvent, CStaticMesh::DrawNode,
  _find_aim_blend_values: no change).

## Per-row findings (time-boxed)
- random (= late2's iSNDrandom): carry type/inline param type and 6 tail spellings all inert (60
  difflib). Orig 'addi r7,r4,0' = the first carry-in 0 not folded.
- SFILTER_unpackxalf: block-scoped/separate XAFSTATE locals (8 forms) do not create the unused
  8-byte slot at 32 (2 or 11).
- PublishEvent: an inline Dispatch(subject, item, event) with a 'callback' local gives the
  original's loop-2 schedule but rotates it/event/subject (88); callback as inline PARAM 4.
- CheckLineCollision (18): orig keeps the final 'types' in r19 = the raw m_flags register; ours
  r27. Copy/no-copy/reference/decl-position, GetTypes() inline (moves the table low: 108),
  operator| spellings, mask placement: all 18-24.
- DisplayHint: 7 bodies x 120 decl orders: 12 minimum (unchanged).
- LLCharObjCompositeMorph: only the FIRST DecodeMorphFrame expansion differs (r8/r9/r5 rotation in
  its three unrolled copies); the CompositeFrame expansion is exact. A second inline for the
  first call (accepted two-copy form) with 20+ line forms: 36; 'short e = entry' copy puts the
  swapped value in r8 like the original but adds clrlwi/extsh (30, length +6).
- _find_aim_blend_values: only the 4th TriangleBlend expansion differs (f7/f8 for -1-x / -y);
  TriangleBlend2 for that call with 4 statement orders: 26-40 (worse).
- CVolBox::TestCollision(CVolSphere): 'CVector3 local(x, y, z)' ctor form gives the original's
  store order (x, y, z) but shifts FPRs (38 vs 28).
- ResetShell: flags/SetGameType statement orders 22-65.
- CBulletDecalManager Init: store-chain 'resource = m_resource = TLT...()' (accepted form) and
  '= 0' initialiser: 56 (worse than the member reload's 22).
- ARAM_alloc_node: 36 decl orders of AramAllocLow/High locals: inert (184).
- MIX_audioslice: orig dry-loop bound through the first loop's sndmix base (r27), effect test via
  m->effects (r25), last loop walks m itself in place; respelling those: 57-99, base 59.
- MOHFL_VD_create: ctor local order/type/const, named result, inline ctor: 32-132, base 32.
- SetFinishedLevelData: 24 SaveAmmo local orders: 22 min (base).
- Skipped: DrawFragCount, InitMultiplayer, BIFunc_MPAddPlayerHealth (multiplayer);
  __sinit_rcmpbase_cpp (needs late2's DECODER ctor / RCMP_SYSTEM dtor); TLT_LoadLevelContentsFile
  (needs late2's TLT_LoadFromRepository in the unit for the string position); DrawTextCentered,
  FlagAsOccupied (declined forms); RenderSystem (1371 diff), the 200+ real rows.

## tsweep leads (single type change; none landable as is)
- CPartBin::Link: 'char skinned' (for bool) makes EVERY register match (62 -> 13 difflib); only the
  extsb/extsb. that a signed char adds are left. bool/unsigned char/int/short and decl placement of
  the bool: 62-120. The knot is skinned's value form, not the rest of the function.
- LLCharObjCompositeMorph: ChangeEndian reading back through 'unsigned short*' gives every register
  right (real 18 -> 14) but loads lhz/srawi instead of lha/rlwinm (logic differs: not usable).
- BIFunc_AIStartCoverPointWalkToStored: prototype 'StartCoverPointWalkToStored(int)' 12 -> 3
  (same as the const-flag form); the mangled name fixes bool, so not usable.
- ARAM_alloc_node: AramAlign returning int/unsigned long: difflib 184 -> 70 but real 25 -> 35.
- SetFinishedLevelData: 'short mission' 22 -> 17 only with added extsh (junk).
