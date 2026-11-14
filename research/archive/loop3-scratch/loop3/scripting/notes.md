# scripting lane notes

## Session 12 (2026-11-12) - no landing; DoTargetCheck 61 -> 2 (natural) / 0 (unlandable view); work dir s12/
- DoTargetCheck: (1) the 'rlwinm r0 + mr r29' knot is the RETURN TYPE: 'bool DoTargetCheck(...)' (already declared bool in
  landed src/game/bsbifunc_schedules.cpp:45) + 'bool result;' + case 4 'result = GetScript(...);' (pointer->bool, no '!= 0').
  Case 1's compare then lands straight in r29 and case 4 keeps the orig's 'srwi r0; mr r29'. BEST61 + this = 2 real
  (s12/dtc/copy_bool.cpp), but that still needs the declined identity Copy().
  (2) Dead copies: CW deletes BOTH copies unless the float-copy target is pinned. Nothing natural pins it: named
  CAIFilterRealPosition(CVector3 v) local (real class; its 'x(v.x)' ctor matches AIFilter_queries), RS-style dead
  'a = otherPosition' with the weak by-value operator= (gives pair+pair, not pair+float), empty inlines taking it by ref/value,
  member calls, '&t;', 'T& t = *this' members: all deleted (s12/dtc/g1-g6). The ONLY pins found: a ctor whose body writes
  through a cast of this ('((float*)this)[0] = v.x', or 'float* p = (float*)this'). With that body on
  CAIFilterRealPosition(CVector3 v) and 'CAIFilterRealPosition targetPosition(otherPosition);' between the GetPosition calls,
  plus the bool fix: CODE EXACT 0 (s12/dtc/DISC0_ctorcast.cpp, fast score). NOT landable: the same body changes
  AIFilter_queries' matched code (frame 32 -> 48), so it isn't the original ctor (s12/viewcmp.py checks a view edit
  against a landed unit). RS (MOH3RDC.ELF 8010f434) has the same two dead copies: RS source = 'a = otherPosition' with RS's
  by-value operator= (temp via copy ctor). The FL analog of that is still unknown.
- BSFileReadClass (414): the whole final-loop diff is ONE colouring choice: the hoisted count load ('lwz r11,100(r1)'; ours r0).
  With count off r0 the orig puts m_states/list temps in r0, values in r12, and the 8-way copy needs r23. Inert: count copies
  ('int numStates = count'), shared 's' / while loop / 'count > s', 'pos' as a new offset var, index types, statement order of
  the lists read (s12/bsf/g1, g2: all 414). Worse: swapping in place in data (482), pointer walks for the list swap (471-478),
  state->m_lists for the base copy (445). Permuter can't parse it (templates).
All claims released.

