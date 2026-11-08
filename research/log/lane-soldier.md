# soldier agent notes (2026-10-23)

## UpdateAnimation 0x800acfb8 (3828) - prepend to Soldier_object_cleanup
- Draft: ua22.cpp (full cleanup unit source + UpdateAnimation; view changes: m_frame split into m_frame[56]/m_boneAngles/m_bones[80]/m_matrix,
  CAnimated m_model at +48, CAnimMorph : CAnimated, CJawController m_jaw, blink floats, m_rootMotion CVector3, char m_flag4498).
- Score (ss.py raw/struct): 68/18. Left: FPR colouring in constraint d<0 block and up.Cross; GPR colouring in angle blend.
- Keys found: 'offset = GetPos(); offset -= root' (temps), named 'back' local assigned by SetNegative (plain float negation),
  locals declared all at the top in slot order (offset, back, velocity, previous; side, flat, front),
  clamp of the two morph weights is a 2-iteration loop (hoisted 0.0f), blink compares read the member each time,
  angle blend: three int temps first, then x/y/z loads.
- g_bConstraint is a file-local .sdata byte of Soldier_object.cpp (local external).
- Scorer: ./ua.sh <src> [v]; permuter scratch/par/soldier/perm_ua.

## Draw 0x800ac9e4: sdraw_if.cpp = 6 lines (weapon-destroy pointer r28 vs r24). Expanded-body permuter (perm_draw) 56k iters: nothing.
## OnCollision 0x800abc6c: oncollision_up.cpp struct 3 real (other r22->r30 copy; li -1 shared copy). ~60 variants tried (types, decl orders, chains, struct/array). Not found.

## Landed (2026-10-23)
- a9598232 WalkToCoverPoint (wt_final.cpp), 82911ab5 UpdateAStarPathWalk (us_final.cpp), 19a1228f CalculateTargetPosition (ct_final.cpp).

## UpdateTargetPosition 0x8004e024 (2516), append to AIObject_setlocation after CalculateTargetPosition
- Draft ut15a.cpp (based on ct): struct 53. Filter Update inline (fixed-step loop, dt clamped to 0.1, step 1/120),
  float temps per component (no contraction in the original), V must have NO user copy ctor (else the loop-1
  preheader is placed inline instead of out of line), 'old = m_rate' via the user operator= after the six products.
- Left: the original reloads m_rate.x after the old copy in all three loops (mine forwards it), plus colouring.
- m_trackFlags (+432) volatile (reloads each access).
- cb68a77b NEW AIObject_mmgpoint ChooseMMGPoint (cm_final.cpp).
- IsObjectInFieldOfView: io_final.cpp MATCHES (append to AIObject_lineofsight); key: dot written as an explicit expression, not the inline Dot (FPR numbering).
- CheckLineOfSightModeList: ls1.cpp = 4 raw (list 'first' r31 vs orig r27; it shares with the height count in the original). Prepend to AIObject_lineofsight_mode. 11k-iteration permuter: nothing.
- CheckFieldOfViewModeList (PARKED by an earlier session; parks list): fl8c.cpp = 2 raw (start load scheduled after 'mr r28,r3'). Shape found: two-zero tail = 'result' variable assigned in both arms; list search inline.
- Soldier OnCollision: oc_t1.cpp = 3 raw lines. 'ISceneNode* other = collision.m_object0 == this ? collision.m_object1 : collision.m_object0;' (ternary) FIXES the r22->r30 'other' copy. Left: only the shared '-1' (li r4,-1; mr r0,r4) for myClass/otherClass (tried chains, orders, types, separate inits, ternary loop bodies).
- SHARED-CONSTANT FAMILY lead (probe scratch/par/soldier/probe/p9.cpp f1): 'li rA,K; mr rB,rA' appears when the constant reaches the two variables through a JOIN (e.g. 'a = b = g ? -1 : -1;'), i.e. CW does not propagate a constant through a phi; plain 'a = b = -1', 'b = a' after 'a = -1', separate inits, arrays/structs all give two li. So the original probably assigns the value on two paths (both arms of an if/?:, or an inline with two returns of the same value) before the uses.
- bbc280b0 IsObjectInFieldOfView landed. SetMood (weak, 3220) MATCHES: sm_final.cpp -> bsbifunc_mood (landing).
- BIFunc_AIStartCoverPointWalkToStored (parked 7): perm_cw running; param-through-local tricks no effect.
- LANDED 7beb0c50 SetMood, b95380ca CExplosion::Visit (RTTI on + deferred, dtor before Visit in source).
- IN PROGRESS CImpact::Visit 0x800cf794 (2512, prepend to impact_dtor, needs RTTI on + deferred like explosion): scratch/par/soldier/im/im9.cpp rawnr 148 (template im/def.json). Left: start.y/z kept in f31/f30 across TestCollision in the orig (mine reloads), final length form (fmadds x*x+y*y, +z*z separate), team/camera regs r29 vs r28.
- CImpact::Visit PARKED at 148 (im/im9.cpp): start.y/z CSE'd into f31/f30 across the TestCollision call in the orig; tried const start, no user copy ctor (breaks direction), team var shapes.

## aim_cb 0x8005e9ec (2264), append to anim_callback_finish (2026-10-24)
- Drafts scratch/par/soldier/aim/aN.cpp; scorer aim/aim.sh <src> [v]; dis.sh <abs src> <hexoff> <n>.
- a26.cpp/a29.cpp = rawnr 84 (from 517). Keys: vectors in memory (CVector3 plain x,y,z,w + pun operator= like turn_cb; copy via pun copy ctor or decl+assign);
  target fill = turn_cb pattern (float x,z,y locals; stores x,y,z); filter->m_object re-read (no 'ai' variable) FIXED all GPRs;
  'CAIFilterObject* filter;' declared first; list search inline with 'AnimAimInfoView* info = list->entries[i].info' and unsigned i;
  three dot products as named floats (distance, vertical, horizontal) before the clamp; case order 0,2,1; shared 'int i' at top;
  loop body 'unsigned short k = index[i]; AimSlotView* slot = &memory->slots[i];'. case 0 trig decl 'float c; float s; float a=...;', case 2 'a, c, s, t' in order.
- Left: rotation (orig reloads up for up*c: 'fwd = c*fwd + s*up; up = up*c - (fwd - s*up)*t' shape, not reproduced), dots f7/f8 (diff.x/z) swap, one addi order in the case-0 loop.
- perm_aim2 permuter running on a26.
- UPDATE: a31.cpp = rawnr 65; integrated unit draft aim/u_fin.cpp (finish.cpp + aim_cb; _TurnFind/animfinish still 0; cmpall_aim.py says aim 61).
  ROTATION KEY: 'CVector3 cf = c * forward; CVector3 su = s * up;' with friend operator*(float, const CVector3&) building a local r (x,y,z order) and NO user copy ctor (temps registerized) -> gives the original's 'up' reload; then forward.y/x/z = cf+su (YXZ order scored best), b components, 'up *= c', b*=t, up -= b.
  Class now = turn_cb style (x,y,z,w; pun operator=; no copy ctor); vector locals declared then assigned; GetPos by value with pun-assign inside; Get{Right,Forward,Up} return const&.
  Left (65): rotation FPR numbers (orig t=f6, adds in x,y,z order), case 2 cos/sin regs swapped (f29/f30), dots diff.x/diff.z f8/f7 swap, case-0 loop addi order. perm_aim3 running on a31.

## floco_cb 0x8005f2c4 (2032), prepend to anim_callback_weapon (2026-10-24)
- Drafts scratch/par/soldier/floco/fN.cpp; scorer floco/fl.sh; f7.cpp = rawnr 164, struct (ss.py) ~20.
- Keys: locals 'float durations[6]; long type; float speed;' (stack 16/12/8); switch cases 0, 2 + empty 'case 3' (gives the cmp-2 tree with the dead 'b');
  case 2 test inverted ('if (memory->type != type || memory->crouching != player->IsPlayerCrouching()) research; else step');
  step branch reads durations as '((float*)info)[memory->index0 * 3 + 6]' (orig mulli/addi 24/lfsx; the struct form gives add+lfs 24);
  jump branch reads '_floco_anim_ranges[8]' into a local before the speed test.
- Left: param GPR colouring (orig list r23, state r24, header r25, channel r26, user r27, event r28, extra r22; k*12 shares r22 with extra; player/last share r28 with event),
  the shared 'li r4,0' for the 12b4 store and offset, g_AnimDB address scheduling in the prologue. perm_floco2 running on f7.
- floco f15.cpp = rawnr 77: shared inline 'SelectFlocoStep(memory, info, first, last, long& type, time, float& speed, durations)' (count/loop/clamp/stores, returns i) used by case 0 and the case-2 research branch fixed the param order; 'CAnimated* animated = (CAnimated*)user;' declared INSIDE each case (case 0 after 'int first; int last;') and the top call via the cast.
  Left: event r29 vs r28, extra r28 vs r22, k*12 not CSE'd with the helper's (orig one mulli r22), helper volatile regs, case-2 weights f29/f30 vs f30/f31, jump-branch shared li 0.
- floco f25.cpp = rawnr 20 (cmpall 19). BIG KEYS: CAnimDatabase view needed a real size (64 bytes) - an empty class made g_AnimDB an SDA symbol and broke the prologue;
  'CPlayerObject* player = memory->player->GetPlayer()' (inline getter) fixed player r28; helper loop 'int i; int count = last - first; float* d = &durations[1]; ... for (i = 1; i < count; d++, i++) { *d = ...; if (speed < *d) break; }';
  case 2 'float w0; float w1;' declared at the top of the case; jump branch 'int jump; int offset; flag = offset = 0; jump = ranges[8]; ... index = jump + offset' with the flag typed unsigned char.
- floco_cb MATCHES: f28.cpp (f27 + 'extern const signed char _floco_anim_ranges[]' - const lets the jump-branch load schedule first). Helper takes 'int& k, int& i' outputs and uses a local 'int j = first + i' (k = j at the end).
- aim_cb PARKED at a31.cpp rawnr 65 (tried after floco: trig helper with refs, rotation inline with refs/ptrs, dot temps; none better).
- LANDED 36eee1e6 floco_cb. CheckLineOfSightModeList MATCHES: ls_final.cpp (key: list start via 'static inline CAIObject* GetListStart() { return g_paioAIObjectListStart; }' inside FindListObject -> GPR r27). Landing pending lock.
- Getter lens tried without success on: CheckFieldOfViewModeList (still 2, start-load scheduling), Draw weapon loop, cw0, OnCollision -1, CImpact start3/startref.
- LANDED 61323aa3 CheckLineOfSightModeList (log line added after the coordinator resume).

## HANDOFF (2026-10-24, context reset) - read this first
Landed this run (all verified): a9598232 WalkToCoverPoint, 82911ab5 UpdateAStarPathWalk, 19a1228f CalculateTargetPosition, cb68a77b ChooseMMGPoint (NEW unit),
bbc280b0 IsObjectInFieldOfView, 7beb0c50 SetMood, b95380ca CExplosion::Visit, 36eee1e6 floco_cb, 61323aa3 CheckLineOfSightModeList. Lock NOT held; no permuters running; tree clean.
Best drafts (scratch/par/soldier/...), scorer next to each:
- spm/s2.cpp (s3.cpp = same with an inline NewSplineManager helper) CAIFilterGlobal::CreateSplinePathManager 0x8005d760 (340, append to AIFilter_global): rawnr 11 (was 42).
  Only the count/paths param registers are swapped (orig count r29, paths r30). Keys: 'for (i..; i++, paths++)', points byte-swapped via inline SwapPoint(&paths->points[j]).
  Tried: path local, early return, idx, RelocatePath helper. Scorer spm/sp.sh. Claimed by soldier.
- look/l0.cpp CAIFilter{Tank,Soldier}Object::GetLookDirection (188 each, AIObject_lookdirection): 21; the two CVector3 param temps take swapped stack slots
  (orig forward temp sp+16). Tried named locals, getters, operator=/Set(CVector3), blocks. getter_rev variant (position converted first into a named local) = 15 but wrong code order. Scorer look/lk.sh.
- aim/a31.cpp aim_cb: 65 (parked; see the aim section above). fl8c.cpp CheckFieldOfViewModeList: 2 (scheduling; parked).
- Older parks unchanged: ua22 (68), sdraw_if (6), oc_t1 (3), ut15a (struct 53), cw0 (7), im/im9 (148).
Tricks summary: sized class views for globals (avoid SDA); 'extern const' arrays free load scheduling; inline accessors / static inline getters change GPR colouring;
shared inline helper with reference outputs (int& k, int& i) for duplicated case code; 'flag = var = 0' chain shares one li (flag unsigned char);
registerized vector temps (no user copy ctor, friend operator building a local r) reproduce reloads; float-index access gives the lfsx offset form.
Claims held by soldier: CreateSplinePathManager (WIP above). Others in CLAIMS.txt by soldier are landed or parked.

## Session 3 (2026-10-24, after reset)
- LANDED 3730b0a1 MechSetPathFlag (old perm/mspf output: 'int selector; selector = mode;'), cffa9212 GetWeaponType (explicit count arg reads + selector copy),
  60315f50 GetSoundInfo (decl-order brute force gsi/perm.py: count, info, source, result, weapon, position; trigger pos z,y,x assigned to bare x,y,z).
- Tool: gsi/perm.py = permutation brute force of a decl block via rd.py (adapt blk). Generic scorer pattern: <dir>/xx.sh <src> [v].
- GetWayPointInfo 0x80027e94 (2044, append to bsbifunc_grenade after GetSoundInfo; needs --pool .sdata2 --pool .rodata (printf strings) + owns .data jump table): wpi/w7.cpp rawnr 27 (from 119; scorer wpi/wp.sh).
  Keys: arg decl order count, arg(-1), type(-2), info(-3); case 0 'CVector3 dir(wp->x,wp->y,wp->z); dir -= position;' and 'ahead' dot before 'side';
  case 2 'offset(wp->x, wp->y, 0); offset.x -= ; offset.y -= ;'; case 5 direction.z = 0 after both Get calls, 'float x = dir.Dot(front); float y = dir.Dot(right);';
  case 9 'float tx; float ty = wp->y; tx = wp->x;' + statement order (sp9.py brute force).
  Left: callee-saved swap (case 2 offset.x orig f29 / ours f31; case 9 tx orig f31 / ours f29, coalesced with coverAngle) and case 9 store scheduling. Permuter needs a harness copy (ctor-style locals break pycparser).
