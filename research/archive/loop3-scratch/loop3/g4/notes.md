# g4 (2026-11-13, day 2 group 4): re-attack with the 11-12 rules

Tools here: d.py KEY [draft|-] [ctx] (sbs diff of an index.tsv row, compiled from any file; lis/addi 0 and (0) lines are relocs),
obj.py KEY [draft] (our objdump with branch offsets). Probe dirs per function (xalf, str, dcb, dh, cdo, rsf, tcb, upt).

Result: no landing, no draft improved, nothing committed. Every row below is unchanged in research/drafts.

- SFILTER_unpackxalf (1): the hole is NOT padding: frame size is equal (176) in both; locals are sorted by size, and the orig has a
  third 8-byte object between saved@24 and discard@40 (ours puts the 8 spare bytes above discard). Named XAFSTATE locals without an
  address scalarise (no slot): block-scoped saved (v1/v2/v3), extra named copies (v4-v6), unused decls (u1/u2), struct-assign forms
  (v7-v10, same code). Slot-creating forms: an inline returning XAFSTATE by value ('saved = LoopState(state)', r1: discard@40 but
  extra copy code) and an inline GetState wrapper (c4: discard@40, temps 16/24, saved@32, +2 instrs). Lead: the orig's X is created
  BEFORE saved (reverse creation order), i.e. a named or early temp 8-byte object that leaves no code.
- SNDtimeremaining (3): voice r31 vs hoisted sndgs base r29 (ours base r31). Decl orders, a gs local, VoiceRecord/getframe inlines,
  record at top: all 3 or worse (str/).
- death_cb (5): brchk2's ble 126/125 is alignment noise (targets identical). Left: posture/location loop IV-init order (orig inits the
  kept/located counter + its *4 store IV before the i pointer). 6x6 counter-spelling grid (for-init, comma, hoisted, c local) and
  variable reuse across loops (count/kept/located): inert or worse (dcb/).
- DisplayHint (6): orig hint ptr in r30 (objective's reg), first texts[next] load through r30, 'mr r31,r30' copy for the loop, then
  r30 += 8 in place for nextIndex. 18 statement-order/form variants + 48 decl orders without a texts local: inert (dh/).
- CreateDumpObject (56): Rising Sun has CDumpObjectContainer::AddDumpObject/AllocateDumpObject/GetAvailableObject (out of line
  there; Frontline has no such symbols, so inline). Inline-member form (cdo/a1.cpp): position/local/kind/extra colour right but
  type/value climb above object (69 filtered vs base 56); with RetV 39 (base+RetV 31). Param order of AddDumpObject inert. A
  forwarding overload AllocateStaticObject(int) = base (56). Not a landable improvement.
- SFILTER_rsf (5): 7 position-recompose spellings (rsf/): all worse; knot is the 0xffff mask constant created before the fraction load.
- TestCDBBranchNodeCollision<CLine3> (1): __construct_array ctor/dtor hi-part registers. Implicit/explicit SLocalFrame ctor/dtor
  combos (tcb/): inert. Orig emits the dtor (0x80070d48) before the ctor (0x80070d84).
- ResetShell (18): brchk2 branch flags are alignment noise (all case targets equal); left the 0/1 r3/r4 swap (zero3 already
  exhausted) + r2 reloc lines.
- UpdateProximityTriggerStatus (9 with the declined 'copy = object'; 24 plain): orig g_scene base r30 > data r29 > object r28;
  plain forms (no object local, player hoisted, user reused): 24 or worse (upt/).
- Not reworked (blocked or exhaustively worked before, see research/log): AllocateStaticObject (rodata block owned by Init),
  STREAM_kill (only the declined void** validhandle keeps header in memory; a reference param would change the landed fragment's
  shared inline), TLT_LoadLevelContentsFile (declined dead branch), DoTargetCheck (declined Copy), BIFunc_AddBuddy and
  GetInitialBuddyOrders (buddy.cpp .bss + BuddyInfo partial-SROA knot), rcmpbase rows (wait on the DECODER ctor, group2/3),
  GetExtents / FindIntersection / box-triangle / DMMorph / MIX_audioslice / SNDVOICEI_alloc / decodexac / CScene::Draw (long FP or
  GPR colouring knots, brchk2 clean, no rule fit found in the time), BIFunc_UpdateFragCount (multiplayer, real 1016).
Nothing running, nothing held.