## Session 11 (2026-11-12) - no landing; two drafts improved (work dir s11/)
- DoTargetCheck: 96 -> 61 real, struct 23 -> 3 (s11/dtc/BEST61.cpp, template bsbifunc_corpsesearch.json). The orig's two DEAD
  stack copies (pair copy A@8 of other@40, then float copy B@24 of A) come from: CVector3 WITH the user pair copy ctor
  '*(Pair*)this = *(const Pair*)&o' (that cast keeps a copy's stores alive even when dead) + a by-value inline
  'inline CVector3 Copy(CVector3 v) { return CVector3(v.x, v.y, v.z); }' + 'const CVector3& rb = Copy(otherPosition);'
  declared after 'CVector3 position; CVector3 otherPosition;', + session-soldier head 'ISceneNode* node; int result; result = 1;
  node = ...'. Frame 112 and all slots now right. LEFT: (a) the math reads rb (B@24) where the orig reads other@40; reading
  otherPosition instead makes CW drop B and A (rb unused -> eliminated, named refs never keep a temp); (b) 'rlwinm r0 + mr r29'
  vs orig 'rlwinm r29' (7 result spellings g7/: inert). Clue: g8/x2.cpp ('CVector3 b = Copy(otherPosition);', math on other)
  keeps A and B AND reads other@... but adds a third pair copy B -> b (frame 128). The orig = x2 without the b copy.
- BSFileReadClass: 708 -> 414 real, struct 7 -> 6 (s11/bsf/BEST414.cpp, template bsevents.json). Args loop
  'while (count-- > 0)' removes our extra 'mr r8,r9' count copy (126-form grid g1.py: statement orders x for/while/end-pointer/
  index loops; 'while (count--)' also removes it but gives beq for ble). LEFT: final loop count in r0 (orig r11), which frees
  r0 in the orig's copy loop and pushes parent/bytes temps up to r23. Inert: 96 header orders x count types x cast (g2.py),
  count declared at 7 top positions (t/).
- SweepAllExtents (193): 27 more forms (s11/sae/): 'start = OSGetTick(); ... OSGetTick() - start' scalars, late-folded uses
  ('int profile = 0; if (profile) OSReport(...)', 'if (this == 0)', 'for (i < 0)', empty loops), variable-index/pointer-walk
  arrays. All either DCE everything or keep the adds/stores. 'while (t0 + t1 + t2 < 0) {}' keeps first 80 lines exact (frame,
  r28 this, neg r31) but keeps the adds too. Not a DCE-pass-count effect (a dead final sum is removed completely).
- CheckForObstructions, CreateDumpObject: checked for the POD-vector symptom (fused x*x from a stack vector, field kept in
  f31/f30): none; both are pure GPR rotations. Not worked further. MechFollowPath: no new idea, not touched.
All claims released.

## Session 10 (2026-11-12) - CImpact::Visit LANDED (176f30c6, 2512 B, impact.cpp now one whole-file unit)
- Visit 598 real / 57 struct -> 0 (vis/MATCH0.cpp = src/game/impact_dtor.cpp). KEY (new, in TRICKS): CVector3 with NO
  constructors (aggregate + pair-copy operator=). The implicit copy is still lfd/stfd, but it doesn't take the source's address,
  so CW keeps start.y/z in f31/f30 across TestCollision and fuses 'x*x' (fmadds). Any ctor (even 'V() {}') scalarizes the vector
  instead; the user pair-cast copy ctor marks it address-taken. Probes vis/probe*.cpp (vis/probe.py dumps an object).
  Then: hit point as a local 'dx = end.x - start.x; ...; dx *= t; ...; hit.x = start.x + dx; target = hit' (TRICKS Lerp form),
  team test 'owner->m_team == node.AsPlayerObject()->m_team' (no local), 'CCameraView* camera;' declared at function top,
  final distance spelled inline in both sqrtf calls (no length local), CLine3(start,end) { SetSE(start,end); } + '~CLine3() {}'
  (the 5th extab action destroys the line with the weak __dt__6CLine3Fv). Unit = all of impact.cpp with explosion's flags
  (-inline deferred,auto, -RTTI on); weak CLine3/IVisitor dtor extab entries removed with "stripped_objects" in the manifest.
- MechFollowPath (83): coordinator's block-1 ideas all inert: s10/g1.py = 7 arg spellings ('*(*stack-(count-n))', '(*stack)[n-count]',
  sp[] local, '*stack + n - count', '(*stack - count)[n]', '*stack - count + n', '(*stack)[-(count-n)]') x 24 read orders of args 1-4 x
  speed float (in call / 'float fspeed = speed' after the if / pre-scaled) x block-2 locals-vs-direct: 1008 compiles, block 1
  identical in every struct-41 form (best real 81 is struct 68). C-style all-top decls + hill climb (s10/c0.cpp): inert. Speed/reverse
  control-flow forms (raw/speed split, 'reverse = speed > 1000', default-false, inverted test, s10/r*.cpp): 83-114. Parked.