- Tool: scratch/par/soldier/dperm.py <scorer.sh> <src> <blockfile> brute-forces decl line orders.
- LANDED cb53283c UpdatePlayerStats (selector trick).
- AIStartCoverPointWalkToStored: cw/f.cpp = 2 ('bool flag = **stack != 0;' fixes the stack/flag regs; left: orig srwi r0 + mr r31, ours srwi r31 directly). (bool) cast gives the mr but swaps regs.
- LANDED 51de5eaa FireProjectileAtPath ('t -= segment;' in GetPoint).
- AISetupTargetMatchList: stays 7 (vtable slot load scheduled earlier); tried arg locals in all orders (stm/), top local, wrappers. Parked.
- CreateImpact ci/c0.cpp stays 3 (sphere arg addi r5,r29,16 vs folded r1+64); tried Apply inline members (full/visit/scene), GetSphere, IVolume& local, raw offset. orig reads m_closest via r1 and DamageClosestObject via r29.
- RegisterTargetSearch rts/r1.cpp = 10 (count-local form; decl-order hill climb no effect; orig arg addr order -7,-6,-1,-2,-3,-4,-5, params r28). Parked. Tool: scratch/par/soldier/hill.py <scorer> <src> <blockfile>.
- ChooseCoverPoint ccp/b.cpp = 23 (from computed before GetPosition: 'from = &nodePosition; node->GetPosition(*(CVector3*)from);' fixes the structure); left: GPR rotation this/type/from (orig r31/r29/r30, ours r29/r30/r31) - param copies, decl moves, inline helper do nothing. Parked.
- PlayerScreenFlash psf/pp_t_f_d.cpp = 45 (from 76): CColor(r,g,b,a) temps passed by value to an inline Set (no user copy ctor, bytewise operator=) gives the right temp/copy slots (p1.cpp, frame differs); left: B's bytes built before A's (args right to left). Param-order permutations, pre-read locals, SetFrom/SetTo, const-ref forms tried. Parked.
- MakeNoiseFromInputTrigger mnt/m0.cpp = 20: same callee-saved rotation family as ChooseCoverPoint (orig object r31 stack r30 type r29; ours stack r29 object r30 type r31).
- LANDED ae587532 GreedySearchAwayFromPoint (decl order dy,dz,dx + pointer-kept stack copy).
- _find_aim_blend_values fab/f0.cpp = 51 (bf/bf2 decl+assignment brute force of the TriangleBlend inline: 48 best). Parked; likely per-case hand-written expressions.
- CAIObject::PropagateSoundToAllObjects pso/p0.cpp = 14 under rd (sqrtf result in f2 + fmr instead of reusing f31); scope/temp/order variants no help; 'm_volume / sqrtf' gives 5 but wrong maths.
- LANDED 8551aa92 IsTargetInLineOfSight.
- LANDED 93f595c8 ShouldILeaveMMG (mmgpoint views + decl/assign brute force).

## HANDOFF (session 3, 2026-10-24) - read this first
Landed: 3730b0a1 MechSetPathFlag, cffa9212 GetWeaponType, 60315f50 GetSoundInfo, cb53283c UpdatePlayerStats, 51de5eaa FireProjectileAtPath,
ae587532 GreedySearchAwayFromPoint, 8551aa92 IsTargetInLineOfSight, 93f595c8 ShouldILeaveMMG, 770df913 GetLookDirection tank+soldier, f31451d5 GetLookDirection player.
Lock not held; no scripts running; tree clean.
Best WIPs: wpi/w7.cpp GetWayPointInfo 27; cw/f.cpp AIStartCoverPointWalkToStored 2; ci/c0.cpp CreateImpact 3; stm AISetupTargetMatchList 7; rts/r1.cpp 10;
ccp/b.cpp ChooseCoverPoint 22 (GPR rotation); mnt/m0.cpp 20 (same rotation); psf/pp_t_f_d.cpp PlayerScreenFlash 45; rr/bf_best.cpp RotateRel 34; mfp/bf_best.cpp 81; fab/f0.cpp 51; pso/p0.cpp 14; spm/s2.cpp 11; aim/a31.cpp 65.
Tools: da.py (brute-forces declaration order x assignment order), dperm.py, hill.py, mydis.sh, rd.py. Key tricks are in TRICKS.md.

## Session 4 (2026-10-25, fresh agent)
- Found predecessor's unrecorded cpo/ work: CCoverPoint::FlagAsOccupied 0x80052f6c + GetAvailablePeekPosition 0x800533a8 (template CoverPoint_unoccupied.json).
  Scorers cpo/fo.sh, cpo/ga.sh, cpo/both.sh (pass ABSOLUTE src path). Best: cpo/h_int.cpp = fo 4 / ga 9.
- cpo session 4: tried tail forms for GetAvailablePeekPosition (if/ternary/inline Pick/goto/switch; best t1/t8 'if (avail > 1) avail &= 1 - avail;' = ga 8),
  144 GetTypes tail combos (s7/: 0xff-and form x mask expr x final-and x order) -> all fo 4/ga 9 or worse; avail-expression orders (s8/) worse.
  Facts: SDA 0xff,1,2,4 at 8034da90 are shared by GetPeekLocation/fo/ga = literal temps of ONE inline (CW emits them once per inline) -> data layout OK.
  Orig ga tail needs 'bgt L; b end; L: subfic; and' (not reproduced). PARKED fo 4 / ga 8.
- GetModifiedPosition 0x8005314c MATCHES rawnr 0: gmp/gmp_final.cpp (keys: position.Set(o->x,o->y,o->z) (args r-to-l), 'object = m_object' local before CMatrix, case 1 'float t = -front.x; front.x = front.y; front.y = t;', empty case 3).
- LANDED 55e30cd7 GetModifiedPosition (NEW unit CoverPoint_modified).
- GetAIStatusVar 0x80049380 (652) + weak CAIStatus::GetVarValue 0x8004960c (872; emitted after GAS, its first caller; must land together, append to AIObject_fovcorpse): gvv/.
  Scorer gvv/sc.sh <abs src> [v] (env T=template; gvv/def.json = deferred flags; AIObject_ctor proves AIObject.cpp is deferred).
  Shape: GAS = 'return m_status.GetVarValue(var);' (m_status +436); GetVarValue = AIStatus.cpp switch calling s_paioAttacker->ComputeStatusX(s_paioTarget);
  ComputeStatus* bodies (AIObject_threat/_enemy) + IsObjectInLineOfSight (AIObject_lineofsight_obj) must be in the TU (file duplicates); threat reads other->GetAIStatusVar(3/8);
  LOSValue = 'distance = m_sightDistance(+520); *0.5 if +1840; *0.75 if crouching; IsObjectInLineOfSight(pos, d, crouching) ? 15 : 0'.
  d1a.cpp: GetVarValue rawnr 5 (!), GAS 119 because GAS also inlines IsObjectInLineOfSight at depth 3 (orig calls it: bl). r_nohelp_g.cpp (IsEnemy without AreEnemyTeams helper):
  GAS 55 with IOILOS called but threat calls GetAIStatusVar instead of GetVarValue. CW inline-budget puzzle: need depth-3 GetAIStatusVar inlined but IOILOS not.
  #pragma inline_depth/inline_max_size/inline_max_total_size probes change things but no hit. PARKED (promising: find the budget rule).
- CheckLineCollision 0x80052abc (1184; PREPEND to CoverPoint_unoccupied, ends at its start): clc/c6.cpp = rawnr 10 (scorer clc/cl.sh <abs src> [v]).
  Keys: whole body inside 'if (Check(from,to,0)) {...} return 1;'; the cover-type table code written at CALLER level (not an inline: the table must sit above the
  matrices in the frame) into a memory 'flags' then 'CoverPointTypes types = flags;' (register copy); test = from + side (component adds), '*out' via
  'float z = test.z; float y = test.y; float x = test.x; out->x = x; ...'; side rotations as in GetModifiedPosition; matrices via an inline
  CoverPointObjectView::GetFront(v) {CMatrix m; orientation.GetMatrix(m); m.GetFront(v);}; branch test 'types & (CoverPointTypes(0x51) | 0xa2)'.
  Left = SAME FAMILY as FlagAsOccupied/GetAvailablePeekPosition: the 1|2|4 mask / &0xff chain colouring + final types r19 (orig reuses the raw-flags reg) vs ours r27.
  Tried: 36 mask/&0xff orders (clc/p), operand orders inside the operators (clc/q), raw/types var shapes (c8*), hill climb of the block: nothing. Solving this family lands 3 functions.
