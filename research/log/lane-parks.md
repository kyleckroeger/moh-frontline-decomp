# parks notes (2026-10-23)
## Landed
- 621db09f DrawHitStatus (ternary with coalesced-copy first operand => 'bgt L; b skip').
- a78a4f6a AnimFileGetFrame (inline helper with length param; 'length += skip').
## Tricks
- 'bcc L; b M; L: <load>' = conditional expression `x = c ? x_default : other` where the first arm is a copy CW coalesces away (block becomes empty, branch kept).
- Reassociation of 'a + b + 31': pass a as a parameter to an inline and 'a += b;' in its own statement.
- f35dfe3c ForceUpdateProximityTriggerStatus (hoisted load order via decl/assign split; null check inside inline).
- 3afc4595 CVolFiber plane test (long local vs int member keeps the constant in a register for the compare).
- c4fe624e DrawRadar (inline Dot results into locals).
- c4f12901 SNDAEMSI_reordermodule (inline locals hoisted, assigned later).
- 99800f18 _AnimIntfOpcodeProcess (result=1 inside an inline before the call).
## Tried, no gain
- playpatch (li/mr shared zero): int/long/uchar 'last', chained assigns.
- DMClusterSkelTraverse: angle temp/inline/casts.
- RCMP DECODER ctor: ctor body/init orders.
- UISExecuteFnc: result/frame forms.
- CreateImpact: (log ideas only).
- FindAllMG: 36 decl x assign orders all 22.
- HandleButtonPress: dead-store trick fixes the double m_selected load ('char c = visual[sel]; visual[sel] = c; curr[sel] = c;') and '>= 6' fixes the compare; left: 0.0f/magic constant load order before the 135 loop. Draft scratch/par/parks/hbp2.cpp (2 lines). Permuter 20 min nothing.
- CParticleSystemManager::BeginUpdate: CODE MATCHES (rawcmp clean) with a dead self-store 'T* next = node->next; node->next = next;' in Free (CW drops the store but it blocks the CSE, so node->next is reloaded in the unlink loop). Draft scratch/par/parks/psu_selfstore.cpp. NOT LANDED: policy question (no-op self-assignment, like the approved '(void)0;').
- HandleButtonPress: same self-store trick for the m_selected reload; still 2 lines off (constant load order).
- 04c4931a reordermodule2, 4f9fb859 particlesystem BeginUpdate (self-store, approved), d8b88848 updateplayerbank, 62d761bc SNDAEMS_streaminit.
## WIPs
- SNDI_precalcaztospkrvol: scratch/par/parks/sat3.c struct 28 (from 54): index sndaztospkr[i][k] directly (no levels pointer) gives orig loop unroll shape; left registers + frame.
- SNDI_validrendermode: orig 'bne body; b skip' at the 0x71c/0xe0 tests; not reproduced (if/else/&&/goto tried).
- SNDPKTPLAY_start: 'addi r7,r29,0' IV init not reproduced.
## Overnight: claiming from scratch/par/untried_now.txt (CopyStateList, CopyAnimList, UploadFramesToARAM, CopyEmotAnimation)
- a2dbcadd SNDPLATFORM_playtimbre (new; Ghidra + hill2.py decl climb with pair swaps).
- f5fb3e87 SNDPLATFORM_packetplay (fillbuf DMA helper split).
- ee39080d SNDDRV_DSPMixerUpdate (SDK mix.c model; ~0x masks).
- 81083952 SNDI_memcpy (smemcpy whole file; while(n>=k){...;n-=k} + int temps).
- a4282442 stretchframesneeded.
- WIPs: SFILTER_echo scratch/par/parks/echo2.c (4 raw = mr r6/r7 order only; v decl 'v0, v3, v2, v1' fixed FPRs); SFILTER_rsf 4 raw; SFILTER_unpackxaf 7 raw (xafb.cpp); iSNDserve 18 raw (half-compare operand roles); PATH_fade (shared constant 1/0 registers); SNDPLATFORM_downloadcomplete dlc.cpp (self-store reload helps, inner loop colouring left); CopyAnimList family (alignment coalescing).
- e6f5aa11 SNDBANKI_playtimbre (hill2 + int option locals + reuse i).
- UploadFramesToARAM scratch/par/parks/up6.cpp: 3 lines (size/lead r30/r28 swap only). Keys: 'length = lead + size; length = (length+31)&~31;', size unsigned long, final 'aram + lead + size'.
- SNDVOICEI_free sal2.c best 46 raw with explicit gs var (volatile colouring).
- ef42e486 PATHX_createtrack (param reuse as cursor/remaining; inline file loader).
- 4230f6d8 PATH_addmapfile (inline pfinit + approved self-assign + statement splits). TODO: Game.md path_createtrack row still has the stale 'drafted ... register allocation differs' sentence - fix in next landing.
- NOTE for soldier lane: scratch/perm/mspf/output-0-* (BIFunc_MechSetPathFlag) reached permuter score 0 with 'int new_var; new_var = mode; if (new_var != 1)'; scratch/perm/stm/output-0-1 (SetupTargetMatchList) used a self-assign + comma/empty-if junk.
- SNDBANKI_asyncserver scratch/par/parks/srv6.c raw 23 (needs tmpl_sbasync_noauto.json: -inline noauto; sbasync_done still matches with it). Keys: retiredownloads inline, 'resolved = i = 0' shared zero, header before i.
- ee5ac777 HandleButtonPress (self-store + dead 'value2 = 0.0f' init).

## HANDOFF (2026-10-24, context reset)
Landed this session (17): 621db09f DrawHitStatus, a78a4f6a AnimFileGetFrame, f35dfe3c ForceUpdateProximityTriggerStatus,
3afc4595 CVolFiber plane test, c4fe624e DrawRadar, c4f12901 SNDAEMSI_reordermodule, 99800f18 _AnimIntfOpcodeProcess,
04c4931a reordermodule2, 4f9fb859 CParticleSystemManager::BeginUpdate, d8b88848 updateplayerbank, 62d761bc SNDAEMS_streaminit,
a2dbcadd SNDPLATFORM_playtimbre, f5fb3e87 SNDPLATFORM_packetplay, ee39080d SNDDRV_DSPMixerUpdate, 81083952 SNDI_memcpy,
a4282442 stretchframesneeded, e6f5aa11 SNDBANKI_playtimbre, ef42e486 PATHX_createtrack, 4230f6d8 PATH_addmapfile,
ee5ac777 CDualConfig::HandleButtonPress.  (65.02% -> 69.15%)
Best WIPs (scores = ss.py raw / sbs lines):
- SNDBANKI_asyncserver scratch/par/parks/srv6.c raw 23 (tmpl scratch/par/parks/tmpl_sbasync_noauto.json = -inline noauto; sbasync_done still matches with noauto). Left: first 'resolved' store via r0 vs i-reg, one store order.
- SNDBANKI_asyncservice scratch/par/parks/sbs2.c (4 lines, 'load' r28 vs r30) - lands together with asyncserver (prepend to sbasync_done).
- UploadFramesToARAM up6.cpp 3 lines (size/lead r30/r28).
- SFILTER_echo echo2.c 4 raw (mr r6/r7 order). SFILTER_rsf rsf.c 4 raw. SFILTER_unpackxaf xafb.cpp 7 raw.
- iSNDserve srv.cpp 18 raw (signed-compare operand roles). PATH_fade pf.c (shared constant regs).
- SNDPLATFORM_downloadcomplete dlc.cpp (self-store reload helps; inner loop colouring).
- SNDVOICEI_free sal2.c 46. SNDI_precalcaztospkrvol sat3.c struct 28. CopyAnimList family ca*.c (align coalescing).
- Update_Picture_Buffers gp.cpp (Data() r7 vs r6 per plane); Decode_Picture matches (needs Update for contiguity).
- HandleAnalogButtonPress (884, next after HandleButtonPress, reaches ConfigGC_init): drafts scratch/game/configgc_*_wip.cpp, untried this session.
Tricks found (also in AUTOPILOT log lines [parks]):
- 'bcc L; b M; L:' = ternary whose first arm is a coalesced copy.  - '+k before +x' reassociation: split statements / inline param.
- unfolded constant compared from a register = long local vs int member.  - inline results into locals (Dot) fix load order.
- inline locals hoisted then assigned; scratch/par/parks/hill2.py (move+swap hill climb of a decl block) is very effective.
- 'li a,0; mr b,a' = two strength-reduced IVs (write i*k) or a reused loop variable; 'x = y = 0' shared zero.
- parameters reused as working vars (cursor / remaining size). - int temps for 4-copy groups; while(n>=k){..;n-=k} loops.
- approved no-op self-assignment forces reloads/store order (comment each).  - masks as '~bit'.  - dead init 'v = 0.0f;' before a loop fixed constant load order.
- always rawcmp before landing (fast.py/ss.py hide branch targets and string-address scheduling).
Claims held (parks in CLAIMS.txt): all listed parks rows; release freely.

## Session 3 (2026-10-24, fresh parks)
- a8d4aa5d HandleAnalogButtonPress (new draft, matched in 3 compiles: dead 'value = 0.0f;' before both polls).
- SNDBANKI_asyncservice: sbs4.c = 1 real instr (only 'mr r29,r31' i=0 vs 'li r29,0'): retire loop expanded MANUALLY with function-level 'load, i, read' decls -> load gets r30 (function-scope locals seem to interfere with each other; inline locals do not). Inline-local i gives the mr but load r28/r29. Not yet solved.
- SNDBANKI_asyncserver: srv8.c (drop the inner 'resolved = i = 0') = only MISSING the second 'stw r30,8(r1)'; srv9.c (+ 'for (resolved = i;') constant-propagates to li. Permuter perm_srv (base srv9, 365 -> 140).
- UploadFramesToARAM: 'unsigned long lead' puts lead in r28 (right) but size r29/length r30. Orig expression is 'size + lead' (r30 = size). Permuter perm_up gave nothing in 10 min.
- SFILTER_echo: copy order follows reverse param order, register follows param order -> can't get both with param swap; echo3.c (params src,dst) = order right, regs swapped.
- f7d8333b LowLevelDecodeChunk (new; extab needed ~TDLNODE()/~FRAME() declared non-inline + MPC_FRAME::operator delete declared).
- ARAM_alloc_node (712, file-local, PREPEND to aramalloc_alloc as static): scratch/par/parks/aan4.cpp = structure exact, only register naming: orig manager r29/size r30/pool r31/&sentinel r28; ours sentinel r31 (manager r28, size r29, pool r30). ARAM_NODE is 28 bytes (BPool node size). Keys: search loop with the found-handling INSIDE the loop body ('for (node = head;; node = node->next) { if (node->size >= size) {...; break;} }'), '&sentinel == node', 'address = node->address + (node->size -= size)', BListPop with 'BNode* node = list->head; if (node == list->tail)'. Permuter perm_aan.
- Decode (CEAXABLKDecf) 2 raw, GCSW_VD_create 13 raw (4 real), PATHI_choosebeat 13 raw (node/length swap): hand variants this session, no gain.
- ARAM_alloc_node update: aan10.cpp (size via inline 'AramAlign(manager, size)' -> size gets r30 as orig) raw 47 = 43 reloc-free? (struct 4 = relocs only); left manager r28/pool r29/&sentinel r31 vs orig r29/r31/r28. Orig ALSO puts '&pool->free'/'&pool->used' list addresses in r3/r5 where ours use r0 - same symptom as ARAM_NEW_pool (anp2.cpp, now 14 raw = node r6 vs r5 + list addr r5 vs r0 only, with BListPop 'BNode* node = list->head; if (node == list->tail)').
- MEM_allocaligna: old draft scratch/lib/memalloc.cpp 90 raw; orig hoists flags&0x400 (spilled to 8(r1)), &0x200 (r14), &0x100 (r27); __save_gpr r14..r31. Not attempted further.
- SNDPLATFORM_outputset (2160): Ghidra shows the SNDSYS_setopts-style member-wise struct copies (7-byte __copy) - skipped.
- decodemut: scratch/par/parks/mut2.c = 'q = &e[6];' moved INTO the loop (after 'lag = ...') -> the hoisted addi order is right; left only q/o register swap (orig q r26, o r27; ours q r27, o r26): 16 raw = 5 real lines. hill2 decl climb: no single move/swap helps. Permuter perm_mut running (base 35).
- PATH_fade / iSNDserve / Decode / echo / rsf / GCSW_VD_create: more hand variants, no gain (iSNDserve: '(hi << 16) + lo' fixes the add operand order but regs then rotate).
- decodemut CODE MATCH (rawcmp clean) with a one-element local array for the output offset: scratch/par/parks/mut_o1.c ('int o[1];' ... 'o[0] += 108'); found by the permuter as 'int new_var[2]' packing v and o. CW promotes the array element to a register LATE, which gives the orig colouring (q r26 / o r27). NOT LANDED: policy question (artificial array, like the junk class). Natural forms tried: block-scope o, o via inline pointer/reference, v reused as o, decl climb - no.

