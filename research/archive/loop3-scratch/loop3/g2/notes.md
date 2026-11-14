# g2 (day 2, group2.tsv) notes

Tools: tools/d.py KEY [draft|-] [ctx] (diff only + SCORE, len fixed), tools/sc.sh, tools/var.py KEY base variants.py,
tools/unitchk.py UNIT [src] (score every function of a landed unit against a modified source).

## Rows
- AddAmo (2): props pointer r4 vs r5. Inert: type/weapon/bool locals, CanAddAmo spellings (local p, reversed compare,
  max/res locals, GetMaxReserve, if-return, IsFull negated, unsigned long, int return), (short) arg, != 0. Parked.
- SNDPKTPLAY_start (2): loudest-voice IV 'addi r7,r29,0 ... lha 4(r7)'. Byte-offset forms (E/M..Q) give lhax + addi-in-loop (16),
  others 129-131. Parked (40+ older forms).
- CStaticObject::OnCollision (3): line 28 is a relocation; real diff = point.x reload f7 vs coalesced f8. Not retried (222 forms).
- AnimStStartState (10): orig compare is 'state == (ushort)id' (cmplw state,id) - the draft's 'id == state' only scores lower.
  Registers: state load r3 / id clrlwi r0 in orig; ours the reverse in every form (loop forms 231, _Same/_Uses
  inlines in all arg orders, ushort copy, int casts, nested !=). Parked.
- TLT_LoadFromRepository (13): iterator class (as PathNodeList), find single-exit, end local, &m_data[] spellings,
  late it, end-first, == / reversed != : all inert or worse. Parked.
- MOHFL_VD_create (17): ctor locals: no-local 182, height first 24, int/decl-late 17. Parked.
- AddBuddy (22 -> 6): push_front through the iterator object 'list_node<T>* pos = begin().m_node;' fixes the r5/r6 node/pos
  swap (push_front is not instantiated in the landed buddy units, so their copies can take the same body).
  Left 6 = the inlined GiveBuddyOrders' x/y/z FPRs (orig x f29, z f31). GiveBuddyOrders decl order x,y,z gives AddBuddy 0
  but breaks landed buddy_head (UpdateBuddyCheckpoints/Progress 6 each): both contexts need opposite orders = the declined
  per-fragment inline difference. Searched: 24 decl perms x 4 init forms of the inline (both scored), 24 x 5 x 2 forms of
  AddBuddy's own copy, call-site spellings (local ptr, vec_iter, m_data + i): none gives both 0. QUESTION-free park.
- CQuaternion::Multiply (27): real3 ohc statement/decl climb 1500 iters -> 26 (ugly order, not kept). Natural CVector3
  forms (Cross/Dot/Scale/Add on 3-float views, scalar one-liners): 41, no f31 (orig needs 14 live values). Parked.
- CBulletDecalManager::Init (31 -> 28, scratch/loop3/g2/bd/BEST28.cpp): name read as pair 1 of the record counted from the
  resource data ('((DecalParam*)resource->data)[(unsigned int)(i * 6 + 1)].name', DecalParam = landed decal_reset view
  + an inferred const char* union member) gives the original's second IV (li 8, +4); angle via m_params[i * 6 + 5].
  Left: resource ranks lowest (orig r31) and the '...rodata' string base offsets (224 vs 108: pool, not code).
  Decl orders of resource/i/name/size (all 24+6), init-at-decl, for-int, warning block as inline: inert.
- random (82 -> 79, scratch/loop3/g2/rnd/out/bt.cpp): same algorithm as iSNDrandom (group3): addc/addc2 inline form.
  Left: orig first step 'addi r7,r4,0' (carry-in 0 NOT folded: looks like a late constant, e.g. an unrolled loop) and
  the final 'addi r0,r3,1; addi r3,r3,1' (result trick did not take).
- SweepAllExtents (192, len -10): the orig keeps 't = -OSGetTick()' negs in r29-r31 (never read) while every '+=/-='
  add is gone; ours (array, scalars, struct, int/unsigned/long long/float, (void) uses) drops all of them. Looks like
  a late dead-code pass in the original that removed only the adds; no source form found. 'sort()' inline body and
  calls otherwise consistent (check_draft 96.9%/100%).
- SNDPKTPLAY_start (2), more forms: loudestvoice(&arr[slot]) (A) = exact except the missing 'addi r7,r29,0' (CW emits
  'add r7,r0,r29' directly; 1 instruction short, 129 by shift). Byte-offset trees base + (off + i * 2) + 4 (x4/x6/x9,
  t1/t2/t5 with a char* base param) = 9: 'mr r7,r29' int IV + lhax; caller-level loops reusing the function's i
  (caller/c3..c6/d1..d4): 15-16 with other colours. A sub-struct at offset 0 (hdr) folds too. i/best types: inert.
  Reading: the original's IV init keeps an unfolded '+0' (late constant), like random's 'addi r7,r4,0'.