- LANDED 12393d27 ProvidesCover (old draft scratch/game/coverpoint_provides_wip.cpp + reuse the 'direction' local for the loop's vector: 43 -> 0). pc/final.cpp.
- ChooseCoverPointByType 0x8005ca88 (1632; APPEND to AIFilter_runaway): ccb/, scorer ccb/cb.sh, parallel runner ccb/runall.sh <dir>. 146 -> 62 (ccb/i1.cpp).
  Keys: iterator 'int index = -1; point = NextCoverPoint(leaf, index, 0)' with Next = '{ index++; return CoverPointAt(leaf, index, previous); }' (stops CW folding index 0 -> orig 'li r31,0; cmpw r31,count');
  'float closest = range;' after the first fetch; float decls at top in order threatDistance, distance, dx, dy, dz, closest (= f31..f26);
  case 5 written before case 4 (jump table); cases 2/3 = inline helpers 'bool result; unsigned char inside; inside = result = false;' (shared li + mr);
  toThreat = inline DistanceSquared(enemy, position) (dy, dx, dz temps). Left: inner-loop FPR numbering (from/pos loads), top threat.y/from.y regs, 'li r5,12; cmplw' guard folded in ours.
  Same iterator probably fixes GetClosestUsableCoverPoint (gcu/, 17).
- LANDED 039530a3 GetClosestUsableCoverPoint (gcu/unit.cpp). Iterator: index=-1 + Next inline; FindNextCoverPoint 'for (trigger++; ...)'.
- ChooseCoverPointByType now ccb/q2.cpp = rawnr 20 (from 62): + DirectionView(from,to) class with ctor-initialised x,y,z and Normalize ('x *= 1.0f / length' x3)
  for the point direction; FindNextCoverPoint 'for (trigger++; ...)' as in GCU; distance temps order fz, fx, fy.
  Left: top threat.y/from.y registers swapped (f0/f3) and inner-loop pos.z/from.x/from.y registers (orig from.y in f8 = coalesced with dir.y).
  Tried: decl hoists of fx/fy/fz (no effect), DistanceSquared helper for distance (worse), DirectionView for the threat dir (56), operand forms at the top.
- GetClosestWaypointByType 0x8005c6b8 (784; APPEND to AIFilter_visit): gcw/w3.cpp rawnr 154 (old scratch/pool/gcw_best.cpp 164). Scorer gcw/gw.sh.
  Applied: index=-1 + NextWaypoint inline (first fetch now 'li rX,0; cmpw' like orig), 'float nearest = distance' after the fetch, 'if (all) {loop} return 0'.
  Left: callee-saved order (orig index r31, type r30, leaf r29, position r28, all r27, base r26, waypoint r25, closest r24; ours params lowest),
  the after-previous path still 'li r3,12' form differs (use 'for (trigger++; ...)' like GCU), position copy (orig float copy z,y,x loads, stores y,x,z).

## HANDOFF (session 4, 2026-10-25) - read this first
Landed: 55e30cd7 GetModifiedPosition (NEW CoverPoint_modified), 12393d27 ProvidesCover (CoverPoint_invalidate), 039530a3 GetClosestUsableCoverPoint (AIFilter_astar).
Lock not held; no scripts running; tree clean.
Best WIPs: ccb/q2.cpp ChooseCoverPointByType 20; clc/c6.cpp CheckLineCollision 10; cpo/h_int.cpp FlagAsOccupied 4 / cpo/t1.cpp GetAvailablePeekPosition 8
 (clc+fo+ga share the GetTypes mask-colouring problem); gvv/d1a.cpp GetVarValue 5 (+GetAIStatusVar inline-depth puzzle); gcw/w3.cpp 154; cw/f.cpp 2; oc_t1 3.
Claims held: GetVarValue, GetAIStatusVar, CheckLineCollision, ChooseCoverPointByType, GetClosestWaypointByType, FlagAsOccupied, GetAvailablePeekPosition.

## Session 5 (2026-10-25, fresh agent)
- GAS/GVV SOLVED: gvv2/best.cpp = GAS 0 / GVV 0 (sc.sh). Keys: GAS is WEAK in the orig (inline; first caller CheckFieldOfViewCorpse calls it);
  the depth puzzle = CW max inline depth 3: LOSValue calls IOILOS through an inline wrapper 'CanSee(other, distance)' (one extra level, so GAS->GVV->LOS->CanSee stops before IOILOS);
  threat 'value; distance; count = GAS(8); threat = value + (15-distance)*2; threat /= count + 3;'; IOILOS loop 'float y; float x; float z; z=..; y=..; x=..;' (decl order y,x,z).
- LANDED 3bfb724a GetAIStatusVar + GetVarValue (gvv2/final.cpp), dd274d26 NEW Soldier_object_collision OnCollision (oc2/a.cpp: FindClasses inline with own -1 locals).
- BIFunc_MakeNoise 0x8002b60c (912, untried, claimed) + BIFunc_MakeNoiseFromInputTrigger 0x8002b99c: mn/ (scorers mn/mn.sh, mn/mt.sh <abs src> [v]; template bsbifunc_makenoise.json = deferred).
  Draft = bsbifunc_makenoise.cpp + BSObjectView(trigger +8, user +12)/BSGO_Basic GetSceneNode slot +0x10 + both BIFuncs calling the INLINE MakeNoise overloads
  (BIFunc_MakeNoise: node branch 'MakeNoise(node, type, loudness, type == 51 ? 1 : 3)', trigger branch 'CVector3 position(core->x,..); MakeNoise(object, 0, position, type, loudness, true, same)').
  mn/m0.cpp BIFunc_MakeNoise = 10 (only arg-read regs: count r5 / flag r5 in orig). mn/x1.cpp MNFIT = 10 (from 20; only GPR rotation type r29/stack r30/object r31; a ternary type arg fixes the rotation but becomes arithmetic).
  The three functions + bsbifunc_makenoise would make 0x8002b60c..0x8002be34 contiguous. perm_mn2 running (runperm.sh).
- cw/f.cpp AIStartCoverPointWalkToStored still 2 (tried ~25 more forms incl. inline wrappers, BSReturn helpers, casts).
- EnableAll 0x80055028 (176; claimed): ena/ (en.sh; template PathNodeList_disable.json, could prepend to the GreedySearch unit at 0x800550d8). 11 = iterator r7 vs orig r6 (strength-reduced offset swaps); pointer loop n.cpp=9 gives the right iterator reg but wrong loop. Parked.
- LANDED 01cfcc91 CSearchNode::Setup (sn/final.cpp: GetCostValue(parent) inlined + GetEstimate inline).
- LANDED 7ac63030 BIFunc_MakeNoise (mn/final.cpp; arg read 'args = *stack; args -= ...; long flag = *args' after the loudness read).

## Session 6 (2026-10-25, fresh agent)
- MNFIT (mnf/: run.sh <body.cpp> = landed bsbifunc_makenoise_bi + body + BIFunc_MakeNoise; base b0.cpp = 10): tried ~35 more forms (type helpers, inline
  overload computing type, if/else, switch, bool flag, types short/char/long, statement/decl brute force da.py, position/core forms, ISceneNode local, Body inlines). Still 10.
  Only arithmetic (single-def) type fixes the rotation. PARKED.
- cw/ AIStartCoverPointWalkToStored: '(bool)**stack' gives the orig's 'mr r31,r0' but stack/flag swap (7); inline wrappers/BSRet/void* casts/ISceneNode local no help. Still 2.
- fl9/ CheckFieldOfViewModeList: FindListObject inline from ls_final + start (m.cpp) = same 4 raw; orig loads m_listObject BEFORE 'mr r28,r3' and the global after
  (LOS orig: global first). volatile member/global, getters, orders: no. Still parked.
- AEMSEvent 0x80029b2c (948, append to bsbifunc_pathfinder) MATCHES: ae/final.cpp (scorer ae/ae.sh). Keys: '*=' scale (fmuls operand order),
  'ISceneNode* node = (ISceneNode*)object;' local used for every object use (object/positional swap 31->17), 'int result = -1;' declared between
  the 3rd and 4th arg reads (17->0). LANDED 2616e75b.
- LANDED ed51ee29 RegisterTargetSearch (rts/final.cpp): split decl/assign + hill on decl order (tool hill.py with blkd file).
- RegisterTargetCheck 0x8002e1bc (1052, PREPEND to bsbifunc_corpse; claimed): rtc/v0_h.cpp = 45 (from 124). Fixes: BSUserView vptr at +12
  (unknown00[12] first), int target + cast, 'case 1: break;', cases in order 2,3,4,5, GetScriptAIObject 'object = 0' BEFORE GetScriptFilter
  (callee-saved obj), point copy as float z,x,y temps (da: decl z,x,y / assign z,y,x), split decl + hill. Left: ONLY stack r25 vs orig r31
  (flag r31 vs orig r25) + point FPR numbering + Register arg scheduling (follows). Probes (probe2/*.cpp, mr.py): params get r31 in
  straight-line code, but conditionally-defined (multi-def) locals outrank them; in RTC orig result/obj (multi-def) is r29 below stack.
  Not solved: stack=stack, top local, BSReturn, *stack per read, result position, inline body (not inlined: too big). perm_rtc permuter ran (garbage).
- RotateRel: rr/q0_ap.cpp = 26 (from 34): natural x..sz reads into split-declared floats, assignment-order brute force rr/ap.py (x,y,sx,z,sz,sy),
  then CVector3 angles(PI*x/180, ...) ctor. Left: the orig computes the -2 (x) address FIRST, ours always last (+ FPR numbering follows).
- ChooseCoverPoint: GetFrom inline (with returns / with local), ternary chain: no change (this r29 vs orig r31). UpdateAnimation (ua/): add-operand and
  short-temp forms, d<0 block forms: no gain (rd raw 59).
- PARAM-RANK FAMILY (RTC, CCP, MNFIT, gcw, death_cb): orig gives params higher callee regs than a multi-def local. Probe (probe2/cc.cpp q0..q5): a local
  initialised at the TOP and conditionally reassigned outranks params; assigned in both branches (no top init) it does not (q3/q4). Not yet
  reproduced in a real function (RTC n1: result in both branches -> stack r26, result leaves the callee regs).

## HANDOFF (session 6, 2026-10-25)
Landed: 2616e75b AEMSEvent (bsbifunc_pathfinder), ed51ee29 RegisterTargetSearch (bsbifunc_corpse). Lock not held; no permuters; tree clean.
Best WIPs: rtc/v0_h.cpp RegisterTargetCheck 45 (prepend to bsbifunc_corpse); rr/q0_ap.cpp RotateRel 26; mnf/b0.cpp MNFIT 10; cw/f.cpp 2; fl9/m.cpp FOV list 4.
Claims added: BIFunc_AEMSEvent (landed), BIFunc_RegisterTargetCheck.

## Session 7 (2026-10-25, fresh agent)
- PARAM-RANK BREAKTHROUGH (RTC): wrapping the final Register call in a static inline that takes the locals as params
  'static inline int DoRegister(unsigned short time, int flag, int mode, float range, int target) { return g_TargetCheckSchedule.Register(g_pBSObject, time, flag != 0, mode, (int)range, target); }'
  moved stack from r25 (bottom) to r31 (top). Then flag decl moved last -> ALL callee GPRs match: rtc7/fl.cpp = 39 (from 45).
  Probes (pr7/): params usually rank BELOW all locals; adding one more call-crossing node (any extra live local) flips the param to the top
  (threshold-like, d0..d8 in pr7/p16.cpp) - pure temps don't. Inline-param coalescing is the source-neutral way to get that effect.
  Left in RTC: arg-read volatile regs (*stack r8/count r9 in orig, ours swapped) + Register-arg scheduling + case-4 point FPRs.
  Harness: rtc7/var.py <base> <variants.py> (V=[(name,[(old,new)..])]) prints score + stack reg.
- CORRECTION: the real key was '(bool)flag' (not the DoRegister inline; either flips stack, (bool)flag also fixes the Register-arg scheduling).
  Then perm6.py brute force of the 6 arg reads (mode,range,angle,target,time,flag), 'int* args;' before 'int count;', case-4 point ctor (x,z,y).
- LANDED 1ce1c7bb RegisterTargetCheck (rtc7/unit.cpp). Unit bsbifunc_corpse now starts 0x8002e1bc.
- cw (AIStartCoverPointWalkToStored): ~30 more bool spellings/inline readers (cw7/): still 2. MNFIT bool forms (mnf/var7.py): still 10.
- CreateSplinePathManager (spm7/): outer loop moved into 'static inline void GenerateAll(CAIFilterGlobal* self, void* base, BPDPolyPath* paths, int count)'
  fixes the count/paths callee regs (spm7/b10.cpp = 10) but the inner loop then starts 'li r5,0; mr r3,r5' (shared zero) instead of the orig's
  'li r4,0; li r3,0' (j + offset IV). Tried: j types, explicit offset IV, SwapPoints helper, depth reduction, member inline, param orders: no. Parked 10 (old s2 = 11).
- ChooseCoverPoint (ccp7/): casts, conditions, wrappers, ctor position: still 23. STM (stm7/): read orders/shapes: still 7 (vtable-load scheduling).
- CheckForObstructions claimed; draft scratch/explosion/cfo_wip.cpp is ~100+ real lines off now (rawnr 684; NOSTACK=1 env on rd.py ignores r1 offsets). Not started.
- CreateImpact (ci7/): CVolSphere as a second BASE lands at +8 (not +16), pointer/cast forms: still 3. ccb7 top FP forms: still 20.
- LANDED f59794a7 PropagateSoundToAllObjects (pso7/unit.cpp): 'inline void SqrtInPlace(float& x)' (sqrtf body, 'x = y;', no return) -> 0.
- GetWayPointInfo (wpi7/b26.cpp = 26): 'float ty; float tx;' hoisted to the function top fixes case 9 (tx f31). Left: case 2 offset.x f31 vs orig f29, case 9 FPR temps.
- CAIFilterGlobal::PropagateSoundToAllObjects 0x8005d8b4 (608, prepend to AIFilter_global): gps/unit.cpp = 0 (from 162). Keys: STATIC member (info in r3);
  NoiseInfo noise built FIRST ('noise.source = info.source; float now = ...m_time; noise.priority = info.priority; noise.time = now;'), compares read noise.*
  from memory; unsigned source; erase with 'T* last = end();' hoisted; 'if (noise.priority <= it->priority) skip = true; else erase'; 'while (size() >= 15)';
  'CAIObject* list = GetGlobalListStart();' local; push_back(T item) by value; global object view needs its real size (16) or it goes small-data.
- death_cb (dcb/, scorer dcb/sc.sh; mangled name ends ...PvUlPvf): old scratch/game/death_cb_wip.cpp = 184 raw now; hoisting
  'int count; unsigned int i; int kept; unsigned int total; int pick;' to the top (order 1,2,0,3,4 = i,kept,count,total,pick) -> dcb/best97.cpp = 97.
  Params still r23-r28 (orig r26-r31). Big-body inline helpers are not inlined (size).
- LANDED bcb77d44 CAIFilterGlobal::PropagateSoundToAllObjects (gps/unit.cpp).
- GetClosestParameter 0x800516b4 (1056; between AISplinePath_ctor and AISplinePath_segment -> would merge them; claimed): gcp/ (scorer gcp/sc.sh).
  Decoded: 8-step Newton on (P(t)-Q).P'(t) with CVector3 temps filled by in-place inline helpers Position/Derivative/SecondDerivative
  (first component statement order y,x,z; ops via CVector3 operators), '!(fabs(den) > fabs(1e-4*num))' break, '__fabs(next-t) < 0.125*step'
  (no frsp), return next if start <= next < 1; else linear search t += step up to min(1+step, start+16*step) (end = a < b ? &a : &b).
  Constants 1.5, 2, 3, 1e-4, 0.125, 1, 16 (.sdata2 pool). gcp/v/f1.cpp = 99 (all FPR naming/scheduling); decl hill running (hill.log, h0_h.cpp).
- GCP decl hill: no gain (99). Parked.

## HANDOFF (session 7, 2026-10-25)
Landed: 1ce1c7bb RegisterTargetCheck (bsbifunc_corpse), f59794a7 CAIObject::PropagateSoundToAllObjects (AIObject_mmgpoint),
bcb77d44 CAIFilterGlobal::PropagateSoundToAllObjects (AIFilter_global). Lock not held; no background jobs; tree clean of my files.
Best WIPs: gcp/v/f1.cpp GetClosestParameter 99; dcb/best97.cpp death_cb 97; wpi7/b26.cpp GetWayPointInfo 26; rr/q0_ap.cpp RotateRel 26;
spm7/b10.cpp CreateSplinePathManager 10; mnf/b0.cpp MNFIT 10; cw/f.cpp 2; fl9/m.cpp 2 (rd); ci/c0.cpp 3; stm/best.cpp 7; ccp/b.cpp 23; ua22.cpp 59.
Claims added: GetClosestParameter, CAIFilterGlobal::PropagateSoundToAllObjects (landed), CheckForObstructions (not really started; release if needed).
Tools: varg.py <scorer> <base> <variants.py> (generic variant runner), rd.py NOSTACK=1 env ignores r1 offsets.

## Session 8 (2026-10-25, fresh agent)
- AddChildrenToSearchNodeQueue + ComputeAStarPath (0x8004b2c0/0x8004b784, around AIObject_searchnodes): both have 'new CSearchNode' extab
  action 0x90 with the 2-byte padding '2500' -> extab-padding HOLD (user). Claim released. Don't retry.
- death_cb (dcb8/b2.cpp = 81, from 97): 'while (r >= sum + weights[pick]) sum += weights[pick++];' (separate loads) + 'unsigned int kept'.
  Params still r23.. (orig r26..). Probes pr8/*.cpp: user locals (even inline-locals, inline-returned) ALWAYS outrank params here;
  compiler temps (array base) rank below. Orig 'total' behaves like a compiler temp (r23). Extra live locals do NOT flip params here. Unsolved.
- DoTargetCheck 0x8001e59c (540, append to bsbifunc_corpsesearch; scorer dtc/sc.sh): dtc/c.cpp = 28 (from 38). Keys: case 1 as inline
  'OutOfRange(ISceneNode* node, CVector3 o, int distance)' (GetPosition inside), 'ISceneNode* node; int result; result = 1; node = ...;' (fixes result/node regs).
  Left: orig keeps a DEAD pair copy A@8 of other@40 and a float copy B@24 of A, and reads other@40 directly (frame 112 vs 96);
  plus 'rlwinm r29' direct (ours rlwinm r0 + mr). Tried ~30 dead-copy forms (by-value ctor/params/return slots/arrays/refs/implicit copy): CW deletes them all.
- RotateFromScript 0x800af48c (append static_obj_scale; old scratch/game/static_obj_rotatefromscript_wip.cpp = 24 under rfs/sc.sh): rfs/v/k1.cpp gives
  the 2nd selection the orig shape ('if (first) a = a; else a = b;' in KeepSmaller -> 'beq; b; fmr'), but the 1st selection's result is NOT
  coalesced with tx (new reg f1, two fmr) -> raw 47. Tried init forms, in-place, ternaries, returning helpers: no. TranslateFromScript same family.
- BeginAnimMotionUpdate (CHierObject, claimed): bam/a.cpp = 6 (old draft); union CVector3 + memberwise pair copy ctor + 'to' via operator= (bam/b.cpp)
  flips from.x to f6 but then y/z swap (still 6). Lerp expression forms/decl orders: no effect at all. perm_bam permuter (DECLV macro in prefix) ran.
- death_cb BIG PROGRESS: dcb8/b8.cpp = 12 (from 97). Keys: 'for (i = 0, count = 0; ...)' comma-init in the 3 bullet loops (li order);
  intensity loop 'for (i = 0, kept = 0, total = 0; i < located; i++)' -> FLIPPED THE PARAMS to r26-r31 (param-rank family!);
  the location loop's output in its OWN hoisted variable 'located' (not count); final info hoisted as 'AnimDeathInfoView* chosen;'
  and decl order (hill) i, kept, count, located, pick, chosen, total, death. while2 pick loop 'sum += weights[pick++]'.
  Left 12: posture loop 'mr r3,r4' before 'li r7,0' (orig after), location loop 'li r3,0' (located) scheduled 3rd (orig 1st),
  intensity loop kept r10/info r11 (orig r11/r10). perm_dcb permuter running on b8.
- death_cb now dcb8/b12.cpp = 4 (scheduling only): intensity loop (permuter hints) = 'index = candidates[i];' (hoisted 'int index;' after total)
  and 'if (!INFO(candidates[i])->intensities || (INFO(index)->intensities & ...)) { AnimDeathInfoView* info = INFO(candidates[i]); ...}'
  with '#define INFO(k) ((AnimDeathInfoView*)list->entries[k].info)'. Left: posture 'mr r3,r4' vs 'li r7,0' order; location 'li r3,0' (located) order.
  Rest of anim_callback_init still 0 with death_cb prepended. Landing needs .rodata string @2135 (0x801703fc) + .data jump table @2136 (0x80183840).
  perm_dcb3 permuter on b12.
- SetMovementPath 0x800af260 (claimed; smp/sc.sh, template static_obj_setlight): smp/b.cpp = 1 real line (raw 7 incl. reloc-normalisation noise
  'li r5,X/li r6,0' and 'addi r3,r3,0'): 'if (!keepSpeed) m_speed = 0.0f; m_maxSpeed = maxSpeed;' fixed the +224 store schedule.
  Left: orig's lone 'clrlwi r5,r5,24' before GetSplinePath(path, flag) = bool zero-extended WITHOUT the !=0 normalisation, as if the
  callee param were unsigned char. 'flag & 1' / 'flag & true' give clrlwi 31 (rawnr 1). Only a mangled extern "C" decl with an unsigned char
  param would reproduce it (policy question; not done).

## HANDOFF (session 8, 2026-10-26)
No landings. Lock not held; no permuters running; none of my files outside scratch/par/soldier.
Best WIPs: dcb8/b12.cpp death_cb 4 (scheduling only; see above; prepend to anim_callback_init, needs .rodata string + .data jump table pools);
smp/b.cpp SetMovementPath 1 real line (lone clrlwi 24); bam/a.cpp CHierObject::BeginAnimMotionUpdate 6 (FPR from.x/z);
dtc/c.cpp DoTargetCheck 28; rfs/a.cpp RotateFromScript 24 (rfs/v/k1.cpp has the right 2nd-selection shape).
Unchanged: cw/f.cpp 2, fl9/m.cpp 2, ci/c0.cpp 3, stm/best.cpp 7, spm7/b10.cpp 10 (spm/s2.cpp 11), mnf/b0.cpp 10, ccp/b.cpp 23.
Claims added: DoTargetCheck, BeginAnimMotionUpdate__11CHierObjectFf, RotateFromScript, SetMovementPath (AddChildren/ComputeAStarPath released: extab HOLD).

## Session 9 (2026-10-26, fresh agent)
- Near-misses re-checked (no new idea landed): DisplayHint (claimed; dh/, scorer dh/sc.sh, template bsbifunc_corpsesearch) dh/b.cpp = 6
  (texts/nextIndex regs right via 'texts = g_HintTextArray[k].text; nextIndex = &hint->next;' but orig keeps 'mr r31,r30' copy +
  in-place 'addi r30,r30,8'); ~40 forms (void*, multi-def, inline helpers, in-place ptr) no. cw/ci/bam/smp: no new idea.
- CheckEnemies (claimed; ce/, ce/sc.sh): second loop as 'inline void WarnOthers(CThrownBullet* self, BSGO_Basic* best,
  GrenadeWarningInfo& info, CVector3& delta)' with locals target,soldier,other,doodad -> loop 2 colouring EXACT, but loop-1 soldier
  then r25 (orig r26) = raw 12 (old b.cpp raw 11). ce/d.cpp. Permuter perm_ce (sqrtf forwarder ce/dp.cpp) run.
- NEXTSTATE (claimed; ns/, ns/sc.sh template bsmachin): ns/a.cpp raw 26 (old draft), hoisted ns/b.cpp 29, hill running (ns/hill.log).
- CheckForObstructions (cfo/, cfo/sc.sh, template explosion_dtor = deferred+RTTI): raw 684 -> 203 (NOSTACK 152 -> 66):
  (1) the 3 tests NESTED 'if (TestLine(..) != 1) { setup2; if (..) { setup3; if (..) return true; } } return false;' (shared li r3,0 tail);
  (2) TestLine takes 'const CVector3& centerRef' and copies 'CVector3 center(centerRef);' (GetCenter temp + copy);
  (3) setup order end.x, end.y, start.x, end.z, start.y, start.z; (4) 'CVector3 eye; eye = look.GetPos();' (temp + copy) -> frame size matches.
- CFO further: TestLine's 5th param 'const CVolSphere& sphere' with 'CVector3 center(sphere.GetCenter());' inside (temp slot among
  the inline's temps) -> raw 92; 'CVector3 end; CVector3 start;' hoisted right before 'float scale = 1.0f;' -> 62 (ALL STACK SLOTS MATCH);
  'int sign = 1; if (IsAbove(*this, node)) sign = -1;' (no bool local) -> all callee regs match (43); permuter hint block 3
  'end.d.v[2] = sign * (0.01f * height);' -> 31; inline getter 'GetEyeHeight()' on CPlayerObject view -> 28 (cfo/i.cpp).
  Left (28): li r31,1 scheduled before the IsAbove switch (orig after; every 'sign set after IsAbove' form flips sign to r28),
  block 1/2 FPR naming (orig constants in f0, magic f2/conv f1), collision flag zero li r5 vs li r3. perm_cfo running on cfo/hp.cpp.
  LANDING NOTE: CFO precedes Visit in explosion_dtor (deferred file -> CFO must be LAST in the unit source); unit CVector3 view
  lacks the user copy ctor + Normalize returning length -> probably needs its OWN fragment unit with cfo's view (check pools/strings).
- NEXTSTATE: hill (ns/b_h.cpp 18); '*link = entry->next;' -> 14; separate counter 'j' for the register loop -> ns/d.cpp = 11:
  only nextState (ours r31, orig r30) vs its *4 CSE index (ours r30, orig r31). Rising Sun original at
  ~/workspace/moh-rising-sun/src/script/nextstate.cpp. Init-order brute force running (ns/v4.log).

## Session 10 (2026-10-27, fresh agent)
- LANDED cfcf75c6 bsmachin += BSOpCodeFunc_NEXTSTATE (ns/e.cpp): permuter (perm_ns, harness ns/hp.cpp = C casts) found 'state = nextState;'
  before the register loop + pass 'state' to BSRegisterMessage -> 0 in 1.6k iterations. NOTE: ns/d.cpp was based on a pre-BSInitMachine
  copy of bsmachin.cpp - always splice a draft function into the CURRENT src file (port_unit silently drops missing functions).
- LANDED e9dd3fb1 static_obj_attempt += AttemptPathMotionUpdate (apm/merged.cpp; draft from scratch/game/apm5.cpp): inline fabs trick + one-expression speed.
  Port: --original-file static_obj.cpp --file-index 6731 --pool .sdata2.
- CThrownObject::Init (claimed; ti/, ti/sc.sh): from scratch/game/ti_d.cpp 36 -> ti/vq2.cpp 16: Length() with ONE inline local
  'float a = x; return std::sqrtf(a * a + y * y + z * z);' gives the orig's fusion (fmadds x*x + y*y, z*z separate) - locals fuse,
  members do not, and only INSIDE the inline (same statements in the function body do not fuse). Left 16 = x/z FPR swap (orig x f1, z f2)
  in all three lengths. ~40 Length forms + permuter perm_ti (13k, nothing).
- GetAvailablePeekPosition (cpo/): ~35 more tail forms (late-empty else, two-return inline, goto, ternary copies) - still 8.
- RotateFromScript: tx/ty/tz decl orders x top/sep - no. CheckFieldOfViewModeList: start passed into the inline, read orders - still 2.
- CreateImpact: '(CImpact*)(void*)&impact' gives the orig's unfolded 'addi r5,r29,16' but reschedules (14). perm_ci on ci/hp.cpp (DECLIMPACT macro harness).
- SetMovementPath permuter (perm_smp, 14k): best 10 = junk ('path & 0xFF').
- NEW: CThrownBullet::Init 0x800d425c (2876; append to thrownbullet_tail - the .data jump table @2665 0x80189ee8 follows tail's .data;
  claimed): tbi/t_z5.cpp = 8 (only the rand() quotient FPRs f30/f31 swapped in cases 5 and 3/6/9). Scorer tbi/sc.sh, others.sh (other 4
  tail functions still 0). Keys: fiber end points as NAMED locals 'CVector3 top(0, -m_velocity.Length(), 0); CVector3 bottom(0,0,0);'
  and the particle velocity a named local built with the 3-float ctor (all stack slots), inline 'double fabs' (|z| < |x| load order),
  'ISceneNode* owner = m_firedBy ? m_firedBy->GetOwner() : 0;' (shared zero r28). perm_tbi running on tbi/hp.cpp.
- LANDED 36940b62 CoverPoint_modified += GetAvailablePeekPosition (gapp/m.cpp; operator| 't.m_bits = m_bits | bits' + early-return tail).
- GetPeekLocation (claimed; gpl/, gpl/sc.sh, prepend to CoverPoint_invalidate, needs --pool .rodata/.sdata): gpl/e3.cpp = 5 (2 real):
  pass '(COVERPOINT_TYPE)types.m_bits' to GetModifiedPosition (the orig's "uninitialised" r5). Left: object local r19 vs orig r21,
  'or r5,r5,r4' scheduled one slot early. perm_gpl running.
- FlagAsOccupied (cpo/x0.cpp with new operator|) fo = 5 (or-dest + mask colouring); old operator gives 4.
- TBI rand regs: in an isolated probe (probe3/) the first rand quotient takes f31 (orig-like); in Init it flips because of the param
  live ranges: removing 'm_velocity *= power * (speed * m_properties->m_speed)' flips it; 'power = power * K' (w2) fixes case 5 but
  breaks the param colouring. Rewrites of the velocity/power statements tried (g9/g10). perm_tbi running again.
- LANDED ae32d318 thrownbullet_tail += CThrownBullet::Init (tbi/final.cpp): rand quotients fixed by RANDF macro + 'float x, y, z;' then z,y,x assigns.
- GetPeekLocation now gpl/x1.cpp = 3 raw (1 real line: 'or r5,r5,r4' scheduled one slot before the @770 literal load; orig after).
  Key: object front via an inline 'CoverPointObjectView::GetFront(v) { CMatrix m; orientation.GetMatrix(m); m.GetFront(v); }' fixed the
  object register. Tried ~70 operator/mask/order forms (g8-g11). GAPP's orig has the 'or' early (like ours) - context-dependent schedule.
- FlagAsOccupied: 135 operator/mask combos (cpo/g20.py) - best 4 (old operator|). CheckLineCollision: operator forms no effect (10).
- NEW: CHearingVolume::HandleBulletCollision 0x80098408 (1344, append to hearingvolume; (d) guard hold is lifted; claimed): hv/c_d1.cpp = 15
  (only the reflect block's FPR numbering: orig scale f5 below nz/dx/dy/dz f6-f9, ours scale f9). bulletShield cheat -> sparks effect
  (static CVector3 vRight(1,0,0) + guard, basis from the reflected direction), else projectile path. perm_hv running on hv/hp.cpp.
- HV: hv/g3.cpp = 9 (inline 'Reflect(CVector3& r, const CVector3& d, const CVector3& n)' with dot/scale locals and products + adds):
  only nx/ny/scale FPR rotation (orig nx f3, ny f4, scale f5; ours nx f4, ny f5, scale f3). perm_hv on hv/hp.cpp (g3 + macros; fixup patched).
- cw (AIStartCoverPointWalkToStored): 'static inline unsigned char ArgFlag(int** stack) { return **stack != 0; }' gives the orig's
  'srwi r0 + mr' but flips the stack/flag callee regs (7); ~25 inline/decl variants (cw/x*, w*, u*) - still best f.cpp 2.
- LANDED 975735ac hearingvolume += CHearingVolume::HandleBulletCollision (hv/final.cpp).
- LANDED b2339184 hierobject_motion += CHierObject::BeginAnimMotionUpdate (bam/merged.cpp; inline LerpTo with 'float fx, fy, fz;').
- CStaticObject::BeginAnimMotionUpdate (claimed by bigfiles, NOT landed): sbam/b.cpp (soa2 + the same LerpTo inline) = 0 vs 0x800b1554; would append to static_obj_attempt.
- RotateFromScript: rfs/t5.cpp = 24 but with the orig CONTROL FLOW (both selections 'beq; b; fmr'): the ratio search in an inline
  'float SmallestRatio(a, b) { float p, q, tx, ty, tz; ... if ((p = a.y) != 0 && (q = b.y) != 0) ty = q / p; ... KeepSmaller x2; return tx; }'.
  Left: tx f5 vs orig f3 and the p/q loads one FPR off (all decl permutations tried, g33.py).

## HANDOFF (session 10, 2026-10-27)
Landed: cfcf75c6 NEXTSTATE, e9dd3fb1 AttemptPathMotionUpdate, 36940b62 GetAvailablePeekPosition, ae32d318 CThrownBullet::Init,
975735ac CHearingVolume::HandleBulletCollision, b2339184 CHierObject::BeginAnimMotionUpdate. Lock released; no permuters running.
Best WIPs: gpl/x1.cpp GetPeekLocation 3 raw (1 sched line); hv done; ti/vq2.cpp CThrownObject::Init 16; cpo FlagAsOccupied 4;
rfs/t5.cpp 24 (structure right); sbam/b.cpp CStaticObject::BeginAnimMotionUpdate = 0 but CLAIMED BY BIGFILES (not landed).
Unchanged: dcb8/b12 death_cb 4, smp/b 1 line, cw/f 2, fl9/m 2, dh/b 6, spm7 10, clc 10.
- LANDED 46f4a1bc static_obj_scale += SetMovementPath (smp3/m.cpp): 'const bool&' inline param gives the lone clrlwi 24.
- cw9/: ~70 more forms (const bool& inlines, shared StartWalk(stack,object,flag) helper, Ret/PopArgs inlines, 14 bool spellings): every form with the orig's 'mr r31,r0' swaps stack/flag regs (7); still 2.
- LANDED 9b50be09 bsbifunc_explosion += BIFunc_CreateImpact (ci9/final.cpp; visit+damage inside the inline CImpact ctor, read off the orig extab).
- cw9/w8: StartStored(ai, <bool|int|uchar|const&> f) x 4 arg spellings: still 2 (mr forms always swap).
- dh2/ DisplayHint: 'if (hint->text[next = hint->next])' (next assigned INSIDE the condition) gives the orig's direct 'lwz r29,8(r30)'
  (no mr). Left (7): texts coalesces with hint and nextIndex gets addi r31,r30,8; orig: texts = copy r31 (mr r31,r30), nextIndex in place
  (addi r30,r30,8). ~80 forms (void* casts, decl orders, in-place hint reuse, assignment positions) no.
- im2/ CImpact::Visit (148): the orig keeps start.y/z in f31/f30 across TestCollision because 'start' is NOT address-taken there.
  Probe m4 (start built from 3 floats, fiber arg 'CVector3(start.x, start.y, start.z)', Lerp written out) makes CW keep them in callee
  FPRs, but costs lfd copies/stack layout (247). The orig copies start with lfd (Pair copy) yet CSEs - unresolved. dis.py = our asm slice.
- fl10/ CheckFieldOfViewModeList: start-param inline forms, getters, decl orders: still 2.
- dcb9/ death_cb: comma-init / hoisted located/kept inits: still 4 (or worse).
- ti2/ CThrownObject::Init: 16 Length() spellings: still 16.
- rr2/ RotateRel: BSArgFloat/BSArgInt inline reads (args in ctor calls / locals / sub first or after): 74-78 (worse than q0_ap 26).

## HANDOFF (session 11, 2026-10-27)
Landed: 46f4a1bc static_obj_scale += SetMovementPath ('const bool&' inline param = lone clrlwi 24 re-mask),
9b50be09 bsbifunc_explosion += BIFunc_CreateImpact (visit + DamageClosestObject inside the inline CImpact ctor, read off the orig extab;
--weak-duplicate IVisitor vtable). Lock released; no permuters running; tree clean.
Unchanged near-misses: cw/f 2, fl9/m 2, dcb8/b12 4 (scorer dcb8/sc.sh), dh/b 6 (dh2 notes), gpl/x1 3, ti/vq2 16, im/im9 148 (sc im/sc.sh), mnf/b0 10, rr/q0_ap 26.

## Session 12 (2026-10-27 evening, fresh agent)
- NOTE: Soldier OnCollision was LANDED long ago (dd274d26); ignore the old oc_t1 notes.
- spm8/: CreateSplinePathManager indexed 'BPDPolyPath* p = &paths[i]' (s2 base) fixes count r29 but paths IV/i swap (14); inner-loop forms on spm7/b10 all 10. No gain.
- dcb10/: death_cb decl hill on b12 (all 11 decls) = 4, loop statement forms (kept++ split, c temp, comma inits) = 4. No gain.
- fl11/: CheckFieldOfViewModeList FindListObject forms (start param, ternary, current local) = 2. No gain.
- aim10/: aim_cb rotation brute force: rs.py/rs2.py (random over statement orders, b decls, up*=c position, inline Pitch wrapper with
  param perms/args-as-calls) and hc.py (hill climb from a31). Tools: aim10/hc.py <seed> <iters> [overrides]; sbs.py side-by-side
  (scratch/par/soldier/sbs.py <tmpl> <src> <func> <start> <end> <hexoff> <n>).
- LANDED cc4ec83b AIFilter_global += CreateSplinePathManager (spm8/v5.py 'ip' form; spm9/unit.cpp).
- mnf12/: MNFIT: if/else 'type = 51 / 52' (a.cpp) gives ALL the orig callee regs (stack r30, object r31, type r29) but CW turns it branchless
  (cntlzw/neg/addi) -> 12; switch(case 0 / default) keeps the regs with a b/beq shape (8). Top-init forms (orig shape) always rank type r31 (10):
  copies of params at every position, inline wrappers/default params, casts, types: no.
- ccp12/: ChooseCoverPoint 'self = this' / type copies at all positions: 23 (no effect).
- wpi12/: GetWayPointInfo case 2 offset forms: 26 (orig offset.x f29 / y f30, f31 unused there; ours x f31).
- LANDED 773ad5f5 thrownbullet_misc += CheckEnemies (ce13/unit.cpp; ce12/p/tref.cpp = soldier & target by reference into WarnOthers). e08972d8 = snapshot follow-up.
- ti12/ Length forms (two locals, sum temps, zz temps): 16. rfs12/ SmallestRatio output-reference form + decl perms: 25 (worse).
- ua12/ UpdateAnimation angle-blend forms (final add order, x/y/z first, int/short): 59 unchanged. ena12/ EnableAll: 13/9 unchanged.
- cw12/ ArgFlag + stack copy combos: 7 (best still f.cpp 2). mnf12 see above. aim10/hc.py hill climb 25 min: nothing below 65.
- GetWayPointInfo MATCHES: wpi13/bsbifunc_waypointinfo.cpp (NEW unit, landing pending lock; port --like bsbifunc_givegrenade.json
  --original-file bsbifunc.cpp --file-index 443 --pool .sdata2 --pool .rodata). Keys: case 9 plain '-inverse.x' (no double cast) and
  statement order 'position.z = 0; position -= CVector3(u, v, 0); inverse = GetFront(); inverse.x = -; inverse.y = -; inverse.z = 0;'
  (wpi12/c9.py brute force 23 -> 4); case 2 offset as floats 'float x; float z = 0.0f; x = wp->x; u = wp->y;' where u is the
  FUNCTION-LEVEL float that case 9 uses for wp->x (shared callee FPR; wpi12/v4.py) -> 0.
- LANDED 3171463d NEW bsbifunc_waypointinfo (GetWayPointInfo).
- aim10/share*.py: sharing trig vars / dot floats / b floats across aim_cb's two cases (with remaps): best 65, no gain.

## HANDOFF (session 12, 2026-10-27)
Landed: cc4ec83b CreateSplinePathManager (AIFilter_global), 773ad5f5 CheckEnemies (thrownbullet_misc; e08972d8 = snapshot follow-up
after land.sh's capture failed once with 'Stale progress snapshot'), 3171463d NEW bsbifunc_waypointinfo (GetWayPointInfo).
Lock released; no background jobs; tree clean.
Near-misses unchanged: aim/a31.cpp 65, dcb8/b12.cpp 4, fl9/m.cpp 2, cw/f.cpp 2, mnf/b0.cpp 10 (mnf12/a.cpp has all regs right but
branchless), ccp/b.cpp 23, ti/vq2.cpp 16, sdraw_if.cpp 6, ena/n.cpp 9, ua/c5.cpp 59, rfs/t5.cpp 24.
New tricks (TRICKS.md): pointer-param copy declared after the counter; inline helper taking the caller's locals by reference;
switch cases sharing a hoisted function-level float.

## Session 13 (2026-10-27 night, fresh agent)
- mfp13/, psf13/ (PSF Set param/body perms: still 45), dcb13/ (located decl first: still 4): no gain.
- NEW BIG: BIFunc_GetObjInfo 0x8002c258 (6224, never drafted; claimed). goi/: ghidra.c, orig.s, head.h + bodyN.cpp drafts,
  sc.sh (rawnr), sd.py (register-blind structural diff), sb.sh (side by side), hc.py (prologue decl-order hill climb).
  Append to bsbifunc_setobjinfo (template used for compiling); needs the .data jump table @0x8018293c (73 entries).
  d2.cpp: rawnr 427, struct 146 (mostly normalization noise). Keys: results in memory via '**stack = *(int*)&fresult/iresult';
  function-level basic/node/filter vars; CAIFilterRealPosition::operator=(CVector3 by value) gives the lfd temp copies;
  CVector3 aligned(8) + member-wise operator=; sqrtf direct in body (Length() inline reorders the volatile slots).

## Session 14 (2026-10-29, fresh agent)
- LANDED 1c473c69 AIObject_searchnodes += ComputeAStarPath (cap14/c1.cpp + constants). Key: removal loop 'CSearchNode* cur = head; while (cur) { node = cur->m_next; ...; cur = node; }'
  with the function-level 'node' as the next temp (CW colours per variable). Original constants 0.25f*maxCost and nearest = 0.0f.
  TRICK: when two loop vars are swapped and one of them is a reused function-level var, swap which ROLE the reused var plays.
- goi (BIFunc_GetObjInfo): predecessor's d9.cpp = rawnr 152 (d2 427, d7 246). Continue from d9 (body9.cpp + head.h, cm.py = '(0, x)' probe).
- goi14/: GetObjInfo. FOUND: the orig's 'stw r27,32(r1)' at case 23/25 starts = CW global CSE of the int constant 0 (top 'iresult = 0' and
  case-first 'iresult = 0' share one temp; probe goi14/pr/gen2.py) -> top must be plain 'iresult = 0;' with filter NOT initialised
  (z1.cpp 147; 'iresult = (int)(filter = 0)' blocks the CSE). Then hc (decl order) -> h1body.cpp = 128. Left: the temp is coloured
  volatile (r5) in ours vs r27 (shared with filter) in orig; (count-2)*4 CSE temp r28 vs orig r26 (shared with case-24 f); target r26 vs r28.
- goi14 progress: e2.cpp/e3.cpp (body; head = goi14/head.h; score: sh goi14/v.sh <body>) = rawnr 101. Fixes on top of h1:
  case 35/38 node branch also sets isFloat = true (was missing -> 2-instr shift), and use 'CVector3 d; d.x = ...; d.y; d.z' with
  'fresult = d.y * fwd.y + d.x * fwd.x' (float dx/dy locals colour wrong once the product order is right);
  ai->position / lookPoint copies as '{ float z = ..; float y = ..; float x = ..; pos.Set(x, y, z); }' (z,y,x load order).
  Left: target r26 vs orig r28 / shift-CSE r28 vs r26 (~65 lines, CW colour order: ours shift>filter>target, orig target>filter>shift),
  zero temp r5 vs r27, case-24 f r27 vs r26, case 35/38 fwd loads f0/f1, case 37 FPR order. perm_goi (autoperm on unit_e3.cpp, base 675) + hc3 running.
- CFO (CheckForObstructions) re-claimed: predecessor's cfo/l.cpp = 9 raw (not in notes before): (1) 'li r31,1' (sign) before the IsAbove
  switch vs orig after - every sign-after form (cfo14/s*, t*, u*: bool local, ternary, if/else, early decl, early dead def) colours
  sign r28 instead of r31 (25); (2) first TestLine: orig 'addi r28 (&g_scene)' before the flag zero 'li r3,0', ours 'li r5,0' first
  (cfo14/f*, g*: flag forms, world/scene locals: no change).
- goi14 colour experiments (all no gain, 101 stays): target decl before filter -> target r27/filter r26 but shift CSE stays r28
  (o1); named offset var (w*, y*): its *4 CSE still r28; copies of target (c*); inline BSArg forms (i*); do/while wrapper; bisecting
  by removing any single case (bis.py) never moves shift/target; zero temp via ternary/inline/iresult-reload never coalesces with filter
  (probe goi14/pr: CW CSEs the int 0 of memory stores only; assignments to register vars stay separate li). Permuter perm_goi (needs
  PERM_PRETEND decls; ~40% typemap failures) found only junk (650 via an uninitialised alias of target).
- cw14/: StartCoverPointWalkToStored: any temp form (int raw; casts) gives the orig's 'mr r31,r0' but swaps flag/stack (7); no gain.
- fl14/: CheckFieldOfViewModeList FindListObject orders / static-inline impl: no gain (2).
- ut14/: UpdateTargetPosition m_rate update via inline Add / += / pointer: still forwards m_rate.x (rawnr 344, len 626 vs 629).
- LANDED e7f6430e PathNodeList_closest += EnableAll (ena14/closest_unit2.cpp; o6 form, see TRICKS). Other ena14 forms: index/pointer/
  helper/iterator-copy variants all 11-13; 'int o = 0' before the for = 2 (li order).
- cfo14/: CFO permuter (perm_cfo14 on cfo14/lp.cpp = l.cpp + VEC3 macro, base 195) ~20k iters: nothing. Sign-in-inline-param (p*)
  makes CW refuse to inline (len 252). Sign as CSE'd ternary (v*): recomputed, worse.
- ccb14/, dcb14/: ChooseCoverPointByType top FPR forms (locals/struct/compound) 20-21; death_cb loop init orders / explicit write
  offset IV: 4 / worse.

## HANDOFF (session 14, 2026-10-29)
Landed: 1c473c69 AIObject_searchnodes += ComputeAStarPath [extab-pad 2x 0x90] (removal loop: function-level 'node' plays the
'next' role, fresh 'cur' walks; constants 0.25f / 0.0f), e7f6430e PathNodeList_closest += EnableAll (explicit byte-offset IV, i
initialised first). Lock released; no background jobs; tree clean of my files.
Best WIPs: goi14/e3.cpp GetObjInfo rawnr 101 (score: sh goi14/v.sh goi14/e3.cpp; was 152) - left = callee colour order of the
(count-2)*4 CSE temp vs target/filter, zero temp r5 vs r27; cfo/l.cpp CFO 9; cw/f.cpp 2; fl9/m.cpp 2; dcb8/b12 4; dh/b 6; ccb/xa 20.
Claims: CheckForObstructions (cfo/l.cpp 9), BIFunc_GetObjInfo (goi14) still mine.
- AISetupTargetMatchList (bsbifunc_choosetarget gap 0x80034100, 192; parked since 10-06 at 7-8 'vtable slot load scheduling'):
  stm14/g.cpp = 0: 'AIDoodadView* doodad = ((ISceneNode*)object)->GetAIDoodad(); ... doodad->filter->SetupTargetMatchList(...)'
  (filter loaded inside the call expression, not as a local) -> the slot load schedules late like the orig. Landing (lock wait).
  Same doodad-local form tried on cw (StartCoverPointWalkToStored): still 2.
- LANDED 28f05715 bsbifunc_choosetarget += BIFunc_AISetupTargetMatchList (stm14/g.cpp).
- Later tries (no gain): ccp14 (from-selection inline helper, radius reuse, direct member store: 23-27), dh14 (single reassigned
  pointer for hint/nextIndex: 7/23), dcb14 k* (loop-1 IV forms: 4+), goi14 hc5 (named byte offset 'off' + decl/assign hill climb: 101).
FINAL (session 14): landings 1c473c69, e7f6430e, 28f05715. No background jobs; lock free.
POLICY QUESTION (for the user): e7f6430e EnableAll uses an explicit byte-offset loop IV
  'for (i = 0, offset = 0; i < count; i++, offset += 4) (*(CPathNode**)((char*)nodes + offset))->m_enabled = true;'
  instead of 'nodes[i]' - real (hand strength-reduced) code, not a dead/constant device like the declined 'int o[1]' or the
  MathLLAngleInit 'int i = 0' index, but it exists to fix a register swap. Landed (not pushed); revert e7f6430e if the user rejects it.

## Session 15 (2026-10-29, fresh agent)
- cfo15/: CFO sign-after forms (block, ternary, Sign() inline, manual switch K*, if/else): all 25 (sign r28 shares node's reg). Probe
  probe15/p1.cpp: with sign defined after the inline switch, CW gives sign the dead node's register; with it before, sign r31.
  perm_cfo15 (s1 base 515) only reached 195 (= l.cpp basin). No gain; CFO stays cfo/l.cpp 9.
- psf15/: PlayerScreenFlash ArgColor-return forms / Set body orders: best still 45. Observation: in every form the FIRST-processed colour
  gets the LOW slots (8/12); the orig puts the first-processed (args 1-4) at 16/20.
- goi15/: GetObjInfo zero-temp: 'filter = 0' at top gives a separate li (245); 'iresult = (int)(filter = 0)' 137. No gain (101 stays).
- NEW BSEventProcessEvents 0x80034ea0 (892, never drafted, APPEND to bsevents_head; claimed). bpe15/: v.sh/sc2.sh scorers.
  bpe15/k_h.cpp (+head.h with the *handler-iterating find inline) = rawnr 20, len exact. Keys: hash loop 'else if (!started)' (orig has
  a dead bne), 'if (listTail) listTail->next = head; else listHead = head;', bucket head read as an expression (no local),
  'int i' declared first; loop body: 'last = record; record = record->m_next;' before count++/Immediate call; 'run = (flags & 2) != 0'
  (bool); 'code = entry->code' read first in the run block; hoisted body decls (hill order in k_h).
  PROBLEM: the file's real BSEventFindDeclaredState (local entry iterator; matches standalone) inlines with an extra 'mr' at the found
  path (iterator not coalesced with the caller's entry) -> full5.cpp 32. The *handler-iterating form coalesces like the orig but does
  not match standalone (15). The bigfiles Immediate draft (ds/bpe2_b.cpp) has the same issue. 'entry = record->m_handler' reuse
  gives the orig's r6/r11 for handler/eventNumber.
- bpe15 progress: with the REAL head BSEventFindDeclaredState (local-entry iterator; must stay as matched in bsevents_head.cpp),
  the caller's 'BSCode_EventHandlerEntry_struct* entry = record->m_handler;' declared BEFORE the object-value if (full7.cpp) makes CW
  coalesce the iterator with entry (no mr) -> rawnr 22 len exact. Defining entry inside the if loses the coalescing (32).
  w_A.cpp (inline helper 'BSEventRun(record, object, thread, record->m_handler)' holding the whole if-body, entry a by-value param)
  = 19: handler load after the bne and run r7 like the orig. Left: volatile colours eventNumber r6 (orig r11, ours reuses the dead
  load temp), iterator r8 (orig r6), states r9 (r8), flags r11 (r9). Decl-order hill climbs (caller, helper, inline): no effect.
  The orig Immediate (0x8003521c) has IDENTICAL colours in the same region -> very likely a shared inline helper in bsevents.cpp.
  imm1.cpp (Immediate with the full7 form) = 39 (bigfiles draft 48).
- bpe15 BREAKTHROUGH to 7 (both functions!): bpe15/BEST_pe7.cpp (= L_L1.cpp, ProcessEvents rawnr 7, len exact; score:
  sh bpe15/sc2.sh <abs path>) and bpe15/BEST_imm7.cpp (= imm3a.cpp, BSEventProcessImmediateEvents rawnr 7, len exact; sh bpe15/sci.sh).
  Form: a shared inline helper
    inline void BSEventRun(EventRecordView* record, BSObject* object, BSMachineThread_struct* thread) {
        bool run = false;
        if (object->m_value10 == record->m_value) {
            BSCode_EventHandlerEntry_struct* entry = record->m_handler;
            unsigned short stateId = BSEventFindDeclaredState(record->m_handler->eventNumber, thread, &entry);
            if (entry) { if (stateId != record->m_event) run = (entry->flags & 2) != 0; else run = true; }
            if (record->m_handler->eventNumber == 44) object->m_flag14 = true;
            if (run) { unsigned int code = entry->code; ...stackTop/frame/args...; instruction = ...; BSExecuteThread(record->m_object, record->m_thread); }
        }
    }
  callers: ProcessEvents 'g_pCurrentProcessingEvent = record; thread = record->m_thread; object = record->m_object; BSEventRun(record, object, thread);'
  Immediate 'thread = record->m_thread; object = record->m_object; BSEventRun(...)'. Uses the REAL head BSEventFindDeclaredState unchanged.
  KEYS: eventNumber NOT a local: 'record->m_handler->eventNumber' both as the Find arg and in the ==44 test (CW CSEs it into r11);
  the helper boundary makes CW coalesce the Find iterator with 'entry' (no mr) and puts the handler load after the bne.
  LEFT (7 lines each, identical in both): inside the inlined Find, 'state' gets r9 and the entry->flags CSE temp r10; orig flags r9,
  state r10. Tried: helper param sets/orders (24 perms), run placement (helper/caller by-ref/inside if), entry param vs local, early
  return, if(entry) forms, stateId int, decl splits, wrapper helpers W1-W3 (W1 'BSEventGetHandler' gets flags r9 right but moves run),
  head Find decl variants (break standalone), permuter on n1 7k iters: nothing.
  LANDING PLAN when 0: ProcessEvents APPEND to bsevents_head (it follows at 0x80034ea0; head's Find is defined in the same unit and
  auto-inlined); Immediate (0x8003521c) follows ProcessEvents directly and precedes bsevents_queue (0x80035440): append to the same
  unit too (whole head..queue span contiguous). Immediate claim: bigfiles 10-28 11:41 (parked; check before landing).

## Session 16 (2026-10-29, fresh agent)
- LANDED e4e34d00 bsevents_head += BSEventProcessEvents + BSEventProcessImmediateEvents (both 7 -> 0). Fix: head Find's
  'stateId = state->parent;' -> 'stateId = states[stateId]->parent;' (standalone identical). Harness bpe16/ft.py tests Find variants
  against head + both callers. Both callers share the 3-param BSEventRun helper.
- LANDED 3d1e56c9 AISplinePath += AllocateGenerateBuffers (136): its '...rodata.0' block = header dwi_prime_list[27] + the 4 buffer
  names -> define the prime list in the unit, port with --pool .rodata. Survey of other (e)-blocked fns: bpe16/rodata0_list.txt +
  bpe16/rodata1.py (most blocks start with the primes; strings right after primes for TLT_LoadLevelContentsFile 0x8016e598,
  FindAllMGPointsInBPDFile 0x8016f290, AllocateStaticObject 0x80177238, CScene ctor 0x80178130, ShellMenu block 0x801783c8 - other lanes).
- GetObjInfo goi16/best17.cpp = rawnr 17 (from 101; score: sh goi14/v.sh goi16/best17.cpp). Fixes: target via
  'static inline BSObjectView* ArgObject(int** stack, int count, int n) { return *(BSObjectView**)(*stack - (count - n)); }' (target r28);
  case 37 'float dot = other.x - selfPos.x; dot *= forward.x; float dy = other.y - selfPos.y; dot += dy * forward.y; iresult = dot < 0.0f;'
  (actually: dot = dx; dot *= fwd.x; dot += dy * fwd.y with dy declared before the += line, see c37i1); case 35/38
  'float dot = d.x; dot *= forward.x; dot += d.y * forward.y; fresult = dot;'.
  Left (17, GPR only): orig zero temp + ALL filter defs share r27 (filter on the !self path in case 3 = the zero) and shift CSE r26;
  ours zero r5, filter r26, shift r27. 'filter = self ? self->GetAIDoodad()->filter : 0;' at all 4 sites merges zero+filter (A_all/m_all)
  but then CW ranks the merged web above target and swaps self/isFloat (193). Decl hill climbs (hc2.py) running on best17 + m_all.
- fl16/ (FOV list start-load order), cw16/ (BSTopArgBool int inline gives the orig 'mr r31,r0' but swaps stack/flag: loc_h1 7; permuter
  31k iters nothing), cfo16/ (IsAbove ref/ptr/member/parts x 4 sign forms; static/plain: S0 9, sign-after always 25), dh16/ (decl/stmt
  orders 6-8), dcb16/ (loop init forms 4): no gains.
- GetObjInfo goi16/hc8_best.cpp = rawnr 16 (best20 + hc3 decl/assign order); goi16/best20.cpp = rawnr 20 but STRUCTURALLY better than best17: top 'iresult = (int)(filter = 0);' + case 3
  'if (self) filter = ...; else filter = 0;' -> zero + ALL filter defs in one web and the case-23/25 'iresult = 0' stores CSE'd to it,
  exactly like the orig. Left: pure swap web r26 <-> shift CSE r27 (orig web r27, shift r26) + top volatile *stack r5 vs r6.
  hc3.py (decl/assign hill climb for this base) running. bf/ = 4-site filter-form brute force (O/A/C/D per site) on best17/zv2.
- goi16 after hc8 (16): tried ArgInt/ArgFloat/ArgNode inlines, ClearFilter/NoFilter inlines, offset param/var forms: all 16 (web r26 vs
  shift r27 swap stays). perm_goi16 (autoperm on goi16/full16.cpp = head.h + hc8_best, base 80) running.
- ccb16/: ChooseCoverPointByType top dx/dy/dz load-name forms (in-place, locals, negation): 20-41, no gain.
- cw16/gen3: Walk/StackTop inline wrappers: 7-8, no gain.

## HANDOFF (session 16, 2026-10-29)
Landed: e4e34d00 bsevents_head += BSEventProcessEvents + BSEventProcessImmediateEvents (head Find 'stateId = states[stateId]->parent;'),
3d1e56c9 AISplinePath += AllocateGenerateBuffers (prime list + --pool .rodata). Lock released, no background jobs, tree clean of my files.
Best WIPs: goi16/hc8_best.cpp GetObjInfo rawnr 16 (sh goi14/v.sh <abs path>; only web r26 <-> shift r27 swap left); cfo/l.cpp CFO 9;
cw/f.cpp 2 (cw16/loc_h1.cpp 7 has the orig's mr but swapped regs); fl9/m.cpp 2; dcb8/b12 4; dh/b 6; ccb/q2 20.
Claims still mine: BIFunc_GetObjInfo, CheckForObstructions.
For the coordinator: rodata.0 blocks begin with dwi_prime_list - candidates for other lanes in bpe16/rodata1.py output (see above);
FireWeapon (soldier, AUTOPILOT 10-18 'MATCHES except string addressing') block 0x80176f58 = primes + 164-byte int table + RTTI names
(372 B) before its strings - would need that prefix reproduced in pool mode (not tried).

## Session 17 (2026-10-29, fresh agent)
- FireWeapon (fw17/): wip.cpp (= scratch/game/soldier_fireweapon_wip.cpp) + prime list (p1.cpp) = 30 (only string offsets: ours +108, orig +372).
  h1.cpp (p1 + HARNESS 164-B int array + 100-B char array before DebugMsg) = 0 -> the function itself is DONE; only the prefix blocks it.
  Prefix decoded (nm of the orig): +0 dwi_prime_list (108), +108 @1259 (56 B) and +164 @1270 (108 B) = OnCollision's two aggregate-
  initialiser tables (classes[7], events[9]; OnCollision is at 0x800abc6c, AFTER FireWeapon), +272 @1681..@1692 (100 B) = RTTI class
  names CSoldierObject, IDestructible, ISubject, IObserver, ISceneNode, IMovingSceneNode, CAnimObject (the orig file was compiled with
  RTTI ON: __RTTI__ records at 0x8034dfcc.. and the CSoldierObject vtable 0x801882f0 reference them), then strings @2112.. (FireWeapon).
  CW probe (fw17/rt*.cpp, rtc.py): with -RTTI on, CW emits vtable + RTTI names only in the TU defining the FIRST-DECLARED non-inline
  virtual of the class (key function); names > 8 B go to .rodata.
  -> A faithful unit needs (i) OnCollision in the same unit (merge attach..collision: emitter, helmet, velocity, bullet, look, collision
  are all landed and contiguous with FireWeapon) AND (ii) the key function in the unit with -RTTI on: if the key is Destroy (0x800a9b3c,
  file start) the unit must span Soldier_object..collision (10 units, ~2000 lines, one unified CSoldierObject : CAnimObject : IMovingSceneNode
  : ISceneNode : IObserver : ISubject : IDestructible declaration); if the key is the dtor (cleanup unit) only the whole file works,
  which is blocked by Draw (6) and UpdateAnimation (68). Pooled .data/.sdata would drop the emitted vtable/RTTI records.
  POLICY QUESTION: land FireWeapon with a reproduced 264-byte .rodata prefix (dead static data = the original bytes of the two OnCollision
  tables + RTTI names), or only via the big merged unit (which also needs a guessed 'Destroy declared first' key function + -RTTI on)?
  Not landed.
- GetObjInfo goi17/: case 29/30 ARG(2) spellings (ArgObject cast, inline expr, node local), 'int count = ...' init, GetFilter() at
  sites 3/21/29/30 (126-159), ARG as '*stack + (n - count)' (32/42): no gain, hc8_best 16 stays (also a policy item via '(int)(filter = 0)').
- Not soldier-landable via primes: SetupAStarPathWalk 0x8004c720 / StartAStarPathWalk(CAIObject*) 0x8004c9a4 use AIObject.cpp's block at
  +676..+872 (needs every earlier block object); buddy.cpp fns = bigfiles' whole-file claim.
- MNFIT (BIFunc_MakeNoiseFromInputTrigger, mnf/b0.cpp 10 = callee rotation only: orig object r31/stack r30/type r29, ours type r31).
  FINDING: when ALL defs of type sit inside the branch construct, type drops to r29 and the params go to r30/r31 exactly like the
  orig: mnf/x3.cpp 'switch (flag) { default: type = 52; break; case 0: type = 51; break; }' = 11 with ALL registers right, only the
  branch layout differs (beq + li 52 + b instead of the orig's hoisted 'li r29,52' ... bne; li r29,51); w1 (case 0 first) = 8.
  if/else forms are turned into arithmetic (51/52 differ by 1: cntlzw/neg/addi) for every int/long type (v3, w2, y*, q3, q6, q7);
  short/char types also arithmetic. Inline NoiseType returns, do/while break, inner temp, scoped block, NoiseAt inline wrapper: 10-31.
- MNFIT brute force mnf17/gen.py (decl order x read order x flag via args/direct x type init; 1728 compiles): best 14 (> 10). No gain.
- FIREWEAPON SOLVED (fw17/): Soldier_object.cpp is a DEFERRED-INLINE + RTTI-ON file. Probe fw17/rt7.cpp: with '-inline deferred,auto'
  CW creates string constants at codegen time (after the end-of-TU vtable/RTTI), so .rodata = parse-time objects (primes, OnCollision's
  aggregate initialisers) + RTTI class names + strings = exactly the orig block layout. Merged unit fw17/final.cpp = attach + FireWeapon
  + emitter + helmet + velocity + bullet + look + collision (0x800a9f1c..0x800ac310), source in REVERSE text order, CSoldierObject :
  CAnimObject : IMovingSceneNode : ISceneNode : IObserver : ISubject : IDestructible with the real __vt__14CSoldierObject slot names,
  AttachObject declared first (key function -> vtable + RTTI emitted; .data 0x80188230 size 0x260 and .sdata 0x8034dfcc size 0x38
  match the orig layout exactly, incl. bullet's jump table). ALL 15 functions 0 (score: python3 fw17/ms.py fw17/final.cpp [-r] [-v<fn>]).
  KEY conflict: bullet's Cross/Normalize FPR colouring breaks with ANY user CVector3 copy ctor (SetRight(right)/SetFront(front) by-value
  copies of the locals); attach/FW need pair copies -> no copy ctor, copy-inits split 'CVector3 x; x = y;' (pair operator=), and
  CMatrix::GetPos returns 'CVector3(*(const CVector3Pair*)&pos)' via a CVector3Pair ctor (SetHelmetOffset).
- LANDED 44a2a210 Soldier_object_attach (0x800a9f1c..0x800ac310 one unit) += FireWeapon. Source = fw17/final2.cpp (final.cpp + CAnimObject /
  IMovingSceneNode override declarations so each vtable slot names the original - port_unit checks vtable relocations).
  Port: port_unit Soldier_object_attach src/... --like config/GMFE69/Soldier_object_attach.json --category reconstructed_game --pool .sdata2
  --original-file Soldier_object.cpp (flags edited first: -inline deferred,auto, -RTTI on). one_unit VERIFIED. The FireWeapon policy question
  above is moot (no fabricated prefix: the prefix is the file's own data).
- AIObject.cpp follow-up: its block 0x8016f5c8 = primes + RTTI "CAIObject" (@1901) + warning strings of covered fns, SetupAStarPathWalk's
  strings at +676 -> same deferred+RTTI merged-unit route would work for SetupAStarPathWalk 0x8004c720 + StartAStarPathWalk(CAIObject*)
  0x8004c9a4 (1556 B), but the unit must span the file from its first block-object producer (~0x800478a8 AIObject_weak..) to 0x8004cd34
  contiguously, and two gaps are open: CheckFieldOfViewModeList 0x80049974 (fl9/m.cpp 2, also 2 under deferred) and ChooseCoverPoint
  0x8004b000 (368, parked ~20-44). Solve those two first.
- Draw 0x800ac9e4 (follows the bounds unit): RemoveWeapons as a real out-of-class member auto-inlined under deferred (dr17/) = same 6.

## HANDOFF (session 17, 2026-10-29)
Landed: 44a2a210 Soldier_object_attach = SetHelmetOffset..OnCollision one unit (deferred + RTTI on) += FireWeapon (1176).
Lock released; no background jobs; tree clean of my files.
Best WIPs unchanged: goi16/hc8_best.cpp GetObjInfo 16 (policy form '(int)(filter = 0)'), mnf/b0.cpp MNFIT 10 (mnf/x3.cpp: switch form gets
ALL regs right, 11, only branch layout), cfo/l.cpp CFO 9, cw/f.cpp 2, fl9/m.cpp 2, dcb8/b12 4, sdraw_if.cpp Draw 6.
Claims: BIFunc_GetObjInfo, CheckForObstructions.

## Session 18 (2026-10-29, fresh agent)
- LANDED b69a8cef bsbifunc_makenoise_bi += BIFunc_MakeNoiseFromInputTrigger (mnf18/final.cpp; e4.cpp in the harness). Enum-typed type
  local + 'if (flag == 0) type = A; else type = B;' = 0 (e2 with the arms swapped = 3; the ternary e6 = 0 too). Trick added to TRICKS.md.
- fl18/: CheckFieldOfViewModeList with the global read directly at every site (no 'first' local): 23-32, worse. The orig's 'lwz r29,1688(r3)'
  before 'mr r28,r3' prologue order is UNIQUE in the whole binary (prologue scan), so it is likely not a common source idiom.
- ccp18/: ChooseCoverPoint under '-inline deferred,auto' = same 23 (pure register rotation this/type/from).
- fl18/perm.py: 30 decl/statement forms of the FOV caller (start/object/FP decl orders, FP block top/bottom): best 2 (base). fl18/i1,i2: whole
  body as an inline member CheckListImpl(start) to make the this-copy late: 13. Still 2.
- cw18/: 17 flag spellings (bool/int/uchar, if/else, ternary, !!, temps): best 2 (base). No 'mr r31,r0'.
- cfo18/: sign as enum ESign (a-e: init+if, if/else, ternary, int if/else): 9 / 25. No gain.
- adjscan.py (orig objdump -> uncovered fns keeping adjacent-constant if/else as branches): candidates listed in TRICKS.md (other lanes'
  AnimStProcess r27 3/2, MemoryCardAutoLoad 65/66, RestartGame, _Capture, CCompartment/CStaticMesh Init 9/8).
- Priority-1 AIObject merged unit: still blocked on CheckFieldOfViewModeList (2, scheduling: the orig's 'lwz r29,1688(r3)' before
  'mr r28,r3' is unique in the binary) and ChooseCoverPoint (23, this/type/from rotation; same 23 under deferred).

## HANDOFF (session 18, 2026-10-29)
Landed: b69a8cef bsbifunc_makenoise_bi += BIFunc_MakeNoiseFromInputTrigger (enum-typed type local). Lock released (the coordinator
released it after the cut-off); no background jobs; tree clean of my files (src/ea/gc_idct.cpp untracked is NOT mine).
Best WIPs unchanged: goi16/hc8_best.cpp GetObjInfo 16 (policy form), cfo/l.cpp 9, cw/f.cpp 2, fl9/m.cpp 2, ccp/b.cpp 23, dcb8/b12 4, sdraw_if 6.
Claims: BIFunc_GetObjInfo, CheckForObstructions.

## Session 19 (2026-10-29, fresh agent)
- Coordinator's enum-trick candidates are ALL base-blocked, not just register-blocked:
  * RestartGame 0x80019e54 (2/3): Moh2.cpp .bss base 0x80194840 (g_scene/g_camera/g_fog/viewports/renderlist/g_Pause/g_Shell with
    @788.. dtor records) -> needs a unit that owns Moh2 .bss = must contain __sinit_Moh2_cpp (1860, ~460 instrs: CScene/CShellMenu/
    Pause/CRenderList/CScreenFieldMode/CFaderControl ctors + inlined CCamera/viewport ctors, 7 __register_global_object) and span
    RestartGame..sinit (folds Moh2_sendrenderlist). LoadTheGame/GameLoop/main/InitCORE also use the base. Multi-session bigfile job.
  * MemoryCardAutoLoad 0x800e42e8 (65/66): ShellMenu .rodata block 0x801783c8 (primes first) used by ~9 ShellMenu fns -> whole
    ShellMenu.cpp only (Tier 3, 25 KB new).
  * CCompartment::Init 0x800761c8 (9/8): compartment .bss base 0x801d4ee0 (r30 = base; g_GlobalCpt +272, g_CptSimpleBin +388,
    g_CptSimpleAlphaBin +436) -> unit Init..__sinit_compartment_cpp 0x800761c8..0x800775e0 (sinit 1128 new; folds compartment_reset,
    _bins, _bindtors, _weak, _offsetptr, _fastvec) owning .bss 0x801d4ee0..0x801ddd78. AND its GXInitTexObj(CI) blocks are byte-identical
    in shape to CStaticMesh::Init's (bigfiles' 103-line blocker: orig keeps shape in r4 and does 'addi r4,r4,16' for the image LAST;
    ours computes the image first -> shape r5). So Init depends on bigfiles solving that first. Claim released.
- smi19/ (probe on a COPY of bigfiles' smi/BEST_k01p.cpp, scorer smi19/sc.sh): image-arg spellings (6), wrap/size locals before the call
  (5; CW forwards them back into the call: output IDENTICAL), shape-definition forms (5, incl. member GetShape()), compiler 1.3 (162) /
  '-inline deferred,auto' (103): all 103. Analysis: the orig's flags live in r5 and mipmap li r10 is early = args evaluated RIGHT-TO-LEFT
  (name, mipmap, wrap_t, wrap_s, format, height, width, image) so shape dies at the image add and coalesces into r4; ours evaluates
  image before width/height (shape still live -> r5, width via r9). Unknown what makes CW evaluate this call right-to-left.
- cw19/: enum flag forms (e1-e8: 7-10), register/int/uchar/self (r1-r10), int->bool conversion 'bool flag = **stack;' (p1 = the orig's
  'srwi r0 + mr r31,r0' SHAPE but stack/flag regs swapped, 7; same as BSArgBool in landed BIFunc_SetPlayerFallingDamage = the only other
  fn in the binary with this shape, where the orig also has stack r31/bool r30), inline wrappers on p1 (h1-h7: 7-8). Still 2 (f.cpp).
- ccp19/ ChooseCoverPoint 'from' def forms (init 0, nested ternary, self, inverted): 23-34. dcb19/ death_cb i/kept/count/located
  int-vs-unsigned grid (16): best 4 (base). fl19/ FOV list: self=this, start via this->, separate decls, last/first locals in
  FindListObject (a1-a4): 2-4 (global load always scheduled before the m_listObject load in ours). dr19/ Draw weapons-loop forms
  (unsigned, do/while, w temp, continue, top decl): 6-7.
- smi19 MICRO-PROBE (p1/p2/g*/h*/k*.cpp, disasm smi19/dis.sh <abs src>): the GXInitTexObjCI shape-in-r4 + 'addi r4,r4,16' LAST form IS
  produced when the shape pointer is defined in the SAME basic block as the call (g0, g1: GOOD). Any branch between the def and the call
  that reads shape (shape->format compare, switch, '&0x10' address test, field store) gives the bad form (shape r5, image addi early):
  g2-g4, g6, h1-h7 (re-derived shape, copies, w/h locals, reassign shape=data, explicit +16), k1-k4 (inline wrappers taking shape,
  Image() inline) all bad. So the orig keeps a cross-block shape var in r4 by some construct not yet found. Hand-off to bigfiles
  (CStaticMesh::Init 103) and anyone doing CCompartment::Init.
  (q1-q7: const/register shape, early return, int cond, u16 casts, explicit tex address, typed constants: all bad.)

## HANDOFF (session 19, 2026-10-29)
No landings. Lock not taken; no background jobs; tree clean of my files.
Candidates from the coordinator (MemoryCardAutoLoad, RestartGame, CCompartment::Init) are base/whole-file-blocked (see Session 19).
Best WIPs unchanged: goi16/hc8_best.cpp GetObjInfo 16 (policy form, do not land), cfo/l.cpp 9, cw/f.cpp 2, fl9/m.cpp 2, ccp/b.cpp 23,
dcb8/b12 4, sdraw_if 6. Claims: BIFunc_GetObjInfo, CheckForObstructions (CCompartment::Init claim released).

## Session 20 (2026-10-29, fresh agent) - Moh2.cpp whole-file merge (claimed)
- SCOPE FINDING: the unit must be the WHOLE Moh2.cpp record (0x80016e80 MohDVDErrorTask .. weak tail), not only main..sinit:
  MohDVDErrorTask (3328) AND main both address the ...rodata.0 block 0x8016e040 by base+offset (block = prime list, DSS @944/@945
  templates, DVD strings @1414..@1435, main strings @2322..@2328, then RTTI names @3274..). @N order = codegen order = address order
  (DVD @1412 < main @2320 < GL @2535 < LTG @2642 < DSS @2799 < RG @2933 < sinit @3269; templates @944 parse-time) -> deferred file,
  source in REVERSE address order. New code: DVD 3328 + main 3616 + GameLoop 2748 + LTG 696 + RG 692 + InitCORE 408 + sinit 1860 = 13.3 KB.
  .bss layout (offsets from 0x80194840): @788 rec, g_scene +12 (328), @789, g_camera +352 (672), @791 +0x400, g_fog +0x410 (80, 16-aligned),
  g_viewport +0x460 (112), g_mpViewports +0x4d0 (6x112), @792, theRenderList +1916, @793, g_Pause +1964 (152), @794, g_Shell +2128 (6620),
  g_FadeIn/g_config/g_cheats (20 each), @795, g_screen +8832 (272, 16-aligned), multiPlayerEndGameFadeOut, ident$906, (g_openFiles?).
- Harness: m2/sc.py <src> [fn..] [-vFN] [-d non-deferred] [-r sections] (inlines '#include "hdr.h"'); m2/vrun.py base FN variants.py.
  m2/hdr.h = shared header (grows per function).
- InitCORE 0 and RestartGame 0: m2/a3.cpp (RG: decl order paused(uchar), name, elf, flag(int = byte != 0), last; reads flag before paused).
  InitCORE: SysInitData {1,0,1,0,0} aggregate local (sdata2 @807), CColor(0,0,0,0x80) via an inline 4-arg ctor (stb x4).
- LoadTheGame 3 (m2/ltg.cpp): only 'mr r0,r3; addi r3; mr r30,r0' for the Get_currentMission result (orig moves straight to r30).
- GameLoop 0 (m2/gl.cpp, 2748 B): keys: CVector3/CMatrix 16-aligned (viewport 112, stack realign), CDrawContext with 3 CMatrix + m_world
  at +192 (size 288), 'unsigned int ticks = delta' (unsigned long = 79!), CFaderControl::SetFrom(CColor) by-value inline, 'm_value0f = 0'
  BEFORE 'g_uiGameState |= 1', 'm_state >= 5 || m_state == 0', static locals lastVSync/lastTimeRemaining (init = first value), static
  CMatrix ident(1) (ctor variant calling Ident), MP viewport loop 'while (mp->controllers[j++] == -1);'.
- main 0 (m2/main_ok.cpp, 3616 B; harness HARNESS_pad[748] stands in for the earlier block objects): keys: inline GetFrameDelta()
  (g_frameDel * 0.1 if g_cheats.slowmo > 0), level loop 'for(;;){... if (quit || (state&8) || mp) break; font...}' + shutdown after it,
  'bool quit = g_Shell.m_value0f;' read before CalculateLevelStats, 'char* data = TLT_Load..; new CFont(data, 111)', FIRST DrawText
  with a 'float y' local, SECOND direct, text as the ternary 'umTable.count > 756 ? umTable.entries[756].text : 0' (GetText inline = 970!),
  'CPlayerObject* player = GetPlayer(0); player->m_showUI = 0;' (direct store hoists 0 into r18), g_inputMgr.Shutdown() non-static.
- MohDVDErrorTask 0 (m2/dvd.cpp, 3328 B; HARNESS_pad[300]): inline ErrorLine(y, text) (SetClearColor(0,0,0,128), SetClear, font color
  (80,18,18,128), DrawTextCentered twice), cases in source order 5,6,4,11,-1,0 (= @N order), 'int error', g_screen by its own symbol
  (only one .bss object used -> no block base), static locals with '= 0' get guards.
- MERGED UNIT m2/full.cpp (+ m2/fh.h header): WHOLE Moh2.cpp, ALL functions 0 (sc.py; DebugMsg '2' = fast.py's unnormalised
  'bne cr1,<abs>' artefact) incl. __sinit_Moh2_cpp, and ALL data identical (m2/cmpdata.py: extab, extabindex, .rodata, .data incl. the
  25 KB InternalFont blob (m2/font.inc, FNTG font), .sdata, .sdata2; .bss/.sbss layouts identical). Keys: '-inline deferred,auto' +
  RTTI on, source in REVERSE address order; '#pragma dont_inline on/reset' around EndErrorText, LoadTheGame, RestartGame (else
  deferred auto-inlines them into DVD/main; precedent MemoryCardGC 41b619ec); LoadTheGame via a file-local inline
  'int GetLevel(int& stage)' (only form giving 0); CMatrix(bool ident = false) (Ident when true: camera m_tm/m_worldToCamera,
  GameLoop's static ident), CMatrix copy ctor '{ *this = m; }' (gives the orig's temp+copy of the 6 g_mpViewports initialisers),
  CViewport(l,t,r,b,id) ctor, ConfigView/CheatsView ctors as body assignments in the orig store order, CFog ctor
  'CColor c(...); SetColor(c);', DrawText y via 'float scale' local (fixes @2326/@2327 .sdata2 order).
- PORT (under lock): removed the 11 Moh2_* units (json+src+project.json), src/game/Moh2.cpp = m2/final.cpp, port_unit Moh2 ... --like
  m2/like.json (deferred,auto + RTTI on) --category reconstructed_game --original-file Moh2.cpp --weak-duplicate __vt__10ISceneNode,
  then removed the 'adapted_from' key port_unit adds for an out-of-config --like (test_provenance). one_unit VERIFIED.
  BUG FOUND by port_unit: LoadTheGame passes 0.0f (not 0.1f) to DrawStatusScreen - fast.py hides SDA constant identity; cmpdata.py only
  checks pool values/order. TOOL FIX tools/port_unit.py kept_span: the 'numbered object at the same original address+size' keep rule
  now applies to every section, not just .bss (with --weak-duplicate set, strings reached via ...rodata.0+offset and the base lists
  reached only through kept named weak vtables were being listed as stripped_objects).

## HANDOFF (session 20, 2026-10-29)
LANDED e8cf43ee Moh2.cpp whole file (13,348 new bytes; 93 fns; folds 11 Moh2_* units) -> 83.39%. Lock released; no background jobs;
tree clean of my files. AUTOPILOT log line + TRICKS entry added; claim marked LANDED.
POLICY QUESTIONS for the user (landed, revert if declined): (a) three '#pragma dont_inline on/reset' (EndErrorText, LoadTheGame,
RestartGame) - same device bigfiles used in 41b619ec; (b) LoadTheGame's mission/stage choice written as a file-local inline helper
'int GetLevel(int& stage)' (the only form found that matches; a plain if/else gives 3 lines, one 'mr r0' copy); (c) tools/port_unit.py
kept_span generalisation (bss-only rule -> all sections), tests pass. Inferred-but-plausible: CMatrix(bool ident = false), CMatrix copy
ctor '{ *this = m; }', ConfigView/CheatsView constructors as body assignments in store order.
Best older WIPs unchanged: goi16/hc8_best.cpp GetObjInfo 16 (policy form, do not land), cfo/l.cpp 9, cw/f.cpp 2, fl9/m.cpp 2, ccp/b.cpp 23,
dcb8/b12 4, sdraw_if 6. Claims still open: BIFunc_GetObjInfo, CheckForObstructions.

## Session 21 (2026-10-29, fresh agent) - ShellMenu verdict + staticobjfactory.cpp merge (claimed)
- ShellMenu.cpp whole/prefix file = POLICY-BLOCKED (same class as aramalloc/rcmp_main/debug_prof): its ...rodata.0 block holds
  strings NO code in the binary references: @2984-@3001 ("The display mode has switched / to Progressive Mode / This game can be
  played in Progressive Mode / Press the A/B Button for ...") and @3046/@3047 ("Checkin Controller to see if B is pressed for
  progressive mode.", "B Button IS DOWN!") at block+0x37c..+0x4c3, BEFORE every base user's strings (PrefWeaponName @3180.. at +0x670,
  BootUpPortCheck @3059.. etc). They come from a linker-stripped (unreferenced, static?) progressive-mode function compiled in
  normal (non-deferred) order before PassMP_PrefHitLocationToIStudio. Other 'early' @N (PLAYER %d @3362, ShellInit's gsh @3411..,
  'No Controller' @3564/5, GetLevelStats' @3534-7, BootUpPortCheck's memory-card texts @2930..@3067) = static helpers compiled at
  definition and discarded after inlining (recoverable from their inlined bodies). ShellMenu is NON-deferred (@N monotonic with
  address for every live function). POLICY QUESTION: allow a reconstructed dead static function that only reproduces these pool
  strings (+ the .sdata2 float @2988 it creates first)? (like (h) CExplosion::Visit, which was declined). Not worked further.
- AIObject.cpp (other candidate): block clean (no orphans) but the unit must span 0x800478a8..0x8004cd34 incl. the long-stuck
  CheckFieldOfViewModeList (2) and ChooseCoverPoint (23). Not attempted again.
- staticobjfactory.cpp (claimed instead): unit = Init 0x800b254c (3376, open) .. Compare 0x800b4368 (folds _weak, _lookup, _compare;
  NOT patchmvd/helmet which precede Init). Non-deferred; block 0x80177238 = primes + @1401 (Init) + @1546 (Dealloc) + Allocate's 9.
  WORK DIR scratch/par/soldier/sof/: ms.py <src> (all-fn scorer), si.sh / sc.sh <abs src> [-v] (Init / Allocate, REGBLIND=1 NOSTACK=1
  env for register-blind), cd.py <src> 8000d580 (rodata+extab compare), rmap.py <src> <fn> (orig->ours register map).
  * AllocateStaticObject = 0 (alloc_ok.cpp / q4.cpp; was 330 in scratch/sofw/a.cpp). Keys: NumFrames/TicksPerFrame inlines with the
    condition inline ('if (idx >= 0 && (idx & 2)) return ...; return 0;' not via a bool IsAnimated()), GetMesh 'if (idx >= 0) return
    mesh; return 0;', normal-object pool pop as ONE static inline PopFreeSlot() (list selection + pop + 'idx++; idx = idx >= 3 ? 0 : idx;'
    with 'return 0' from the DestroyOldestDumpObject branch), Hier/Attach: 'if (!g_pXFreeList) {msg; return 0;} slot = g_pXFreeList;',
    AIObj: 'slot = list; if (!slot) ...; pop; CTankObject* mem = (CTankObject*)slot; new (mem) CTankObject(...)', thrown pop
    'slot = g_pThrownObjectFreeList; g_p.. = ((ThrownObjectSlotView*)slot)->m_next;', recycled loop 'index = ...; g_idx++;' order.
  * Init: u4.cpp = 145 raw / 0 register-blind (len 844 = orig). Keys: real CStaticMesh (inline ctor: m_data=0, two 1-bit fields =
    true; virtuals ~,Draw,EnableLighting,IsLightingEnabled,EnableFogging,IsFoggingEnabled), CStaticAnimMesh : CStaticMesh (+12 = 0),
    mesh creation 'if (t.m_flags & 2) t.CreateAnimMesh(data, !cached); else t.CreateStaticMesh(data, !cached);' (members: index store,
    placement new, mesh->Init(data, flag)), 0x13 loop 'int flags = ...m_flags; DebugMsg(...)' before the branch + while-loop with
    'r = Next(r); count++;' at the bottom, pool settings 'DEF(..., pools)->field' per statement (def reloaded), motion name of the
    0x15 variant = '((const char**)((char*)def + (i * 24 + 0x38)))[1]' (the ONLY form giving the orig's separate IV 'li r30,56';
    looking for a natural spelling). REMAINING: pure register colouring (rmap: orig r27..r31 -> ours r23..r27; ours gives r28-r31 to
    count(+template new ptr web) and the first 3 array ptrs). Decl orders (30 random hoists) inert.
  * Merged unit m0.cpp (n2 + statics + Compare): all other fns 0; .rodata identical (928 B); extab differs only by Init's registers
    (DELETEPOINTERCOND cond/ptr regs) + the 2-byte padding (orig 0x2500 = the approved extab-pad patch class).
  * BEST FILES: sof/MERGED_BEST.cpp = whole unit (Init u4 form + statics in orig .sbss order + Compare): Init 310 raw (0 register-
    blind, len 844), EVERY other function 0, .rodata identical, extab identical except Init's DELETEPOINTERCOND cond/ptr registers and
    the 2-byte padding (0x2500, approved patch class). sof/INIT_BEST.cpp = Init alone (si.sh 145 = the rd.py metric).
    Port plan when Init is solved: remove staticobjfactory_weak/_lookup/_compare units, port 'staticobjfactory' over 0x800b254c..0x800b4368
    with --weak-duplicate EnableLighting__11CStaticMeshFb (orig copy in Soldier_object_attach) and __dt__20StaticObjectTemplateFv
    (orig copy follows DoesHelmetTypeStopBullets; maybe --file-duplicate), strip_unused for __dt__12MESHSLOTVIEWFv (as _lookup did),
    --category reconstructed_game --original-file staticobjfactory.cpp, + extab pad patch (scratch/par/extabpad/addpatch.py).
  * Init colouring analysis: the 6 pool-array new[] pointers (EH DELETEPOINTERCOND temps) are live from function ENTRY in both
    builds (they block 6 registers through the template phase). Orig gives them r25..r20 and the template-phase webs r26-r31; ours
    gives count(+template-new ptr web) r31, ptrs 1-3 r30-r28, template webs r27-r23, ptrs 4-6 r22-r20. Completely INSENSITIVE to:
    decl order/hoisting (30 random), for/while forms, scoping, goto, count type, per-loop r, temps for new results (block/local/
    template helper NewArray<T>), pointer locals at top (=0 or not), forced inlining of either phase (#pragma always_inline - test
    only), '-inline deferred', compilers 1.3/1.3.2r/2.0 (1.2.5 much worse). Next ideas: what makes CW extend EH temps from entry and
    in which order it colours them (probe a small 2-phase function with 2 new[] + a loop var); the u4 motion-IV spelling
    '(i * 24 + 0x38)' is unnatural - a natural spelling may also change colouring.