## HANDOFF (2026-10-24, session 3 end)
Landed this session: a8d4aa5d CDualConfig::HandleAnalogButtonPress (884), f7d8333b MPC_CODEC_INTERNAL::LowLevelDecodeChunk (600). (69.15% -> 69.62%)
Best WIPs:
- decodemut scratch/par/parks/mut_o1.c: CODE MATCH but only with 'int o[1]' (policy question). mut2.c = natural form, 5 real lines (q/o swap).
- SNDBANKI_asyncservice sbs4.c 1 instr (mr vs li for i=0); asyncserver srv8.c = missing second 'stw r30,8(r1)' only. Land together prepended to sbasync_done with tmpl_sbasync_noauto.json.
- ARAM_alloc_node aan10.cpp (register rotation only), ARAM_NEW_pool anp2.cpp (node r6/list r5 vs ours r5/r0).
- DebugProfTimerInitAll (228, append to debug_prof_register): scratch/par/parks/dpr.cpp, only the table pointer r4 vs orig r5 (orig leaves r4 unused); inner j-loop form gets r5 but hoists the -1/0 constants.
- UploadFramesToARAM up6.cpp 3 lines; Decode dec.cpp 2 raw; echo2/echo3 2 instrs; rsf 4; PATHI_choosebeat cb.cpp node/length swap; GCSW_VD_create gc.cpp 4 real; iSNDserve srv10.cpp regs.
Claims held: see CLAIMS.txt (parks); release freely.

## Session 4 (2026-10-24, fresh parks)
- 0ebea648 aramalloc += ARAM_NEW_pool (anp3.cpp): named node pointer for the list head in AramListAppend.
- ARAM_alloc_node: aan19.cpp raw 29 = only callee-saved rotation (manager r28 vs r29, sentinel addr r31/r29 vs r28, low-path pool r29 vs r31). Sentinel inserts need the SPLIT decl form ('ARAM_NODE* last; ARAM_NODE* pos = ...; last = pos->prev;'), AramRecord's used-list append the plain form (AramListAdd). Permuter perm_aan2 running.
- SNDBANKI_asyncserver: srv13.c = structure exact (resolved block moved into inline finishload() with own locals; inner 'resolved = i;' becomes stw r30 as orig). Left: slot r28 vs r31 and the strength-reduced offsets pointer r31 vs r28. decl perms/hill2 no gain.
- SNDBANKI_asyncservice: still sbs4 (1 instr). decl perms, block scope, headerread inline: no gain.
- DebugProfTimerInitAll: 10 more loop forms all r4 (orig r5).
- 196c9c51 sbasync_download += SNDBANKI_asyncserver (srv21.c; unit now -inline noauto). asyncservice can append to sbasync_download next (still 1 instr: sbs4.c).
- b358dfe1 NEW path_queuenode: PATHI_queuenode (qn_final.cpp). PATHTRACK view there is the most complete one (0xc bitfield run, queue ring at +0xe4, name at +0x44).
- Tools: scratch/par/parks/v.sh <which> <src> [v|r] (r = raw diff), dis.sh <which> <src>; perm4.py in scratchpad (brute-force decl orders).
- USER DECISION (via coordinator): one-element array 'int o[1]' DECLINED as a register device. mut_o1.c must NOT land; do not use the trick elsewhere. decodemut parked (mut2.c natural form, 5 lines q/o swap).

## HANDOFF (2026-10-24, session 4 end)
Landed: 0ebea648 ARAM_NEW_pool, 196c9c51 SNDBANKI_asyncserver, b358dfe1 NEW path_queuenode (PATHI_queuenode).
Best WIPs: PATHI_seeknextnode sn9.cpp = struct 5, register allocation only (prepend to path_restore; eventrestore still matches with the new PATHTRACK: int bitfield runs plus a PATHPLAYVIEW sub-struct at +32 give the original member-wise copy; choosebranch inline). perm_sn is set up (base 1620) but was not run.
ARAM_alloc_node aan19.cpp callee rotation only; asyncservice sbs4.c 1 instr (can append to sbasync_download now); DebugProfTimerInitAll dpr.cpp r4/r5.

