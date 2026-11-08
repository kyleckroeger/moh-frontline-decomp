# bigfiles notes (agent bigfiles, 2026-10-23)

## CPlayerObject::ConstrainVelocity 0x800a055c (1936, exact gap player_tm..player_bullet) -> NEW unit player_constrain
- best: cv12c.cpp = 1 instruction off ('and r0,r0,r4' operand order of the on-ground bit & onFloor), struct 2 = reloc noise only.
- keys: found = first != end assigned to a bool var (srwi form), reused: found = second != end inside; found = len > eps;
  loop 3 iterates begin itself; 'data = m_collisions.m_data; begin = data;' (permuter); decl order end, begin, data, it, first, second;
  m_velocity zero stores x = y = z = 0 (stores z first); bx/by/bz products (no fnmsubs); Normalize returns length (cross);
  first length via direct sqrtf; CVector3 union with named struct c (CW anonymous-struct bug), this->c in methods.
- permuter: perm_cv2 (base.c needs this-> members and PERM_PRETEND struct defs, see perm_cv/base.c).

## CreateDumpObject 0x8003fc2c (1640, exact gap objcreate_dump..objcreate_playerscript) -> append to objcreate_dump
- cdo5.cpp: instruction stream exact except callee-saved colouring (~50 lines) + health compare operand order.
- cdoP.cpp = macro-wrapped version for the permuter (perm_cdo).

## LANDED
- ecb70b82 player_constrain (ConstrainVelocity 1936) ; 2c311a8d player_mmg (SetUsingMountedMachineGun 3424)

## BSFileReadClass 0x80035aa8 (3768) - from scratch/bsf/b8.cpp (80 struct)
- bsf1_0.cpp: 'int n = lists; for (k < n)' -> 8x unroll matches: struct 13 (from 80)
- bsf2a.cpp: final parent-copy loop reuses a function-scope 'unsigned int j' -> struct 12 (relocs + one missing callee reg r23 + count copy mr + li 104 order)
- bsf4_0.cpp ('s <= count - 1' from permuter hint) raw 299 (from 426) but struct 18 and frame +16
- perm_bsf (base bsfP.cpp = bsf2a with C casts) running
## CreateDumpObject: parked at cdo5.cpp (instruction-exact, ~55 lines callee-saved colouring); objcreate.cpp is '-inline auto,deferred' (no effect)