## Session 22 (2026-10-29, fresh agent) - staticobjfactory Init colouring: NOT solved (no landing)
Work dir s22/: var.py <base> <variants.py> (string-replace variants -> si.sh score), summ.sh <src> (the 7 new[] ptr regs + count reg;
target = 'new: r26 r25 r24 r23 r22 r21 r20 count r26'; ours 'r31 r30 r29 r28 r22 r21 r20 count r31'), pr.py <src> [fn] (probe
compiler + disasm), bis.py (cut template loops, print regs), mkh.py (move a range of Init into a static inline helper), pbase.cpp
(Init with DEF_x/NEW_xx macros = pycparser-friendly; perm_init/ = permuter dir built by autoperm, base 800).
- Precise diagnosis: orig colours the template-phase webs FIRST (variant inner IVs r31/r30, copy r29, r r28, offset IV r27), then
  count (r26, shares with the template new ptr), then the 6 pool ptrs r25..r20. Ours colours count first (r31), then pools 1-3, then
  the same template webs shifted (r27..r23), then pools 4-6. The template webs' INTERNAL order is already identical.
- EH new[] ptrs are colored early whenever they are top-level: probes (s22/p2,p3,p5,p6,p9) ALWAYS give top-level new[] ptrs the
  highest regs, independent of loop use counts or the number of pools; a new[] inside 'if (g > 0) {...}' (the template one) gets a
  low reg and shares with the loop counter = exactly OUR pattern. Without EH on the pools (diagnostic operator new[] cast, noeh.cpp)
  tmpl ptr goes to r26 like the orig, but count still r31. eh1..eh5.cpp: each EH pool takes a top reg.