- BSFileReadClass (struct 7, real 708): orig count in r11 (ours r0) gives the orig an r0 temp in the final copy loop and pushes one
  offset to r23; args-loop count/arg forms (bsf/a*.cpp) and final-loop statement orders (bsf/o*.cpp) inert or worse. Parked.
- CreateDumpObject (56): GPR param/object colouring; volume/node-insert inline helpers 72-94. No vector lever. Parked.
- PlayerScreenFlash: CColor without 'CColor() {}' 42 -> 39 real (psf10/BEST39.cpp, struct 31). Orig temps pair per colour (B c@8
  ret@12, A c@16 ret@20); ours [Bc, Ac, Aret, Bret]. Set sig/body orders (48), named locals (59+), 4-arg ctor, by-ref Set: no fix.
- CheckForObstructions (25): GPR rotation (sign r31, this r30 in orig), not vectors; sign forms inert. Parked.
- Structural scan of all lane drafts (sc2): only PlayerScreenFlash 26, DoTargetCheck 23, MechFollowPath 47 (index draft),
  BIFunc_AddBuddy 29 / CreateBuddyDataStructures 10 (buddy .rodata chain, gated by GetInitialBuddyOrders), BSInitBIFuncs 127
  (policy question) are structurally off; the rest are colouring.
All claims released.