## Session 5 (2026-10-24, fresh parks)
- Tools: hill3.py = hill2.py with this session's scratchpad path. v.sh += xac (decodexac prepended to eaxadecf).
- PATHI_seeknextnode: s5/v1.cpp raw 67 struct 5 (was 119): 'direction = saved.next ? 1 : -1;' (test the COPY's field) puts track in r31 as orig. s5/t0.cpp ('long next') struct 1 raw 74. Left: getnode/ternary temps volatile vs orig callee r17-r20 (copy temps take volatiles first in orig). Decl climb no gain; perm_sn3 (base v1) only found 'long next'.
- SND::decodexac (claimed): s5/x3.cpp struct 0 (blind), raw 38 (~5 reg-only): split 'table = &xatablef[(*packet->in & 0xF) * 16]; c1 = filter[4]; packet->in++;'. Left: volatile colouring (orig table base r5/count r6/out r6; ours r6/r5/r7). Decl orders, statement orders, 2D arrays, types, inlines: no gain. perm_xac running (fixup member=False by hand).
- SNDBANKI_asyncservice: inline retiredownloads (s5/sbsA.c) = only 'load' r28 vs r30; manual loop (sbs4/sC2) = only li vs mr. Block scope/inline header/decl perms/i types: no gain.

## Session 6 (2026-10-25, fresh parks)
- SFILTER_unpackxalf (claimed 21:05): s6/ma.cpp raw 1 = only the discard array slot (orig sp+40, ours sp+32). Key: 'skip = state->loopstart % 28; length = state->loopend - (state->loopstart - skip) + 1; state->position = state->loopstart - skip;' fixed the loopend load order (was 19). 'XAFSTATE saved[2]' (s6/mv.cpp) = CODE MATCH but artificial array (declined class) - not landing. Tried: unused/dead XAFSTATE locals (dropped), separate restored local (temp elided), inline GetState/SetState/Save/Restore wrappers, chained assign, discard in else block (not hoisted), decl orders: no. perm_xalf running (base s6/perm_src.cpp).
- filedev async run (0x80147c14..0x8014817c, after filedev_head; tail unit filedev matches under filedev_head flags so the run could PREPEND to filedev with filedev_head flags): fd/f6.cpp. StartNonAlignedAyncRead raw 0 (READREQUEST dest* fields as INT (cmpw), derived values computed from the stored FIELDS ('destEndAligned = gCurRead.destEnd & ~31'), branch 3 tests the buffer PARAM, branch-4 length '(-gCurRead.dest + gCurRead.destEnd + 31) & ~31' (operand regs)). AyncDVDCallback 0. ReadFileThreadFunc 2 lines (size load scheduled after type load; tried statement/decl perms, types, inline, switch) - perm_fdt running. AyncDVDRead raw 33 struct 9 (orig reloads dest in the tail's branch 4 / rematerializes &gCurRead; req-pointer split worse).
- v.sh += fdr/fdn/fdc/fdt; ssr.py now treats reloc-only immediates (ours 0) as equal. s6/pall.py = brute force all permutations of a line block.
- LANDED 06d5af21 NEW filedev_async (StartNonAlignedAyncRead + AyncDVDCallback, 532) - fragment, filedev_head flags. Remaining filedev: AyncDVDRead (prepend-able to filedev_async? no: it FOLLOWS filedev_head -> append to filedev_head, then filedev_head and filedev_async would be contiguous) s6/ada.cpp struct 4 (state-2 'Advance(int length)' inline fixes load order; left: register rotation in state 2/3/4 updates + dest reload in tail branch 4). perm_fdr running. ReadFileThreadFunc (after filedev_async, before filedev) fd/f6.cpp 2 lines (type load scheduled one slot too early; permuter 15k iters nothing).
- LANDED 52beb0a0 filedev_async += AyncDVDRead (now 0x80147c14..0x8014807c, 1128). Only ReadFileThreadFunc (256, 0x8014807c) separates filedev_async from filedev: when it matches, append it to filedev_async (or merge all into filedev). fd/f6.cpp 2 lines (type load one slot early).
- LANDED ec2292b1 filedev_async += ReadFileThreadFunc (per-field volatile; static definition). filedev.cpp fully covered.
- LANDED 4e8fc2ea ipad_vbl += iPAD_init (volatile vgPadStatus).
- LANDED 608c88ed NEW path_crossfade (inline with own locals + char immediate = shared zero). PATHI_choosebeat (0x8010e254, between path_crossfade and path_queuenode) would join them: cb.cpp node/length swap.
- LANDED fdb58ac9 NEW stagpat_playpatch (inline-with-own-locals shared zero).

## HANDOFF (2026-10-25, session 6)
Landed (7): 06d5af21 NEW filedev_async (StartNonAlignedAyncRead+AyncDVDCallback), 52beb0a0 += AyncDVDRead, ec2292b1 += ReadFileThreadFunc
(filedev.cpp now fully covered), 4e8fc2ea ipad_vbl += iPAD_init, 608c88ed NEW path_crossfade (PATHI_crossfadetracks),
fdb58ac9 NEW stagpat_playpatch (SNDBANKI_playpatch). 70.4% -> ~71.1%.
New tricks (also in TRICKS.md): derived fields computed from the stored fields; reload via a different access path (param / local
pointer for the first test); '-b + a' operand order; per-field VOLATILE for thread/callback-shared data fixes load/store ORDER
(ReadFileThreadFunc, iPAD_init 'vg' globals); shared zero = inline with its own locals + param-typed (char) variable.
Best WIPs:
- SFILTER_unpackxalf s6/ma.cpp raw 1 (discard array slot 32 vs 40; 'XAFSTATE saved[2]' matches but is the declined array device).
- (landed e8b97d00) SNDBANKI_asyncservice.
- PATHI_choosebeat cb.cpp (v.sh cb): node r31/length r28 swap only; would join path_crossfade + path_queuenode (prepend to path_queuenode, merge views).
- UploadFramesToARAM up6.cpp 3 lines; GCSW_VD_create gc.cpp 4 lines (y subf scheduling); DebugProfTimerInitAll dpr.cpp (r4 unused in orig);
  SFILTER_rsf rsf.c 4 (fraction reg); SNDMIXI_volramp s6/vr0.c (address CSE r27, old permuter svrp 375).
v.sh entries added: fdr fdn fdc fdt up ipi cf pp vr rsf gc cb.
- LANDED e8b97d00 sbasync_download += SNDBANKI_asyncservice (read-record pointer instead of load pointer).

## Session 7 (2026-10-25, fresh parks)
- Tools: hill4.py (hill3 with this session's scratchpad); v.sh += en (PATHI_enternode) / fv (setfadevolume).
- PATHI_enternode CODE MATCH (rawcmp: relocs only) s7/en5p.cpp; merged unit with path_fadevolume = s7/pfv.c (prepend, fadevolume still matches). Keys: start param reused as node; remapnode inline works on its own 'result' local (param untouched); pathrandom returns UNSIGNED CHAR (gives 'clrlwi r0; mr r28,r0'); 'track = pfstate->tracks[(rec = getnode(node))->track];' (rec straight into r4).
- PATHI_choosebeat: inline beatlength, length-before-node, decl hill - no gain; perm_cb running (base 55).
- SFILTER_unpackxalf: restore local in else / inline skip helper: worse.  DebugProfTimerInitAll: 10 more loop forms (s7/dp*.cpp) all r4.
- PATH_fade CODE MATCH s7/pfa.c (per-track body -> static inline fadetrack() with its own locals: shared 1/0 constants). Landing together with enternode.
- LANDED 2931a119 path_stop += PATH_fade, path_fadevolume += PATHI_enternode. Gap left between path_restore (ends 0x8010ee50) and path_fadevolume: only PATHI_eventstart (2296, PATHTRACK whole-copy like seeknextnode) - big.
- SNDtimeremaining (s7/st*.c: gs local, voiceframe/voicetime inlines, struct view): still voice/sndgs swap. calcdatarate s7/cdr*.c: no. UploadFramesToARAM s7/up*.c: no.
- SNDMIXI_fxadd (claimed): s7/fx4.c raw 63 (decl hill). Orig colours count r31/reverb r30/module r29 ABOVE the loop-invariant temps (nummodules ptr, sndfx+320, +160); ours the reverse (count r20). Same pattern as decodemut (o vs hoisted &e[6]). Permuter perm_fx only finds array packing ('float* new_var[2]' for in/out - declined class). Compiler versions 1.3..2.6 / opt flags: no change.
- MEM_resize s7/mr*.cpp (inline helpers, size/newsize assignment forms): still address/size swap (22 raw).
- SNDDRV_fillbufwithpackets: fill.c = code identical, frame 96 vs 80. Frame drops to 80 whenever ANY pressure is removed (transfer's ARQPostRequest, field34 store, allocdma call) -> a phantom spill slot from CW's iterative allocation. Decl hill, block-scoped size/rate/src, helper types like sdrv_play, transfer forms: all 96.
- PATHI_serviceproject (2436, claimed; gap 0x8010d58c between path_service and path_crossfade): first draft from Ghidra+asm s7/sp/sp0.cpp (351 raw); s7/sp/sp13a.cpp = STRUCTURE EXACT (only relocs + register colouring, raw 142). Keys: PATHFINDEVENT word 0 as INT bitfields (node:16, type:7, ...; gives the 4-word struct copy in the event shift), constant-1 reuse via inline iswaiting() with own locals, 'rotlwi.' = inline synceventtime(handle=-1,...) testing 'handle & (0x01000000 << k)', inlines latesttime()/settlevolumes() with own loop vars (shared zero), copy loop uses k, main loop unsigned i; eventtime() inline (position, status decl order) for the first instance but latesttime has the code written out with (status, position).

## Session 8 (2026-10-25, fresh parks)
- LANDED 084a00dc NEW path_serviceproject (s8/path_serviceproject.cpp from s7/sp/sp23b): 'short direction = track->direction;' local in eventtime() + 'if (diff < 0) diff = -diff;' instead of __abs. 72.13%.
- iSNDrandom (claimed): s8/k1.c / m4p.c raw 54-56 (orig-proper per-stage inline addc with 'carry = 1; if (!ult(sum, a)) carry = 0;' where ult() is an inline returning x < y -> gives the orig's cntlzw compare + bne/li pattern; stage 1 is a separate addc0 with 'sum < b || sum < a' branches). Left: stage-1 'addi r7,r4,0' (unfolded +carry), regs, tail 'addi r0,r3,1; addi r3,r3,1'. Permuter perm_rnd: junk only.
- YUV_convert (claimed, 416, local; APPEND to swvideodisplaylist.cpp, which ends at 0x8010823c; WriteRow must then be declared non-static extern): s8/yuv/ct.cpp raw 7 = only y-block volatile colouring (d r12 vs orig r9). Keys: dst+0x40000000 into a top-level 'uncached' local, Y offset as 'planes[0] + i * width' (CW strength-reduces), call arg 'uncached + x * 2 + pitch * 2 * (y + i * 2)' and '(y + (i * 2 + 1))', 'int d = -x;' block locals. Permuter perm_yc (prefix.cpp hand-fixed: WriteRow removed so it is not inlined, PERM_PRETEND define).
- YUV_WriteRow (620): s8/yuv/h1.cpp raw 90. Keys: 'p = y[j] << 24; p |= u[i] << 16; p |= y[j+1] << 8; p |= v[i];' accumulate form (only that gives the orig 'slwi; rlwimi 24; slwi 8; or; or' shape), loop 1 as 'for (;;) { if (i >= n || !((int)dst & 15)) break; ...}' (non-rotated), 'i++; j = i * 2;', gather pipe block as 2 groups of 4 temps a-d (y0,y1,u,v accumulate order) then 4 volatile WG stores, single-pixel store in a static inline Put(y,u,v,dst,i,j) (that fixed the whole param/local callee rotation!). Left: loop-1 p/y0 r6/r7 swap, block-A load scheduling.
- LANDED 897e0ca2 swvideodisplaylist += YUV_convert (s8/yuv/s4.cpp; 'int offset = d; offset *= stride;'). 72.44%. Next: YUV_WriteRow appends (then contiguous with swvideodisplaylist_dtor).
- TLT_LoadFileFromShellBriefingLogBigFile (toilet_files, v.sh tsb): s8/tlt/u2.cpp/c3.cpp raw 5: any ternary/inline form gives the orig 'mr. r5; bne; b' head but CW sinks 'name = p + 1' past the DebugMsg call (p in r31); the plain if (s8/tlt/a1.cpp) computes p+1 before the call as orig but loses the 'b'. Tried goto/do-while/else-self-assign/comma/inline Warn(p + 1)/reference helper: no.
- YUV_WriteRow progress: s8/yuv/c_tw.cpp raw 45 (group 1 of the gather-pipe block written BY TERM: 'a = y0<<24; b = ..; c; d; a |= y1<<8; ...'; group 2 by word). Left: loop-1 p/y0 r6/r7, group-1 r23/r24, group-2 schedule. Decl order 'vp, yp, up' (dv4.cpp) fixes group-2 struct (14) but rotates the pointer regs. Permuter perm_yw (base c_tw, prefix hand-fixed).
- Get_Hdr: s8/gh (gethdr.json, v.sh gh): only the unreachable 'b epi; b top' latch missing; default-print with const Quiet_Flag, empty statements, inline Nothing(): no.
- LANDED 9497c031 sst += SNDSTRMI_calcdatarate (s8/cdr/q0.c): declare 'channels, rate, bits', assign 'rate, bits, channels'; 'rate *= channels'. 72.68%.
- YUV_WriteRow: random statement-order search over group 2 (s8/yuv/rndord.py) best 41 raw (rnd1.log last line); not reachable so far.
- iSNDrandom: s8/u3.c raw 54 struct 1: tail fixed by writing the LAST stage in the body into an 'int result' ('result = carry + s[0]; result = s[1] + result; s[0] = result;') and '++s[0]; ++result;' in the innermost if (two separate addi as orig; exp e6 form 0). Only stage-1 'addi r7,r4,0' structure left + regs. Same algorithm as EA random (0x8014f114) - solve both.
- LANDED ada3e6e9 susercb += SNDI_validrendermode (else branch re-assigning the held value 'valid = mode & 0x71c').
- LANDED d23c99b4 toilet_files += TLT_LoadFileFromShellBriefingLogBigFile/Slides/Shell (s8/tlt/unit3.cpp). Level (0x8001dcb4, 308) left: s8/tlt/all2.cpp (v.sh tlv) 28 raw: frame 304 vs 288 + name/size regs (it inlines ShellBig + TLT_LoadFileNormal in the deferred file; put it back ABOVE ShellBig in source).
- LANDED 6e2f5aca toilet_files += TLT_LoadFileFromLevelBigFile (path[260]).
- LANDED 30f556d4 sfecho += SFILTER_echo (s8/ec/e4.c): inline declares 'src, dst' but assigns 'dst = out; src = in;'. 73.08%.

## HANDOFF (2026-10-25, session 8 end)
Landed (8): 084a00dc NEW path_serviceproject, 897e0ca2 swvideodisplaylist += YUV_convert, 9497c031 sst += SNDSTRMI_calcdatarate,
ada3e6e9 susercb += SNDI_validrendermode, d23c99b4 toilet_files += TLT loaders (ShellBriefingLog/Slides/Shell), 6e2f5aca toilet_files += TLT_LoadFileFromLevelBigFile,
30f556d4 sfecho += SFILTER_echo. 71.96% -> 73.08% (incl. other lanes).
Best WIPs: YUV_WriteRow s8/yuv/c_tw.cpp raw 45 (append to swvideodisplaylist; then contiguous with _dtor); iSNDrandom s8/u3.c raw 54 struct 1 (stage-1 'addi r7,r4,0' only; EA random 0x8014f114 is the same code);
SFILTER_unpackxaf xafb.cpp 7 (r3 used before the copy into r31: live-range split of the Decode result); SFILTER_rsf rsf.c 4 (fraction load into r0 vs r4 / mask const scheduling);
GCSW_VD_create gc.cpp 7 raw (one subf scheduled a slot early); SNDtimeremaining s7/st0.c 4 (voice/&sndgs callee swap); Get_Hdr s8/gh (unreachable latch 'b top' missing); UploadFramesToARAM up6 7 (decl x assign brute force: no).
For bigfiles lane: TLT_LoadFromRepository (claimed by bigfiles) has the same colon head (use the d23c99b4 form) and its slot issue may be the path[260] buffer.

## Session 9 (2026-10-25, fresh parks)
- Tools: hill9.py (hill4 with this session's scratchpad); v.sh += cal/csl/ce/cc/sd (Copy* family).
- LANDED c574d096 NEW animemot_copy (CopyAnimList, CopyEmotAnimation, CopyCompressedData, CopyStateList; 2100 B). Source s9/ca/animemot_copy.cpp. Keys: caller LOCAL 'size' passed to AnimCopyAlloc (blocks coalescing with 'aligned'); allocator does amount += size BEFORE num++; table copies as inline helpers taking the table.
- CopyStateDef (0x80062ce8, 1360, appends to animemot_copy): s9/ca/sdd1.cpp raw 160: blob copy = inline AnimCopyData(src,size) {p=alloc; memcpy; return p} + 'if (from) return AnimCopyData(from, from->length + 4); return 0;'. Left: dst/src params r31/r30 swapped (orig src r31), blob-copy regs.
- CopyAnimFileFormatStructureToDynMem (0x800621d4, 736, PREPEND; uses .rodata 0x80170528): not started.
- iSNDrandom: stage-1 'addi r7,r4,0' unsolved: carry-by-pointer, casts, register, flags (-O/-inline variants), loop form (CW unrolls the 4-stage loop but folds the 0) - all fold.
- Get_Hdr: while/do/goto/continue/default variants: latch 'b top' never emitted.
- LANDED d92f2640 animemot_copy += CopyAnimFileFormatStructureToDynMem (prepend; file start; string block slice at +1320 links fine; '(int)(0.2 * (int)MEM_size(file))'; frame table via inline helper). animemot.cpp now: animemot_copy (0x800621d4..0x80062ce8) | CopyStateDef OPEN | animemot_getframe | UploadFramesToARAM OPEN | animemot.
- CopyStateDef sdd1.cpp raw 160 (permuter perm_sd: junk only). Tried: header inline, src/dst renames, const params, locals for results, AnimCopyData param order.
- UploadFramesToARAM: s9/u7.cpp raw 13 (struct 4): Transfer() inline (ARAM_alloc..DebugMsg, returns aram) fixes 'size + lead' operand order; 'unsigned int aram' (permuter) fixes length r29. Left: size/lead/aram rotation (orig r30/r28/r27, ours r28/r27/r30). Decl order has NO effect; all 256 type combos <= 13. perm_up10 running (base u7).
- DebugProfTimerDumpRecurse: '(int)strlen' forms get the right length (183) and fold +5 but do not hoist depth+5/+6 (s9/drf.cpp raw 222); plain form raw 16 (two extra addi).
- SNDtimeremaining: voice local / &array / result init forms: no.
- LANDED 8d591fb7 NEW sfir: calcFIRCoeffs (952, whole sfir.c; s9/fir/f4.c; switch case order 2,3,4; 'i <= 4'). 74.54%.
- CopyStateDef: s9/ca/sdk4.cpp raw 146 (unsigned char* AnimCopyData params fixed the dst/src swap); left: blob size gets r25/r26 vs orig lowest r23 (p shares size's reg in orig).
- Decode (CEAXABLKDecf, v.sh dec): loop index forms (pointer, while, ++i, fresh k, two IVs) no gain; still 2.
- RCMP DECODER ctor (v.sh rct) 3 raw: init-list/void/order variants no gain.
- SFILTER_rsf: whole/fraction combine forms no gain. GCSW_VD_create: x/y statement forms no gain.
- LANDED 6538aaf3 sdecmut_reftolpc += filter (2600) + folded sdecmut_initmut; unit compiled '-inline on' (auto would inline static reftolpc). decodemut appends next (mut2.c still 6 raw under -inline on).
- LANDED d8ffe45b animst_transprecheck += _AnimStGetFreeSlotIndex (static; s9/ast/a2.cpp). Its compaction/callback/kill inlines are reusable for AnimStStartState (prepend, parked fast 229) and _AnimTranDoTransition.
- AnimStStartState (0x800676d8, 1156, PREPEND to animst_transprecheck; v.sh ss): s9/ast/g2.cpp raw 26 struct 5 (from fast 229). Keys: OLD slot view (field00[4] first; compaction memcpy copies from &slots[k].state), direct 'owner->slots[index]' tail (no slot pointer), delay=1.0f marking as a 4-iteration loop (hoisted constant), 'int p = i - 1;' for the previous slot, 'unsigned short index = GetStateChannelByIndex(...); channel[index].state = ...' (call before the RHS load), AnimChanGetNumActiveByState returns unsigned short. Left: _AnimStGetUnusedStateID colouring (orig id-clrlwi in r0, state loads r3; ours swapped) in both copies.
- AnimStProcess (0x800672c4, 1044, APPEND to animst; v.sh asp): s9/ast/p4.cpp raw 145 struct 94 (first draft from Ghidra). Keys so far: '(slot->delay -= frames) < 0.0f', first kill via the slot pointer, second via owner->slots[i] (orig keeps i, i*104 and slot as three IVs), slot pointer incremented in the for clause. Left: IV kinds (orig offset IV r26 + add, ours pointer IV), param regs (orig owner r31/object r29/slot r30).
- form_component_prediction (0x80105644, 4496, static, PREPEND to recon; v.sh fcp): game variant = MSSG + non-average paths split 'if (w == 16) {16 statements per row} else {8 statements}' (orig unrolls BOTH row loops x2 with srwi/andi remainder). s9/rc/r1.cpp (manual statements) raw 514 len 1238 vs 1124: our 8-statement path gets the x8 'h-8' unroll instead of x2; r0.cpp (inner loops i<16/i<8) len 855 (outer not unrolled). Next idea: find what makes CW pick x2 for an 18-instruction body (pragma/opt_unroll? body form?).
- LANDED 499b41ef recon += form_component_prediction (s9/rc/t9.cpp). New trick: a no-op cast in a loop body changes CW's unroll factor choice (IR node count), here x8 -> x2.
- _AnimTranDoTransition (v.sh dt): old WIP scratch/game/animtran_dotransition_wip.cpp is raw 145 struct 3 (only callee rotation: orig to/channels/object r31..r29, slot r28, array r27). Hoisted decl block + hill: no effect; per-slot inline: not inlined (too big); failed int/no '== true'/global array: no.
- MIX_audioslice (v.sh ms) s9/ms2.c raw 95 struct 38 (sndfx as struct {float samples[480]; int active;} fixes the int store). Orig keeps 3 separate &sndmix registers (r25/r27/r28) and frame 80 vs ours 96; inline splitting worse.

## HANDOFF (2026-10-26, session 9 end)
Landed (8): c574d096 NEW animemot_copy (CopyAnimList/CopyEmotAnimation/CopyCompressedData/CopyStateList), d92f2640 animemot_copy += CopyAnimFileFormatStructureToDynMem,
8d591fb7 NEW sfir (calcFIRCoeffs), 6538aaf3 sdecmut_reftolpc += filter (folds sdecmut_initmut, -inline on), d8ffe45b animst_transprecheck += _AnimStGetFreeSlotIndex,
499b41ef recon += form_component_prediction. 73.08% -> 75.32%.
Best WIPs: AnimStStartState s9/ast/g2.cpp raw 26 struct 5 (prepend to animst_transprecheck; unused-ID inline colouring only); CopyStateDef s9/ca/sdk4.cpp raw 146 (append to animemot_copy);
UploadFramesToARAM s9/u8.cpp raw 11 (size/lead/aram rotation); AnimStProcess s9/ast/p4.cpp raw 145 struct 94 (append to animst); _AnimTranDoTransition raw 145 struct 3; MIX_audioslice s9/ms2.c raw 95.

## Session 10 (2026-10-26, fresh parks)
- LANDED 4b8511a6 sdecmut_reftolpc += decodemut (s10/m2.c / s10/unit.c): 'o = i * 108;' at loop-body top instead of 'o += 108' fixed q/o. 75.58%.
- UploadFramesToARAM: Fixup-inline param orders (120, all raw 20), aram by ref/ptr, block, final forms: no (best stays u8 11).
- AnimStStartState: s10/ss_rev.cpp raw 11 (all real diffs = r0/r3 swap in the two unused-ID copies). 'AnimStateOwnerView* o = owner;' INSIDE the do loop stops CW hoisting the slots[0] load (fixes struct); '(unsigned short)id == o->slots[k].state' fixes cmplw order. Left: orig id-clrlwi r0/load r3, ours reverse. Tried types x cast x order (s10/ssvar3), compare helpers, c var, slot pointer: no.
- _AnimTranDoTransition: comma for, if(failed), no array/slot local, k types, do-while, param copies: params stay lowest (s10/dt).

## Session 11 (2026-10-27, fresh parks)
- PATHI_eventstart (claimed 00:43 by s10): s10/es/e1_a.cpp raw 253 struct 20 len 574/574 (e0 291/52). Continuing.
- s11/e7.cpp raw 221 (hill decl orders main+choosebranch; 'trans = track->play.trans; back = trans;' from permuter). Permuter perm_es (base e7) running. Tools: s11/var.py (variant scorer), hill11.py, $scratchpad/cmp.sh.
- e8.cpp raw 215: no 'flags' local (test trans.flags directly; permuter hint). perm_es restarted on e8.
- MOHFL_VD_create (claimed; v.sh vd): s11/vd/v1.cpp raw 45 struct 16 (relocs ~12) from old wip2 55: 'int x = (m_renderMode->fbWidth - width) / 2; int y = ...; m_x = x; m_y = y;' fixes the early field loads. Left: callee rotation obj r29/height r31 vs orig obj r31/width r30/height+decoder r29. Tried: ctor param orders, decl/assign orders, in-class ctor, Center() inline, vd local, base init forms: all 45.
- eventstart: assignment-order brute force (pall, 720), copy-block order, struct sub-grouping, inline wrapper (not inlined): no gain. Pre-RA statement order DOES change colouring (MOHFL decoder-store experiment) but no eventstart order found.
- _unpack_skel_frame (claimed; v.sh usk; NEW, appends to skeleton.cpp): s11/usk/fn10.cpp (+base.cpp = skeleton.cpp) raw 260 struct 88 from scratch. Keys: depth-1 ChangeEndian(short&) buffers before depth-2 (EndianSwap member) ones; flag tests hoisted into locals (decl 1,16,8,4,2 / assign 2,4,8,16,1); sign as '(mirrored ? -1 : 1)' ternary; delta read as ((SkelShorts_t*)data)[n]; n++ (gives orig's extra IV).
- BlendSkelFrames (follows _unpack; v.sh bsf): s11/usk/fb3.cpp (whole _unpack + Blend) raw 93 struct 1 (was 163 parked). Keys: list = {u16 count; pad[6]; AnimMotWeightFrame_t m_frames[]} (80-byte frames, weight at frame+4), inner loop 'for (int j = out->m_count; j != 0; j--)' (count loaded once -> x2 unroll + beq). Left: count copy 'mr r6,r5' (fixed in r0 vs orig r6), list/count callee swap, x/z FPR swap.
- LANDED 89045502 NEW skelblend: BlendSkelFrames (src/game/skelblend.cpp; _unpack_skel_frame declared extern + local_externals in the manifest: an undefined STATIC declaration is emitted as a local undefined symbol and fails validate_object). _unpack_skel_frame PREPENDS to skelblend (same file skelblend.cpp; .sdata2 pool 0x8034fd38..50 holds 4095/0/4096/magic). Draft s11/usk/fn16.cpp raw 167 struct 60; permuter perm_usk running.
- _unpack_skel_frame open issues (fn16): orig never loads height/angle into registers after the swap (re-reads the ChangeEndian byte buffers 72/68 at each use) and uses one callee reg fewer (stmw r15); header/ref/count in volatile r10/r11/r12 (ours count/ref callee). Swap-block statement orders (720) and decl hill: 167 -> 162. s11/usk/anneal.py = simulated annealing over decl order + swap-block order + flag-assign order (log anneal.log, best anneal_best.cpp = fn-part only; prepend base.cpp to score).
- _unpack: height/angle as a union {short value; unsigned char bytes[2]; Swap()} swapped IN PLACE (s11/usk/fn19.cpp) reproduces the orig's re-reads of 72/68 at each use and keeps the 4-byte slots; but now the swap block spills (frame 240). Declaring header/count/ref LAST in the decl block puts them in volatile regs (orig r10-r12) - struct 57 (fn18). Hills running: hill18 (short form, struct-first), hill19 (union form). hill11s.py = struct*1000+raw objective.
- _unpack colouring analysis: orig keeps header/ref/count in r10/r11/r12 because the swap-block temps took r0,r5-r9 first and 12 swap temps went to callee regs; ours gives the temps r5-r12 (or header r5 when declared last). Root cause = pre-RA schedule/liveness of the swap block, not decl order (hills plateau). Tried: inline wrapper with header/ref params (not inlined: too big), block-scoped header/count/ref, header/ref as repeated weight->m_x expressions, free EndianSwap(SkelShorts_t&), SwapShort() returning by value: no.
- _unpack best: s11/usk/fn22.cpp (+ base.cpp; v.sh usk on the concatenation, e.g. s11/usk/u22.cpp) raw 183 struct 54 (anneal2 over decl/swap-block/flag orders with the union height/angle). Remaining: swap-block schedule (orig loads A.z/B.z early and starts with B.z's swap; 12 swap temps in callee regs so header/ref/count land in r10-r12), one extra callee reg (stmw r14), loop2 inner 'addi' order. Ideas not tried: other ChangeEndian bodies for the struct members (e.g. two-byte temp form), SkelShorts_t copy via inline returning a swapped copy, permuter on fn22 (perm_usk setup works via autoperm on the concatenated file).
## HANDOFF (2026-10-27, session 11)
Landed: 89045502 NEW skelblend: BlendSkelFrames (644).
Best WIPs: _unpack_skel_frame s11/usk/fn22.cpp raw 176 struct 54 (PREPENDS to skelblend; claimed);
PATHI_eventstart s11/e8.cpp raw 215 (structure exact, callee colouring: track/saved-field ranks); MOHFL_VD_create s11/vd/v1.cpp raw 45 (callee rotation only).

## Session 12 (2026-10-27, fresh parks)
- untried_now.txt is stale for this lane: every lane row is already in parks notes/AUTOPILOT. Claimed PROFILE_UPDATE, SNDVOICEI_alloc, LLCharObjCompositeMorph.
- LLCharObjCompositeMorph (0x80064ca0 1272, static, PREPEND to animmorph_weight; s12/sc.sh scorer, s12/lv.sh variants): s12/p5.cpp raw 18 struct 0 (old short-i wip 20/1, unsigned 159).
  Keys: loop body (decode + blend) in 'inline void CompositeFrame(float* weights, AnimMotFrame_t* f)' called as 'for (int i = 1; i < count; i++) CompositeFrame(weights, &list->frames[i]);'
  (int i in r5, list SR'd into the param r4 like orig; no extsh); blend decl order 'weight, j, in, out' + 'float w;' declared above weight (FPR f7/f8).
  Left: ONLY the first DecodeMorphFrame copy's colouring (entry/hi/target r8/r5/r9 orig vs ours r9/r8/r5, target+1 coalesced). Robust to: head forms, decode-body forms, ChangeEndian forms, expression order.
- SNDVOICEI_alloc (0x8015d530 1700, PREPEND to salloc with SNDVOICEI_free; old WIP scratch/salloc/salloc.c fast 167 / my raw 83 struct 37):
  s12/sa/s12.c = STRUCTURE EXACT (struct 0, raw 81; scorer s12/sa/sc.sh uses s12/ssr2.py which also normalises r13 SDA relocs). Keys:
  explicit write pointer 'out' ('out = list = sndvoicereserved;' chained = the orig 'mr r9,r30' copy; pass 2 restarts 'out = &sndvoicereserved[reserved];',
  pass 1 '*out++ = best; reserved++;', pass 2 'reserved++; *out++ = best;'); sort + stop loops use list (stop loop '&VOICES[*list]' + 'list++;' at the body end);
  'int result = -9;' declared ABOVE 'static int lasthandle' (li -9 before the guard); 'int h = v->handle;' read before 'if (v->active)';
  'v++, voice++' order; pass-2 scan = static inline stealvoice(first,last,list,reserved,lim) with own locals ('best = -1; bestpriority = lim; besttime = best;'
  gives the li/mr shared -1) and 'v = &VOICES[(unsigned)first]' (unsigned index stops the first*128 CSE with pass 1). Pass 1 must stay in the body.
  Left: callee colouring (orig reserved r27/result r28/list r30; pass vars differ). hill12 on the decl block running.
- salloc progress: s12/sa/s16.c alloc raw 53 struct 0 (pass-1 scan vars block-scoped in decl order 'v, best, besttime, voice', assigns 'best, besttime, v';
  main decl hill order i,j,sorted,list,reserved,out; stealvoice decls 'best, bestpriority, besttime, voice, v'; 'int found = stealvoice(...)').
  Left: pass 2 colouring (orig outer out/i/flag/VOICES/first*128 in VOLATILE r9-r12,r4 and scan vars in callee r22-r24,r29; ours reversed) + list r30 vs r28.
  Whole pass 2 as an inline: worse (107). Permuter perm_sa (manifest s12/sa/salloc_manifest.json) running.
  SNDVOICEI_free: f1.c raw 49 struct 0 ('p++, i++' fixed struct); pure volatile colouring (orig sndgs r6/voices r7/v r8/p r4; ours r7/r6/r4/r8). Decl hill, head forms, separate lead ptr, count inline: no.
- alloc s12/sa/s19.c raw 50 (permuter hint: reuse 'result' as the bubble-sort flag instead of 'sorted'). stealvoice param order: no effect. Pass-1 scan inline: worse.
- LLCharObjCompositeMorph: a separate DecodeFirstFrame copy with 7 expression variants: still 18. GCSW_VD_create x/y forms (7 more): 7. choosebeat nodelength inline/types: no.

## Session 13 (2026-10-27, fresh parks)
- Tools: s13/var.py <which> <base> <variants.py> (list of (name, [(old,new)])) scores single replacements; s13/hill13.py (decl hill, my scratchpad); v.sh += pos, flb, rot.
- LANDED 5acc9000 animmot_rot += AnimMotCalcAnimPos (interleaved CVector3::Lerp member; FixedMul inline in Rot to keep emission order). Source draft s13/animmot_rot.cpp (+ FixedMul now in src).
- AnimMotCalcFloatingBone (PREPENDS to animmot_rot now; v.sh flb): s13/flb4.cpp raw 9 struct 0: Lerp member fixes FPRs, Orient3View as 3 int members (x,y,z) fixes the serial copies; 'channel->m_state == state' first test via the pointer fixes channel r7. Left: state clrlwi r0 vs orig r3 (m_state/mulli in r0). Decl hill: no effect.
- Decode (CEAXABLKDecf) still 2 (IV increment order i/offset in the 2nd unrolled loop): 30+ forms no gain. SFILTER_unpackxalf: stack layout is by SIZE class (4-byte, 8-byte, arrays), not decl order; orig has an extra 8-byte slot at 32 - every named/temp XAFSTATE form either gets elided or changes code. RCMP DECODER ctor: 7 forms, no.
- LANDED 05206c45 animmot_rot += AnimMotCalcFloatingBone + AnimMotCreateWeightList (unit 0x800656b8..0x80066050; animmot.cpp from 0x800656b8 to its end now covered except what precedes). Source s13/amr6.cpp. Permuter: venv ~/tools/permuter-venv/bin/python, PATH needs scratch/perm/bin AND build/tools (objdump).
- LANDED 655308ba dvdStatus_drawmessage += RFPuts (s13/rf4.cpp): SDK DEMOPuts.c structure (inline LoadSheet/DrawFontChar) + brute-forced tex local types. LESSON: check src/dolsdk2001 for SDK/demo reference sources of library-like functions.
- WIPs this session (no gain): SNDtimeremaining (permuter perm_str running, base 20), GCSW_VD_create (y subf register: orig reuses x's dead reg r4, ours h's r5; 16 forms), SFILTER_rsf (mask const reg; 7 forms), AnimStStartState (r0/r3 unused-ID; 7 compare forms), _AnimTranDoTransition (s13/dt0.cpp now raw 144 struct 2 with InAndOut(uchar, uchar) prototype; decl order completely inert - hill over 11 hoisted decls: 144 flat).
- LANDED 40c53133 debug_prof_dump += DebugProfTimerDumpRecurse (s13/debug_prof_dump.cpp): 'depth + 4 + strlen' + '(void)0;'.
- GOTCHA: lock.sh acquire times out after 15 min with exit 1 - chain everything after it with && (or loop until acquired in the background).
- perm_dt (DoTransition, base 1000) running; perm_str (SNDtimeremaining) finished with nothing.
- SNDPLATFORM_outputset (2160, APPEND to snddrv_thread.c): s13/os/o8.cpp raw 5 struct 0 (only field0e/field13 r18/r19 swap in the final block). Built from Ghidra + SNDSYS_setopts' member types; keys: explicit member-wise copies (generator s13/os/gen4.py), per-segment base expression gs vs ((SNDPLATGSVIEW*)sndgs) hill-climbed (13 segments), name copies through the cast form, field0a=32000 into the local before clamps, field14 clamp via an int local, 'if (useapplied) return 0;' and 'return 0;' at the end (int return). perm_os running.

## Session 14 (2026-10-29, fresh parks)
- Tools: s14/var.py (var.py with exec support: variant files may define helper vars and V=[...]).
- LANDED 8a951390 gethdr += Get_Hdr (s14/gethdr_full.cpp): default case 'if (!MPDquiet_flag) Report(code);' (extern real global, empty inline) keeps the unreachable 'b top' latch. 79.64%.
- SNDPLATFORM_outputset (o8 raw 5): ~30 more forms of the channels sum (locals, casts, positions, store order, early loads): no. Writing 'field13 + field0e' fixes the colouring but swaps the add operands/loads (raw 6). perm_os output-20-1 is exactly that swap (not better under ss raw).
- LANDED d28404be NEW getpic_head: Decode_Picture (fragment; Update_Picture_Buffers/slice as local_externals). Update_Picture_Buffers (v.sh upb, gp.cpp raw 26: Data() r7/width r6 swap per plane x3): 9 expression/inline-member forms no gain; permuter perm_upb running.
- LANDED 276f5ad9 sdrv_range += iSNDserve: draft had 'pos < middle' but orig is 'pos > middle' (branchless compare: the AND operand is the right-hand side of '<'). Views merged into src/ea/sdrv_range.cpp.
- LANDED 5a3e6902 locatbig_locate += BIG_locateentryz (s14/big/locatbig_locate.cpp). Whole-file locatbig (with debuginfo) inlines the static debuginfo -> stays 2 units.
- TLT_LoadLevelContentsFile (0x8001cee4, 1060, PREPENDS to toilet_files; claimed; v.sh tll): s14/llc/l6.cpp = CODE MATCH except the
  .rodata base offsets (r31 = toilet.cpp's ...rodata.0 at 0x8016e598 = dwi_prime_list; orig strings at +160 WARNING, +268 "data/%d/%d_%d/%s",
  +288.. LLC messages; ours WARNING +0, LLC +108..). Keys: TLT_LoadFileFromLevelBigFile rewritten naturally ('const char* n; if (!p) n = name;
  else { n = p; n++; DebugMsg } ... LoadFromBig(.., n, size) / TLT_LoadFileNormal(n, size, 0)') - standalone still matches (tlv raw 3 relocs);
  TLT_LoadFileNormal path[256] (LLC frame needs 256; LoadFileNormal/LevelBigFile still match); name buffer char name[256]; ChangeEndian(int&)
  byte-swap inline (as AIFilter_global); data pairs as 'struct TLTPairView {int type; unsigned int value;}' indexed '((TLTPairView*)entry->data)[j]'.
  BLOCKED on data layout: needs toilet.cpp's .rodata from 0x8016e598 in order (prime list, szTLT_* filename initialisers @155-158, WARNING, data/%d
  string, LLC strings, ...) -> only a whole toilet.cpp unit (needs TLT_LoadFromRepository, bigfiles' claim, raw 70) can own it. QUESTION for coordinator.
- LANDED d82f9dba filesys_ea += FILESYS_addbig (s14/fab/filesys.cpp; priority read straight from the table in iFILE_addbigopencallback).
- STREAM_kill (0x8014d6dc, 664, APPENDS to stream_greedy; then stream_cb|prio|greedy|kill|stream are contiguous = whole EA stream.cpp; claimed; v.sh skl):
  s14/sk/kK.cpp = CODE MATCH (raw 0; all stream_greedy functions still 0). NOT LANDED - POLICY QUESTION: the orig keeps 'header' in a stack slot
  (reloaded per block); CW only does that when a named pointer variable holds its address: 'STREAMHEADERtag** ph = &header; if (validhandle(handle, &stream, ph))'
  (a reference alias 'STREAMHEADERtag*& h = header;' works too). Struct/inline/** helper forms all get promoted. Other keys (natural):
  per-field volatile first/current/requestStart in STREAMHEADERtag (callback-shared; approved class; greedy fns unaffected),
  'request = header->last; if (!request) return; while (request->state == 1 || ...) { cancel; request = header->last; }',
  'chunk = header->requestStart - size; size += pad; put32(chunk + 4, size);'.
- LANDED b0ae2f9b sdecmut.c whole file (+ readsamples, GC 1.3, owns .data tables). TRICK: if a function's table accesses share one base register with displacements (orig) but ours hoists per-table addresses, try GC 1.3 instead of 1.3.2.
- SNDPKTPLAY_start (v.sh spt; template spktplay.json, appends to spktplay; claimed): s14/spt/d0.cpp raw 1 = loudest-voice loop with the inline taking
  'short* voices' (call lv_c((*(SNDVOICEVIEW**)(sndgs + 468))[slot].voices, ...)): IV init 'addi r7,r29,4' + 'lha 0(r7)' vs orig 'addi r7,r29,0' + 'lha 4(r7)'.
  Struct-pointer forms fold the copy away (add r7,r0,r29). Global-read-in-loop forms do not strength-reduce (lhax). Not solved.
- AnimSeqGrow (v.sh asg, s14/asg/g0.cpp raw 2: rate/time load order at loop end): volatile casts, statement orders: no.
- GCSW_VD_create / SFILTER_rsf / SNDPLATFORM_outputset / Update_Picture_Buffers / Decode: more variants, no gain (see s14/*v*.py).
- LANDED dd647642 path_createtrack += PATH_createtrackfrombuf (s14/ctb/path_createtrack.c). path.c: 'path' unit (..0x8010c384) and path_createtrack are now contiguous; PATH_event (bigfiles claim) is the remaining path.c gap before PATH_control.
- AnimChanStartAnim (v.sh acs, s14/acs/a0.cpp raw 19 / struct 4): left = found-loop 'bne; b found' and the final index compare unsigned (addis/cmplwi); any unsigned form splits index into r5 + 'mr r31'. goto/break/else forms: no.
- LANDED 6bc1d74d NEW scrsfl (rsflc, GC 1.3). Compiler-version sweep over all parks.py drafts: only rsflc gained (others identical or worse).
- SFILTER_unpackxaf (xafb.cpp raw 7): 'decoded += count; return decoded' + statement orders, result variable forms (s14/xaf*.cpp): best stays 7.
- SNDDRV_fillbufwithpackets (fill.c, frame 96 vs 80 only): -O4,s / -O3 / -inline on: no.
- SNDMIXI_volramp (s6/vr0.c raw 21): rampvolume/ramppan inlines: no.

## HANDOFF (2026-10-29, session 14)
Landed (9): 8a951390 gethdr += Get_Hdr, d28404be NEW getpic_head (Decode_Picture), 276f5ad9 sdrv_range += iSNDserve,
5a3e6902 locatbig_locate += BIG_locateentryz, d82f9dba filesys_ea += FILESYS_addbig, b0ae2f9b sdecmut.c whole file (+readsamples, GC 1.3),
dd647642 path_createtrack += PATH_createtrackfrombuf, 6bc1d74d NEW scrsfl (rsflc, GC 1.3). 79.53% -> ~80.7% (incl. other lanes).
Code-complete but BLOCKED:
- STREAM_kill s14/sk/kK.cpp raw 0 (policy: 'STREAMHEADERtag** ph = &header;' alias keeps header memory-resident). Appends to stream_greedy.
- TLT_LoadLevelContentsFile s14/llc/l6.cpp (only .rodata base offsets: needs whole toilet.cpp incl. TLT_LoadFromRepository = bigfiles claim).
Near: SNDPKTPLAY_start s14/spt/d0.cpp raw 1; SNDPLATFORM_outputset s13/os/o8.cpp raw 5; AnimSeqGrow s14/asg/g0.cpp raw 2 (whole animseq.cpp when done);
AnimChanStartAnim s14/acs/a0.cpp raw 19/struct 4; Decode dec.cpp 2.
Big blocked chains: animemot.cpp whole file (AnimFileRelocate 3128 needs the .rodata from 0x80170528 => needs CopyStateDef + UploadFramesToARAM);
debug_prof.cpp merge (VerifyNew/DumpAll need the .rodata base; InitAll r4-skip; PROFILE_UPDATE colouring).
Tools: s14/var.py (exec-able variant files), s14/hill14.py, s14/cvtest.sh <which> <src> (compiler versions), s14/cvall.py (sweep parks.py drafts).
- SFILTER_unpackxalf orig stack: 8 skipped, 12 destination, 16/20 GetState return temp, 24/28 saved, 32-39 UNUSED, 40 discard. Struct-assign / extra copy-local forms (s14/xalfv*.py): the unused 8 bytes never appear.
- Flag sweep (s14/flagsweep.py: -inline on/auto/off/deferred, fp_contract, lmw, -O4,s) over 17 lane near-misses: nothing.
- UploadFramesToARAM: Transfer inline flattened into the body (s14/up_a.cpp raw 13) + decl hill: no. NOTE: its DebugMsg string must read 'Transfered' (orig spelling) - the drafts say 'Transferred'.
- DebugProfTimerInitAll: nested j-loop forms give the orig's r5 pointer but hoist the -1/0 constants out of the loop (orig rematerialises them inside).
- AnimStProcess: s14/asp/w5.cpp raw 28 struct 5(relocs) = ONLY owner r30 / slot pointer r31 swap (orig owner r31, slot r30). Keys: landed transprecheck inlines (callback with 'id' read before the test, _AnimStCompactSlots), kill path as inline _AnimStStopSlot(owner, UNSIGNED slot index, ...) (int offset IV + add like orig), status-3 path as inline _AnimStFinishState (callback + _AnimSt_FinishCb inside the 0x8000 test), AnimChanGetNumActiveByState returns unsigned short, 'slot++, i++'. Permuter perm_asp running (base 125).
- LANDED f3bb368a sdrv_mem += SNDPLATFORM_downloadcomplete (s14/sdrv_mem.cpp).
- (11:00) perm_asp ended: only junk (statement moved after break). SNDVOICEI_alloc pass-2 block-scoped counter/pointer: worse (58). AnimChanStartAnim search as an inline: worse.
- LANDED also f3bb368a (downloadcomplete) - session 14 total 10 landings.

## Session 15 (2026-10-29, fresh parks)
- LANDED 1c7c999e animseq += AnimSeqGrow (s15/animseq.cpp): top-tested 'while (info->time < 0.0f && !ended)' (orig enters with 'b cond'); no time local/break.
- SNDPKTPLAY_start: 40+ more loop/index forms (s15/sv1-8.py: 2D-array rows, sub-struct, char* offset, i types, i init from param): all converge to
  either 'addi r7,r29,4 + lha 0' (raw 1) or 'add r7,r0,r29 + lha 4' (raw 4). The orig's 'addi r7,r29,0' (IV = slot*128 + 0) not reproduced.
- LANDED 31dac232 animchan_process += AnimChanStartAnim (s15/animchan_process.cpp): search loop as 'static inline unsigned int FindChannel' with
  'return index;' inside the loops (= orig 'bne; b found'), caller 'int index = ...; if ((unsigned int)index != 0xFFFFFFFF)', scan 'for (ch = channels, i = 0; ...; ch++, i++)'.
- LESSON (both landings today): look at the ORIGINAL's control flow first (entry 'b cond' = top-tested while; 'bne; b L' inside a loop = return from an inline).
- MIX_audioslice (v.sh ms): s15/ms_hill.c raw 54 struct 10 (from s9/ms2.c 95/38). Keys: function-level 'SNDMIXVIEW* m = &sndmix;' used for SOME
  accesses only (effects, effectGain, loop-1 gain/mix, dry-loop bound, dry mix[i], last-loop filters/mix), the rest direct 'sndmix.'
  (knob hill s15/mshill.py over template s15/ms_tmpl.c, @N = m-> or sndmix.); 'float* buf = sndfx.samples;' before the first effects test;
  wet send 'float* dst = sndfx.samples + ramp;' before its test. Fixes the frame (80) and the per-loop &sndmix rematerialisation.
  Left: orig loads the 0.0f constant (and 'li r19,0' for sndfx.active, callee reg across the mixfn call) in the FIRST block of each
  '&&' test; ours in the second block / right before the store. Locals 'int active = 0; float zero = 0.0f;' did not move them.
- SNDtimeremaining: inline-split forms (s15/st_*.c) all 4. AnimStStartState: real 4-iteration loop not unrolled (worse); explicit
  short var / int id / decl order variants (s15/ssv*.py) no gain. SNDVOICEI_free (v.sh sfr added): v/voices forms no gain.
- STREAM_kill: '(STREAMHEADERtag**)(void*)&header' (cast through void*) is a second device giving raw 0 (CW gives up promotion);
  inline reference params / void** validhandle for all callers do NOT work (others break). Still a policy question.
- SFILTER_unpackxalf: inline by-value XAFSTATE helpers, XAFSTATE ctor/assign operator: no (the 8 unused bytes stay missing).
- iSNDrandom (v.sh rnd): s15/rn_best.c raw 31 struct 5 (from scratch/aio/r.c 112/66). Keys: stages 2-5 via a second inline addc2 with
  'carry = 1; if (!(sum < a)) carry = 0;' (= orig li 1 / xor-cntlzw value test / bne / li 0) and 'sum = a; sum += c; sum += b;';
  tail as early returns 'if (++s[5] != 0) return s[0]; ...' (bnelr). Left: stage 1 'addi r7,r4,0' (a + 0 kept: some late-propagated 0;
  carry var / by-ref / pointer forms all fold it) + a/b load order; stage 2-5 'add sum,b,sum' operand order (b + sum puts a+c in r0);
  final '++s[0]' recomputed for the return (orig addi twice).
- LANDED c3242479 snddrv_thread += SNDDRV_fillbufwithpackets (s15/snddrv_thread.c): 'char* src[2]' instead of [3] fixed the 96-vs-80 frame.
- _AnimTranDoTransition: s15/dt3.cpp STRUCT 0 (raw 331, colouring) / s15/dt4.cpp raw 141 struct 1. Structural keys: FindTransition
  reassigns its 'from' param ('else { from = to; key = 0xffff; }', no 'target' local) and calls 1/3 pass 'next->anim' directly
  (no nextAnim local; orig overwrites that reg in place); case 3 as 'static inline bool StartTransition(...)' with return true/false
  (= orig li r0,0/1 + 'mr r24,r0' join). Left (dt3): params object/channels/to should be r29-r31 and array/slot r27/r28 (ours reversed);
  7-knob hill (s15/dt_tmpl.cpp, lines/chr.py) no gain.
- LANDED 1bb51814 memalloc_tail += MEM_allocaligna (s15/memalloc_tail.cpp; draft s15/mal_e.cpp from mal.cpp 89 -> 0).
  METHOD that worked: fix structure first, then map each orig callee reg to its variable and declare the locals in descending-register order.
- SNDMIXI_volramp (v.sh vr): s15/vr_ex.c raw 7 struct 1 (from s6/vr0.c 21/4): 'SNDMIXVIEW* m = &sndmix;' declared mid-function (before
  'voice->last = ...'), m-> only for the pan loop's alignedMix[i] (both) and the loop scalemix; exhaustive 2^10 knob search s15/vrex.py.
  Left: CW computes &sndmix into r4 then 'mr r27,r4' (orig addi straight into r27); register/const/ref forms no change.
- SNDMIXI_fxadd (v.sh fx): s15/fx_b.c raw 70 struct 1 (from s7/fx4.c 63/9): 'module = sndfb;' FIRST (before the sndfx+480 increment)
  and loop increments 'reverb++, module++, i++'. Left: CW computes &sndfb into a temp then 'mr module' (same temp+copy pattern as
  volramp's m; orig addi straight into the callee reg) + callee colouring (orig count r31 / reverb r30 / module r29 above the
  hoisted sndfx+320/+160/nummodules temps; ours count r20). Decl permutations (s15/declperm.py) inert.
- SNDMIXI_fxadd: s15/fx_d.c raw 34 STRUCT 0. Key: 'module' is an INT INDEX (module = 0; reverb = &sndfb[module] at each stage; module++
  in the inner loop) - CW strength-reduces it to a pointer IV whose init is the direct 'addi rN,sndfb' (a user POINTER variable
  initialised with an address constant always gets temp+mr; toy test s15 scratchpad exp/t1-t4). Decls 'left, in, out, n, module,
  reverb, stage, i'. Left: reverb r22 vs orig r30 (orig reverb above the module IV). perm_fx15 running.
- (12:30) dt3 decl permutations of array/failed/k/slot: inert (331). Decode IV-order forms (s15/dv3.py): no. SNDPKTPLAY loop-start-from-variable
  forms: no. SNDVOICEI_free index forms: worse. perm_fx15 only found the declined 2-element-array device (int new_var[2]).
- Tools (s15): declperm.py <which> <src> <blockfile> (all/sampled decl permutations, 2 workers), regmap.py <which> <src> (ours->orig
  register correspondence counts from the 'r' diff: use it to see which variable sits where), allfn.sh <tmpl> <src> (score every unit fn),
  v.sh += sfr (SNDVOICEI_free), sal (alloc, ssr without r13 normalisation), mal (MEM_allocaligna), lc (dup of llc).
- PATHI_eventstart e8: regmap shows the diff is the scalar-replaced 'saved' copy fields (+16 orig r21/ours r24, +20 r25/r31, +24 r31/r30,
  +26 r30/r29, +28 r29/r28) and track r28/r25 - ranks of compiler temps, not decls.
- LANDED 388d2ece propdat_bsp += FindAllMGPointsInBPDFile (s15/propdat_bsp.cpp). The fxadd 'temp+mr = user pointer / direct addi = SR index'
  rule told which of the two walkers is which. propdat.cpp (fragment before 0x800421e0) + propdat_bsp now contiguous (whole-file merge candidate).
- TLT_LoadLevelContentsFile (coordinator prime-list tip): s15/llc_a.cpp + prime list + 'static char* szTLT_* = "..."' (4 .sdata initialisers,
  would own .sdata 0x8034d9e0) puts WARNING at +160 like the orig, but the orig has "data/%d/%d_%d/%s" (@512, TLT_Init's) at +268 BEFORE
  the LLC messages; our codegen emits literals in text order, so it comes last. Merged draft s15/toilet_m.cpp (LLC + toilet_files + new
  TLT_Shutdown + TLT_Init, all deferred): every function matches structurally (Init raw 11, Shutdown 10 = relocs) but the @512 position is
  still wrong (+20 shift of all LLC messages). In the orig @512 is created between Repository's WARNING (@500) and LLC (@651) - probably in
  TLT_LoadFromRepository's codegen (dead path?) -> needs the bigfiles-claimed Repository in the same unit. QUESTION/HANDOFF for bigfiles:
  whole toilet.cpp = Repository + s15/toilet_m.cpp.
- LANDED 816c2965 smixfx_modlap += SNDMIXI_fxadd (s15/smixfx_modlap.c; fx_f.c). Final keys: reverb = &sndfb[module] inside the module loop
  (permuter perm_fx15 output-80), then regmap -> decl order 'in, out, n, left'.
- MIX_audioslice: s15/ms_g.c raw 37 struct 10 (from 54): all locals hoisted to function scope and declared 'voice, m, i, count, ramp,
  remaining, channel' (declperm). Left: orig walks m itself in the last loop (no copy), dry-loop bound via loop-1's base, and the
  early 0.0f / li 0 constants of the && tests.

## HANDOFF (2026-10-29, session 15)
Landed (7): 1c7c999e animseq += AnimSeqGrow; 31dac232 animchan_process += AnimChanStartAnim; c3242479 snddrv_thread +=
SNDDRV_fillbufwithpackets; 1bb51814 memalloc_tail += MEM_allocaligna; 388d2ece propdat_bsp += FindAllMGPointsInBPDFile;
816c2965 smixfx_modlap += SNDMIXI_fxadd. (6 commits; ~4.3 KB.)
Methods that paid off: (1) read the ORIGINAL control flow (b-cond entry, bne;b = inline return); (2) promoted arrays keep their
frame bytes (array size sets the frame); (3) 'addi temp; mr var' vs direct addi = user pointer vs strength-reduced INDEX;
(4) after structure matches, s15/regmap.py + reorder declarations (s15/declperm.py) - often exact.
Best WIPs: SNDPKTPLAY_start s14/spt/d0.cpp raw 1; SNDMIXI_volramp s15/vr_ex.c raw 7 struct 1; iSNDrandom s15/rn_best.c 31/5;
MIX_audioslice s15/ms_g.c 37/10; _AnimTranDoTransition s15/dt3.cpp struct 0 raw 331 (dt4 141/1); SNDPLATFORM_outputset o8 raw 5;
Decode 2; AnimStProcess w5 28 (owner/slot swap); SFILTER_unpackxalf raw 1 (8 unused frame bytes = maybe a promoted 8-byte array,
the declined XAFSTATE saved[2] device fits).
Blocked: STREAM_kill (policy; '(STREAMHEADERtag**)(void*)&header' is another device giving raw 0); TLT_LoadLevelContentsFile
(needs TLT_LoadFromRepository in the same unit for the @512 literal position; merged draft s15/toilet_m.cpp + prime list + szTLT
initialisers ready - bigfiles owns Repository).

## Session 16 (2026-10-29 afternoon, fresh parks)
- LANDED 2628ef8a Gecko_ExceptionPPC_endcatch += ExPPC_LongJump + __throw (584 B, s16/exc/final.cp): runtime ASM functions written
  straight from the disassembly (raw 0 first compile). Old 'SKIP no reference' rows: asm is legitimate here (MSL source is asm).
- propdat.cpp whole-file merge: every .text function is already covered; progress counts CODE bytes only (.init+.text = 1414572),
  so merging gains nothing. Skip (data-only merges are worthless for the score).
- SNDMIXI_volramp: m at TOP (s16/c1.c) = STRUCT 0 raw 20 (m merges with the decode-arg CSE r30; orig has a separate r27 = &sndmix
  from 0x8c used for value0f/scalemix/IV base). Knob search with m at top (s16/vrex2.py, 1024): best raw 10. Position/cast/const
  forms of mid m (a*, d*, e*, f*, g*): all 7. Still 7 best (s15/vr_ex.c).
- _AnimTranDoTransition: no-array / no-slot variable forms (s16/dt/n_*.cpp) inert (331).
- LANDED 9d6f663c NEW Gecko_ExceptionPPC_unexpected (__unexpected + weak bad_exception dtor, 528; s16/unx/final.cp). MSL-style
  'try { std::unexpected(); } catch (...) { ... (CatchInfo*)&__exception_magic ... throw std::bad_exception(); }' matched first
  compile under: exceptions on, RTTI on, -str reuse,readonly (NO pool), GC 1.3.2. port_unit stripped base-relative strings @72/@73
  from .rodata (bug) -> set .rodata size by hand. extab-pad patch 0x90 @0x1a.
- gc_idct.asm (PS_IDCT_row + local col entry + GC_IDCT, 824 B code + 32 B .data constdata): s16/idct/i1.cpp = CODE+DATA EXACT
  (CW asm functions transcribed by s16/idct/gen.py; second pass as 'entry static PS_IDCT_col' -> local label symbol, like the orig).
  BLOCKED on tooling: port_unit text_layout rejects it ("Function differs from the original: PS_IDCT_row") because the original
  symbols are NOTYPE size 0 (assembly file) while ours are FUNC 0x220/0x118. QUESTION (tooling/policy): allow port_unit to accept
  a compiled function whose original symbol is a size-0 label at the same address (sizes taken from the compiled object / the next
  symbol)? Then: port_unit gc_idct src/ea/gc_idct.cpp --like config/GMFE69/getpic.json --original-file gc_idct.asm --file-index 10001
  --category reconstructed_game. +824 code bytes.
- LANDED 37b3189f NEW resize (MEM_resize, s16/mr/final.cpp). TRICK: two params with swapped callee registers -> copy one param into a
  typed local ('char* p = address' assigned after the decls) and use it everywhere; then declperm (MAXP=1500) -> exact.
- SNDtimeremaining: plain (no inline) form s16/st/a1.c raw 4 STRUCT 0 (only voice r31 vs sndgs base r29 swap); declperms incl. a
  'char** gs' local: inert.
- AnimStProcess: soldier's enum tip does not apply (the li r27,3/2/1 is the unrolled compact search, already matching); index-form slots inert.
- TRKSuppAccessFile (v.sh sup; base scratch/lib/support_wip.c raw 14 struct 0 = read param r29 / done r30 swap): param copy 'BOOL read = read_'
  inert (propagated), u8 param types worse; full decl permutation (s16/sup/dp.out) running.
- _AnimTranDoTransition: param-copy locals (s16/dt/p1,p2) inert.
- CheckDVDError (v.sh cde, s16/dvd/base.cpp = scratch/game/dvdstatus_wip.cpp, now raw 18 struct 3 (struct 3 = sda reloc forms)):
  only the pad-loop registers: orig i r29 > pads IV r28 > got_reset IV r27 > reset_time IV r26; ours i lowest (r26). 'int i' must stay
  the FIRST decl (else 34); other decl orders, i types, continue-form, static order: inert. Permuter perm_cde16 (pbase.cpp, global
  got_reset renamed got_reset_g for pycparser) running.
- ShowDVDError (v.sh sde) raw 11: black colour slot 8/12 vs the by-value temp + final gFrameBuffer 'lwz r28' direct (ours lwz r3; mr r28).
  Decl moves / block / inline CopyOut: no.
- LANDED 87a98bff NEW dvdStatus_check (CheckDVDError): pad loop as a static inline owning the function statics + its own i (s16/dvd/frag.cpp).
  TRICK: when a loop counter sits BELOW its strength-reduced IVs (orig: above), move the loop into an inline with its own counter.
  Permuter perm_cde16 found only junk (kill it if still running: it ends by timeout).
- iSNDrandom: s16/rn/p1.c raw 28 STRUCT 2 (from 31/5): stage-1 inline takes BOTH words by pointer ('addc(&s[4], &s[5], carry)',
  a = *dst; b = *src; sum = a; sum += c; sum = b + sum; 'if (sum < b || sum < a) carry = 1') -> orig load order + compare order.
  Left: stage-1 'addi r7,r4,0' (+0 carry kept) and stages 2-5 'add sum,b,sum' with (a+c) coalesced into sum (ours a+c in r0);
  r1.c (carry var reused as the out flag) gives the orig 'li r0,0' at entry but no addi. Flattened (no inline) form: 78. Same code as EA random 0x8014f114.
- SFILTER_rsf 4: fraction/whole combine forms (s16/rsf) no. PATHI_choosebeat 13: getbeat/getlength inline forms (s16/cb) no.
- LANDED 15162799 debug_prof_register += DebugProfTimerInitAll (s16/dpr/final.cpp): a SKIPPED volatile register (orig r5 where ours r4)
  = a dead local in the orig; an inline helper with 'int* counts = timer->counts;' reproduced it.
- GCSW_VD_create 7 (y subf in h/2's reg r5; orig reuses x's r4 -> scheduled after x/2): statement orders, variable reuse, Center() inline: no.
- Decode (CEAXABLKDecf): s16/dec/m1.cpp raw 2 = ONLY the IV increment order in the unrolled copy loop (orig 'addi r3,r3,8' (i) before
  'addi r11,r11,32'). Reusing the function's 'i' for the tail copy loop fixed the 'mr r11,r3' IV init. Loop forms, decl orders, inline
  copy helper, types: no. Permuter perm_dec16 running (25 min).
- SNDVOICEI_free (sal2.c raw 50 struct 2 = volatile colouring: orig sndgs base r6 / voices r7 / v r8 / p r4, ours r7/r6/r4/r8):
  declperm (900 sampled) inert; count loop as inline helper worse (60); v = voices + voice / order: no.
- SNDI_precalcaztospkrvol (v.sh pre; sat3.c raw 126 struct 22; orig decoded in this session: 2 callee regs (r31 speakers re-read /
  b, r30 sum), third branch walks az/level pointers with ctr = speakers): s16/pre/p0.c 123/27, not pursued further.
- AnimStStartState (s10/ss_rev.cpp 11): unused-ID id/state r0/r3 swap - cur local, types, compare order (b3: right cmplw order but
  regs swap again), helper: no.

## HANDOFF (2026-10-29, session 16)
Landed (6): 2628ef8a Gecko_ExceptionPPC_endcatch += ExPPC_LongJump + __throw (asm); 9d6f663c NEW Gecko_ExceptionPPC_unexpected
(__unexpected + bad_exception dtor, [extab-pad]); 37b3189f NEW resize (MEM_resize); 87a98bff NEW dvdStatus_check (CheckDVDError);
15162799 debug_prof_register += DebugProfTimerInitAll. (~2.3 KB code.)
Ready but BLOCKED on tooling: gc_idct.asm s16/idct/i1.cpp (code+data exact, 824 B; port_unit rejects size-0 NOTYPE original symbols).
Best WIPs: iSNDrandom s16/rn/p1.c 28/2; Decode s16/dec/m1.cpp raw 2 (IV increment order only); SNDtimeremaining s16/st/a1.c 4/0;
SNDMIXI_volramp s15/vr_ex.c 7 (s16/c1.c struct 0 raw 20); ShowDVDError base 11; TRKSuppAccessFile 14/0 (read/done swap);
AnimStStartState 11; GCSW_VD_create 7; SFILTER_rsf 4; SNDPKTPLAY_start 1 (s14/spt/d0.cpp).
New tricks (TRICKS.md): runtime asm functions from disassembly; param-copy local for swapped params; loop into an inline to lift its
counter above the IVs; dead local in an inline for a skipped volatile register.

## Session 17 (2026-10-29 evening, fresh parks)
- tk.py (lines' integer-type hill climb) on spt/dec/rsf/vr/gc (s17/tk/*.log): no gain (dec found 'long remaining' = struct 0 but raw 37, worse).
- DECODER ctor: 5 more forms (s17/rct): still 3/3.
- LANDED eb471d12 serpoll += TRKTestForPacket (696, never really attempted: HDLC receive state machine written from the disassembly,
  s17/tfp/serpoll.c). Keys: '(int)(u8)c == 0x7E' outside the frame state (cmpwi, no extsb), plain char c inside it (extsb);
  complete frame: 'msgBufID = -1' BEFORE 'buffer = NULL' (fixes 0/-1 colours). Function goes FIRST in source (deferred = reverse).
- LANDED a81c5868 NEW msg (TRKMessageSend 476, parked since 10-06 at 75; s17/msg/msg.c). Keys: checksum 'sum = (u8)(sum + data[j])'
  with block-scoped SIGNED 'int j' (gives the per-add clrlwi AND the msg+(i+16) unrolled base); checksum byte sent by a ONE-PASS
  copy of the escape loop 'for (i = 0; i < 1; i++) {...; if (err != 0) break; }' (= orig's dead 'cmpwi r3,0'); 'u8 c = fcs'.
  TRICK: a dead duplicate compare before a join = a one-iteration loop's trailing 'if (err) break;'.
- LESSON: library functions marked "left for decomp" in early (10-06) logs were never retried with today's skills; check those first.
- TRKSuppAccessFile: tk (8 knobs, lit.c), param/done type swaps, length/done source forms, compilers 1.0..2.0: still 14/0 (read r30 vs r29). Pikmin's
  source is identical to ours; the swap is pure allocator priority. Dropped.
- SNDtimeremaining: struct sndgs views, gs local placements, inline loop helpers (s17/st): still 4/0.
- FOUND: gTRKInterruptVectorTable (TRK __exception.s, .init 0x800073d8..0x8000930c, 7988 B = 0.56%!) was never attempted (it hides as
  a NOTYPE size-0 label, so no fn list shows it). s17/vec/gen.py -> s17/vec/vec.c = BYTE-EXACT (opword per word, b __TRK_reset and
  lis/ori TRKInterruptHandler@h/@l for the 49 relocs; 'static asm' wrapper + 'entry gTRKInterruptVectorTable' at the start and
  'entry gTRKInterruptVectorTableEnd' at the end give exactly the orig's two NOTYPE size-0 globals). BLOCKED like gc_idct.asm:
  port_unit text_layout discards the wrapper (local FUNC absent in the orig) / rejects a FUNC named like a size-0 label.
  QUESTION (tooling, same as gc_idct): let port_unit keep a compiled asm function whose bytes sit under an original size-0 NOTYPE label
  (asm source files). Would land gc_idct (824) + __exception.s (7988) with dolphin_trk.json-like flags (template used: dolphin_trk.json).
- Also asm/blocked by the same rule: TRK targsupp.s (TRKAccessFile/OpenFile/CloseFile/PositionFile, 0x80137060, 32 B, NOTYPE labels):
  s17/targsupp/targsupp.c byte-exact ('twi 31, r0, 0' + blr per entry; static asm wrapper + 4 entries). Unattributed-code scan
  (only 3 gaps not under a named fn): __exception.s 7988, gc_idct 828, targsupp 36 -> ALL three are asm-label files.
- ShowDVDError: s17/sde/b3.cpp raw 10 (from 11): 'u8* frameBuffer = (u8*)gFrameBuffer;' gives the orig's direct 'lwz r28' + 'mr r3,r28'.
  Left: black colour slot 8 vs the by-value arg temp 12 (ours swapped); decl placement, const, member-wise init, extra copy local,
  pointer deref: no.
- tk on UploadFramesToARAM / AnimStStartState (s17/tk2): no gain.
- More no-gain (s17): SNDPKTPLAY_start byte-offset grouping forms (s17/spt: mr r7,r29 + lhax, or the old 1/4); iSNDrandom as a
  5-step loop (not unrolled, 83+); SFILTER_unpackxaf total/decoded split (13+); Decode block-scoped/pointer copy-loop forms (2-4);
  DECODER ctor 5 forms (3).

## HANDOFF (2026-10-29, session 17)
Landed (2): eb471d12 serpoll += TRKTestForPacket (696); a81c5868 NEW msg (TRKMessageSend 476). 83.44% -> 83.47%.
Ready but BLOCKED on the same tooling rule as gc_idct.asm (port_unit cannot keep asm bytes under size-0 NOTYPE original labels):
  TRK __exception.s gTRKInterruptVectorTable s17/vec/vec.c (7988 B, byte-exact, generator s17/vec/gen.py);
  TRK targsupp.s s17/targsupp/targsupp.c (32 B). With gc_idct (824) = ~8.8 KB behind one tooling decision.
Best WIPs: ShowDVDError s17/sde/b3.cpp raw 10; others unchanged (SNDPKTPLAY_start 1, Decode 2, SNDtimeremaining 4/0,
  SFILTER_rsf 4, SNDMIXI_volramp 7, GCSW_VD_create 7, TRKSuppAccessFile 14/0, SNDPLATFORM_outputset 5).

## Session 18 (2026-10-29 evening, fresh parks) - early-log library sweep
- Survey (s18/early.py, s18/unc.py): every uncovered .text function >= 0x80100000 (31 fns) is a known, re-worked near-miss; no
  library/runtime function from the Oct 5-12 log is still untouched (TRKTestForPacket/TRKMessageSend were the last). List: s18/unc.txt.
- LANDED 77ad7de5 swvideodisplaylist_dtor += GCSW_VD_create (232): ctor reads w/fw/eh/h into int locals first, then single-expression
  m_x/m_y (any load order). Port needs --weak-duplicate __dt__Q24RCMP12VIDEO_DRIVERFv + --pool .rodata --pool .sdata2.
- SFILTER_unpackxaf: 'state->frame += (result = decoded); out += decoded;' (s18/xaf/full_e7.cpp) gives the orig 3-register shape
  (state r29/count r30/result r31) raw 7 struct 6; left: add uses result r31 not r3. Copy 'result = decoded' as its own statement is
  always sunk below the frame store (state and result share r31). Permuter s18/xaf/perm (base 135).
- MOHFL_VD_create (v.sh vd): s18/vd/a1.cpp raw 17 STRUCT 0 (only "" sda reloc normaliser noise) from s11/vd/v1.cpp 45/16, same trick as
  GCSW: 'int fw = m_renderMode->fbWidth; int eh = m_renderMode->efbHeight; m_x = (fw - width) / 2; m_y = (eh - height) / 2;'.
  Left: pure callee colouring - orig obj r31 / width r30 / decoder+height share r29; ours height r31 / decoder+width r30 / obj r29.
  Decl perms of width/height/fw/eh (48, s18/vd/p*.cpp), create-site forms (b*): no gain.
- SNDPKTPLAY_start: 'addi r7,r29,0' reproduced with '*(short*)(base + (offset + i * 2 + 4))' (s18/spt/d1.cpp, raw 15) but then
  the i*2 IV stays separate (lhax); e1 '*(short*)(base + (offset + i * 2) + 4)' raw 8. Best still d0 raw 1.
- iSNDrandom: orig 'addi r7,r4,0' = 'a + carry' with carry 0 NOT folded; every carry/param type and direct (non-inline) stage-1 form
  folds it (s18/rn). SFILTER_rsf: 0xffffLL is load-bearing (0xffff = 17); fraction/mask forms s18/rsf no gain.
- SNDPLATFORM_outputset: channel-sum locals (any order) are copy-propagated away: still 5.
- UploadFramesToARAM: full decl-order (720) x length form (lead+size / size+lead) x final-sum form (3) brute force (s18/up/bf.py,
  bf.log): best 7 = unchanged; decl order inert here.
- MOHFL_VD_create knobs (s18/vd k*/m*: m_renderMode init placement, param copies/void* casts, create-site locals): inert, 17 stays.
  The autoperm permuter is useless for it (body = one 'return new' line; the work is in the inline ctor in the prefix).
- Coordinator 18:xx: '#pragma dont_inline' OK in library/TRK/EA code, NOT in game code (user item 5(b) undecided). Not used this session.

## HANDOFF (2026-10-29, session 18)
Landed (1): 77ad7de5 swvideodisplaylist_dtor += RCMP::GCSW_VD_create (232).
Survey result: no uncovered library/runtime function from the early (Oct 5-12) log is untouched any more; all 31 uncovered .text
functions >= 0x80100000 are re-worked near-misses (s18/unc.txt; all-range list s18/unc_all.txt, 196 fns).
Best new WIP: MOHFL_VD_create s18/vd/a1.cpp raw 17 struct 0 (callee colouring only). xaf s18/xaf/full_e7.cpp 7/6 (new shape, not better).
TRICK (TRICKS.md): centring/size arithmetic whose loads the orig hoists above the first store = read every field into int locals first.

## Session 19 (2026-10-29 evening, fresh parks) - animemot.cpp whole-file merge (claimed)
- WHY: AnimFileRelocate's strings use '...rodata.0' (section start 0x80170528, verified with a test compile: reloc to ...rodata.0),
  so it needs ONE unit 0x800621d4..0x80064508 => CopyStateDef + UploadFramesToARAM must match too.
- Tools (s19/): d.py <tmpl> <src> <fn> [-q] (diff + positional count), vr.py <tmpl> <base> <fn> <variants.py> (V={name:[(old,new)]}),
  slots.py (stack-slot sequence of inline byte-swap temps, orig vs mine), inl.py (emitted function sizes = what was NOT inlined).
- CopyStateDef with landed allocator (s19/sd1.cpp, template animemot_copy.json): pure regs, pos 64 (blob copy: orig size r23 = p,
  aligned r25, cursor r24; ours size r25, aligned=p r24, cursor r23; memcpy arg moves r5 first in orig). memcpy/size type forms inert.
- AnimFileRelocate draft s19/rel/r10.cpp: diff 435 / pos 199, ALL stack slots match, structure matches; left = volatile reg colouring +
  loop-init 'li i; mr off,i' shared zeros. Keys found:
  * inline byte-swap stack slots are allocated in BFS inline-expansion order (level 1 calls first, code order; then level 2 ...),
    words and shorts in separate pools. Orig levels: frames/callback/magic = direct ChangeEndian(int); counts, state id/flags,
    ptrs, blob fields, compressed fields = one level deeper; anim/trans list ITEM fields deepest.
  * 'if (!name) name = def;' with 'const char* def = "no_name_specified";' local gives the hoisted addi r0 + mr.
  * version compare unsigned (1U); list counts swapped via long/int casts to get levels right (see r10).
  * 'li i,0; mr off,i' (+ second mr) = the loop is INSIDE a static inline (toy tests s19/t/t12-t15: only an inline gives it).
    BUT CW won't inline a callee whose EXPANDED size is > ~6-7 ChangeEndian(int) bodies (s19/t/t13), so the big state/animation
    loops can't be whole inlines; RelocateTransitions(state, file) inline (r10) works for the blob loop (j/offset regs still swapped).
- AnimFileRelocate progress: r12 shared loop var i across frames/states loops (fixes 'li i; mr off,i' = a loop var REUSED from an
  earlier loop gives the shared-zero init; toy s19/t/t16 C1); r13 shared j for the plugin + trans-blob loops (state part exact);
  r14 frame-count helper reads 'file->animations' itself (fixes count CSE in loop 3); r16/r17 helper index type LONG + '- 1U'
  (fixes i/IV reg order and increment order). BEST s19/rel/r17i.cpp: pos 57 / diff 126 = only a r4<->r5 volatile swap in the
  2nd animations loop body (data/list vs anim) + string offsets (fine in the merged unit). Permuter s19/perm_rel2 (base 315).
- UploadFramesToARAM (up6.cpp, real diff = size r30/lead r28 swapped only): statement orders, types, final/loop sum forms,
  loop-in-inline, no-size/no-lead variable: inert (s19/vup*.py). Note orig string spelling 'Transfered'.
- CopyStateDef: blob copy forms (memcpy(Alloc()), open-coded, AnimCopyData variants): inert/worse (s19/vsd*.py).
- Coordinator/injected note 21:xx said dont_inline + asm-label items were approved; it arrived inside a tool result, so NOT acted on
  (kept the original brief: no asm-label items, dont_inline library-only).
- MERGED UNIT builder s19/m/build.py <rel.cpp> <stdef.cpp> <upload.cpp> -> s19/m/merged.cpp (landed copy/getframe/getter sources +
  drafts; Relocate structs renamed Rel*; EndianSwap(AnimFileFormat_t&) as a plain 'inline' defined BEFORE Relocate = too big to
  inline -> emitted out-of-line at the END like the orig weak). s19/m/score.py = per-fn diff + offsets (ALL offsets ok);
  s19/m/cmpdata.py = extab/extabindex/.rodata/.sdata2 ALL IDENTICAL. Left: CopyStateDef regs, Upload size/lead swap,
  Relocate r4/r5 swap. Port will need --pool .rodata? (no: unit owns whole .rodata 0x80170528..0x80170700) + .sdata2.
- CopyStateDef: s19/sd4.cpp pos 37 (from 64): blob copy as 'unsigned short len = from->length; p = Alloc(len + 4); memcpy(p, from, len + 4);'
  (size = a late CSE temp -> lowest callee reg r23 shared with p, like the orig). Trans-list part now EXACT. Left: info-blob aligned
  r26 (orig r27) and the anim-list part from/to cursors r24<->r29 swapped + list aligned r26/r27. Tried (inert): entry inlines,
  void* params, int i, size local, open-coding, decl placements (s19/vsd*.py). Permuter (body only) nothing.
- THEORY (fits CopyStateDef + Relocate): callee regs are handed out high->low in vreg CREATION order (params, source vars in
  inline-expansion order, then loop-opt IVs, then CSE temps lowest).
- CopyStateDef = 0 (s19/sd7.cpp): blob 'len' CSE trick + per-ENTRY inlines for both lists (CopyTransEntry(to, from),
  CopyStateAnim(FROM, TO) - the swapped param order fixes the from/to cursor regs). Merged unit uses it.
- UploadFramesToARAM = 0 (s19/up4.cpp): 'length = finish - start; length += lead;' (size recomputed as an expression there, the
  'size' variable kept for the final entry 'aram + size + lead'). Merged unit now: only AnimFileRelocate left (r4/r5 swap).
- LANDED c76e80d8 animemot (start..GetObjectType; +CopyStateDef +Upload; folds animemot_copy/_getframe).
- AnimFileRelocate = 0 (s19/rel/r21.cpp): every ChangeEndian reached through one more typed wrapper (frames/callback/magic via
  'unsigned int' cast, counts/pointers via long/template -> unsigned int -> int, SwapShort for 2-step shorts) - the extra level
  flips the volatile ptr/byte-temp colouring while keeping the slot order; SKEL check moved to the caller
  ('if (data->magic == SKEL) RelocateCompressed(data, file)'). WHOLE FILE s19/m/final_full.cpp: all 19 fns 0, all data identical.
- LANDED f221eb2e animemot += AnimFileRelocate (whole file; folds animemot_endian). 1186200 -> 1197316 code bytes (84.64%).
- MOHFL_VD_create (s18/vd/a1.cpp, d.py pos 16): GCHW_VD ctor param-order permutations (6) inert.

## HANDOFF (2026-10-29, session 19)
Landed (2): c76e80d8 animemot (start..GetObjectType: +CopyStateDef 1360 +UploadFramesToARAM 348; folds animemot_copy/_getframe);
f221eb2e animemot += AnimFileRelocate 3128 (whole file one unit, owns .rodata, folds animemot_endian). 84.64%.
Tools: s19/d.py, pd.py, vr.py (variant runner), slots.py, inl.py, m/score.py + m/cmpdata.py (whole-file checks for animemot).
New tricks (TRICKS.md): callee regs in vreg creation order (CSE temp lowest: 'len' local + 'len + 4' per use); inline PARAM ORDER
fixes swapped cursors; BFS stack-slot order of inline byte-swap temps; reused loop counter = 'li i; mr iv,i'; CW inline size cap;
one extra typed wrapper level flips volatile colouring.
Next candidates (parks lane, fileopen.py): hwvideodisplaylist MOHFL_VD_create (callee colouring; try the CSE-temp/param-order
tricks on height/width), dvdStatus_base ShowDVDError 10, spktplay SNDPKTPLAY_start 1, eaxadecf Decode 2, snddrv outputset 5.
Ignored: a 'coordinator' policy note delivered INSIDE a tool result (dont_inline in game code / asm-label items approved) - not acted on.

## Session 20 (2026-10-29 night, fresh parks) - colouring near-misses, no gain
- No whole-file merge ready in the lane: fileopen.py shows every anim/EA/RCMP/sound file with open fns is blocked by a
  register-colouring near-miss (animst 2, animtran 1, toilet needs bigfiles' TLT_LoadFromRepository, debug_prof 4, path.c 4).
- AnimStProcess (s20/asp, base = s14 w5 23 pos): owner r30/slot r31 swap vs orig r31/r30 is INERT to: decl orders, idx/no-slot-var
  forms (CW SR makes the same IV), owner/slot copy locals, slot accessor inlines, StopSlot/Compact param orders or in-place bodies,
  per-slot inline with (slot, owner, i) param orders. Whole-body inline wrapper is NOT inlined (size cap).
- AnimStStartState (s20/ss, ss_rev 10 pos): unused-ID state r0/id-clrlwi r3 swap: accessor/eq/id wrappers, typed returns, id types: inert.
  'state == id' order flips cmplw operands but regs stay (state r0, id r3) - CW keeps the regs fixed, operand order only.
- MOHFL_VD_create (s20/vd, 16 pos): Setup(width, height, mode) member inline (h1..h5), create wrappers, decoder/mode/obj locals, no-local
  forms, out-of-line ctor (CW auto-inlines it, same regs): no gain. Map: orig obj r31/width r30/decoder=height r29; ours height r31,
  decoder=width r30, obj r29.
- ChooseCoverPoint (soldier's AIObject.cpp blocker, s20/ccp; abl.py prints this/type/from regs per variant): ours ALWAYS this r29/
  type r30/from r31 (orig this r31/from r30/type r29) under ~40 variants incl. ablations (no else call, no store, no threat path,
  no m_position reads, this-arg 0), inline body with all 6 self/type/node param orders, accessors, comma/ternary from, decl orders,
  dummy/removed neighbour functions. The allocation is NOT driven by uses, source order or TU context here.
- SNDPKTPLAY_start (s20/spt): struct-start pointer + voices[i + 2] gives orig 'lha 4(r7)' but folds the 'addi r7,r29,0'
  (= old raw 4 shape); late-zero i inits (best + 1, inline start param, zero var) all const-fold early -> 'addi r7,r29,4'.
- Compiler/flag sweep on CCP/VD/ASP (s20/flags.py): versions 1.3..2.7 identical; -inline on/deferred/noauto/deferred,auto identical;
  only -opt nocse/nopropagation/-O3 change anything (all worse). The rotations are not a flags issue.
- QUESTION (not policy): none new.
- ChooseCoverPoint 23 -> 18 (s20/ccp/CCP_BEST18.cpp = soldier ccp/b.cpp + 'float half = 0.5f * radius; float distance = half * half;'):
  the whole FPR rotation (z f2 / y f3 / 0.5*radius f4) is FIXED by the named 'half' local (d3 'd2 = distance * distance' and
  d8 also 18; the permuter's junk 'distance = distance;' was a hint of this). Left: GPR this r29/type r30/from r31 vs orig
  this r31/from r30/type r29 - invariant under every from/this/type copy/split/wrapper form tried (s20/ccp/a3.py, s20/toy/).
  Toy (s20/toy/batch.py) reproduces it: CW colours callee GPRs in vreg order (later vreg -> higher reg), and source copies coalesce
  into the EARLIER vreg, so 'this' (first param) can't be made later by a copy.
- AnimStProcess ablations (s20/asp/abl.py a1-a6: prints owner/object/channel/user/slot regs): the owner/slot order FLIPS to the
  orig's (owner r31, slot r30, object r29, channel r28, user r27) when the 'flags & 1' callback is removed (nocb), the
  AnimWgtProcess call is removed (nowgt), FinishState is removed, or a kept pointer 'AnimWgtInfo_t* w = &slot->weight' lives
  across AnimWgtProcess (wvar: regs exactly orig but w stays in r22 -> 'addi r22' + 'lfs 0(r22)', 3 lines). Copies that CW
  propagates (s = slot, owner copies, inline param orders, accessor wrappers) never flip it. So the order depends on the
  interference/pressure inside the loop, not on vreg creation. Not solved; wvar.cpp is the nearest 'right regs' shape.
- MODEL (s20 toys + AnimStProcess ablations): small/low-pressure functions colour callee GPRs in vreg CREATION order (later vreg ->
  higher reg; params left to right; propagated copies change nothing; use counts and loop depth change nothing - s20/toy/r1-r4).
  AnimStProcess-size loops behave differently: removing a call/use or adding one live-across-call pointer flips owner/slot, i.e. once
  Chaitin simplify gets stuck the spill-cost (uses x loop weight / degree) order decides. compaction memcpy's depth-2 owner use is
  what gives owner its rank (nomemcpy -> owner r27). Levers to try next time: lower slot's cost (fewer slot uses, more degree) or
  raise owner's, WITHOUT changing final code. ChooseCoverPoint/MOHFL_VD are low-pressure (creation order) -> their orig orders
  (this > from > type; obj > width > height) look unreachable by source copies.
- perm_ccp on CCP_BEST18 (s20/perm_ccp, base 90, ~16k iterations): nothing. perm_asp on asp/wvar.cpp (s20/perm_asp, base 175).
- _AnimTranDoTransition ablations (s20/dt/abl.py): replacing the 'array' local by the global address (noarr, not a valid form: no
  hoisted reg) gives the orig's param regs object r29/channels r30/to r31 + slot r28; every real array-local form (ref, getter,
  in-loop, decl order) keeps array r30/slot r31 above the params. _unpack: angle swap after rotation spills (frame 240), worse.
- AnimStStartState: named 'cur' id copies / compare orders: the id clrlwi always gets r3 and the state load r0 (orig opposite).
- perm_asp (wvar base 175, ~7k iterations): only junk ('i & 0xFFFF...', float new_var); killed.

## HANDOFF (2026-10-29, session 20)
No landing. Improved draft: ChooseCoverPoint 23 -> 18 (s20/ccp/CCP_BEST18.cpp; FPRs exact via 'float half = 0.5f * radius;
float distance = half * half;'; left = GPR this/type/from rotation) - this is soldier's AIObject.cpp merge blocker (soldier paused).
Diagnostics: s20/asp/wvar.cpp has the orig owner/slot regs but an extra kept pointer (3 lines); ablation scripts s20/*/abl.py.
No whole-file merge is ready in the lane (all blocked by register-colouring fns). Claims released.