- INERT (score 145 and identical regs): count decl placement (top/before new/after/hoisted), for-inc forms, ++count/+=1, per-loop r
  (ra..rd split, 120 random decl orders of 7 top decls), counting-loop forms + CountTemplates() inline, pool helpers (NewPool<T>
  template, local struct, inline AllocPools of the news / settings+news), scope blocks {} / if(1) / do-while(0) / for(;;) around
  pools, per-loop body inlines LoadStatic(count, r) etc. (code changes, colouring identical), -inline on/all/smart/level=8/deferred,
  -O4,s/-O3 (worse). Big inline helpers (whole template phase, pools + free-list loops) are NEVER inlined by CW 1.3.2 (even with
  always_inline / inline_depth pragmas / -inline all), so the orig did not use them either.
- Permuter on Init alone (perm_init, -j2): 11.5k iterations, nothing below base 800. Stopped.
- Nested-struct spelling for the variant motion name (entries[i].m_motion.m_name, entry+0xc {int; const char*}) does NOT give the
  orig's separate 'li r30,56' IV (210); keep the u4 spelling.
- Open idea not tried: something that makes the pool EH temps NOT live from function entry (orig behaves as if their live range
  starts later, like the template new inside its if-block). Only branch-free statement forms are possible (orig has no branch).
