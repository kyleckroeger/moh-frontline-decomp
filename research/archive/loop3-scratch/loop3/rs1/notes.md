# rs1 (Rising Sun helper-boundary pass over geometry + Audio/Shell rows), 2026-11-11

Tools (scratch/loop3/rs1/): rs_syms.txt / fl_syms.txt (objdump -t of RS MOH3RDVD.ELF / FL Moh2RelGC.elf),
rsbl.sh <RS symbol> [-d] (bl targets or disasm), cmp.py <FL mangled names...> (FL bl targets + RS same-name
functions' EXTRA bl targets = helpers RS keeps that FL inlines), real.py <func> <file> (score.real on a file).

## Survey (cmp.py over all unblocked Collision/Animation/Audio/Shell rows)
Geometry: no lead. _find_aim_blend_values / aim_cb / anim* (RS animation is EALA Character, no counterpart);
CVolBox::TestCollision(CVolSphere), both GetExtents, GetNearestPoint, Multiply: RS calls nothing extra;
TestCDBBranchNodeCollision<CLine3>: RS has no CLine3 branch template (different CDB code); Rotate: draft
already has BuildRot; CCDBVolBox::TestCollision(CTriangle): draft already has the 3 separate() overloads;
CVolBox::TestCollision(CVolCapsule): RS boxcapsule_testaxis(ds, dd, half, r) = draft's AxisSeparates with
the first two params swapped: swapping is worse (fast 206 -> 290), draft diffs are contact-block scheduling.

## Systems (Audio, Shell/HUD)
- PATH_event 0x8010b93c (1884): IMPROVED real 26 -> 6 = scratch/loop3/rs1/pev/BEST6.cpp. Keys:
  (1) RS PATHI_getmastertrack as 'static inline void PATHI_getmastertrack(void)' {latency = 0; leader = -1; beat = 0;
  for (int i...) {track = tracks[i]; if (track && !track->paused && isplaying(track)) {if (latency < track->latency)
  {...; beat = track->current->beat;}}}}, called AFTER the event field inits (54 -> 34 fast; 'before' 58);
  (2) event field order node, 2a, type, 2b, track, ... (34 -> 18); (3) record index as a 3-statement chain
  'int index = eventnum * map->beats; index += map->beats * (j * map->events); index += pfstate->beat;' (one-statement
  forms reassociate beat first; 'index = A + B; index += beat' gets FACTORED into (eventnum + j*events)*beats = 1 instr short).
  Left: r10/r11 colour of beats vs eventnum*beats (6 lines). Inert: 120 association forms, index type/scope/decl position,
  map/beats locals, map decl position. (The eventstart draft already calls this loop 'findleader'.)
- SNDPKTPLAY_start: RS SNDPKTPLAYI_gethighchannel(int voice, int channels) (index the voice table inside):
  every form (global, local base, &voices[voice] pointer, inits) is worse (17-33 fast vs 4): it folds the
  'addi r7,r29,0'. lv_c(short*) stays best.
- Scroll: RS Reset split = draft (already inline). PopUpMessageHandler_Loop: RS redesigned (float arg, viewports).
- PATHI_seeknextnode / eventstart, SNDVOICEI_alloc (isreserved(voice,count): identical code), SNDI_precalcaztospkrvol
  (RS float equalpower = other version), MIX_audioslice (RS MIXI_stop internal), DrawBulletStats (RS HUD rewrite): no lead.
- LANDED 95aae29e pumhandler_umt_unload += CUserMessageTable::Load (436): RS CreateUMStructFromFile inline member
  (re-reads m_index/m_text), loop locals hoisted to its top in order entry, end, i, extra, length. Figure 1,239,036 B.
- PATH_event draft in research/drafts updated to BEST6 (in the same commit).

## Handoff
Lock released, claims released, nothing running. Best WIP: PATH_event research/drafts (6 lines, r10/r11 colouring of
beats vs eventnum*beats; try a decl sweep of locals hoisted into PATHI_getmastertrack / the j loop, per the Load lesson).
Not done: Video/System/Other rows of the systems lane (outside this brief); cmp.py can survey them in one run.