## 2026-10-23 night
- LANDED be2f5005 scene_sort += sort_ (2556).
- READY TO LAND: TibPolygon_process.cpp (this dir) = TibPolyDraw + TibPolyInit + Process, all 0 (relocs only); extend unit TibPolygon_process back to 0x8008bcd4, --pool .sdata2 (strings + floats).
- PATH_event (path_stop.c, 1884): pev4.cpp = 93 raw / struct = relocs + 1 zero-coalesce ('mr r4,r3' j-offset from latency zero) + colouring. PATH_pause needs 3x '(void)0;' (user-approved allowance) to stay out of line. perm_pev running (base pev4).
- quick_sort_ (scene, 804): qs3.cpp struct 0 / raw 37 (insertion loop i/prev colouring); inline iter_swap for depth-2 swap temps.
- BSFileReadClass: bsf2a.cpp struct 12 (r23 extra callee reg in orig; count reg).
- CreateDumpObject parked cdo5.cpp.
- LANDED 9686171b TibPolygon_process += TibPolyDraw/TibPolyInit.
- PATH_event: pev5.cpp raw 75 (permuter hint: re-read pfstate->tracks[j] in the leader test). Only real diff left = zero coalesce (0x150 mr vs li) + colouring.
- CPartBin::Link (dmesh_bins, 1492): pl10.cpp struct 67 / pl11a struct 61 - orig keeps THREE 16-byte copies of each light position (72,56,88) before GXInitLightColor; not reproduced yet.
- Render__7CSprite (876, sprite.cpp, before sprite_draw): claimed, not started.
- LANDED 86a56411 sprite_draw += CSprite::Link + Render.
- CFont::Link (font_render prepend, 1812): fl5b.cpp struct 2 (= relocs) / raw 72 callee colouring; decl order has NO effect; perm_fl running. tmpl_font.json adds include/dolphin-sdk.
- SoundSchedule::Update (soundschedule_sound prepend, 1864): ssu1.cpp struct 125 - views/flow right; slot/copy layout of GetAzimuth/FindClosestPlayer by-value args unresolved (orig: inline dead param copy at 24 + both call-arg copies made BEFORE the FindClosestPlayer call).
- CMSHMatBin/CCPTMatBin Link: function-static guards (init$ / CurIndexType / prevVertFormat) -> skipped.
- SoundSchedule::Update: ssu4a.cpp (ISceneNode::Position() by-value inline in the MP azimuth path) reproduces the MP azimuth copy STRUCTURE (dead 24 + two pre-call copies) but slots differ; ssu1/ssu4b best score (struct 124).
- CParticleSystem::UpdateBoundingBox (2948, before particlesystem_moving): ubb17.cpp struct 4 (2 relocs + 1 sched) / raw 133 FPR colouring (part 2). KEYS: lo/hi extremes memory-resident via 'CVector3* plo = &lo;' (address taken, unused) + Turn writes through plo->; products kept separate (product locals then +=, no fmadds); vector Scale/+= members for p0/p1; ternary min/max (left-to-right loads). hill_ubb2.py (decl order of part-2 floats) running -> ubb18.cpp.
- CLightVolumeManager::GetVolume (956, lightvolman_volumes prepend): gv6/gv7 struct 4 (relocs only), raw = FPR numbering of the direction lerp only. Keys: 'if (m_volumes[0]) return m_volumes[0]; return Default();', truthiness 'if (blend->intensity)' (fcmpu f,zero), lerp as separate -,*=,+= statements, inline BlendDirection(float* dst, const float* src, t). gvperm3.py running (decl orders).
- AllocateObject (CAnimObjectFactory, 0x80095bc8, 1368, AnimObjFactory_head tmpl): aoa12.cpp struct 18 / raw 41. Remaining = only the
  placement-new temp: orig `beq; mr r3,r26; bl ct; mr r26,r3; mr r29,r26` per case (temp kept in memory's reg), ours `mr r3,r26; beq; bl; mr r29,r3`.
  Plus "head" string li 0 vs addi r2 (pool noise). KEYS found: single CVector3 local reused with inline Set() (one stack slot 8),
  `if (!v)` for y/z zero checks, z/y/x load order, real CAnimated members (m_animated at 48, m_head at 12656) instead of AT() casts,
  soldier block as macro with `CSoldierObject* object = (CSoldierObject*)object0` (cast once), `if (memory) {...} return object;`.
  Tried for the temp: memory=new/object=memory, void* memory, Construct<T> templates, static/reinterpret casts, decl orders, discard result,
  placement delete (adds EH, worse), base-class conversions, compilers 1.3/2.0 (same), 1.2.5 (len matches but rest differs).
- ubb23.cpp: UpdateBoundingBox raw 112 / struct 4 (sx/sy/sz via inline Mul(t2, sa.d.v, i) helper; hint from perm_ubb)
- PeekLeftRight (player_crouch tmpl, 0x8009ce84-0x8009d0f8): plr1_dir_local.cpp struct 3 (relocs only) / raw 44 FPR numbering.
  KEY: `CVector3 dir; CVector3 local;` declared at the top (named locals take the high slots in declaration order; right/pos copies are temps
  allocated after) -> all stack slots match. perm_plr running (base 220 -> 155 junk: a dead later use of oz changes FPR colouring).
- SetFinishedLevelData (0x8009d470-0x8009d5f8): sfl0.cpp 18 struct (this/i swap); decl perms, by-ref i, explicit weapons ptr, loop in body: no gain.
- DrawTextCenteredF (tmpl_font.json, 0x8007cbb0-0x8007cdb0): dtc0.cpp struct 9 (3 commutative operand orders + right/0.5 load order); operand order is
  canonicalised (source order, helper inlines, float decl perms have no effect).

## HANDOFF (2026-10-24, context reset) -- bigfiles
Lock NOT held, no permuters running (perm_plr killed after 8 h: only junk 150/155; perm_ubb killed).
Landings this run: ecb70b82, 2c311a8d, be2f5005, 9686171b, 86a56411, 37942399 (see AUTOPILOT log).

READY TO LAND (exact, raw 0 / struct 0):
- CPropertyAnimLight::BeginUpdate(float) 0x800779e0-0x80077bd4 (500 B): bu5.cpp (self-contained). Scored with
  TM=config/GMFE69/light_propertyanim_dtor.json. Goes between light_animmanager (ends 0x800779e0) and light_propertyanim_dtor
  (starts 0x80077bd4): land as NEW unit (e.g. light_propertyanim_update, --category reconstructed_game, --like light_propertyanim_dtor,
  --pool .sdata2 for the u8->float magic double @ -30520(r2)), or prepend to light_propertyanim_dtor. Check the record's file-index.
  KEYS: `char m_reverse : 1` bitfield (signed test), ctr loop `for (int i = frames; i != 0; i--)`, wrap loop
  `m_frame = m_frame - m_data->frameCount`, colour bytes into float locals r,g,b then intensity then a, `x *= intensity` then stores,
  and `float intensity;` HOISTED to the function top (fixes FPR numbering).

Best WIPs (struct = register-blind diff, raw = full):
- AllocateObject (CAnimObjectFactory) 0x80095bc8-0x80096120, TM AnimObjFactory_head.json: aoa12.cpp struct 18/raw 41. Only the placement-new
  temp colouring left (orig keeps new temp in memory's reg r26: `beq; mr r3,r26; bl ct; mr r26,r3; mr r29,r26`). ~20 variants tried (see above).
- PeekLeftRight 0x8009ce84-0x8009d0f8, TM player_crouch.json: plr1_dir_local.cpp struct 3 (relocs)/raw 44, FPR numbering only.
- UpdateBoundingBox 0x8007fbe8-0x8008076c, TM particlesystem_moving.json: ubb23.cpp struct 4/raw 112.
- DrawTextCenteredF 0x8007cbb0-0x8007cdb0, TM tmpl_font.json: dtc0.cpp struct 9 (commutative operand order, canonicalised).
- SetFinishedLevelData 0x8009d470-0x8009d5f8: sfl0.cpp struct 18 (this/i swap).
- Older: pev5 (PATH_event raw 75), qs3 (quick_sort_ struct 0/raw 37), bsf2a (BSFileReadClass struct 12), cdo5 (CreateDumpObject),
  pl11a (CPartBin::Link 61), fl5b (CFont::Link struct 2/raw 72), ssu4a (SoundSchedule::Update ~124).
Current claims (bigfiles): AllocateObject, PeekLeftRight, SetFinishedLevelData, DrawTextCenteredF, BeginUpdate, UpdateBoundingBox, CFont::Link,
  SoundSchedule::Update, CMSHMatBin::Link (skipped, static guard), CStaticMesh::Draw/DrawNode (skipped, bss base).
NEW TRICK: hoisting a plain `float x;` declaration to the function top changes FPR colouring (BeginUpdate fixed this way). Note: the env vars
  TM/FN/A/B must be exported in the same shell call as s.sh (a missing TM silently scores against player_tm -> "len N 0").

## 2026-10-24 (successor run)
- LANDED 914f62de NEW light_propertyanim_update (BeginUpdate).
- AllocateObject SOLVED: aoa35.cpp raw 4/struct 4 (pool relocs only). Key: `if (m_free) { CAnimObject* memory = m_free; m_free = m_free->m_next; object = memory; ...`
  (test m_free directly, memory declared INSIDE the if -> the placement-new temp coalesces into memory's reg), sprites base from `object` not `weapon`,
  `float x, y, z;` hoisted then z,y,x assigned. Merged unit source = AnimObjFactory_head.cpp (append to AnimObjFactory_head).
- LANDED 658fe04a AnimObjFactory_head += AllocateObject.
- DrawTextCenteredF: dtc32.cpp = 2 real lines (from 4): 'width / 2.0f' and 'right / 2.0f' (division by 2 -> x*0.5 keeps source operand order; '*0.5f' is canonicalised const-first).
  Left: both '(float)GetWidth() * var' fmuls come out (var, conv); orig (conv, var). CW canonicalises leaf-first; 'w *= v' keeps order (dtc41) but then the conv is width's own
  reg (viewWidth f30 not f29); named fw (dtc39) keeps order but the mul gets forward-substituted (+f27, frame +16); inline helpers add frame +16.
- SetFinishedLevelData sfl0: struct 18 = relocs only; raw = this r29/i r28 swap (orig this r28 shares with base). Base-class holder, self copies, int i at top, for-scope: no change.
- DrawTextCentered (0x8007cdb0-0x8007cf50, tmpl_font.json, claimed): dtxp/center_scale_viewWidth_viewX_width.cpp 8 raw lines (dtx1 = decl left,top,right,bottom
  for the slots). Same conv*var operand-order issue, plus orig keeps width separate from viewWidth (width f28 = center's reg, viewWidth f30 = scale's reg), DrawText scaleX/scaleY swapped.
- particlesystem_create.cpp: Create(void*) + Create(ul) both exact (pc40/pc74). Keys: placement new into the pool Alloc() result with 'if (ps) ps = new (ps) T(..); else ps = 0;', the whole setup body in ONE inline helper (stack slots), the Create(ul) search loop in an inline helper (one result reg), 'CParticleSystem* ps' (base type!) fixed the r30/r31 swap.
- LANDED 8e9163ce NEW particlesystem_create (Create void*/ul). Next: UpdateSystem (us0.cpp = scratch/game wip, struct 10/raw 112, this/count swap) could prepend to particlesystem_create.
- UpdateSystem: us7.cpp struct 5 (relocs) / raw 38 = only this r31 vs r30 (count) swap. Keys: emission block in static inline Emit(ps, dt, last), 'm_ticks - (last = m_emitTicks)', '(float)(60.0f * life)' stops the fmadds. Tried: self copy, member/ref helpers, count decl/forms, seed helper, whole-body inline.
- CPropertyAnimLight ctor: merged unit source light_propertyanim_dtor.cpp (append to light_propertyanim_dtor), relocs only. Keys: mem-initialiser for the short bitfield (halfword write; assignment gives stb), 'CVector3 dir; dir = m_matrix.GetRow(1);' with GetRow '{ CVector3 r = row; return r; }' (two copies, read first), '/ 16.0f'.
- LANDED 91581a9a light_propertyanim_dtor += CPropertyAnimLight ctor.
- Scroll (popup): pop0.cpp raw 8/struct 8 = only the scheduling of the inlined Reset tail (as parked before); explicit body + 250-step statement-order hill climb: no change.
- CScene::Draw (0x800da67c, append to scene_closestplayer, claimed): sd4.cpp struct 9 / raw 46. Left: orig computes camera+192 (2nd CDrawContext ctor arg) into r29 BEFORE the screen/viewport calls (one more callee reg, frame 416 vs 432); the SModCommand loop copies 16 bytes (w3 loaded before w2) and has 'nop' after __ptmf_scall (we get the nop only when calling through cmd->m_fn / a ptmf local, not through a struct copy).
- SetFinishedLevelData: more tries (inline Finish helper, loop in helper, self copies): this/i swap persists.
- __dt__25CParticleSystemTextureBinFv: ptb0.cpp EXACT (relocs only) -> NEW unit particlesystem_texturebin_dtor (pending lock). Key: bin declares Render override before its dtor (table not emitted), CRenderBin ~ inline (table UND).
- TOOL: hoist.py <src> <tmpl> <fn> <start> <end> <marker-line> <iters> "T name"... : hoists 'T name =' decls after the marker line, hill-climbs their order (score raw+10*struct, +1000 on length change).
- EndDraw: ed2.cpp struct 5 (inline SetColor(CColor) by value, g,r,b,a stores) — load order/data reg left. CFont::Link hoist hill-climb: 91 -> 82 (fl5b_hoist.cpp).
- LANDED d46f0241 NEW particlesystem_texturebin_dtor.
- OnCollision (player, 0x800a1278, prepend to player_health, claimed): poc15.cpp raw 41/struct 6. Keys: SContact built field by field (object, depth, then if/else normal with component negation), push_back(T x) by value, 'other' from a repeated compare, weapon=0 then health=0. Left: shared-zero init placement + callee colouring (hoist.py running -> poc15_hoist.cpp).
- OnCollision: poc15_hoist.cpp raw 6/struct 6 = relocs + ONE scheduling diff: orig 'li r30,0; mr r3,r27; mr r29,r30' (weapon = shared zero of health) vs ours two li. Tried: chained x=y=0 (int typed weapon too), orders, casts, init placement.
- LANDED f1bd6039 CPropertyParticleSystem ctor (append to particlesystem_cpropertyparticlesystem_dtor).
- UpdateSystem: us23.cpp raw 7/struct 7 = relocs + ONE swap (first virtual call in Emit: orig 'mr r3,r30; lwz r12,28(r30)', ours reversed). Key: Emit(void* p, ...) { CParticleSystem* ps = (CParticleSystem*)p; } fixed the this/count reg swap (typed param/self copies/templates did not). Tried: cast forms, param order, CRenderBin* base, hoisted decls.

## HANDOFF (2026-10-24 ~13:40, successor run end) -- bigfiles
Lock released, no background jobs. Landings this run: 914f62de (BeginUpdate NEW unit), 658fe04a (AllocateObject), 8e9163ce (particlesystem_create NEW:
Create void*/ul), 91581a9a (CPropertyAnimLight ctor), d46f0241 (CParticleSystemTextureBin dtor NEW), f1bd6039 (CPropertyParticleSystem ctor).
Best WIPs (1-2 line diffs):
- UpdateSystem 0x800807e0-0x80080b24 (TM particlesystem_texture.json, prepend to particlesystem_create): us23.cpp 1 instruction swap.
- OnCollision (player) 0x800a1278-0x800a1580 (TM player_health.json, prepend to player_health): poc15_hoist.cpp 1 scheduling diff (shared-zero li/mr).
- DrawTextCenteredF dtc32.cpp 2 lines (conv*var operand order); DrawTextCentered dtxp/center_scale_viewWidth_viewX_width.cpp 8.
- Scroll (popup) pop0.cpp 8 (inlined Reset scheduling); EndDraw ed2.cpp struct 5; CScene::Draw sd4.cpp struct 9; CFont::Link fl5b_hoist.cpp raw 62/struct 2.
- SetFinishedLevelData sfl0 (this/i swap, void* trick did not help), PeekLeftRight plr1 (FPR).

## 2026-10-25 (third run) -- bigfiles
- LANDED f1f3e506 ShellMenu_levelstats += GetCumulativeStats (944). KEY: shared-zero li/mr = locals of a static inline (MostHit(const unsigned int*)) + read temp 'unsigned int v = values[j]'.
  Merged unit source keeps the shared stats table as m_stats (+0x1914).
- LANDED 749d5e63 NEW UserInterface_score: DrawScore (1428) first compile (Ghidra + weaponname views; 'static char buffer[80] = "";' -> .data).
- CalculateLevelStats (0x800e6374, append to ShellMenu_levelstats): slm.cpp = merged unit + CLS using MostHit -> raw 11 / struct 2: ONLY location r30 vs orig r29
  (orig reuses the loop-1 pointer reg). Tried decl perms, call forms, first loop inline (worse 26-28), types. perm_slm (base 45) 20k iters no gain, killed.
- OnCollision (player): pzD0.cpp raw 9 / struct 2 (shared zero FIXED via inline PickUp(self, other, void*& dump) with own health/weapon locals);
  left: weapon r31 vs orig r29 and dump r29 vs r31 (swap). permz5/6 decl+ref-placement brute force: no. (Function-level poc15_hoist = 6 but li/li.)
- MoveStats (smm_*.cpp): smm_j ('int* active' pointer in the for-increment 'i++, active++, dest++') gets the orig IV order but active becomes an addi'd
  pointer (orig: SR of this-based m_activeWindows[i]); smm_g/h/p raw 2-4. Parked.
- DrawViewportEdge (0x800be108, prepend to UserInterface_score): ds/dve3.cpp raw 50 / struct 31 (from 98, length now exact). Keys: flags
  'bool top = g_NumPlayers == 2 ? index == 1 ? 1 : 0 : (bool)(index == 2 || index == 3) != 0;' (double !=0 in the else path), team colour in an inline
  'CColor TeamColor(int team)' with own locals 'unsigned char b; g; r; r = 0; g = 0; b = 0;' + 'return CColor(r, g, b, 255)' (shared zero li/mr/mr), copied per vertex with
  CColor member-wise operator=. Left: g_NumPlayers/index volatile swap (r4/r5) through the flags. perm_dve running (base dve3).
- LANDED 66f33770 NEW pumhandler_add (AddPopUpMessage 1504): FindSlot inline helper fixed the param regs (see TRICKS).
- Pause_update (PauseUpdate 1328) landing; PauseDraw MATCHED: ds/Pause_unpause.cpp = Pause_unpause unit + PauseDraw (objective loop in inline
  member DrawObjectives(), 'SetColor(CColor(255,255,255,255))' temp) -> append to Pause_unpause after Pause_update lands.
- CreateDestructable: 9 explicit DebugMsg calls (orig unrolled) -> ds/cd1.cpp raw 101/struct 44 = callee colouring only (params low in ours);
  inline splits (NewDestructable, Place, AttachScript, NewBox, trigger->core) no gain; SetBoxes/whole-body inlines refuse to inline.
- MemoryCardLoadGame (ds/mlg0.cpp raw 74/struct 10 with cardquery tmpl): this/slot/file rotation; OpenFile inline forms no gain.
- DrawViewportEdge permuter: autoperm base.c compile fails (PERM_PRETEND line with my typedef); fix needed before running.
- LANDED 94097ada NEW Pause_update (PauseUpdate 1328), f5b405c0 Pause_unpause += PauseDraw (436).
- quick_sort_ (scene, 804): ds/qs6_ahi.cpp raw 27 / struct 0 (from qs3 37): bubble loop 'I prev = i - 1;' inside the for, insertion_sort_'s
  second loop through an inline unguarded_linear_insert_(I last, T value, C comp) helper, third loop inline as before. Left: the else-branch
  (second inlined insertion copy + third loop) callee regs r27/r28/r29 rotated.
- Tried without gain: CalculateLevelStats (member/Evaluate inlines), SetFinishedLevelData (SaveWeaponAmmo inline), Add_MPPlayerKilled
  (AddWeaponKill inline, AddStat for victims), MemoryCardLoadGame.
- CUserMessageTable::Load (0x800b85bc, append to pumhandler_umt_unload): ds/umt3_Bh0.cpp raw 22 / struct 7 (from 102). KEYS: count via the
  ChangeEndian(unsigned short&) -> ChangeEndian(short&) bytes[2] overloads (the two stack slots), 'int pos = 2;' declared before the count,
  for-increment 'i++, pos += 2'. Left: text/indexFile(pos) r29/r31 swap, 'li r7,0; mr r4,r7' shared zero for '*end = 0', 'end + (extra + 1)'
  association (CW canonicalises every spelling tried).
- CAnimObject::DetectGround (0x8009491c, 892, untried): ds/dgs0.cpp EXACT as a separate fragment with weapon_bullet's views (CVector3 =
  union d {pair, v} with IMPLICIT copy, CLine3(CVector3, CVector3) by value + inline dtor -> --weak-duplicate __dt__6CLine3Fv). Merging into
  anim_obj_update failed: that unit's CVector3 has a user pair-copy ctor, which adds a 16-byte temp copy of the by-value arg.
- static_obj BeginAnimMotionUpdate: tried copy-ctor/operator= removal variants: worse. (A union CVector3 needs renaming .x -> .d.v[0]; untried.)
- LANDED 3a85e6fd NEW anim_obj_ground (DetectGround 892). devicemgr += CGCDevice::UpdateActuators (784, first compile) landing.
- CGCDevice::Update (0x800845ec, 876): ds/devicemgr_u.cpp EXACT (relocs only) = devicemgr + Update; keys: switch(err) case 0/-3, -1, default;
  'else if (y != 0.0f) ratio = ...; else ratio = 1.0f;' (branch sense), 'x *= scale' (double scale), 'm_cleared[i] = 128.0f + x' and
  '= 1.7 * pad->triggerL' without (int) casts. Land after UpdateActuators.
- OnCollision: coordinator's shared-zero tip already applied (pzD0 has the li/mr); left weapon/dump r29/r31 swap; typing weapon void*/health
  long/uint/short: no change.
- LANDED 0914aa67 devicemgr += CGCDevice::Update; bsbifunc_soundoffset += PlaySoundFromTriggerOrObject (psto_e.cpp, first compile + 2 fixes) landing.
- Gameloop_Loop (0x8007a810, mainloop_game tmpl, new unit): ds/glWIN.cpp raw 15 / struct 10 (from 40/26). Keys: g_scene view sized 328 (was SDA!),
  'CScene* scene = &g_scene;' local + hoisted decls camera, matrix (const CMatrix* = &player->m_matrix, CSE'd +192), i, player. Left: the
  profile-bin base / scene-copy r30/r31 swap (orig: bin base r31 then scene r30 + copy r31).
- ISirenStartup (ds/is0.cpp raw 40/struct 26): param colouring; inline-helper splits (SetParams/Start/MemSize/Init) no change.
- ResetShell (ds/rs0.cpp raw 27/struct 11): 0/1 constant regs; SetGameType variants no gain.
- CUserMessageTable::Load: ds/umt6.cpp raw 13 (decls 'int pos; unsigned char* index; char* text;' hoisted fix the callee regs). Left: shared-zero 'li r7,0; mr r4,r7' (i / '*end = 0'), entry r6/r9 swap, '(extra + 1)' association.
- Gameloop_Loop MATCHED: ds/mainloop_loop.cpp (gl5 + header) -> NEW unit mainloop_loop (--like mainloop_game, mainloop.cpp idx 3848, --pool .sdata2), landing.
- BSEventProcessImmediateEvents: ds/bpe2_b.cpp raw 54 / len 138 vs 137 (run test flipped 'if (stateId != event) run = flags; else run = true',
  find inline iterating through *handler). Inline find's three 0xFFFF return sites + found path still differ. Parked.
- TLT_LoadFromRepository: ds/tr1.cpp ('name = colon ? (DebugMsg(...), colon + 1) : name;' gives the bne/b shape; len now 143) raw 75 - hash_map find
  part (iterator kept in a reg in orig, ours a stack iterator object) differs. Parked.

## HANDOFF (2026-10-25 ~11:50, third run end) -- bigfiles
Landings this run: f1f3e506 GetCumulativeStats, 749d5e63 DrawScore (NEW UserInterface_score), 66f33770 AddPopUpMessage (NEW pumhandler_add),
94097ada PauseUpdate (NEW Pause_update), f5b405c0 PauseDraw, 3a85e6fd DetectGround (NEW anim_obj_ground), 556462db CGCDevice::UpdateActuators,
0914aa67 CGCDevice::Update, 88cd2d00 PlaySoundFromTriggerOrObject, + a20af8e9 Gameloop_Loop (NEW mainloop_loop). (Old note: if not
landed, it is EXACT and ready: port_unit mainloop_loop src/game/mainloop_loop.cpp --like mainloop_game --category reconstructed_game --pool .sdata2
--original-file mainloop.cpp --file-index 3848, then addunit).
Best WIPs: CalculateLevelStats slm.cpp raw 11 (1 reg); OnCollision pzD0.cpp raw 9 (weapon/dump swap; permuter 17k iters nothing); Load ds/umt6.cpp raw 13;
DrawViewportEdge ds/dve3.cpp raw 50 (index/n volatile swap); quick_sort_ ds/qs6_ahi.cpp raw 27; MoveStats smm_h raw 2; UpdateSystem us23 1 swap;
DrawTextCenteredF dtc32 2 lines; CGCDevice done; EndDraw: ds/edgen.py brute force (decl x assign order of r,g,b,a temps) results in ds/edgen.log.
Exhausted/insensitive this run: CreateDestructable/CreateRootHierObject (hoisting, inline splits), ISirenStartup, ResetShell, SetFinishedLevelData,
Add_MPPlayerKilled, MemoryCardLoadGame, static_obj BeginAnimMotionUpdate, BSEventProcessImmediateEvents, TLT_LoadFromRepository.
No background jobs left except possibly ds/edgen.py (timeout 1200 s).
- TLT_LoadFileFrom*BigFile family: orig passes the colon pointer as DebugMsg's 3rd arg (r5) and computes name = colon + 1 before the call
  ('DebugMsg(fmt, "", colon)'); ds/tsb_c.cpp raw 8 (extra mr r5 + bne/b shape still differs). The ternary form that gave the bne/b head
  in TLT_LoadFromRepository (toilet.json, non-deferred) does not in the deferred toilet_files template. Parked.

## 2026-10-25 (fourth run) -- bigfiles
- LANDED a2ebe550 NEW projectilebullet_draw (CProjectileBullet::Draw 528, draft dir pbd/). Keys in AUTOPILOT log line.

## 2026-10-25 (fifth run, evening) -- bigfiles
- CAnimMorph::Draw (amd/amd_c1.cpp struct 9 = relocs + ONE store-order diff: left.zw store 144 must come after the operator= return temp 24/32).
  amd2/gen*.py: ~45 statement/operator=/Assign/inline variants, none fix it (p7 'right first' moves 144 but breaks others). Parked.
- LANDED b9a10f83 fader_control += CFaderControl::Update (draft fc/fader_control.cpp; fc/hB_hoist.cpp). Unit now RTTI on (owns vtable/RTTI).
- TOOL t5/nb.py <addr>...: neighbouring units of an address.
- LANDED 69e74f86 scene_rmextents += AddNodeExtents (ane/q4.cpp; placement-new list_node ctor gives the word copy). TRICK added to TRICKS.md.
- CheckIfMovieAndMedalNeedsToBeUnlocked (0x800e1914, 776, claimed; prepend to ShellMenu_specialmedals; merged draft cmm/ShellMenu_specialmedals.cpp
  emits in the right order). Best: cmm/i2.cpp raw 6 (2 relocs = jump table + ONE init order: 'addi r28,r1,20' (stars ptr) must precede
  'addi r29,r1,8'); cmm/g3.cpp raw 4 = relocs + IV increment order. Keys: stars copy as a 4-iteration loop (unrolled, separate index maths),
  complete[] zeroed by 'for (j..4) complete[3 - j] = false', count = stages[(unsigned char)m_mission] computed right after the table,
  'if (medals[m-1] != 1) switch(m_mission) {bitfields} else unlock = false;', 'int index = m_mission - 1;' for the two flag bytes (+5116).
  Jump table .data 0x8018a418 (44 B) is unowned (between ShellMenu_unlock and ShellMenu_stats). perm_cmm2 (base i2) running.
- LANDED de8477f7 ShellMenu_specialmedals += CheckIfMovieAndMedalNeedsToBeUnlocked (cmm/p3.cpp; perm_cmm3 found the pointer-copy).
- CPlayerWeaponObject::Draw (0x800a80b4-0x800a8630, 1404, claimed, never tried before): pwd/i1.cpp = 4 real lines (FPR: width conv f28 / top f28 in orig, ours f29), rest relocs.
  Keys: 'clip.y = g_NumPlayers == 2 ? 0.5f * clip.y : clip.y;' (ternary gives the late store + frsp), tan results: 'float aspect = tan(...)' local
  (keeps the camera near/far loads before the calls), branch 'if (g_NumPlayers != 2)', context.m_scale.Set(x,y,z), '(unsigned int)m_reserve == 0'.
  Landing: NEW unit (between player_weapon_selectable and _object_update), --pool .sdata2 .rodata (@733 vertex table 0x80176d78 unowned),
  g_Cam2clip / g_fNearMultiplier local externals; has extab. perm_pwd (harness i1h.cpp, base 35) - needs C89 loop decls (DeclList errors).
- LANDED cded83da NEW animated_draw (CAnimated::Draw 1288; draft adr/s1.cpp).
- pwd (CPlayerWeaponObject::Draw) parked at 4 FPR lines (width/top f28); perm_pwd mostly failing iterations (harness). Tried: decl hoists, y/x order, copies, helpers, x*=/y*= forms.
- CCompartment::DrawNode (0x80075024-0x800756cc, 1704, claimed, never tried): cdn/best.cpp raw 80 / struct 20 (struct = relocs), length exact.
  Left: FP colouring only, in the frustum test (IsOutside inline, bool, 'if (!planes) return false;' gives the bne/b) and the light test
  (Touches inline). Keys: frustum test + light test as static inlines (stack slots: center 24, p 8), 'unsigned long bit = 1 << i' with
  'FrustumPlaneView* plane;' declared before it (GPRs), item loops 'int i;' before the item pointer + material local, SPR stack Alloc inline,
  LinkLights inline (twice), hoisted float decls (hoist.py). Template compartment.json; NEW unit or prepend to compartment_fastvec_tri; needs
  g_LightPool/g_SPRStack/g_pFrustum as local externals.
- LANDED 85f2e2e9 NEW compartment_nodeshadows (DrawNodeShadows 1600; dns/n4.cpp). compartment.cpp is a deferred file (see log). Other recursive 'ladder' functions may be plain recursion that CW inlines 2 levels.
- LANDED fee75de3 UpdateLightingParams (ulp/l1.cpp).
- TLT_LoadFromRepository (coordinator tip applied): tlr/s3.cpp head now matches (const char* n; if (!colon) n = name; else { n = colon; n++; DebugMsg(fmt, ""); } name = n;) + path[260]; tlr/t2.cpp (iterator = value_type*, find with 'last' local) struct 25 / raw 70, len 134 vs 143: the orig recomputes m_data + m_size in each place (end() in the count==0 path, 'last' in the search, end() again in the caller's '!=' with a fresh &g_Repository) where CW CSEs ours. Parked.
- LANDED 282f3a06 font_render += CFont::Link (flk/g3.cpp).

## HANDOFF (2026-10-25 evening, fifth run end) -- bigfiles
Lock released; no permuters running. Landings: b9a10f83 (CFaderControl::Update), 69e74f86 (CScene::AddNodeExtents), de8477f7 (CheckIfMovieAndMedal),
cded83da (NEW animated_draw: CAnimated::Draw), 85f2e2e9 (NEW compartment_nodeshadows), fee75de3 (UpdateLightingParams), 282f3a06 (CFont::Link).
Best WIPs: CPlayerWeaponObject::Draw pwd/i1.cpp 4 FPR lines (never landed; NEW unit plan above); CCompartment::DrawNode cdn/best.cpp raw 80 (FP only);
CAnimMorph::Draw amd/amd_c1.cpp 1 store order; TLT_LoadFromRepository tlr/s3.cpp (head fixed) / t2.cpp (hash CSE); CStaticObject::OnCollision soc n7 2 lines.
Tried without gain this run: SweepExtents, SetFinishedLevelData, CalculateLevelStats, UpdateSystem, UISExecuteFnc, CTexture::Set palette knot, OnCollision(player), DrawTextCenteredF.
Claims still mine: pwd, DrawNode(compartment), CAnimMorph Draw, TLT_LoadFromRepository, OnCollision(static), SweepExtents, CreateWeaponByCRC, UISExecuteFnc.

## 2026-10-25 (sixth run, night) -- bigfiles
- TOOL t6/rd.py <tmpl> <src> <fn> <start> <end> [v]: raw diff with reloc/SDA fields normalised but REGISTERS kept (struct-blind ss.py hides FPR diffs).
- LANDED c382731c NEW player_weapon_draw (pwd2/player_weapon_draw.cpp). Key: decls of FAR-AWAY later-branch locals (far/near/aspect in the
  perspective if/else) hoisted to the function top fixed the FLASH-block colouring (width/top f28). Brute force pwd2/_c*.cpp.
- LANDED 21727d81 NEW scene_update (CScene::Update 2848, draft su/scene_update.cpp; keys in AUTOPILOT log line).
- CCompartment::DrawNode: operand order of 'fabs(..)*e' follows vreg order (source order irrelevant); a0..a2 locals flip it but need f31 (len+4). Decl hill (cdn2/hill_k.log, hill_t.log) no gain. TLT_LoadFromRepository: hash find variants (tlr2/a-e) no gain.
- LANDED df9bcec7 publisher_erase, cee2ccff PathNodeList_erase (void* hole trick; assign-copy). PublishEvent pub/p1.cpp rd 4 (callback load early => rotation 45); q2.cpp (SItem::Has/Notify inlines) rd 8.
- CScene::CScene (0x800dc8c4, 900, NEW/append scene_dtor): sct/c.cpp rd 88 = ONLY register classes (orig: constants 0/1/2 in callee r27-r29, &head ptrs volatile r11/r6; ours the reverse). Strings need dwi_prime_list in the TU (rodata offset 108) and the 'Scene exlusions' typo; SExtent needs an empty user ctor (uninit temp copy). list_node/ctor-body variants no change.
- IObserver/ISubject dtors (0x800d80e0, 0x800d81a0; extend unit 'publisher' with RTTI on, draft obs/a4.cpp): CODE EXACT (needs '#pragma dont_inline on/reset' around the weak NotifyObservers so ~ISubject calls it). BLOCKED on .data order: orig = ISubject bases, IObserver bases, __vt__9IObserver, __vt__8ISubject (names @248 IObserver created first); CW gives bases+vt per class in declaration order. --pool .data fails (pool items with relocations). Also RTTI .sdata 0x8034e2a0-b8 + names .rodata 0x80178104 are unowned publisher.cpp data.
- PassMP_PrefWeaponNameToIStudio (0x800de6b0, 596, append to ShellMenu_sku): pmw/ShellMenu_sku.cpp CODE EXACT (rd 13 = string relocs only). BLOCKED: the long strings ('Stiel Gran'...) come from ShellMenu.cpp's ...rodata.0 BASE (r31 = 0x801783c8, offsets 1648..1720) -> the (e) block rule; needs the file's string block from its start. Keys if unblocked: 'if (preferred >= 0) {...} else strcpy(None)', decl order 'int preferred = -1; int most = 0;', inline-free ternary 'preferred + 407 < m_messages.m_count ? m_messages.m_entries[preferred + 407].text : 0'. Port needs --original-file ShellMenu.cpp --file-index 8612 --category reconstructed_game.
- CAnimObjectFactory::Init (0x80096f50, 616, append to AnimObjFactory): aofi/AnimObjFactory.cpp CODE EXACT (rd 1 = reloc), but EXTAB-PADDING HOLD (new[] DELETEPOINTERCOND field 0x2500 orig vs 0x066c ours) - same open user question as LoadWeaponObjects/AddChildren. Port cmd ready: --pool .sdata2 --pool .rodata --file-duplicate __ct__11CAnimObjDefFv. Keys: hoisted decls (entry,name,item,weapon,mesh,count,index,size,cached), separate var for the weapon loop, '&name[strlen(name) - 3]', 'm_free = (CAnimObject*)(m_pool = DWI_alloc(...))' + the Reset loop written out.
- LANDED b83565f0 CProjectileBullet::Init (pbi/).
- CTexture palette knot (Set(Shape*) tex2/): tried switch((unsigned)*p), unsigned literals/types (merge ' '/'!' into a range), bitfield header type, '0'||'1' natural form (o1.cpp rd 4: CW merges 0/1 itself but keeps signed cmpwi), ternary/bool forms - no cmplwi without the 32/33 merge. Still open.
- PROBE (CW 1.3.2, RTTI on): vtables are emitted at the end in REVERSE order of their classes' key-function DEFINITIONS (base lists in RTTI creation order). 'void Y::f(){} Z::~Z(){} Y::~Y(){}' gives basesY, basesZ, vtZ, vtY (the publisher.cpp pattern) - i.e. orig defines an ISubject virtual BEFORE ~IObserver, which the text order (~IObserver first) contradicts unless something else is going on. obs/ parked.

## HANDOFF (2026-10-26 ~00:30, sixth run) -- bigfiles
Landings: c382731c (NEW player_weapon_draw: CPlayerWeaponObject::Draw), 21727d81 (NEW scene_update: CScene::Update 2848),
df9bcec7 (NEW publisher_erase), cee2ccff (NEW PathNodeList_erase), b83565f0 (projectilebullet_update += CProjectileBullet::Init 1404).
Code-exact but blocked: CAnimObjectFactory::Init (extab padding hold), PassMP_PrefWeaponNameToIStudio (ShellMenu ...rodata.0 base),
IObserver/ISubject dtors (publisher .data vtable order). Near: PublishEvent pub/p1.cpp rd 4; CScene ctor sct/c.cpp rd 88 (reg classes only).
Tools: t6/rd.py (register-exact diff), t6/hill.py (line-order hill climb on a block file), t6/odis.py <base> <from> <to>.
- CUserMessageTable::Load: umt2/z0t1.cpp rd 10 ('text = end + 1 + extra;' fixes the association). Left: shared zero (li r7,0; mr r4,r7 for *end = 0) and extra/offset r6/r9 swap; inline-helper loop forms worse (48).

## 2026-10-26 (seventh run) -- bigfiles
- PublishEvent: pub7/e.cpp rd 2 (from 4): loop 1 'SubjectCallback callback = item.m_callback;' local (fixes its scheduling), loop 2 as
  'iterator it = find(..); while (it != end()) { ...member call...; it = find_next(it); }'. Any callback local in loop 2 (for or while, hoisted
  decl, param copies, map ref) gives the it/subject/event rotation (rd 45). pub/r.sh now takes paths relative to bigfiles/ (pub/x.cpp).
- UISExecuteFnc: frame load via inline Top()/param copy/void* cast/hoisted decl: all rd 3 (insensitive). CUserMessageTable::Load: zero-var forms
  (i = zero = 0, char/int zero, '\0', int i with cast) no gain (rd 10); umt2/r.sh added.
- SweepAllExtents dead-neg mystery: probes (sae/probe.py + v*/w*/u*/t*/s_*.cpp) - CW DCEs every -OSGetTick()/+= chain (types, loops, inline params,
  dead ifs, copies); only an RAII timer struct with a dtor survives (on the stack, not regs). Unsolved.
- NEW_tBigYUVSwizzlerTexture (parked since 10-12 for the x2 unroll): FIXED by the parks unroll trick - '(unsigned char)(expr)' instead of
  '(int)(expr)' on the four table stores gives the original twofold unroll (byuv/gen.py, g03 rd 0). LANDED 3582c690 bigyuvswizzler += (pool .sdata2 .rodata).

## 2026-10-27 (eighth run) -- bigfiles
- PatchUpMVDFile (parked 10-08 at 36): mvd/staticobjfactory_patchmvd.cpp rd 0. Key: user CQuaternion copy ctor + operator= (float members)
  and 'CQuaternion q(frame.rotation); frame.rotation = q; frame.position = sum;' (an inline Set(pos, CQuaternion byvalue) also gives 0).
  NEW unit staticobjfactory_patchmvd (exact gap static_obj_dtor..staticobjfactory_helmet), --pool .sdata2.
- LANDED e1f82aac NEW staticobjfactory_patchmvd (PatchUpMVDFile).
- LANDED 06516bfc static_obj_pathbegin += UpdatePathMotionDestAndSpeed (upm/): swap two independent statements (along before length) fixed
  FPR colouring that 30 decl orders + permuter missed. TRICK: try STATEMENT-order swaps of independent computations for FPR colouring.
- LANDED 76a59419 NEW player_commit (CPlayerObject::CommitUpdate 2268; pcu/). See AUTOPILOT log for keys. TRICK: a by-value CVector3 arg from a
  MEMBER that the orig copies lfd,lfd,stfd,stfd (f1/f0) needs a builtin whole copy: an inferred 'CVector3(const CVector3Data& o) : d(o) {}' and
  'SetPos(m.d)', while the one-by-one copy ctor stays for the dead param copies. Next: CPlayerObject::AttemptUpdate (pau draft, same SetPos issue).
- Parked again: TranslateFromScript (tfs/, rd 5 zero-holder coalescing), PublishEvent (pub8/, rd 2), quick_sort_ (qs/, 27),
  DrawTextCenteredF (dtc/, conv*var operand order), static BeginAnimMotionUpdate (bam/ statement-order brute force: none better than 6).
- LANDED 2dd9a655 player_updateai += AttemptUpdate (pau2/): SetPos whole-copy ctor + chained zero start.
- CPlayerObject::BeginUpdate 0x800a27f8 (9460, NEVER TRIED before; claimed): pbu/ (head.cpp + body.cpp; r.sh; flt.py filters stack-only
  hunks; var.py/hv.py variant runners). d15.cpp = rd 8: ONLY the fiber block start (0xb8-0xd8: FPR colouring of the position copy /
  radius / z) + 2 stfsu reloc-noise lines. Keys: weak CVector3/CLine3 ops declared out of line (orig calls them; the function is past CW's
  inline budget), GetPos/GetFront return CVector3(m.d) (whole copy), CVector3(const CVector3Data&) ctor, '(bool)GetValue(..)' for jump,
  'bool crouch = (bool)GetValue(12)' local, fabs() via an inline double fabs, '/ 2.0f' (not *0.5f), 'if (fire)'/'m_fireValue' truthiness,
  ONE 'CVector3 offset' for both osc branches (slot), an inline accessor IsDisabled() for the m_397_40 test (stops CSE of the 919 byte:
  TRICK - a byte that orig reloads where ours CSEs = read one of the bitfields through an inline accessor).
  Land plan: NEW unit (0x800a27f8..0x800a4cec, between player_updateai and player_vec_add), --pool .sdata2, local externals g_pCamera,
  statics osc$... (.sbss, guarded) owned.
- LANDED 5d55c8ff NEW player_beginupdate (BeginUpdate 9460, pbu/).
- CPlayerObject::Draw 0x800a5180 (4156, never tried; claimed): pdr/h_best.cpp rd 31 = ONLY callee-register numbering in the scope
  section (orig left r22/right r21/top r20/bottom r19, edge/quarter/(right-left) r23, midX r24; ours bottom r23, midX r19). Done: default
  case FIRST in the switch (orig layout), 'insetLeft/insetRight' assigned the same expression twice per branch (orig mr r6/r10 copies),
  duplicate 'verts[1].u = 0.0f' in poly 6 (orig stores it twice), IsDisabled() accessor (919 reload), (unsigned int)clipAmmo, function-top
  decl order (hill.py over pdr/h1.blk: stuck at 31). Needs .rodata from 0x80176a00 (dwi_prime_list + @1074/@1076/@1083/@1092 templates;
  code uses a CSE'd .rodata base + offsets) -> port with --pool .rodata (the prefix of player.cpp's .rodata) and --pool .sdata2.
- LANDED 3614a353 NEW ShellMenu_update (ShellUpdate; shu/).
- LANDED fa1b0396 static_obj_attempt += BeginAnimMotionUpdate (soldier's sbam/b.cpp LerpTo fix, merged into the unit source). CHierObject twin may take the same LerpTo.
- LANDED 6511c531 NEW publisher_setsubject (sets/). TRICK: weak template whose caller-visible callees must NOT inline -> own unit with only the inlines it needs.

## 2026-10-27 (ninth run, 17:20+) -- bigfiles
- AddAmo (amo2/run.py): ~15 more forms (type-first local, inline helpers, unsigned-short, ternary, split locals) all rd 2 (props ptr r4 vs orig r5). Parked.
- CFontDriver::EndDraw (fed/run*.py; RIGHT template is font_driver.json/any font_*.json, same flags): stuck at rd 11 = register colouring
  only (orig data r6 + bytes ascending r7..r9, r0; ours descending r9..r6). Tried: implicit CColor, op= in all 24 orders, font_render-style
  copy ctor (load/store order), GetColor word copy as FontDataView/CFont inline, GetScale inline, statement orders. Parked.
- CPlayerObject::Draw: pdr2/m3.cpp rd 15 (from 31): 'int bottom;' moved after 'int top;' AND 'int midX;' hoisted to the top block
  (4th line, assigned in the case block). Left: top/right r20/r21 swap, midX r24 vs (right-left) temp r23 swap.
- CPlayerObject::Draw SOLVED (rd 2 = reloc noise): pdr2/best_rd2.cpp. Keys on top of h_best: conversions in order left, right, top, bottom
  (statement swap fixed top/right), hoisted 'int midX;' AND hoisted named 'int span;' (= right - left) used in midX and both insets
  (span / 4) - a named local, unlike the CSE temp, gets the orig's colour; then decl-block hill (pdr/hill.py) -> crosshair after width.
- LANDED d2fcb0b3 NEW player_draw (CPlayerObject::Draw 4156).
- LANDED 9de36d54 player_crouch += PeekLeftRight (plr6/: in-place 'peek *= z; peek += base.z', hoisted decl hill, statement-order brute force).
- LANDED 5afaff4a publisher += IObserver/ISubject dtors + RTTI/vtables: '-inline deferred' (no auto) + reversed source order (obs/publisher.cpp, obs/tmpl_def.json).
- CParticleSystemManager::Draw 0x8007eabc (628, NEVER tried before, claimed; append to particlesystem_active, Ghidra-drafted): psd/b1.cpp rd 3,
  len 154 vs 157: everything matches except THREE dead branches at the function end in the orig ('b end; b <search loop 1 entry>;
  b <search loop 2 entry>' after the g_bPaused store) - the inlined sorted-insert search loop seems to have a second preheader. The file's
  .bss head statics (g_ModelToScreenMatrix, g_X/Y/ZAxis, g_WorldToCamera) defined static in the unit give the orig ...bss.0 base + offsets.
  Tried: loop forms (while/for/do/goto/break/continue, q var), early returns, continue in outer loops, p copy forms, FindSlot inline. 
- PeekLeftRight/PlayerDraw/publisher LANDED (see above). Parked again today: AddAmo (2), EndDraw (11 regs), Scroll (8; hill on explicit
  Reset body order: no), PublishEvent (SItem by-value copy fixes the callback load order but gives the rd-45 rotation), quick_sort_ (27),
  player OnCollision (7), CUserMessageTable::Load (10), UpdateSystem (2).
- EndDraw: also tried font_driver '-inline deferred,auto' with CFont::GetColor/GetScale defined after (font_render's CColor copy ctor) - still 11.
- Add_MPPlayerKilled (mpk/): killer param vs killer*256 CSE temp swap (+ team/total); named stats ref/pointer worse (78), decl hill no gain.

## HANDOFF (2026-10-27 ninth run end) -- bigfiles
Lock released; no permuters running. Landings: d2fcb0b3 (NEW player_draw: CPlayerObject::Draw 4156), 9de36d54 (player_crouch += PeekLeftRight 628),
5afaff4a (publisher += IObserver/ISubject dtors + RTTI/vtables, '-inline deferred' no-auto, no pragma).
Best WIPs: CParticleSystemManager::Draw psd/b1.cpp rd 3 (3 dead end branches, len 154/157); UpdateSystem us23 rd 2; AddAmo amo rd 2;
PublishEvent pub7/e.cpp rd 2; Scroll scr/a.cpp 8; CUserMessageTable::Load umt2/z0t1 10; EndDraw fed/ 11; player OnCollision pzD0 7.
Claims: Draw (particle manager), EndDraw, Scroll, IObserver/ISubject dtors (done) + older ones above.

## 2026-10-27 (tenth run, 17:20+ refresh) -- bigfiles
- CParticleSystemManager::Draw SOLVED rd 0: psd2/final.cpp (whole particlesystem_active unit). KEY: the 3 dead trailing 'b' = out-of-line
  split-edge blocks; walking the inline AddBin's search loop on the 'head' local itself (no separate 'p = head' copy) gives them.
  BLOCKED: Draw addresses the file's .bss via the ...bss.0 base reg (g_XAxis etc.), owned by particlesystem.json (__sinit) -> reconstruct
  'Source ranges overlap .bss'. Open user question (b) class. Whole-file route would need UpdateBoundingBox (ubb23 raw 112),
  UpdateSystem (us23 rd 2) + the @32@ thunks folded into one unit 0x8007eabc..0x8008144c.
- LANDED abb13382 particlesystem_texture += UpdateSystem (us/final.cpp; Emit early-return inline with float& last). TRICK: a virtual call's 'mr r3,this; lwz r12' swap at a block start -> move the guarding if INTO the inline as an early return.
- Gameloop_Init (2152, prepend to mainloop_game = src/game/mainloop.cpp): gli/final.cpp rd 6 = noise only (5 .sdata2 strings + g_cheats lo).
  From the 10-21 draft (rd 148). KEYS: big globals must have real sizes (g_scene/g_AnimDB/g_Pause/g_inputMgr were SDA-addressed);
  SetViewports(a, b) inline; g_pPlayers[count] (not a walking pointer) so the slot pointer is a compiler IV; 'for (i = 0, count = 0; ...)';
  SetNear/SetFar are CCamera members (m_near +36, m_far +40, flags +322/+323 as in camera_rows.cpp) called through a
  static inline SetupCamera(CCamera*, fov); the load-progress fraction in a static inline LoadFraction(world) (args set before the if);
  'mission = g_Shell.m_mp.mission = mp->mission;' chains; the loop pointer 'info' declared separately and assigned 'info = mp = &g_Shell.m_mp;'
  (gives the orig's early 'mr r27,r28' copy). LoadLevelSettings stays rd 0 with player->m_camera.SetNear/SetFar.
- mainloop.cpp WHOLE FILE (gli/final3.cpp): + Gameloop_Shutdown (first compile; CPlayerObject virtual dtor slot 0x0c, member object at +1904
  with vtable at +12) + Gameloop_Restart (first compile; ClearNodes() inlines for +4/+8 links, 'SAcquireSet set = {0,1,2,3}' from .rodata),
  folds mainloop_loop (CCamera virtuals, CMatrix, CPlayerObject m_matrix/m_fov). Identifier is Init__NotTheVirtualOne (double underscore).
- LANDED 6885d700 mainloop.cpp whole file (Init/Shutdown/Restart + folded Loop).
- skybox.cpp WHOLE FILE (sky/final.cpp): + CSkyBox::Draw (permuter found: V read into a local, U assigned directly; whole.cpp keeps the
  weak IsVisible/IsDrawEnabled/SVertex ctor OUT OF LINE at the top (text order), CSkyBoxRenderBin's functions INLINE in the class (emitted
  after the sinit), static members defined, RTTI on, --pool .sdata2 --pool .rodata (RTTI-name/constant order differs), weak dups
  __dt__10ISceneNodeFv/__dt__10CRenderBinFv; drop 'adapted_from' after --like scratch tmpl). Permuter fixup bug: patch fixup.py line 20
  with the (?!\s*[.\[]) lookahead for '*(*ref).member' (see TRICKS).
- LANDED d0c6d5b1 skybox.cpp whole file (+Draw).
- Pause.cpp WHOLE FILE (pause/final.cpp): + PauseInit; KEY: '-inline deferred' (no auto) + functions in REVERSE image order (orig
  .rodata = dwi_prime_list, verts template, event template, strings: parse-time templates before codegen-time strings); m_volume/fade = 0.0f
  (pool shares the 0.0 with PauseUpdate's clamp); CFont/CSprite views need declared virtual dtors (else a weak __dt__5CFontFv is emitted).
- CreateWeaponByCRC: permuter wfc/perm (base 95) 14k iterations nothing.
- LANDED 2c7e2559 Pause.cpp whole file (+PauseInit).
- MathLLAngleInit (trig.cpp, 456; would make trig.cpp whole-file with trig.json): trig/a.cpp rd 2 (r30 = &TaylorConst copy for [0]).
  Permuter (trig/perm) found rd 0 ONLY with 'int i = 0; _Math_TaylorConst[i] = 0.16666667f;' (index local) - register device, NOT landed
  (policy question). Also the .bss gap before _Math_InvTaylor (0x44 -> 0x50): old draft uses a placeholder global; aligned(16) is the alternative.

## 2026-10-29 (eleventh run) -- bigfiles
- Tool: wf/fileopen.py <maxopen> [file...] = per original file, open (uncovered) fns vs covered bytes (whole-file candidates).
- LANDED 9ca6d520 shadow += weak IsDrawEnabled (was listed as a weak_duplicate; out-of-line __declspec(weak) def before Draw).
- NEXT: projectilebullet.cpp whole file (only Render 0x800d0b3c 260 open, bss.0 base: g_WorldToCamera +0, g_XAxis +176, g_YAxis +192;
  .bss: g_WorldToCamera 64, @646 12 (CMatrix init guard?), g_BulletBin 100, g_XAxis, g_YAxis). Merge 11 fragment units. Draft dir pbw/.
- LANDED 1384be73 projectilebullet Render..end ONE unit (+Render 260; 8 fragments folded; RTTI on owns .data/.rodata/.sdata).
  Workflow that worked: pbw/score.sh (ss.py every fn of the span vs the fragment baselines), pbw/obj.py (compile -> objdump -t/-h to
  compare data section sizes/order against the original BEFORE porting), vtable slot check (objdump -r of .data vs vtdump.py),
  pbw/land_prep.py (deletes fragments, project.json lines, docs rows). Port: --like a scratch tmpl with RTTI on, --file-index, then drop adapted_from.
- PARKED (no draft): InitMultiplayer 0x800a6408 (1220, Ghidra imp/ghidra.c): uses ...rodata.0+628 GI_SLEEVE strings, but player.cpp's
  .rodata between Draw's tables (owned by player_draw, 492) and those strings holds strings of EARLIER text functions (@2676.. GetWeaponInfoByCRC etc.)
  -> needs the whole player.cpp (43 KB) or a file_duplicate trick. Not worth it now.
- buddy.cpp whole file (claimed 07:10): bud/b2.cpp = scratch/buddy/buddy_full.cpp + lines' blc/n3 range-ctor shape (init_free()/link_range()
  inline members, nested BuddyState {path,moving,active} with self-trick default ctor + user copy ctor, BuddyInfo() : soldier(0), progress(0),
  state(BuddyState())) -> range ctor 81 -> 3; TOTAL 595 -> 527 (scratch/buddy/cmpall.py config/GMFE69/buddy_head.json <src>).
  Still: BIFunc_AddBuddy 217 (orig 'BuddyInfo info;' keeps moving/active/path in stack memory (ctor body stores, stored twice) while
  soldier/progress live in regs; b2 SROAs all, b4 (state default-constructed with self) stores all five), Progress 113, Create 77, sinit 32,
  AddBuddy 35, GetInitial 27, Died 16, list reserve 7. Same partial-SROA knot as before; parked again.
- RenderSystem (particlesystem, 4532, never tried): bss.0 user like Draw/UBB -> whole-file RenderSystem..end = 8.1 KB, but RenderSystem has a
  ~1.5 KB frame and f18-f31 callee FPRs (Ghidra prs2/ghidra.c); with UBB's FPR colouring still open, judged not feasible today.
- objcreate: CreateCollisionVolume (1236, ccv/ghidra.c) + CreateGerman (1732) never drafted, but CreateCollisionVolume uses the objcreate .bss
  base (allocators at bss+12/+80/+160, owned by objcreate.json sinit at the file end) and CreateGerman the ...rodata.0 base -> whole-file only.
- MathLLAngleInit natural forms tried (pointer local c[], *p++ fill, n++ index, inline SetTaylorConst(float*)): first store still via r31
  (block base) instead of r30 -> only the permuter's 'int i = 0' device works (policy question stands). trig/v1-v5.cpp.
- rcmpbase DECODER ctor (80): ctor/CODEC_IDATA forms (init lists, member-wise copy, statement orders, self) all keep 'addi r6 vt; li r5,0'
  (orig vt in r5 then li r5,0). rcd/*.cpp. Parked.
- buddy partial-SROA probes (bud/f1-f4 on flat BuddyInfo): per-field '&moving' pointers, self->, static Clear(this) -> ALL five stored (SIZE 912);
  Zero(T&) inline / Clear(this) static -> all promoted. Address-taking pins the WHOLE object. Orig = 20-byte slot with only +8..+20 in memory
  (= lines' range-ctor finding: 8 unused bytes then the 12-byte state). Unsolved.
- DrawTextCenteredF dtc32: direct g_viewport.m_width/m_x operands (dtc/e1,e2) worse (30/63). dtc/chk.py = exact register diff checker.
- CStaticMesh::DrawNode 0x800f2090 (800, NOT a bss-base user - old skip was wrong; prepend to staticmesh_mshnode_copy, whose weak MSHNode
  copy ctor follows it): smd/g/A3B1C0.cpp = 36 lines, all FPR colouring in the frustum-plane loop (+ item ptr r5/r4). Keys: MSHNode world(node)
  + TransformVector/Point into world.vectors; inline IsOutside(world, planes, clipped&) with 'clipped = 0' AFTER the !planes early-out
  (orig leaves r31 unset), CPlane{m_normal, m_d} + Dot(axis, n) + by-value GetDistance(CVector3 p); 'unsigned long bit;' before the loop,
  material loops with 'int i;' first. Insensitive: fabs term order/assoc, Dot arg order, Dot forms, float decl hill (smd/hill.py: 37).
  Tool smd/cmp.py <tmpl> <fn> <start> <end> <src...> = exact register diff count (relocs masked).
- CMSHMatBin::Link 0x800f1690 (1480, never drafted; guards approved; prepend to staticmesh_render, tmpl sml/tmpl.json = render + GX include dir):
  sml/BEST_n1.cpp = 15 lines (from 115 in ~15 compiles). Left: light position copies (orig first copy at sp+64, used copy at sp+48 read by
  GXInitLightPos, pos pair in f4/f3 above the fctiwz f2/f1/f0; ours swapped slots + FPR numbers). Keys: record words through
  '*(T*)&tag->m_data[index + 36]' (unfolded (i+36)*4), 'unsigned long next' loop test (cmplwi) + 'int command = next' (cmpwi),
  'CDmaTag* t = tag' walking copy (param copy after the guard), mat before list, 'CVector3 position; position = GetLightPos(i)'
  (2 copies/frame 208), GXSetZMode(..., command == 1 ? GX_TRUE : GX_FALSE) (srwi, no byte mask), GX_TG_MTX3x4, GX_GREATER.
  Tried for the copies: named GetLightPos local, inline InitLightPos by value/ref, reference binding, user operator=, decl scopes.
- LANDED e9025359 texture_load += CTexture::Set(Shape*) + Set(ShapeFile*) (1436 B; texture.cpp text complete). Palette knot SOLVED:
  '(unsigned)*palette == ' ' || (unsigned)*palette == '!' || (unsigned char)(*palette - '0') <= 1 || (unsigned)*palette == '2''.
  Drafts tex/FINAL.cpp; brute-force dirs tex/sfp, tex/sfq. NEXT: CStaticMesh::Init (1788) uses the same palette test.
- CStaticMesh::Init 0x800f25bc (1788, never drafted; append to staticmesh_ctor (RTTI on), tmpl smi/tmpl.json = ctor json + GX include):
  smi/BEST_k01.cpp = 103 exact-diff lines, length exact (447). Structure all right: inline CMSHMatBin(int base) ctor (placement new,
  CRenderBin->MSHMaterial->CMSHMatBin vptrs, offsetPtr(shapeFile), '((int)shape & 0x10) == 0x10', palette via the texture GetPalette,
  switch 24/25 CI (+TLUT RGB5A3, entries '*(short*)(palette + 4)') / 22/30 TexObj / OSReport; fmt as a local ternary first),
  loop 1 links the new-expression result ('material = bin + 1' in the for-increment), loop 2 links the pointer itself ('i++, material++'),
  packed CPWrite {char cmd; char reg; u32 value;} x8 + nop/draw/count command list, positions read BEFORE the list pointer,
  wrapS '(GXTexWrapMode)((unsigned char)(m_flags & 8) != 0)' (the only form giving neg/or/srwi; plain '!= 0'/ternaries become
  rlwinm and CSE with wrapT) + wrapT '(m_flags & 8) >> 3'. Left: register choice only - shape r5 vs r4 / flags r10 vs r5 in both texture
  blocks, and the command-list block (orig list r25 + constant 8 in r29 callee-saved; ours list r3, 8 in r25). Insensitive: list decl
  placement, SetCP inline helpers, data arg forms.
- CMSHMatBin::Link -> sml/BEST_t2.cpp = 12 lines (FPR numbering only: orig light pos pair f4/f3 with in-place fctiwz f2/f1/f0;
  ours conversions to f4/f3/f2, pos f1/f0). Slot order FIXED by two named copies: 'CVector3 lightPos = lighting->GetLightPos(i);
  CVector3 position; position = lightPos;' (first copy at sp+64, used copy at sp+48). Insensitive: colour conversion forms, statement order.
  Permuter perm_sml relaunched on t2 (launch.sh, 90 min; check perm_sml/output-*).

## HANDOFF (2026-10-29, eleventh run) -- bigfiles
Landings: 9ca6d520 (shadow += weak IsDrawEnabled, 8), 1384be73 (projectilebullet Render..end ONE unit, +Render 260, 8 fragments folded,
RTTI on), e9025359 (texture_load += CTexture::Set x2, 1436; palette knot solved). Lock released.
Best WIPs (exact register diff via <dir>/cmp.py, relocs masked):
- CMSHMatBin::Link 0x800f1690 (1480): sml/BEST_t2.cpp 12 lines (FPR numbering of the light colour/pos block). Permuter perm_sml running
  (launch.sh 5400 s from ~07:25; pgid in scratch/perm/overnight.pids) - check perm_sml/output-*.
- CStaticMesh::Init 0x800f25bc (1788): smi/BEST_k01.cpp 103 lines, length exact, register choice only (texture blocks + command list).
- CStaticMesh::DrawNode 0x800f2090 (800): smd/g/A3B1C0.cpp 36 lines (FPR colouring of the frustum loop).
- buddy.cpp whole file: bud/b2.cpp TOTAL 527 (range ctor 3 via lines' blc shape); partial-SROA knot unsolved.
Not feasible today: InitMultiplayer (player .rodata block needs whole player.cpp), particlesystem RenderSystem (4532, FP heavy),
objcreate CreateCollisionVolume/CreateGerman (objcreate .bss/.rodata base -> whole file), CCompartment::Init (2628, compartment .bss base).
Claims (bigfiles, this run): shadow/projectilebullet/texture (done), InitMultiplayer (parked), buddy.cpp whole file, RenderSystem,
staticmesh DrawNode/Init/Link.

## 2026-10-29 (twelfth run) -- bigfiles
- Permuter perm_sml killed at 07:42 (best 110 = 'color.b' moved after the lightPos copy, which was the hint).
- LANDED b9369c22 staticmesh_render += CMSHMatBin::Link (1480). sml/land/staticmesh_render.cpp = unit source (Link + Render).
  KEY: brute-forced statement order of the colour/position block (sml2/q_*.cpp, 280 orders): conversion temps get LOWER FPRs when created
  after the position copy; red must stay CSE'd from the test -> 'float red = m_color[i][0]; if (red != 0.0f || ...)'.
  Also: material colour is {172,172,172,255} (cmp.py masks lwz from r2, poolvals/one_unit caught it) - older sml drafts have 255s.
- Tools: aa/dis.py <tmpl> <src> <fn> <start> <end> [q] = side-by-side raw listing; sml2/c.sh, smi2/c.sh wrappers for the cmp.py's.
- AddAmo (aa/): 14 expression/local/helper forms, all raw 2 (props r4 vs r5). Insensitive.
- CStaticMesh::Init (smi2/): list decl at top/loop, walking CPWrite*, SetCP inline, image arg forms (&data[0], shape+1, (char*)+16),
  SetShape(Shape*/void*/ShapeFile*) inline member: ALL stay 103. Very robust colouring.
- DrawNode 36 -> 10 (smd2/BEST10.cpp = smd/BEST10.cpp, 'MSHNode world = node;' for the permuter): 'unsigned long bit = 1 << i;' inside
  the loop + 'CPlane* plane;' before the loop (fixes bit/plane GPRs); r = '(float)f0 + (float)f1 + (float)f2' over
  'double fK = fabs(Dot(world.vectors[K], plane->m_normal));' in order 2,1,0, then d (fabs temps before ALL frsp temps). Left: 4 FPR
  (axis1 z-term fmadds dest f6 vs f5 / lfs 144 f5 vs f6) + item r5/material r4 swap in the item loop (insensitive to 12 loop/helper forms).
  Permuter perm_smd (fixup.py patched with the (?!\s*[.\[]) lookahead) launched 11:10 for 3600 s, pid in scratch/perm/overnight.pids.
- Init: value locals / list position brute force (smi2/x_*) all 103.
- Retried with the creation-order idea, no gain: DrawTextCenteredF (dtc2/: 240 decl orders + operand/int-local forms, stays 2 operand-order
  lines), CStaticObject::OnCollision (soc3/: 734 interleavings of the motion block incl. px temp, stays 2), CAnimMorph::Draw (amd2/:
  operator= bodies + 30 left/right orders, stays 3 store-order lines), Init texture-call arg locals (smi2/y_*, 120 orders, all 103).
- perm_smd stopped 11:19 (17k iterations, stayed 80 = BEST10). perm_smi (Init, BEST_k01p.cpp = this->m_mesh + NewBin() wrapper for placement new) launched 11:19 for 5400 s, base 3580.
- 11:25 coordinator: BSEventProcessImmediateEvents reassigned to soldier (scratch/par/soldier/bpe15/) - do not work on it (released).
- CStaticMesh::InitClass 0x800f2478 (152, not drafted, .bss-base user -> whole-file only): 'static bool bReady = false; if (!bReady) {
  g_pScreen = screen; g_pRenderList = list; list->Register(g_MshSimpleBin /*bss+444*/); list->Register(g_MshSimpleAlphaBin /*bss+492*/);
  g_pLightVolume = 0; memset(bss+336, 0, 96); }'. staticmesh .bss: +0/+64/+128/+192/+256 CMatrix (Actual W2C, W2C, ...), +320 vec,
  +336 96-byte block, +444/+492 bins, light$739 at 0x802da730 (+528, now owned by staticmesh_render). CStaticMesh::Draw 0x800f1e64 (556)
  also bss-base. Whole staticmesh.cpp needs Init (103) + DrawNode (10) exact first.

## HANDOFF (2026-10-29, twelfth run) -- bigfiles
Landing: b9369c22 staticmesh_render += CMSHMatBin::Link (1480). Lock released, tree clean.
Best WIPs: CStaticMesh::DrawNode smd/BEST10.cpp (permuter form BEST10p.cpp) 10 exact-diff lines (4 FPR in the axis-1 z term + item/material
r5/r4 swap); CStaticMesh::Init smi/BEST_k01.cpp 103 (rock-solid: ~300 source variants all 103); perm_smi running on Init (5400 s from 11:19,
pid in scratch/perm/overnight.pids; base 3580, best 3575 = noise) - check perm_smi/output-* and kill by the timeout pid.
Unchanged near-misses retried: AddAmo 2, DrawTextCenteredF 2, static OnCollision 2, CAnimMorph::Draw 3, rcmpbase DECODER ctor 4 lines.

## 2026-10-29 (thirteenth run) -- bigfiles
- Screen.cpp whole file (claimed 11:40): BOTH open functions match standalone (scr/def.json = Screen.json with
  '-inline deferred,auto' + RTTI on): CScreen::SetCurrent scr/sc1.cpp raw 12 = relocations only (first compile; DEMOInit-style
  GX setup, 'return this' early / 'return g_prevScreen'); CScreenFieldMode::_Capture scr/cap5e.cpp raw 8 = relocations only
  (.ssh writer: locals declared dir, shape, header (reverse stack order); bitfield SSHShape {type:7, flag:1, next:24; w; h; cx; cy;
  short x:12 + 4x1; short y:12, mips:4}; switch 'case 0/case 1/case 2: default:' (the case 2 gives the dead 'b'); type
  'if (fmt == 0) 3; else { 5; if (fmt == 1) 4; }'; empty chunk loop; 'int count = m_width * m_height' in the 0x8000 loop).
  BLOCKER = POLICY QUESTION: the file's ...rodata.0 block starts with "CScreenFrameMode" (@669, just before CScreen's RTTI name @671)
  and has "CScreenFieldModePAL" (@1058, just before CScreenFieldMode's RTTI name @1060). No symbol, vtable or code of those
  classes survives in the image: they are RTTI type-name strings of two linker-stripped subclasses (their ctor/vtable/RTTI
  were dead-stripped, the pooled strings could not be). SetCurrent reads its strings at block+84.. so the whole-file unit needs
  those two strings at those positions = discarded (stripped) minimal subclass code (e.g. an out-of-line ctor/dtor of
  CScreenFrameMode and CScreenFieldModePAL that emits their vtable+RTTI), placed in source so the codegen order matches.
  Same class as (h) CExplosion::Visit (DECLINED), but here the class names are attested by the binary. Not pursued further.
  Probe: unused subclasses / dead 'if (0)' strings do NOT emit strings (scr/t1.cpp, t2.cpp).
- perm_smi (Init) killed 11:45 (22 min, best 3575 = noise; Init is rock-solid at 103).
- dwi::quick_sort_<UNodePair> 0x800d8ef0 (804, untried; append to scene_quicksort.json, deferred flags): qs/n1.cpp (non-template
  view) = qs/q2.cpp (template) struct 0, 22 exact register lines (qs/sc.sh = exact diff count). Shape: n<=16 -> inline insertion_sort
  (first+1<last; first+2==last swap; bubble pass 'I prev = i - 1; if (comp(*i,*prev)) iter_swap(i,prev)' from last-1 down;
  then 'for (I i = first + 2; ...) unguarded_linear_insert(i, *i, comp)' with T value by value), else sort_(.., (T*)0),
  'I mid = first + 16', insertion_sort(first, mid), loop over [mid,last). Left: in the else branch's inlined insertion loop
  i/j/next get r27/r29/r28 (orig r29/r28/r27, same as branch 1, which matches) and the final loop next r27 (orig r29).
  Insensitive: helper unguarded_insertion_sort (worse), mid placement, condition inversion (worse), linear_insert forms,
  manual final loop (rotates the final loop only), template vs plain. Manual inlining of everything = 230 (useless for permuter).
- SweepAllExtents dead-neg: tried the 'two rounds = a 2-iteration loop over m_extents[2]/m_pairs[2]' idea (sae2/L1-L3,
  cumulative/fresh/hoisted timers): CW does not unroll it -> no. Also dead float/double conversions, local struct/array timers
  (sae/x1-x4): all DCE'd. Unsolved.
- Permuter perm_smd2 (DrawNode, base smd3/h_5_100p.cpp = 10 lines of the OTHER kind: item r5 fixed, i/material r4/r3 swapped)
  launched ~12:00 for 3600 s, -j2, pgid in scratch/perm/overnight.pids.
- DrawNode decl brute force (smd3/gen.py, gen2.py, gen3.py): two 10-line families only: (A) BEST10 (item r4/material r5),
  (B) h_5_100 ('int j; int i; MSHItemView* item; unsigned long clipped; MSHMaterialView* material;' at the top: item r5 OK,
  i r4/material r3 vs orig r3/r4). Item-loop body forms (shared/local/no material, i/j/k index, inc in for) all keep B at 10.
- Screen.cpp WHOLE FILE draft scr/w2.cpp (reverse image order, scr/def.json = Screen.json + '-inline deferred,auto' + RTTI on;
  static g_* globals; GXColor template in Flip; CRect/CColor with ctors for the CScreen init list): ALL 33 functions
  (0x8001b12c..0x8001bfbc incl. the weak Get*Scale) match by ss.py (raw = relocations only; scr/score.sh), .text 3728 = orig,
  extab/extabindex/.sbss/.sbss2/.sdata/.sdata2 sizes+bytes = orig. ONLY .rodata differs: the two orphan RTTI names.
  scr/WHOLE_rt88.cpp adds minimal 'stripped' classes at the END of the source (= image start): CScreenFrameMode : CScreen
  {~dtor, Rebuild, SetDraw} and CScreenFieldModePAL : CScreenFieldMode {~dtor} -> .rodata byte-identical to the original
  ("CScreenFrameMode" first, "...FieldModePAL" + "CScreenFieldMode" last; scr/gen_rt.py tried 81 placements, only
  both-at-end / PAL-before-FM::_Capture give the orig order). Landing it needs (1) POLICY OK for invented discarded subclass
  code (cf. (h) DECLINED) and (2) tooling: discard those functions (+ their extab/extabindex entries) AND per-object data
  stripping of their vtables/RTTI records in .data/.sdata (project_build only strips whole sections today).
  RTTI-name order rule found (deferred mode): a class's RTTI name is generated BEFORE all function strings when its key
  function is parsed after its base's, else AFTER all of them (scr/e1-e3.cpp).
- POLICY QUESTION (Screen.cpp, 1696 B new + folds 6 units): the original Screen.cpp also defined two classes whose code the
  linker dead-stripped (CScreenFrameMode : CScreen, CScreenFieldModePAL : CScreenFieldMode); only their RTTI type-name strings
  survive in the file's pooled ...rodata.0 block, which CScreen::SetCurrent addresses by base+offset. A whole-file unit is
  ready (scr/WHOLE_rt88.cpp: every function code-exact, all data sections byte-equal) if we may (a) keep minimal
  definitions of those two classes (empty dtor/Rebuild/SetDraw, comment 'stripped by the linker in the original; class
  names attested by the RTTI strings') and (b) extend project_build/port_unit to discard their functions + extab entries
  and to strip their vtable/RTTI data objects. Similar to (h) (DECLINED), but here no function body is guessed beyond
  empty members. Recommendation: ask; do not land without a yes.
- DrawNode 10 -> 4 (smd/BEST4.cpp = smd4/p009.cpp): separate 'MSHMaterialView* m2' for the item loop + top decls in the order
  'int j; int i; unsigned long clipped; MSHMaterialView* material; MSHMaterialView* m2; MSHItemView* item;' (item loop
  index j, material loops i) -> ALL integer registers exact. Left: the 4 FPR lines of the axis-1 z term (unchanged).
- Retried without gain (this run): CScene ctor register classes (sct2/a-k: init-list vs body, bool/int 1, SExtent ints,
  by-value list_node, static empty T: all rd 88+), CStaticMesh::Init (diagnosis: orig evaluates the GXInitTexObj(CI) args
  mipmap/wrap first and the image pointer LAST, so shape stays in r4 and becomes 'addi r4,r4,16'; ours computes the image
  first -> shape r5; known-insensitive to locals), SetFinishedLevelData (sfl/: 96 helper decl/total/outer orders, all 10),
  TranslateFromScript zero-init (tfs/: Set(v)/chained/zero-local forms, best stays 4), decal Init__NotTheVirtualOne (read
  only: orig name IV starts at 8 (+4 field), ours 0 (+12)), DrawNode FPR (smd5-smd8: fK/sum orders, hoisted FP temps,
  Dot forms, normal/vector reference locals, separate dot temps: best stays 4).
- Permuter perm_smd3 on smd/BEST4p.cpp (perm base 30) launched ~12:03 for 5400 s (-j2), pgid in scratch/perm/overnight.pids.

## HANDOFF (2026-10-29, thirteenth run) -- bigfiles
No landing this run. Lock never taken; tree untouched by me (bsevents_head is the soldier agent's).
Best WIPs:
- CStaticMesh::DrawNode smd/BEST4.cpp = 4 exact-diff lines (only the FPR pair in the axis-1 z term; all GPRs exact; key =
  separate 'm2' material local for the item loop + top decl order j, i, clipped, material, m2, item). Permuter perm_smd3
  running until ~13:35 - check perm_smd3/output-* (score < 30) and kill by the pgid in overnight.pids if needed.
- Screen.cpp whole file scr/WHOLE_rt88.cpp: code-exact for all 33 fns + all data byte-equal, but needs the POLICY decision
  above (stripped-subclass stubs) + tooling for per-object data stripping. Do not land without the user's yes.
- dwi::quick_sort_<UNodePair> qs/n1.cpp (q2.cpp template form): struct 0, 22 register lines (else-branch insertion loop
  colouring).
- Unchanged: CStaticMesh::Init 103, CScene ctor rd 88, TranslateFromScript 4, SetFinishedLevelData 10, AddAmo 2.
Tricks: (1) when a few integer registers in a loop are rotated, give each loop its own pointer local (DrawNode 'm2') and
brute-force the function-top declaration order (smd4/gen.py, 720 orders, 60 hit the best); (2) deferred-mode RTTI-name
order rule (scr/e1-e3.cpp, see above).

## 2026-10-29 (fourteenth run) -- bigfiles
- quick_sort_<UNodePair> 22 -> 6 (qs2/f.cpp non-template; qs2/t1.cpp = template form q2 + same fix, also 6 via QSM=$QS):
  ONE shared 'I i;' declared at the top of insertion_sort for BOTH loops (bubble + insertion) -> else-branch insertion loop exact.
  Left: final loop (mid..last) next r27 vs orig r29 (orig next r29 > last-copy r28; ours last r28 > next r27).
  Probes: the else-branch BUBBLE loop is what blocks r29 for the final loop (qs2/prA/prB). Insensitive at 6: uli forms (g/, 128
  combos), manual final loop x 240 decl orders (m/), helper unguarded_insertion_sort (y*, worse 32), bubble forms (bb/), ref
  params (c*), value-local uli (z*, stack slots move), mid placement. 'next declared before last' in uli2 (xq) gives the right
  relative order but r28/r27 (11).
  More no-gain (all stay 6): separate inline copies per branch with independent decl forms (two/, 144), final loop temps/casts/
  do-while (p*), scopes/early return (k*), combined final_ins inline (fi*, worse), linear_insert wrapper (L*: moves stack slots).
  Permuter dir perm_qs ready (non-template view qs2/fp.cpp; mkperm2 run with symbol_range overridden to the template symbol $QS,
  NodePairCompare added to PERM_PRETEND; base compiles). Launch when perm_smd3 ends (killing perm_smd3 was refused by the
  harness classifier - let it time out ~13:35).
- 12:35 coordinator FYI (dwi_prime_list at the start of ...rodata.0 blocks): already known for bigfiles' candidates - CScene ctor
  (sct/c.cpp) has the prime list and is blocked only by register classes (rd 88); AllocateStaticObject needs the open Init
  (3376) as the unit head plus its own 692-line draft (scratch/sofw/a.cpp). No quick win there.
- Random combined search qs3/rs.py (699 variants of swap/uli x3/separate insertion inline/decls/bubble/final forms): min 6, none better.
- CScene ctor re-checked: orig keeps 0/1/2 in r27-r29 only in the pre-call init block (rematerialised after calls) = pressure/colouring order.

## HANDOFF (2026-10-29, fourteenth run) -- bigfiles
No landing. Lock never taken; tree untouched.
Best WIPs: dwi::quick_sort_<UNodePair> qs2/t1.cpp (template, landing form) = qs2/f.cpp (non-template) 6 exact-diff lines (final mid..last
loop next r27 vs orig r29; everything else exact). Permuter dir perm_qs READY but NOT launched (perm_smd3 still running until ~13:35;
killing it was refused by the harness). Launch: setsid timeout 5400 sh scratch/perm/run.sh $PWD/perm_qs -j2 --stop-on-zero > perm_qs.log
and record the pgid in scratch/perm/overnight.pids. NOTE perm_qs only mutates quick_sort_'s body (inlines live in prefix.cpp).
DrawNode smd/BEST4.cpp still 4 (perm_smd3 best = 30 = base after 34k+ iterations).

## 2026-10-29 (fifteenth run) -- bigfiles
- CWorldObject::Acquire 0x800b737c (652, append to world_object_explosion; old park scratch/game/wo_acq_j.cpp, AUTOPILOT 10-16)
  claimed. acq/sc.sh = exact diff count. wo_acq_j 57 -> e2 42 (PushRequest takes loop 1's 'end' as a param: target reuses r29)
  -> e6/e7 same LENGTH as orig (CSE of the compartment end: loop 3's search = static inline Find(m_compartments.next, cend, index)
  using loop 2's named 'cend'; push = insert-shaped 'prev = pos->prev' first like PushCompartment; request.m_unknown4 = 0 BEFORE
  m_index). Left = register colouring only (e7 75 lines, mostly rename shifts).
- LANDED f7ac0ed7 world_object_explosion += CWorldObject::Acquire (652), raw 0 (acq/h/q1.cpp standalone, acq/land/u.cpp = unit).
  KEY TRICK: inline helper PARAMETER COPIES are late vregs (get low registers, can sit in r0); writing the loop in place against
  the named local (cend) gave the original's r7. Also: 'node; prev = pos->prev; node = free;' decl order in the push inline.
- 16:05 perm_qs launched (5400 s, pgid in scratch/perm/overnight.pids).
- toilet.cpp hand-off (parks s15/toilet_m.cpp needs TLT_LoadFromRepository exact in the same unit): Repository still raw 58-70, len 134 vs 143.
  Diagnosis: ours HOISTS the shared 'm_data + m_size' (end() of the count==0 path and 'last') above the 'm_count == 0' test and then
  CSEs the caller's '!= end()' with it; the orig computes it separately in both paths and rematerialises &g_Repository after the loop.
  Tried (tlr3/): find statement orders, if/else with result var, count!=0 first, data/size locals (58), &m_data[m_size], char* arith,
  unsigned size, iterator struct (worse, memory). Unsolved -> toilet.cpp still blocked.
- Coordinator enum-trick tip for CStaticMesh::Init: the adjacent-constant pairs (li 9/8, 6/14) already match as branches in ours;
  the 103 is the shape r4/r5 + GXInitTexObj argument order. GetPalette in place (smi9/p1-p3): worse. DrawNode in-place Dot (smd9/): worse.
- No gain (16:30-17:05): CScene::Draw camera+192 early (sd6/: ref/pointer locals, accessor, free inline W2V, call-result locals: all
  raw 46+; orig keeps &camera->m_tm192 in r29 across both calls = one more callee reg, frame 416); AddAmo still 2; TranslateFromScript
  (tfs2/: IsSooner written in place, 3-arg CVector3 ctor / zero local: stays rd 5); Get_Hdr in untried_now.txt is already landed (parks).

## HANDOFF (2026-10-29, fifteenth run) -- bigfiles
Landing: f7ac0ed7 world_object_explosion += CWorldObject::Acquire (652). Lock released, tree clean.
perm_qs running (launched 16:05, 5400 s, pgid in scratch/perm/overnight.pids; base 40, no improvement yet) - check perm_qs/output-*.
Trick (TRICKS.md): inline-helper PARAMETER COPIES are late vregs -> they take LOW registers (or r0); a pointer kept live across loops
gets the original's higher register when the loop is written in place against the named local.
Blocked: toilet.cpp whole file (Repository hash_map find hoisting, see above). Unchanged WIPs: quick_sort_ 6 (qs2/t1.cpp), DrawNode 4
(smd/BEST4.cpp), Init 103, PublishEvent rd 2, TranslateFromScript 5, AddAmo 2, CScene::Draw raw 46.
- 16:20 CScene ctor: list_node(self/prev,next) param forms for the head address (sct3/h1,h2): rd 88 unchanged. Stopping here;
  perm_qs still running until ~17:35 (check perm_qs/output-*, nothing below base 40 so far).

## 2026-10-29 (sixteenth run) -- bigfiles
- MemoryCardGC.cpp WHOLE FILE (claimed 16:15; base = scratch/mcw/w_full_best.cpp, score: sh scratch/mcw/score.sh <src>; template
  scratch/mcw/like.json): would add CreateNewSaveGame 988 + GetAvailableMemory 284 + MountPortIfRequired 616 (all ...rodata.0 base users)
  by merging prefs/format/status/blocks (+probe/MemoryCardGC?) into one deferred unit. Old blockers: Rewind 119, CreateNewSaveGame 46.
  * RewindSaveGameList FIXED: '#pragma dont_inline on' ... '#pragma dont_inline reset' around GetNumSaveGames' DEFINITION (precedent:
    syncfile.cpp/msgbuf.c); '#pragma auto_inline off/reset' works too; around Rewind itself does nothing (deferred). mc/r3.cpp: all 0 but CNSG.
  * CreateNewSaveGame registers FIXED by lines' tk.py: 'unsigned long slot;' (was long) -> whole register set + frame 208 exact
    (mc/m1.cpp, fast score 8 = switch only).
  * LEFT: switch tree. orig compares -6,-12,-128,-7,-2,-4; ours -6,-12,-128,-7,-3 (CW segment tree; same for 1.1..2.6). Probe
    brute force (mc/pr/e,x,y.cpp): the orig shape needs an extra label -1 with a target distinct from both the break target and default
    at tree time (y-hits all c=-1); '-1: result = -1; break; default: break;' gives the right -2/-4 nodes (mc/sw/m1_resset.cpp, fast 4)
    but leaves 'cmpwi 0; bge T' + 'li r3,-1'. Dead stores/empty inlines/empty if/goto labels vanish BEFORE the tree (no effect).
- Coordinator FYI 16:2x: CStaticMesh::Init blocker = shape must be defined in the same basic block as GXInitTexObj (soldier smi19/).
- SWITCH SOLVED + LANDED 41b619ec MemoryCardGC.cpp whole file (+1888: CreateNewSaveGame, GetAvailableMemory, MountPortIfRequired;
  folds prefs/format/status/blocks/probe; source mc/final.cpp). Switch = labels -128,-12,-6,-4,-1 -> break; case -7 {...}; and LAST
  'case -2: result = (int)result;' (dead self-assignment that survives until after the case tree is built, then vanishes; a
  value-changing body leaves code, dead stores/empty inlines vanish before the tree). Found by a random label-set/target-group search
  over small probe functions (mc/pr/rnd.py: exact orig compare sequence) - reusable for any switch-tree mismatch.
- CPathNodeList ctor (scratch/pnlw/f.cpp, 210): tk.py 22 knobs no gain; early 'const char* mapName' local worse (222). Released.
- PopUpMessageHandler_Loop (scratch/pum/loop.cpp 89): the property switch's missing 'bge' FIXED with the switch-tree search
  (mc/pr/rnd2.py: labels 1/4/8 need an extra 'case 7:' in the break group and case 8 a distinct target):
  'case 7: break; case 8: message = (PopUpMessage*)(void*)message; }' -> exact orig node shape (pum2/y7_msg.cpp, fast 88).
  Plain 'case 7/case 8: break' forms 91. Rest = FPR/GPR numbering (unchanged). tk.py running on pum2/tk.cpp.
- No gain: TranslateFromScript late self-assign casts on time.x/y/z x 6 zero orders (tfs3/, best 5); quick_sort_ late pointer
  self-assigns in uli/final loop (qs4/, 6); PopUp loop tk (7 knobs); CScene ctor tk (0 knobs).

## HANDOFF (2026-10-29, sixteenth run) -- bigfiles
Landing: 41b619ec MemoryCardGC.cpp whole file += CreateNewSaveGame, GetAvailableMemory, MountPortIfRequired (1888; folds 5 units).
Lock released, tree clean. perm_qs still running until ~17:35 (pgid in scratch/perm/overnight.pids; nothing below base 40 at 16:45).
New tools: mc/pr/rnd.py / rnd2.py (switch case-tree label-set search), side.py (orig|ours side by side), fs.py (fast score).
Best WIPs: PopUpMessageHandler_Loop pum2/y7_msg.cpp fast 88 (switch now exact; rest FPR/GPR numbering); quick_sort_ 6; DrawNode 4;
TranslateFromScript 5; PublishEvent rd 2 (one lwz scheduled before the compare in loop 2); AddAmo 2; CScene ctor rd 88.

## 2026-10-29 (seventeenth run) -- bigfiles
- Retried without gain (all stay): AddAmo 2 (amo4/: eval-order locals, phantom pointer locals per the parks 'skipped volatile' trick,
  inline helpers; a single 'long v' reused for props pointer then maxReserve gives the r5 coalescing but moves w to r4 = 4),
  CreateWeaponByCRC 17 (wfc2/: inline Remap helpers, no key copy, in-place loop), PublishEvent 2 (pub10/: hoisting the callback load
  in loop 2 ALWAYS gives the it/subject/event rotation 45 - with or without observer local, inline Notify helpers, top decls),
  RCMP::DECODER ctor 3 (rcd2/: CODEC_IDATA as the 4-arg ctor with default args inlined, body stores, compiler/flag grid 1.2.5-2.7),
  SweepExtents 10 (swe2/: inner loop as static inline Pairs(active,...) fixes the data copy r30 but rotates node/sceneNode/it = 42),
  EndDraw 11 (fed2/: byte-param SetColor, packet Set(data) inline, no data local: all 11; orig data r6 lowest, bytes ascending).
- CInputManager::Update 0x80084c80 (1220, parked 10-16 at 135 lines; claimed): ium/BEST5.cpp raw 5 (2 = r13 relocs) len exact.
  KEYS: ONE 'unsigned long j;' declared in the player loop for the three clear loops (CSEs the zero -> the orig's 9 mr copies),
  'int player' + GetKeyTable(int), no entry pointer ('m_keymap->entries[i].key' folds +4), an 'unsigned char raw' set in the three
  value branches then 'int value = raw;' (join clrlwi), Blocked condition written in the if with (int)input, edge 'value = edge != 0;',
  cubic 'value * (value * (direction * value))', flags read as 'table->keys[index].flags' BEFORE 'key = &table->keys[index]',
  hoisted 'unsigned int analog; unsigned short flags;' at the function top (decl perm search). Left: button-pair shift amounts
  (orig hi amount r6 first / lo r0; ours lo r3 / hi r0) - 40 expression forms no change.
- LANDED 671c9ca4 UserInterface_score += DrawViewportEdge (1160): dve2/h2.cpp (no 'index' local; Is*(player->m_index) inlines);
  unit source dve2/unit.cpp. Old dve3 constants were wrong (+5/-4), caught by poolvals.
- perm_ium (CInputManager::Update permuter dir, base = ium/BEST5.cpp; base.c compiles once PERM_PRETEND is stripped by the permuter)
  ready - launch when perm_qs ends (~17:36).
- Late-vreg (no named local) retries: SweepExtents (sceneNode unnamed: worse 14-102), PublishEvent loop 2 without 'item'/'observer'
  (x1 = rd 3: callback early but observer still after the compare), CreateWeaponByCRC (index as the loop counter by ref: 24).
- MemoryCardLoadGame (ds/mlg0.cpp raw 74): DIAGNOSIS - deleting the inlined SetGameType(1) switch puts slot/file in r29/r28 as the orig
  (this then r31): the inline switch is what demotes 'this'. Tried: 'm_gameType = type; switch (type)' (folds the switch: worse),
  static SetGameTypeOf(CShellMenu*, int), 'int type = 1' arg, self/slot copies, size/crc split decls: all 74.
- perm_qs ended 17:37 with no output-* (base 40 never beaten, 455k+ iterations).
- 17:45 perm_ium launched (5400 s, -j2, pgid in scratch/perm/overnight.pids; base 320). base.c needed two C-parser fixes: the
  qualified 'CDeviceManager::Update()' and 'MathFunClamp<long>(..)' replaced by inline wrappers BaseUpdate()/ClampL() in prefix.cpp.
  Check perm_ium/output-* (< 320); remember to translate the wrappers back.
- More no-gain retries 17:2x-17:45: player OnCollision (dump local inside PickUp: worse 55-61), CalculateLevelStats (unnamed MostHit
  result 12), SetFinishedLevelData (i types/scopes 29), CUserMessageTable::Load (shared counter decl, zero forms: 10), MoveStats
  (smm4/: dest++ in body puts dest first; src local/pointer/accessor forms: orig order mpStats,i,active,dest not reproduced).
- DrawFragCount 0x800bd864 (2212, never drafted; claimed): it is a ...data.0 user (base = headshotImages 0x80188cc0, offsets +0/+944/
  +1856/+1936 = its own .data objects at 0x80189400/0x80189450 after buffer$805 (DrawScore) and buffer$836). Landing needs a unit that
  owns UserInterface.cpp .data from its start: merge DrawFragCount..DrawScore (0x800bd864..0x800beb24, DVE+DrawScore already one
  unit) + define every .data object up to +1936 (vertex tables as initialisers). Not started: orig asm in dfc/orig.s.

## HANDOFF (2026-10-29, seventeenth run) -- bigfiles
Landing: 671c9ca4 UserInterface_score += UserInterface::DrawViewportEdge (1160). Lock released, tree clean.
Best WIP: CInputManager::Update ium/BEST5.cpp (= ium/w_af.cpp; whole input_manager unit source, other 9 fns still 0) raw 5 / fast 6:
only the button-pair shift-amount registers (orig 'rlwinm r6' hi first, lo in r0). perm_ium running until ~19:15 (pgid in
scratch/perm/overnight.pids; base 320; outputs so far are semantic junk - check diffs carefully, translate BaseUpdate()/ClampL() back).
Landing when exact: port_unit input_manager (Update prepended in source = deferred reverse order; --like input_manager.json).
Unchanged: PublishEvent 2, AddAmo 2, CreateWeaponByCRC 17, DECODER ctor 3, SweepExtents 10, EndDraw 11, MemoryCardLoadGame 74,
quick_sort_ 6 (perm_qs finished, nothing), DrawNode 4.

## 2026-10-29 (eighteenth run) -- bigfiles
- DrawFragCount 0x800bd864 (2212): FIRST DRAFT in dfc/. Merged unit = head2.cpp + ds.inc (DrawScore) + dve2.inc (DVE + an unused
  'static char buffer[255] = "";' = buffer$836) + dfc_*.inc, template dfc/tdef.json (= UserInterface_score.json with
  '-inline auto,deferred'): source order DrawScore, DVE, DFC -> text DFC, DVE, DrawScore (deferred reverse) and .data exactly
  0x79a = headshotImages..score$877 (globals as VertData POD arrays, zero placeholders - fill real values before landing).
  DrawScore/DVE stay 0. Tools: dfc/try.sh <inc> (merged fast score), dfc/ud.py (normalized diff), dfc/brute.py (decl-order perm).
  KEYS so far: viewport field read into a float local BEFORE the screen call (orig loads before bctrl); '/ 2.0f' not '* 0.5f'
  (operand order of fmuls/fnmsubs); 'x + 48 + nameWidth + scoreWidth'; separate 'for (int i...)' per loop; the background quad is a
  local VertData[4] initialiser with the L/T/R/B products inside it (rodata copy after the calls); a ViewportView* vp = &g_viewport
  declared before the 3 discarded vf28/vf28/vf24 calls (orig keeps &g_viewport in r19 for the later width load).
  Scores: a 432 -> f 208 -> merged 190 -> h 164 -> x2 94 -> y2 80.
- perm_ium ENDED: 71 outputs, all semantic junk (moved ClampL/continue/'value *= -1'); CInputManager::Update stays ium/BEST5.cpp raw 5.
- DFC z1.inc = 52 (block-1 decl order 'vt,B,vb,T,vr,R,vl,L' from dfc/brute.py fixed all block-1 FPRs). Left: vp=&g_viewport in r17
  (orig r19, above nameWidth r17/scoreWidth r18), vtop f28 (orig f30), loop-1 constant f29 (ours f30). No gain: max() forms (FMax
  inline etc.), vp at function top / reference inline, decl-order brute over 9 outer locals (b2.tmpl, 2000 samples, best 52).
- 21:05 perm_dfc launched (-j2, 7200 s, base 115 = z1 with GetCurrentScreen()/PRIM5 wrappers; source dfc/perm_src.cpp).
  Translate GetCurrentScreen() -> CScreen::GetCurrent(), PRIM5 -> (CSprite::EPolyPrimType)5 when using outputs.
- Real .data initialisers generated in dfc/data.inc (LetterBoxVerts..HitSpriteVerts, colour 0x80808080) - swap in for the zero
  placeholders in head2.cpp before landing.
- 21:1x z2.inc = 44 ('(scoreWidth + nameWidth + 48)' in halfW, from perm_dfc_z1 output). perm_dfc restarted on z2 (base 105, pgid in
  overnight.pids, until ~23:15). Remaining = GPR 3-cycle vp/nameWidth/scoreWidth (ours 17/18/19, orig 19/17/18) + vtop f28 (f30) +
  loop-1 const f30 (f29). No gain: decl orders (any; invariant), long types, vp as ref/const/float*, nameWidth init before vp,
  MaxWidth/ScaleWidth inlines, sharing block-1 floats with block 2/3 (greedy.py: vt shared fixes vtop but breaks vh = 48).
- 21:4x more no-gain on z2 (44): vp only for width is the ONLY form that keeps the r-holder (vh/vtop via vp rematerialise
  anyway = same code; vleft via vp 51; block-1 via vp 84); dead named discards (int/float, via vp) all 44; GetViewRect inline
  helpers (struct return / ref outs) 62-68 except ref-outs with vp param 44; nameWidth/scoreWidth defined at function top still
  r18/r19 (li hoisted to 0x3c) -> the &g_viewport holder ALWAYS takes r17 (lowest) in ours, orig r19. Loop forms (while/outer k1) 44.
  y as one expression 46 (half -> f30). vtop/vh load order/decl variants 46-50.
- tk.py (lines' int-type hill climb, 16 knobs in the DFC body): no gain. Flag variants (deferred / on,deferred / all,deferred): same 44.
- perm_dfc (z2 base 105): 69k+ iterations by 21:40, nothing real (outputs 90/100 reorder vf30 calls = semantic junk). Ends ~23:05.

## HANDOFF (2026-10-29, eighteenth run) -- bigfiles
No landing. perm_ium ended with junk only (CInputManager::Update stays ium/BEST5.cpp raw 5).
Best WIP: DrawFragCount dfc/z2.cpp (= head2.cpp + ds.inc + dve2.inc + z2.inc, template dfc/tdef.json) fast 44; DrawScore and
DVE stay 0 in the merged unit, .data is byte-identical with dfc/head3.cpp (real initialisers). Left = pure renumbering:
&g_viewport holder r17 (orig r19) with nameWidth/scoreWidth r18/r19 (orig r17/r18), vtop f28 (orig f30), loop-1 const f30 (f29).
LANDING RECIPE when exact: source = head3.cpp + ds.inc + dve2.inc + z2.inc (drop the unused Scale inline; header comment
update), replace src/game/UserInterface_score.cpp, port_unit UserInterface_score with a --like copy of UserInterface_score.json
whose '-inline auto' is 'auto,deferred', --pool .sdata2 --pool .rodata, --category reconstructed_game; unit then starts at
0x800bd864 and owns .data 0x80188cc0 (0x79a). Data note: buffer$836 (255 B, unreferenced) is placed as an unused static in DVE;
its real owner is unknown (numbering suggests late in DrawScore).
perm_dfc still running until ~23:05 (pgid in scratch/perm/overnight.pids); translate GetCurrentScreen()/PRIM5 back.

## 2026-10-29 (nineteenth run) -- bigfiles
- Took over InitMultiplayer + player OnCollision claims from paused soldier (CLAIMS line added).
- *** player OnCollision 7 -> 2 *** poc_BEST2.cpp (= poc4/gx_rv_ca.cpp). Structure: PickUp(self, other) inline with locals declared
  'weapon; health; dump;' (inline locals: LATER-declared = HIGHER reg), inits 'health = 0; weapon = 0;' (li/mr), then the two Add calls
  in a second inline 'Give(self, const int& crc, int amount, weapon, health, dump)' (all by value except crc). The by-value 'amount'
  copy is the trigger: without an extra node live across 'if (weapon)', an inline-level call-defined 'dump' always falls to r25 (below
  params); with it dump ranks by decl order. Left: the amount copy 'lwz r5,8(r1)' is scheduled before the 'beq' (orig: inside the branch).
  No-gain probes: crc by value (14/67), extra/kind/other unused params, separate GiveWeapon/GiveHealth (53), crc/amount by ref/ptr (59).
  Scorer: poc4/sc.sh <path-from-repo-root> [v]; generators poc4/gen*.py.
- CODE-INVISIBLE flip trigger: a TYPE-CHANGING by-value inline param ('long health' fed from 'int health') also un-falls dump (r31)
  with zero extra code (poc4/ty_3.cpp = 10: dump r31 OK, but health r29/weapon r30 swapped). Search over param types x orders running
  (poc4/gen23.py -> gen23.out). Tool: poc4/regmap.py <files> prints dump/health/weapon/first/other/coll/this/contact regs.
- *** player OnCollision = 0 (relocs only, byte-compared) *** poc_BEST0.cpp (= poc4/g31/int_val_whd_hw.cpp). Form: PickUp(self, other)
  inline, locals 'weapon; health; dump;' + 'health = 0; weapon = 0;', HitDumpObject, then the switch in a static inline
  'Classify(self, other, const int& crc, int value, int& health, CPlayerWeaponObject*& weapon)' called 'Classify(self, other, crc, crc,
  health, weapon)' (switch(value) + GetWeaponInfoByCRC(value), DebugMsg uses the stack crc), then
  'Give(self, const int& crc, const int& amount, CPlayerWeaponObject* weapon, int health, void* dump)'. KEY: the by-value switch-value
  param of an inline is the extra node that keeps the call-defined inline local 'dump' from falling to the lowest callee reg.
  NEXT: player.cpp span merge 0x8009f58c..0x800a68cc (InitMultiplayer soldier/pim/IM_BEST.inc via pim/mk.py).
- LANDED a4bed99e player_health += CPlayerObject::OnCollision (776). 84.82%.
- player.cpp span merge analysis for InitMultiplayer: block 0x80176a00..0x80176cd4 refs = Draw/IM via ...rodata.0 (base), @3881 GWIBC,
  @4579 OnCollision (now in player_health). @2676 'GelziabarStone' (15) + @2678 'dwi::IVisitor<ISceneNode>' (26) at +0x1ec/+0x1fc are
  UNREFERENCED (no reloc anywhere) = RTTI name strings of a linker-stripped class 'GelziabarStone : dwi::IVisitor<ISceneNode>' (no
  symbols survive). Same shape as the APPROVED Screen.cpp stub (linker_stripped tooling exists, see config Screen.json), but needs
  RTTI ON for player.cpp (current player units use -RTTI off; .data has __RTTI__13CPlayerObject refs so the real file was RTTI on),
  the stub class's vtable emitted BEFORE GWIBC's string, and a merged unit 0x8009f58c..0x800a68cc (split player_weapon at GWIBC,
  fold player_rows..player_init + player_health). Big job; not started.
- POLICY QUESTION (InitMultiplayer / player.cpp merge): the two orphan strings sit BEFORE GWIBC's string, while every other RTTI name
  of player.cpp (BSGO_Basic, ISceneNode, ..., CPlayerObject at 0x80176cf4..) comes at the END of the TU. So 'GelziabarStone' was not
  an ordinary file-scope class (its RTTI would be at the end) but most likely a LOCAL class (a scene visitor) inside a function that
  the linker stripped, placed in the source before GetWeaponInfoByCRC. Reproducing it = an invented stripped function with a local
  class - closer to the DECLINED ShellMenu dead function than to the approved Screen.cpp stubs. Not attempted; ask the user.
- CalculateLevelStats 11 -> rd 1: slm_BEST1.cpp (= slm4/r_d.cpp): location filled through 'static inline void MH2(const unsigned
  int* values, int& location)' (location = 0 first, own 'hits' local) -> location r29 as orig; left ONLY the shared zero (orig
  'li r3,0; mr r29,r3' = location a copy of hits' zero, ours 'li r29,0'). Inline-local location (li/mr) always r30 (slm4/gen4: 72
  variants = 9). CCP (soldier ccp/b.cpp 23): knob/enum searches ccp/t.cpp, t2.cpp (GetFrom/Select/PosOf/LeafOf inlines, converted
  type params): no gain, this always r29. MLG (ds/mlg0.cpp rd 64): SetGameType/IsSlotOpen/OpenFile param types: no gain.
- *** ChooseCoverPoint = 0 (byte-compared, relocs only) *** bigfiles/ccp/CCP_BEST0.cpp (AIObject_storecover.json harness). Keys in
  TRICKS (const self local first, 'int t = type', decls self/from/t, Select inline, float decls + half*half). Found by
  ccp/hc.py (knob hill-climb with {{#group:a||b}} templates, rd score) -> 17, then manual: const self (15), decl perms (7), floats (0).
  AIObject.cpp merge now has ALL open fns at 0: CFOVML soldier/s24/CFOVML_BEST.cpp, CCP ccp/CCP_BEST0.cpp, Setup/Start
  soldier/sap/SAP_BEST.cpp. Merge = span 0x80048134(or 0x800478a8)..0x8004cd34, mixed flags (fovcorpse/ctor units use
  'deferred,auto' + RTTI on -> merged unit likely needs deferred (= reverse source order) + RTTI on).
- LANDED 02723aaa AIObject_storecover += ChooseCoverPoint (368), fd4b3640 AIObject_obstacle += CheckFieldOfViewModeList (384, soldier's
  draft), 8b4aca90 ShellMenu_cardquery += MemoryCardLoadGame (1152; 'CShellMenu* const self = this;' alone fixed it; draft had a wrong
  sound-mode test and a placeholder CRC message - always run one_unit). 84.95%.
- ISirenStartup (is1/gen.py: const copies of p1..p5 subsets x orders, +const self): all 14. Not this-rank.
- CreateDestructable 57 -> 33 (rd): ds/cd_BEST33.cpp = const copies 'TriggerObject_struct* const c_trigger = trigger; int const c_param = param;' at the top (cs/cs.py tool: tries const self/param copies on any draft). Left: param r28 vs orig r30 swapped with the EH-new'd box (r30 vs r28); NewBox/NewScript factories, c_param positions: no gain.
- AIObject.cpp MERGE FEASIBILITY (aio/flagprobe.py): EVERY AIObject unit's functions still compile to 0 under '-inline deferred,auto'
  + '-RTTI on'. SAP_BEST (soldier/sap) under deferred differs ONLY in the string offsets swapped between Setup (+676/+776/+872) and
  Start (+964/+1064/+1160) = deferred codegen order is reverse source order -> put Start BEFORE Setup in the merged source.
  Block 0x8016f5c8..0x8016faac (AIObject.cpp .rodata): primes(108) | @1901 'CAIObject' (RTTI name, 10) | @2289 visionseek+users |
  @2342 visionsetup | @2725/@2730 lineofsight_mode setups | @2840 IsObjectInFieldOfView | @3170/@3171 SetupFieldOfViewCorpse |
  @3179/@3185 FOV setups (obstacle) | @4690-2 Setup | @4796-8 Start. Only Setup/Start use the base (...rodata.0+off) -> one unit must
  own the block from 0x8016f5c8: text span = AIObject_visionseek 0x80048134 .. Start end 0x8004cd34 (17 units: visionseek, visionsetup,
  lineofsight_obj, lookdirection, lineofsight_mode, lineofsight, fovcorpse, obstacle, arbitrary, coverwalkto, coveruseful, coverwalk,
  setcover, storecover, searchnodes, walkastar, patrolwalk) + Setup/Start (soldier/sap/SAP_BEST.cpp, 0 standalone). Sources in REVERSE
  text order, one CAIObject view, -RTTI on so the 'CAIObject' RTTI name is generated first (soldier's FireWeapon/Soldier_object
  44a2a210 recipe: key function declared first; the RTTI record .sdata 0x8034da58 + .data are owned by AIObject_ctor today ->
  strip them in the merged unit (stripped_sections like player_dtor) or extend the merge). fovcorpse has file_duplicates (keep them).
  NOT STARTED (too big for the rest of this run). Payoff 1556 B (Setup 644 + Start 912).

## HANDOFF (2026-10-29, nineteenth run) -- bigfiles
LANDED: a4bed99e player_health += CPlayerObject::OnCollision (776); 02723aaa AIObject_storecover += CAIObject::ChooseCoverPoint (368);
fd4b3640 AIObject_obstacle += CAIObject::CheckFieldOfViewModeList (384, soldier's s24 draft); 8b4aca90 ShellMenu_cardquery +=
CShellMenu::MemoryCardLoadGame (1152). 84.82% -> 84.95%. Lock released, tree clean (scratch only).
NEW TRICKS (in TRICKS.md): (1) 'T* const self = this;' first = param-rank fix (MLG one line, CCP); (2) an extra graph node via a
by-value inline param that names an existing value (Classify(self, other, crc, crc, ...)) stops a call-defined inline local from
falling to the lowest callee reg; inline locals: later-declared = higher reg; (3) scratch/par/bigfiles/ccp/hc.py = knob hill-climb
over '{{#group:a||b}}' templates scored by rd.py (found CCP's 23 -> 17 path); ccp/allk.py enumerates all combos; cs/cs.py tries const
self/param copies on any draft.
Best WIPs: CalculateLevelStats slm_BEST1.cpp rd 1 (shared zero only); CreateDestructable ds/cd_BEST33.cpp rd 33 (param vs EH-new box);
DrawFragCount dfc/z2.cpp fast 44 (unchanged); quick_sort_ qs2/t1.cpp 6; CUserMessageTable::Load 10; SetFinishedLevelData sfl30 11.
POLICY QUESTION: InitMultiplayer needs the player.cpp .rodata block incl. two orphan RTTI strings of a linker-stripped (likely local)
class 'GelziabarStone : dwi::IVisitor<ISceneNode>' -> invented stripped code? (see above). Not attempted.
NEXT: AIObject.cpp merge for Setup/StartAStarPathWalk (plan above; all units verified under deferred+RTTI on).
perm_dfc ends ~23:05 by itself (nothing real found).

## 2026-10-29 (twentieth run) -- bigfiles
- AIObject.cpp merge IN PROGRESS in aio/: build.py (head.h + stripped unit bodies, renames ren.py/ren_auto.py from genobj.py
  canonical CAIObject offsets, chunks re-sorted by original address descending = deferred reverse order), ms.py scorer
  (per-function diffs, merged.cpp default), views.py (CW offsetof probe of every class view -> views.tsv), blk.py CLASS (distinct
  class bodies across units). First build: 0 everywhere except fovcorpse's GetAIStatusVar/GetVarValue (+dup LOSValue), Setup/Start 6.
- FINDING: CW does not AUTO-inline a function that itself contains inline calls (even an empty inline ctor of a local):
  IsObjectInLineOfSight (CAIFilterRealVector3 unused/eye + CAIFilterRealPosition target locals) is only auto-inlined into
  GetVarValue when CAIFilterRealPosition AND CAIFilterRealVector3 have NO user ctors -> the original classes are PODs (no
  default/copy ctors, no dtor). Units that relied on ctors (pair copy ctor: lineofsight, coverwalkto, coverwalk, patrolwalk, sap;
  Position(x,y,z): walkastar/sap GetPosition; Vector3 dtor: visionsetup) must be re-expressed (soldier's recipe: 'V a; a = b;' via
  a pair operator=).
- *** LANDED 61f98ea8 *** AIObject_visionseek = 0x80048134..0x8004cd34 one unit (16 fragments folded) += SetupAStarPathWalk +
  StartAStarPathWalk(CAIObject*) (1556 B). 85.06%. Recipe (aio/): build.py + head.h + ren.py -> merged.cpp, finalize.py -> final.cpp.
  Fixes needed beyond the plan: POD Position/Vector3 (Vector3 keeps only the pair operator=, Position keeps visionsetup's z,y,x
  operator=); visionsetup's raised position read through 'const CAIFilterRealPosition* e = &eye;' (instead of a Vector3 dtor);
  coverwalk direction via matrix.GetFront(direction); walkastar/sap path-node positions through 3-float MoveTo/Copy overloads
  (emulate the ctor temp); IsObjectInLineOfSight 'target = position;' (Position::operator=) fixes BOTH the standalone and the
  GetVarValue-inlined register order (252-variant search of locals found none). Port: --file-duplicate __dt__9CAIObjectFv,
  __vt__9CAIObject, __RTTI__9CAIObject (+ externals 0x8034da58 + local_externals), ComputeStatus{Distance,IsEnemy,LOSValue,
  ThreatValue,Visibility}; then by hand: extab size 436/linked_size 468 + stripped_objects @1459 @1619 @1654 @2523,
  extabindex 492/540 + @1460 @1620 @1655 @2524 (port_unit drafted the dups' record slots as kept). extab_patches.json unit
  renamed searchnodes -> visionseek. one_unit VERIFIED.
- TRICK: CW auto-inline refuses a callee whose body constructs a local of a class with a user ctor (even empty '{}'); explicit
  inline functions are still inlined. Inline helper calls inside do NOT block (tested).
- CalculateLevelStats (slm_BEST1 rd 1, shared zero 'li r3,0; mr r29,r3'): tried in MH2 'hits = 0; location = hits', 'location = hits = 0',
  'hits = location = 0', 'location = 0; hits = location', 'int j = 0; location = j; hits = j', decl swaps, 'int location = 0' at the
  top + 'hits = location' (23), 'i = location = 0' (31): all rd 1 or worse (CW propagates the constant; soldier's lead: the mr appears
  only when the zero reaches both through a join/phi). Not landed. Chained-assignment forms would be policy-adjacent anyway
  (declined GetObjInfo '(int)(filter = 0)').

## HANDOFF (2026-10-29, twentieth run) -- bigfiles
LANDED 61f98ea8 AIObject_visionseek (AIObject.cpp 0x80048134..0x8004cd34 one unit) += SetupAStarPathWalk + StartAStarPathWalk(CAIObject*)
(1556 B) -> 85.06%. Lock released, tree clean. No new 'const self' uses, InitMultiplayer untouched.
Possible next in the same file: UpdateTargetPosition 0x8004e024 (2516, soldier notes 'UpdateTargetPosition' struct 53 / ut14 rawnr 344):
the POD-vector finding may help; the mode unit's invented inline IsObjectInLineOfSight(CAIObject*) could not be replaced by the real
auto-inlined one (IsTargetInLineOfSight 54, depth) - kept.
QUESTIONS: (1) the merged unit uses inferred 3-float inline overloads MoveTo(x,y,z)/Copy(d,x,y,z) for path-node positions (they
emulate the temp of the soldier draft's CPathNode::GetPosition ctor, impossible once the vector classes are PODs) - inferred helpers in
the GetLevel spirit, used at 7 sites; (2) the CAIObject destructor is compiled in this unit only as a file duplicate (to emit the
vtable/RTTI name the file's .rodata needs) - same mechanism as fovcorpse's ComputeStatus duplicates.

## 2026-10-30 (twenty-first run) -- bigfiles
- UpdateTargetPosition 0x8004e024 (2516, append to AIObject_setlocation): ut/UT_BEST15.cpp = struct 0 (only relocs), 15 FPR lines
  (all in loop 3 = the m_offsetFilter Update: orig kb f31/cx f29/by f28/az f27/ay f26, ours kb f26 lowest). From soldier ut15a (struct 53).
  Keys: (1) the m_rate.x RELOAD = the drx term reads the rate through a LOCAL REFERENCE 'const V& rate = m_rate;' (different base vreg, no
  load CSE) -> struct 53 -> 8; (2) half folded 'old *= 0.5f * step;' (constant hoist order); (3) loop cond 'for (; t; t -= step)'
  (fcmpu t,0); (4) 'ax = diff; ax *= ka;' in-place products; (5) both position copies + m_aimPoint copy via Assign (y-first);
  (6) offset block dx,dy,dz assigned x,y,z + 'dx *= d' in place; (7) hoisted decls: loop 'float bx, ax, bz, cz, cx, by, az, ay, cy, kb, ka;'
  with ka = m_a / kb = m_b locals, function top 'float length, d, dx, dy, dz, k, dt, oz, ox, oy, distance;' (dt f31 vs distance f30).
  Tools: ut/so.py (dependency-safe statement order + decl order hill-climb), pd2.py/pd3.py (decl-order), hc.py (knob templates), d.sh (sbs diff count).
- UT tidied: ut/UT_BEST15t.cpp (natural statement order except 'az = ...' before 'by = ...'; still 15). Landing builder ready:
  'python3 ut/mkfinal.py ut/UT_BEST15t.cpp out.cpp' = landed AIObject_setlocation.cpp + sqrtf/operator+=/Update/UTP/class members
  (other 4 unit fns stay 0 with it). Port = append UpdateTargetPosition 0x8004e024 (2516) to AIObject_setlocation (.text then ends
  0x8004e9f8; check extab/extabindex/.rodata/.sdata2 needs with port_unit).
- Loop-3 (m_offsetFilter) colouring: NO single-inline form found (so.py/so2.py statement+decl orders over loop body, caller top and
  offset block; pd2/pd3 decl splits incl. t/step; ka/kb as/not locals; vector temp forms for the third target (g1 68); offset as
  member inline = not inlined). Orig: loops 1/2 colour as our Update, loop 3 = kb on top + the rest reversed.
- EXACT ONLY WITH A SECOND INLINE: ut/UT_DUP0.cpp = sbs 0 / struct 0: the offset filter calls 'Update2', an identical copy of Update
  whose loop declaration is 'float bz, bx, kb, cx, cz, by, az, ay, cy, ax, ka;'. That is a duplicated inline that differs only in
  declaration order = a register device -> POLICY QUESTION, NOT landed. (Plausible real-code reading: the offset filter is a
  different filter class/template instance than the velocity/position filters - the filter classes are inferred views.)
## HANDOFF (2026-10-30, twenty-first run) -- bigfiles
No landing. UpdateTargetPosition: soldier struct 53 -> ut/UT_BEST15t.cpp struct 0, 15 FPR lines (loop 3 only); ut/UT_DUP0.cpp exact
with a declaration-order-only duplicate inline (policy question). Lane otherwise exhausted (CreateDestructable needs const param
copies = the open 'self' question; DrawFragCount unchanged 44).