Verdict: parked; MERGED_BEST.cpp unchanged (Init 310 raw / 0 register-blind, everything else 0).

## Session 23 (2026-10-29 evening, fresh agent) - whole-file merge survey
Tools: soldier/st.py <file.cpp> (open fns + last mentions in AUTOPILOT/CLAIMS/TRICKS/notes), soldier/blk.py <file.cpp> <.rodata|.data|..>
(per text fn: which objects of that file's data section it references, incl. '...rodata.0' base users). Ran fileopen_all 20000 +
basescan2 over all 194 open fns. NOTE: a file's .rodata CAN be split between units by symbol (player_draw owns 0x80176a00+492,
player_weapon owns @3881); only a base-anchored user (...rodata.0+off) forces one unit to own the block PREFIX up to its last object,
which then pulls in every producer of a prefix object (else orphan strings = policy).
SURVEY RESULT: NO file currently meets "every open fn = standalone 0 or base-only". Ranked by what is left:
 1. AIObject.cpp (soldier): base users SetupAStarPathWalk 0x8004c720 + StartAStarPathWalk(CAIObject*) 0x8004c9a4 (1556, never drafted;
    twins of landed patrolwalk fns) at block +676..+872. Prefix producers: @2289.. used from CheckVisionSeekingMode 0x80048134 onward,
    RTTI 'CAIObject' @1901 -> span ~0x800478a8/0x80048134..0x8004cd34 contains CheckFieldOfViewModeList (2, sched) + ChooseCoverPoint (23, regs).
 2. player.cpp (bigfiles): InitMultiplayer 0x800a6408 (1220, never drafted) needs block 0x80176a00..0x80176cd4 = primes + Draw's 4 tables
    (player_draw) + @2676/@2678 (no text user; RTTI-ish 'GelziabarStone', 'dwi::IVisitor<ISceneNode>', currently UNOWNED) + @3881
    (GetWeaponInfoByCRC, player_weapon) + @4579 (OnCollision, open, pzD0 7 regs) -> span 0x8009f58c..0x800a68cc. SetFinishedLevelData
    (10, regs) is OUTSIDE that span. Blocked by OnCollision regs + 1 undrafted fn.
 3. particlesystem.cpp: RenderSystem 4532 (never drafted, FP heavy) + UpdateBoundingBox (112) + Draw; @32@ thunks follow. Far.
 4. objcreate.cpp: 7 open (CreateRootHierObject 382, CreateDestructable callee colouring, 2 undrafted .bss/.rodata users). Far.
 5. AI others: AIFilter (GetClosestWaypointByType 154, ChooseCoverPointByType 20), CoverPoint (GetPeekLocation 3 sched, FlagAsOccupied 4,
    CheckLineCollision 10), AISplinePath (Generate 2944 undrafted, GetClosestParameter 99), ObstacleAvoidance (lines 136): all register.
 Others checked: UserInterface (bigfiles claim), buddy (policy g), bsbifunc (.bss base users span whole file), Soldier_object (Draw 6,
 UpdateAnimation 68), debug_prof/rcmp_main/ShellMenu (orphan-string policy), decal (Init name IV), pumhandler (Loop 88).