## Session 9 (2026-11-12) - session-8 tricks on the parked rows (no landing, no improvement)
Work dir s9/ (generators g*.py per function, s9/mk.py body-file helper for MFP). All claims released.
- BIFunc_MechFollowPath (83, mfp3/BEST83.cpp): every accel form is canonical in CW - raw local + '(accel+1)*(accel+1)',
  separate accel1, accel++/+=1 late, const raw, raw read before the speed test (101), float faccel local, inline Square()/
  Accel()/PerFrame()/DegToRad() helpers, pointer locals for args 5-7, const/plain aggregate of args 5-8 (111+), float locals
  for value/speed (as RotateRel's 'float sub = *(...)'), count forms (short/ptr/ref/index/const, '*stack' local): all 83 with
  IDENTICAL code. The block-2 colouring is not driven by the accel spelling. Orig detail for the next try: the orig keeps the
  stack base in r0 and speed in r7 (ours r7/r10), converts speed late (xoris r30 at +66) where ours converts it first into
  r29, and its address temps for args 5/6/7 sit in r29/r30/r29. Probably needs a different block-1 shape, not accel.
- UpdateProximityTriggerStatus: index draft is already 9 (= German-loop pointer r29 vs orig r31, which is i's register).
  No bit-test/aggregate site (UsesTriggers bitfields already exact). German-loop const copy / scoped var / while /
  'copy = user' / 'T* const' inline param: 9. Dropping the player-loop 'copy =' gives 24.
- DisplayHint (6): orig hint is in objective's reg r30, texts = 'mr r31,r30' copy, nextIndex = hint+8 in place; ours
  hint=texts r31, nextIndex new r30. All 6 statement orders, no-hint/GetHint-twice/array forms, '*nextIndex' read,
  'int& ref' (18), no-texts, const event aggregate: 6.
- AddAmo (2): props pointer r4 vs orig r5. Compare spellings (rev, locals, const ptr, ternary, BeginUpdate's sibling
  'short reserve' form), m_type/weapon locals before the call: all 2.
- CStaticObject::OnCollision (3): const point aggregate inert; all 222 motion forms (x/y/z each 'a = p - l' or
  'a = p; a -= l', every statement order) x motion declared early/late: nothing below 3 (s9/soc/g3, g4).
- PublishEvent (3 here): const/by-value SItem copies in loop 1 (3) or loop 2 (46).
- CreateDestructable: 'CVolBox& rbox = *new (...) CVolBox' and '&*new' forms give struct 0 but box ranks r30 (47) like the
  plain direct form. No session-8 site.
- GetInitialBuddyOrders (7): const/by-value/ref BuddyInfo in DumpReady, const end iterator: 7. Inline helpers for GI's
  debug block (16: index reg + FPR mirror), search loop (305), DumpAttached+DumpReady wrapper (19: FPR mirror).
Conclusion: session 8's two tricks have no natural site in these seven; the remaining diffs are colouring knots.

## Session 8 (2026-11-12) - TriggerEnterAndLeaveMessages LANDED
- TEL 116 -> 0 (tel/MATCH0.cpp = src/game/proxtrig_messages.cpp, new fragment unit, .rodata pooled). Two keys:
  (1) light test '(entry & 0x8000) >> 15' in Enter AND Leave: fixes the 'mr r19 / clrlwi r16' order in all 4 merge copies (116 -> 108;
      '(entry>>15) != 0' also fixes the merge copies but turns the simple-loop tests into srawi.).
  (2) CONST acquire set 'const CWorldObject::SAcquireSet set = {acq[0], acq[1], acq[2], acq[3]};' - it alone produced the unfolded
      'addi r3,r17,4; lwz r3,0(r3)' single core load AND the 4-register template copy; then 'if (core->type)' instead of trigger->core.
  Dead ends (s8/): every pointer/ref/array/base-class/inline form of the core load folds (even volatile); a base-class pointer conversion
  unfolds but adds a null check. Tools: tools/sc2.py (real + structural per file, one compile), tools/dump.py (side-by-side range).
- BIFunc_MechFollowPath: permuter (mfp3/perm, -j2, ~1h): only junk wins (masking accel '& 0xFF', dropping path, double constant) -
  they all add a live value -> callee reg, confirming the orig keeps one more temp live in block 2. Also tried here: const locals
  (bool consts worse, others inert), '*stack' local, 'args = *stack - count' base, full parameterCount reads: all >= 83. Parked at 83.

## Session 7 (2026-11-12) - far-off drafts, structural work (no landing)
Tool: tools/sdiff.py <tmpl> <src> <func> <addr> <size> [-v] = difflib alignment of register-masked asm (structural hunks; sbs.py's
zip view inflates scores after the first extra instruction).
- TriggerEnterAndLeaveMessages: 308 -> 116 real (tel/BEST116.cpp; template proxtrig_point.json). LeaveTrigger now exact; all
  remaining lines are in the 4 inlined EnterTrigger copies. Keys: (1) send loops 'for (i = 0; i < count; i++, messages++) Send(...)'
  (orig increments i before messages) in BOTH inlines: -16..-24; (2) mixed access sites (tel/et2.py, et4.py random search over
  g_pTriggerObjects[index].core / trigger->core / a 'core' local for type, restoreEnter, enter, switch id, acquire): best = type
  check 'trigger->core->type', a 'MOH_core_Struct* core = g_pTriggerObjects[index].core;' local for restoreEnter cond / enter /
  switch id, table form for the rest; (3) acquire set 'int a = ..acquire[0]; int b = ..acquire[1]; SAcquireSet set = {a, b, [2], [3]}'.
  LEFT: (a) orig 'addi r3,r17,4; lwz r3,0(r3)' = ONE core load kept in r3 for type/restore/enter/id/acquire; we either load twice
  (r3 + r6) or (all-'core' forms, tel/cf_*.cpp, cf2_*.cpp) load once with folded offset 'lwz r6,4(r25)' = structure right but 1 insn
  short per copy (-4 total). Reference, pointer-to-field, inline getters, casts: all fold. (b) acquire-set scheduling: orig loads all 4
  template words into r6/r0/r10/r9 and lha c/d interleaved; ours pairs through r3/r0 (a1/a3/ptr/ba/cd2 forms, tel/aq_*.cpp: no fix).
  (c) 'mr r19,r18' / 'clrlwi r16' order at the merge-loop entry (Enter instances only).
- BIFunc_MechFollowPath: 105 -> 83 (mfp3/BEST83.cpp, template bsbifunc_vibration.json). Key: implicit or (bool) cast int->bool
  ('bool flag1 = (bool)*(...)') gives the orig's copy 'mr r5,rX' (the '!= 0' form never copies). Orig block 1 reads ONLY args 1-4
  (path, flags, speed) before the speed>1000 if/else; args 5-9 after it (RS MOH3RDVD has the same split). Orig needs one more callee
  reg (r28 = raw accel) and copies flag1 only (flag2 computed straight into r6). Random searches over decl order / inline-vs-local /
  float temps / flag forms (mfp3/gen*.py, ~6k variants): never both at once; best keeps two copies (len -1). Address-calc order in the
  orig's block 2 is -5,-6,-7,-9,-8 (value before target = call args right to left). Not tried: permuter (old base 5745 'hopeless').
- Not worked: CreateRootHierObject (claimed by mid2, never released), PublishEvent (pure 3-reg rotation, heavily worked),
  BSFileReadClass (struct 12, bigfiles), CImpact::Visit (598 real = colouring knot, start.y/z), CreateDumpObject (heavily worked).
All claims released.

## Session 6 (2026-11-11) - CStaticObjectFactory::Init structure + colouring (no landing)
Init draft (research/drafts/Init.800b254c.cpp) is already STRUCTURALLY EXACT: real 138 (score_draft), len 0, same stmw r20, extab
identical except DELETEPOINTERCOND regs (+ the approved 0x2500 pad class). Every remaining line is one callee-reg rotation:
ours count(+template new ptr) r31, pool new ptrs 1-3 r30-r28, template webs r27-r23, ptrs 4-6 r22-r20; orig template webs r31-r27,
count/template ptr r26, pool ptrs r25-r20. This is the SAME knot soldier session 22 documented (scratch/par/soldier/notes.md ~780-830).
- Work dir sinit/: sc.py (score), rot.py <cpp>... (ours>orig reg rotation summary), gen1.py (inline-helper splits), p1.cpp
  (pycparser-friendly Init: NewXxx(n) inline helpers + expanded DEF + C89 loop vars = 138), perm/ (autoperm dir, base 800).
- INERT this session too (identical regs): all-locals-at-top decl order, per-loop r, 19-loop as for, temps for new results,
  NewArray<T> helpers, count unsigned/long/register, register on r/flags, per-resource-loop inline helpers (InitStaticTemplate(r,count)
  etc., RS-style boundaries; code same, colouring same), -Cpp_exceptions off only as a diagnostic (6 regs). Whole-phase inline
  helpers are never inlined (e1/e2/e4: len -269..-564). 'int count = 0' BEFORE the new (t1) un-coalesces count from the ptr: worse.
- Diagnostics: cutting all pool news (x2) makes the template new group (n r27, flag r28, ptr r26) match the orig exactly; a
  'for (once...)' block around the pool phase (q4) sends the template ptr low (r23) but pool ptrs stay high. Real 'if (pools)' (q2): no change.
- Permuter (perm/, -j2, ~2.9k iterations, globals added to PERM_PRETEND for the type map): nothing below 800. Stopped.
- Variant motion-name spelling (the DECLINED '(char*)def + (i*24 + 0x38)'): with a LevelField {int tag; union {int i; float f;
  const char* s;}} view (as AnimObjFactory_head.cpp), '((const char**)&((LevelFileInfo*)def)->fields[i * 3 + 7])[1]' keeps 138
  (sinit/BEST_w1.cpp). Member forms fields[i*3+7].s / .w[1] / nested entries fold the +4 into one IV (494, len -2). POLICY
  QUESTION for when Init lands: is the w1 cast form acceptable? It is the only natural-ish spelling that gives the orig's 'li r30,56' IV.
- Fallback rsscan: no untried row with a usable RS helper (DoTargetCheck's RS IsNonAITarget is AI-targeting code Frontline lacks;
  BSFileReadClass already uses RS ChangeEndian templates; the rest were done by mid1/scripting).
All claims released.

## Session 5 (2026-11-11) - Rising Sun helper-boundary trick
Landed: 47e3ee8e NEW weaponfactory_create: CreateWeaponByCRC (520), 17 -> 0. Lookup = GetWeaponPropertiesByCRC inlined whole
(RS keeps it out of line); fragment defines it first as --file-duplicate (weaponfactory.cpp is deferred-inline). Draft removed.
RS trick results (scratch/loop3/scripting/rs/, scan tool rs/rsscan.py):
- BIFunc_GetInitialBuddyOrders: RS has NO buddy.cpp (no GiveBuddy/Dump*/BuddyInfo symbols in MOH3RDVD.ELF or MOH3RDC.ELF), so the
  trick cannot apply. Also tried: renaming DumpReady/GI soldier, node, 24 DumpReady loop forms (rs/gi/): all 7. Orig colours node
  before soldier in every DumpReady copy (UBP r28/r27, Died r29/r28, GI r30/r26); ours colours soldier first only in GI.
- UpdateProximityTriggerStatus: RS = UpdateProximityTriggerState(obj,a,b,c) + CheckFullTree(pos,data,a,b). Rebuilt that way
  (rs/u_*.cpp, rs/m_*, rs/n_*, bool/int returns): 13 at best (const copy needed); non-const 24.
- DisplayHint: RS version is a different design (hint index cycling, SoundAliasEvent); nothing to borrow. Stays 6.
- AddAmo: RS CPlayerWeaponObject::AddAmo just calls CWeapons::AddAmo; no helper for the CanAddAmo arg. Stays 2.
- CStaticObject::OnCollision: RS has no extra helpers (sqrtf/normalize inline); soc/n2.cpp = 3 (real 1: reloaded point.x f7 vs
  f8); 50 motion statement-order/form variants + Sub/Normalize/Dot members (rs/soc/): no gain.
- SetFinishedLevelData: RS has no ammo loop; 360 direct-loop decl orders worse (50). Stays 11.
- CreateDestructable (11), CThrownObject::Init (16), TriggerEnterAndLeaveMessages (308 at scratch/pt/BEST.cpp, index draft 804):
  RS boundaries no gain (see TRICKS).
- AllocateStaticObject: CODE EXACT apart from relocations; blocked by its .rodata block (owner Init, 371), not by code.
- BIFunc_AIStartCoverPointWalkToStored: 19 arg/return forms (rs/cw/) all 7 (orig flag r31 > stack r30; ours reversed).
All claims released.

## Session 4 (2026-11-11)
Landed (all into buddy_head, which now covers buddy.cpp 0x80056728..0x80057640; .rodata/.sdata2 pooled, globals extern, no __sinit):
- e47e2460 BIFunc_UpdateBuddyCheckpoints (900): code-exact draft; unit defines dwi_prime_list (head of the .rodata block).
- eff49cf3 BIFunc_UpdateBuddyProgress (1424, was 112): folds buddy_vec_info + buddy_veciter_info_dtor (units deleted).
- d1ba9bc7 BIFunc_BuddyDied (1244, was 16). Keys for all three in research/notes/buddy.md (iterator-by-reference inlines,
  DumpBuddyVector(vector&) extra inline level, GiveBuddyOrders(vec_iter&) overload + 'BuddyInfo* const' param).
- The buddy .rodata chain is strictly sequential: each next function needs all earlier string owners landed.
Parked:
- BIFunc_GetInitialBuddyOrders (next in the chain, gates BIFunc_AddBuddy 216 / AddBuddy 22 / Create 80):
  chk/GI_BEST7.cpp (= chk/g28/const.cpp, full-file draft) = 7 lines: only DumpReadyBuddies' node/soldier registers swapped
  (orig node r30 = DumpAttached's 'buddies' reg, soldier r26 = DumpAttached's soldier reg; BuddyDied pairs them the other way
  and matches). Tried: DumpReady decl orders (single + multi-function climbs, chk/hcm.py), raw node pointer, generic
  DumpBuddyList(list&), const locals, DumpBuddyVector decl climb, soldier/compare/first-block/scope/GiveBuddy call forms
  (96-combo grid chk/combo.py). All 7.
- list<BuddyInfo> range ctor 0x80058988 (lc/state.cpp = 3, unit would be like alarm_list_ctor: reserve as file duplicate):
  the BuddyInfo() temp's body-field stores land at 8/12/16(r1), orig 16/20/24 (orig keeps a full 20-byte BuddyInfo temp at 8).
  72 BuddyState/BuddyInfo ctor/copy forms + iterator passing forms: all 3. Same knot as __sinit_buddy_cpp / Create.
- list ctor knot detail (lc/f_s1.cpp, lc/gs_*): a FLAT BuddyInfo whose body writes through 'BuddyInfo* self = this' (or a
  BuddyState member default-constructed in place with a self-> body) gives the orig's 20-byte temp at 8 with path/moving/active
  at 16/20/24 (right), but then soldier/progress are stored too (2 extra stw at 8/12; 56 lines). The nested
  'state(BuddyState())' copy drops those stores but shrinks the temp to 12 bytes (3 lines). Need both at once.
- UpdateProximityTriggerStatus: natural form 24 -> 13 with a const copy in the inline
  ('inline void UpdateStatus(BSGO_Basic* user, bool a, bool b) { BSGO_Basic* const object = user; ...', upts/w_ccpy.cpp;
  'void* p' + cast is the same 13). Player loop now exact; left: the German-list loop pointer r29 vs orig r31 and the copy's
  'mr r3 / lwz r12' order in the three list loops. Decl climb, own/separate loop variables, accessor forms: no gain.
- AddAmo (24 accessor/const forms, amo2/), DisplayHint (helper by reference/pointer, const locals, decl climb; dh2/),
  CStaticObject::OnCollision (Subtract/SetVector/Difference helpers for motion/away, soc/): no gain (2, 6, 3).
- CreateCollisionVolume (257): objcreate .bss base (block already defined in objcreate_destroy.cpp, reuse it), but the draft
  also differs structurally (placement-new null check 'mr. r31,r3', load order). Not attempted further.

## Session 3 (2026-11-11)
Landed: DisplayPromptText (860) + AddPlayerAmmo (1028) as bsbifunc_prompttext / bsbifunc_playerammo (.bss pooled). Key: inlined
appendMessageToPrompt chunk written 'strncpy(cursor, text, end - text); cursor[end - text] = 0;' (pt/frag.cpp, pt/amo_frag.cpp).
- DisplayPromptInteger: claimed by systems3; with the same append form it is 1 line (pt/b3.cpp, scorer template bsbifunc_ai.json).
- BSInitBIFuncs: code EXACT apart from the .rodata-base immediates (init/BEST_k1.cpp = schedules unit + Init, 8 lines, all
  'addi rX,r29,<rodata offset>'). Fixes: short ChangeEndian through a 2-byte array; m_entries a BIFEntry* swapped via
  'template ChangeEndian(T&) -> ChangeEndian(unsigned int&) -> ChangeEndian(int&)'; entry swaps in 'SwapEntry(BIFEntry&)';
  separate loop counters; 'g_NumPlayers <= 2'. PARKED on a POLICY QUESTION: the file's ...rodata.0 head (648 bytes) is
  dwi_prime_list + strings of DisplayHint (unlanded, 6), ModifyTotalKillableEnemyCount, UpdateFragCount (MULTIPLAYER, unlanded,
  3936 B, @4557 "%s%s's %s"), AttachToSoldierEmptyHand, GetWayPointInfo, BreakPoint, AISetHearingRadius, BSBifuncInvalid (all
  landed). Landing = merge Init into bsbifunc_schedules + compile those 8 as --file-duplicate (debug_prof trick) + --pool .rodata.
  Needs: may unlanded drafts (one multiplayer) be compiled as file duplicates? Only their strings are placed.
- DisplayHint: 5 more forms (dh/a1-a5) all worse; stays 6. CreateDestructable: 120 top-decl orders + param forms (cd/p*, v5.py): stays 11.
- SetFinishedLevelData (sfl/: long i, self copy, static helper, inline health getter, wrapper inline): all 11. CreateWeaponByCRC
  (wfc/: key-less compare, p pointer, lookup init, decl order): 17. GetObjInfo hc8_best.cpp needs the soldier harness prefix (not tried).
Session 3 landed 9256cc51 (1888 B). All claims released.

## Session 2 (2026-11-11)
Landed: 13057d9d RotateRel (528), 471a7662 BSUpdate/CleanUpBIFuncs (264, .bss pooled), f07db75a BSResetBIFuncs (604, .bss pooled),
ea91be6d BSGO_Destructable::Destroy (344, .bss pooled), then displayPromptBuffer + DisplayHintText + DestroyBuddyDataStructures (1396,
.bss pooled; see the autopilot log; buddy_destroy's extab/extabindex sizes were trimmed by hand in the manifest).

WIPs:
- CreateDestructable: cd/a1.cpp = 11 (from 33): int z,y,x declared at the top. Left: param r30 vs box r28 swap (orig param r30, box r28).
  Tried: box decl positions, factories NewBox/NewScript, void* memory, param copy forms (const needed; non-const trigger copy -> 481).
  Permuter: pycparser can't parse this function (new, statics, C++ casts).
- MechFollowPath: mfp/v = 90-102; orig has an extra callee save (r28) and 'mr r5,r8' flag1 copy. Natural forms tried (v1.py, v2.py).
- DisplayPromptInteger pr/v/kc.cpp 15 (with the OLD displayPromptBuffer form; 21 with the landed one), DisplayPromptText pr/v/ne.cpp 20:
  arg reads 'int value = *(*sp - (count - 1)); bool show = *(*sp - (count - 2));', Text wraps append+display in 'if (text) {...}'.
  Left: callee-reg rotation inside the inlined displayPromptBuffer/append body (length r28, port r30, text r24, i r25 in orig).
  Deferred-inline compile (pr/tmpl_def.json, def0.cpp) gives the same 21/20.
- AddPlayerAmmo pr/amo1.cpp 49 (args w,a,n,m with bool notify; slot +100 as a real virtual): one more callee reg in orig (r23).
- BSInitBIFuncs: 225; also blocked by the .rodata base strings (+0x288 from the file's .rodata start); not pursued.
Parked from session 1: AddAmo, CThrownObject::Init, PlayerScreenFlash, TranslateFromScript (see below).

## Session 1 (2026-11-11, restored)
- AddAmo: 13 more forms (eval-order locals, out-of-line/inline CanAddAmo, int return, if-form, local w), all 2. scratch/loop3/scripting/amo/. Parked.
- CThrownObject::Init: copy-order/interleave/temp forms (ti/) all worse (26-171); 16 stays. Parked.
- PlayerScreenFlash: by-ref Set, direct member assigns, named locals, Set(from,to) in order, m_time last (psf/a-h): 84-94, worse than draft 42. Parked.
- TranslateFromScript: still 4 (zero-init colouring). The CSE'd 0.0f temp T always takes f1 and coalesces with the FIRST member
  initialised; orig has T in f3 coalesced with x while z=f1, y=f2. Tried all member init orders, chained zeros, copies, zero local,
  separate float locals, inline Set/Zero/ctors (tfs/). Untested idea: make T interfere with y and z but not x.
- __sinit_buddy_cpp: only 2 extra stores 'stw r6,8(r1); stw r6,12(r1)'. Known knot; not attempted (coordinator: leave alone).
Claims released; session ended after 9 functions in 5 commits.