- CPartBin::Link (33): all 17 scalar locals hoisted + 592-step declaration-order climb (tools/dhc.py): 33 everywhere.
- CPathNodeList ctor (99): 'CPathNodeList* const self = this' (with/without s_pnlCurrent = self): 99.
- MOHFL_VD_create: placement '::new (rcmp_sys.m_alloc(...)) GCHW_VD' (new temp ranking): 158. No ctor symbol in the
  original (inline).
- CheckForObstructions (25): tail (sign + three TestLine tests) as a static inline helper stops inlining (depth): 596.
- BSFileReadClass (414): the header swaps through a 'SwapHeader(BSClass_struct* cls)' helper give the original's
  registers (cls r4) but move the swap slots down one depth group (612); the '(*result)->' spelling gives r4 too but
  reloads each time (899). One fewer callee GPR (r24 vs r23) in the unrolled list loop: the outer count sits in r0
  in ours (orig r11); count spellings (n copy, decl at top, in-place swap, unsigned long, s first) inert.
- aim_cb (61): plain x,y,z order 68; Scale/Sub/Add member forms 68/373; decl orders of c/s/t/a (24) all 61.
- UpdateBoundingBox (56): not retried (heavy FP knot, vec5/fpr2 notes).
- Init__NotTheVirtualOne, resource rank: 'm_resource = TLT_Find...(); resource = m_resource;' (bd/out/back.cpp) gives
  every callee register (resource r31, i r30, this r29, IVs) but reloads m_resource (14 incl. 7 string-offset lines).
  Defined from the call result (BEST28, chains 'resource = m_resource = call' either way, value-returning inline,
  void* cast) resource ranks lowest. Same family as MOHFL's new temp and CheckForObstructions' TestLine result:
  in the original a value copied from a call's r3 ranks high, ours low.
- aim_cb 61 -> 59 (aim/FINAL.cpp): PitchRotate(forward, up, c, s, tan(a)) inline for both copies + statement climb
  (spec2.py with explicit deps). t now f8 (orig f6, was f10). Param orders (t, s, c) with cos/sin/tan as args: 68;
  c/s/t computed inside the helper: 68-325. Copy 2's c/s (orig f29/f30) still swapped.
- _AnimTranDoTransition: without the 'array' local (na) the params rank exactly as the original (to r31, channels,
  object, slot r28) but the address is not kept (391); 'const array' drops array to the bottom (250); decl climbs on
  the hoisted const form: 249. Array placement (after GetTransList, in the if, passed to GetTransList): 419-426.
- MemoryCardBootUpPortCheck 1183 -> 1023 (len 2 -> -5... see mc/): ShowFormatting's letter as the same expression
  as the caller's ('char letter = 'A' + (slot != 0)' inside the inline; was "'B' - (slot == 0)"): the original
  computes it once (r15, extsb later). The unused 'CShellMenu* const self = this;' is dead (no uses). Left: ours
  spills 'result' (stb 1392) and the font data pointer (stw 1396) where the original keeps r14/r18; 'this' ranks
  lowest (r15, orig r31). Decl-order climb running (mc/dhc1.log).
- CQuaternion::Multiply: 'Add3(cross, x * q.w, q.x * w)' value-returning inline gives the original's exact opcode
  sequence (every instruction kind in place), colours off (33): statement/assignment orders (576), Add3 parameter
  orders x statement orders (144), const copies of q.w/q.z/q.y/q.x (all subsets/orders up to 3): 32-33.
  The old draft (Mul + rtc/rtd copies) stays at 27.
- CPartBin::Link: the 'CVector3 tmp = lightPos; position = tmp;' copy chain is load-bearing (200 without; by-value
  InitLightPos/GetLightPos forms 80-242). It is a copy chain that steers slots: flag before landing.

## Status at stop (07:35)
No landings. Draft commits: 9903673d (AddBuddy 22->6, Init 31->28, random 82->79), 4788d1e9 (aim_cb 61->59,
Init 28->14), cde97e72 (MemoryCardBootUpPortCheck 1183->1028). Skipped as declined: InitMultiplayer,
DrawTextCentered, FlagAsOccupied. BIFunc_MPAddPlayerHealth (multiplayer, 256 + len 14) only inspected.
Nothing held, nothing running.