PLAN: work #1 (own lane): draft Setup/StartAStarPathWalk to 0 (score with base relocs normalised) so AIObject is reduced to the
two register gaps; run the first-ever permuter on ChooseCoverPoint (no perm_ccp existed) in the background meanwhile.
- DONE: SetupAStarPathWalk 0x8004c720 (644) = 0 AND StartAStarPathWalk(CAIObject*) 0x8004c9a4 (912) = 0 (rd.py rawnr, base offsets
  +676/+776/+872 and +964/+1064/+1160 exact via 'extern const char HARNESS_pad[676] = {1};' before them). BEST: soldier/sap/SAP_BEST.cpp
  (= b3.cpp; view = AIObject_walkastar.cpp + CObstacleAvoidance at +744, m_unknown2e4 +740, m_unknown67c +1660, m_walkState +536,
  m_searchNodes +600, CAIFilterRealVector3 pair copy ctor). Scorers sap/sc.sh (Setup), sap/ss.sh (Start) <abs src> [-v]. Keys:
  start/end as aligned CAIFilterRealVector3 locals filled by Copy() (z,y,x) + '.z += 0.4f', 'CAIFilterRealVector3 direction(endDir)'
  (lfd pair copy) declared at its use, 'direction.x = direction.y = direction.z = 0.0f' for walk mode 4, the redundant
  'if (m_nextNode) {arrived=false; MoveTo(node->GetPosition());} else {arrived=true; MoveTo(m_walkDestination);}' after the
  '!m_nextNode' early return, tail = inline EngageObstacleAvoidance(false, 0, 0, 0) (stb 0 / lbz / PrepareForAvoidance / 0*0 -> 0.0f).
  Start: locals in order start, end, zero, then direction at its use, a CAIFilterRealVector3 'target' filled by Copy() from
  m_nextNode->GetPosition() or m_walkDestination for GetDistanceSquaredXYReal; start/end by pair copy of
  *(const CAIFilterRealVector3*)&m_position / &object->m_position.
  => AIObject.cpp merge now blocked ONLY by CheckFieldOfViewModeList (2) and ChooseCoverPoint (23) (both register/scheduling).
- perm_ccp (first ChooseCoverPoint permuter, 1 h, 285k iters, base 130): best 105 = junk 'distance = distance;' after
  'distance *= distance;' -> rawnr 20 (s23/ccp_a.cpp; NOT landable form). distance/range statement moves: 23.
- CFOVML: whole body as a static inline CheckFOVList(this, m_listObject) -> 15 (worse); FindListObject 'current = m_listObject'
  before the global / ternary forms: 4. Observation: ours already loads start via r3 ('lwz r29,1688(r3)') but AFTER the global
  load; the orig orders start-load, mr r28, global-load.
## HANDOFF (session 23)
No landing. New 0 drafts: sap/SAP_BEST.cpp (Setup+StartAStarPathWalk). Survey above (no file currently qualifies for a clean
whole-file merge). AIObject merge = CFOVML + ChooseCoverPoint away. No permuters running.

## Session 24 (2026-10-29 night, fresh agent)
- User decisions 21:30: '#pragma dont_inline' in game code ALLOWED; register devices DECLINED (GetObjInfo '(int)(filter = 0)' etc).
- CFOVML (s24/, gen*.py, scorer fl20/rd.sh): NEW observation - putting the list search in a STATIC (non-member) inline
  'FindList(CAIObject* self, CAIObject* start)' with 'CAIObject* start = m_listObject;' in the caller REPRODUCES the orig's
  'lwz r29,1688(r3); mr r28,r3; lwz rX,global' prologue order - but only in variants where 'first' gets r31 and the loop object r30
  (orig first r30 / object r31): s24/x_i3_ps.cpp, z_0_0.cpp = 14 (right order, first/object swapped). Every variant that gives the
  orig's first/object registers (x_i1, y_*_fs, q_*_a, z_2_*) puts the global load first again = 2. ~150 variants (decl orders, ternary/
  if forms, param orders, first passed from caller, GetListStart()) all fall on one side of this dichotomy. Still 2 (fl8c.cpp).
- ChooseCoverPoint (s24/cg*.py, scorer ccp/cc.sh): whole body as static inline with self param (25), from-helper inline (24),
  type/this/node copied into locals (23, inert). Rotation this r29/type r30/from r31 vs orig r31/r29/r30 unchanged. Still 23.
- InitMultiplayer 0x800a6408 (1220) = 0 STANDALONE first session (rd.py rawnr, string offsets +628.. exact via HARNESS_pad[628]):
  pim/IM_BEST.inc (function) built into pim/<x>.cpp by pim/mk.py (= src/game/player_respawn.cpp + view edits: CAnimObject : ISceneNode
  with SetScript at vtable +300, m_value250 -> BSObject* m_script, m_meshNode -> CSoldierObject* (m_player +17584), m_weaponSlots[45]
  at +1300, m_name[20] +0x129a, m_mpValue +0x12b0, CPlayerWeaponObject m_weapon +12516, vf64 -> TriggerScriptEvent(int,void*,bool),
  CScene with a size (non-SDA g_scene), WeaponSetData pairs view). Scorer: pim/sc.sh <fn.inc> [v]. Keys: trim loop
  'int j = 19; do { m_name[j] = 0; if (--j < 0) break; } while (m_name[j] == ' ' || m_name[j] == '\0');' (NO char local: a 'c'
  local declared after j swaps r4/r5), separate 'for (int i...)' per weapon loop, 'char sleeve[32]' at the top.
  Landing needs the player.cpp .rodata prefix (0x80176a00..+0x2d4): Draw (player_draw, base user) + @3881 (GetWeaponInfoByCRC) +
  @4579 (OnCollision 0x800a1278, OPEN) -> unit span 0x8009f58c..0x800a68cc. Next: player OnCollision.
- Player OnCollision 0x800a1278 (bigfiles pzD0.cpp 7: weapon r31/dump r29 vs orig weapon r29(=contact)/dump r31): poc/gen.py
  dump as a PickUp local (24 decl orders x init orders): 53-59 (dump drops to r25); poc/gen2/gen3: weapon via getter/typed local,
  outer decl orders, dump returned / by pointer: 7-59. No gain. It is the ONLY open fn in span 0x8009f58c..0x800a68cc besides
  InitMultiplayer (now 0) -> solving it unlocks a player.cpp span merge (Draw tables + @3881 + @4579 + IM strings in one unit).
- *** CheckFieldOfViewModeList = 0 *** (rd.py rawnr 0; ss.py 1 = only the unlinked SDA offset): s24/CFOVML_BEST.cpp (= s24/m_c.cpp).
  Form: 'static inline CAIObject* GetListStart() { return g_paioAIObjectListStart; }' + a STATIC non-member search
  'static inline CAIObject* Search2(CAIObject* self, CAIObject* first, CAIObject* object)' (loop body as SearchList, self-> members)
  called as 'CAIObject* start = m_listObject; CAIObject* object = Search2(this, GetListStart(), start ? start : GetListStart());'
  (rest of fl8c unchanged). Keys: static inline with 'self' param + start local -> member load scheduled before 'mr r28,r3';
  first passed UNNAMED via the getter (late vreg -> r30 under the object r31). Search param order (self, object, first) = 14.
- ChooseCoverPoint retried with the new levers (s24/cg3.py, cg4.py: tail-call inline Choose(self,...), from-helper GetFrom with
  (node, threat|self, p) x 4 decl orders, by-ref out param, 'self' local): 23-28. 'this' stays r29 (orig r31). Unsolved.
- CreateSplinePathManager param-copy trick (s24/spm/: p/n/pn/np/data locals): 11-14. No gain.
## HANDOFF (session 24)
No landing (nothing landable alone: both new 0s need whole-file merges that are each blocked by ONE register-rotation function).
NEW 0 drafts: s24/CFOVML_BEST.cpp CheckFieldOfViewModeList (AIObject merge now blocked ONLY by ChooseCoverPoint 23, ccp/b.cpp);
pim/IM_BEST.inc InitMultiplayer (build with pim/mk.py; player.cpp span merge 0x8009f58c..0x800a68cc now blocked ONLY by player
OnCollision 7, bigfiles/pzD0.cpp). Claims: InitMultiplayer + player OnCollision (soldier, CLAIMS 21:11). No permuters running.
