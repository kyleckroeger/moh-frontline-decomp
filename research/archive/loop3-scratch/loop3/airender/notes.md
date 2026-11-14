# airender lane notes

Agent 1 (2026-11-11). Tools in this dir: var.py (variant sweeper: python3 var.py tmpl base func addr size variants.py,
variants.py defines VARIANTS=[(name,[(old,new)..])]), dr.py (objdump -dr of our compile), syms.py (our @-labels + .sdata).
Skipped particlesystem/decal thunks + CParticleSystemManager::Draw: they are the systems lane's Phase A (claimed by systems).

## Parks / WIPs
- FlagAsOccupied: fao/BEST3.cpp = 3 real lines (draft was 4). Change: mask lines `types = types & 0xff; types = types & (CoverPointTypes(1) | 2 | 4);`
  (operator&(const int&) value form). Left: colouring of the @-constant loads (orig 1->r7, 0xff->r8, (1|2) result in r6; ours 1->r8, 0xff->r7, result r8).
  CLUE: orig .sdata labels @766=0xff @767=1 @768=2 @770=4 (gap @769 between 2 and 4); table @755. Ours contiguous @14..@17. Something
  creates one extra label between the 2 and the 4 in the original source. ~70 forms swept (fao/v1..v7.py), none moved the colouring.
  Label probes (fao/probe_*.cpp, syms.py): in ours the unrolled loop consumes 7 labels between table and 0xff (orig 10),
  'types & 0xff' 1 label, the mask 3 contiguous. Only 'operator|(CoverPointTypes o)' BY VALUE (fao/m4.cpp) reproduced the gap
  before 4 (but adds stack temps, 59 lines). So the orig likely converts ints to CoverPointTypes temporaries somewhere; unresolved.
  Landed sibling GetAvailablePeekPosition (src/game/CoverPoint_modified.cpp) class gives 5 here.
- CSoldierObject::Draw: 8 (only the inlined RemoveWeapons weapon IV r28 vs orig r24). Pointer loop, for-scoped i, mesh decl position
  (sd/v1.py) all invariant. Matches old notes (permuter 113k no gain). Parked.
- DrawTextCenteredF: 3 (2 fmuls operand orders, conv*var). Mul inline, FWidth inline, compound, -(-x), divide: no gain (dtcf/v1.py).
- GetPeekLocation: gpl/x1.cpp (copy of scratch/par/soldier/gpl/x1.cpp) = 3 (draft index says 6; x1 is better): only the @770 (=4) load
  scheduled after 'or r5,r5,r4' in ours, before it in orig. operator| overloads/by-value/mask |= forms (gpl/v1.py): no gain.
  Orig label structure of the shared GetTypes inline: table @755, 0xff @766, 1 @767, 2 @768, 4 @770 (gap 3 more labels before 0xff
  than ours, 1 between 2 and 4). Each 'if ((types & K).m_bits)' costs 4 labels in both. Mask expression forms e2..e8 (fao/v9.py): no gap.
- DrawTextCentered: dtc/BEST7.cpp = 7 (draft 8): inline 'float CScreen::ScaleX(float f) { return (float)GetWidth() * f; }' for the
  width (width = screen->ScaleX(g_viewport.m_width)) gives viewWidth its own f30 like the orig. Left: the two fmuls operand orders
  (orig conv*var; ours var*conv, invariant to every source order/inline/compound form, dtc/v1-v2.py) + the f28/f29 swap of the
  FONT scale loads. Same multiply-order knot as DrawTextCenteredF; ScaleX through both products moves center after getrecta (49).
- EndDraw: ed/BEST11.cpp = 11 (draft 19): 'stringPacket.color = renderFont->m_data->GetColor();' (by-value return = the orig's word
  temp at 8(r1)) + a member-wise CColor::operator=. Structure now exact; left: byte load order (orig 9,10,11,8 into r7,r8,r9,r0)
  and m_data in r6 vs ours r9. operator= orders (24), temps read orders (24), statement orders/data local (ed/v1-v3.py): 11-14.
- HandleExplosion: hx/f1s2.cpp = 6 (draft 53!). Key: second loop 'for (int j = 0; j < indices.m_size; j++) { if (!SwapCollisionDisabled(indices.m_data[j])) SwapTexture(indices.m_data[j]); }'
  + 'int index;' hoisted. Left: first loop index r6 / m_data r7 swapped (ours r7/r6); invariant to Find forms, push_back, decl
  positions, Contains inline, continue (hx/v1-v4.py). hx/pc.cpp = same 6 in C-parsable form (TriVec/IntVec typedefs + Init) for
  the permuter: perm_hx/ (base 35). Landing needs the old plan: merge compartment_fastvec_tri + compartment_texswap from 0x800756cc.
- HandleExplosion SOLVED (perm_hx found a redundant re-read; natural form): first loop
  'if (tris.m_data[i]->m_texIndex.m_index >= 0) { int index = tris.m_data[i]->m_texIndex.m_index; ... }'. Unit source
  hx/compartment_fastvec_tri.cpp (prepends to compartment_fastvec_tri; the unreferenced weak fast_vec<int> dtor needs
  --linker-stripped __dt__Q23dwi11fast_vec<i>Fv).
- CStaticMesh::InitClass: smic/staticmesh_initclass.cpp = 0 with the pooled .bss block (new unit staticmesh_initclass).
  Destructor-chain records @465/@466 (12 bytes each) are held by 'DestructorChainView' size views so the bins land at +444/+492.
- CCompartment::InitClass: cic/t1.cpp = 0 (pooled .bss block + CLightPool inline Init(256)/Reset as in compartment_shadowbins).
- CStaticMesh::Draw / CCompartment::Draw with the pooled .bss: the block's matrices are CMatrix objects, whose view has the
  inline InitClass-checking constructor that the functions' own CMatrix locals need, so defining them as CMatrix would compile
  an __sinit (forbidden). Needs a size-view type for the block matrices plus casts at the uses; not tried yet.

## Agent 1 handoff (2026-11-11 ~21:55)
LANDED: 056f3dde staticmesh_initclass (CStaticMesh::InitClass 152, .bss pooled); 69455560 compartment_initclass
(CCompartment::InitClass 244, .bss pooled). Both pushed. Verified 1,224,244 bytes.
HandleExplosion (736): code-exact (hx/compartment_fastvec_tri.cpp, prepend to compartment_fastvec_tri), but NOT landable with
current tools: the compile also emits the unreferenced weak __dt__Q23dwi11fast_vec<i>Fv BETWEEN HandleExplosion and the CDBTri
dtor (and its extab between theirs); --linker-stripped fails in one_unit ('Linker-stripped object is inside the kept data: @143')
because data_strip only removes stripped objects from a section's tail. Explicit-instantiation placement doesn't change the order.
Needs data_strip.py to strip a mid-section extab/extabindex item (tooling decision) -> coordinator/maintainer question.
Released claims: FlagAsOccupied, Draw(soldier), GetPeekLocation, DrawTextCentered, DrawNode(staticmesh), EndDraw,
ConstrainVelocity(animobj), Init__NotTheVirtualOne (blocked: string-pool base, decal.cpp tail), CScene::Draw (scd/: frame
416 vs ours 432 and camera+192 computed before the screen call; unsolved), HandleExplosion (parked as above).
Next agent: CStaticMesh::Draw / CCompartment::Draw with pooled .bss need a non-constructing view for the block's CMatrix objects.

## Agent 2 (2026-11-11 ~22:00)
Tool fix 5ef1c047 (port_unit kept_span: moved linker-stripped records not kept by address). LANDED 8efb7975 HandleExplosion (736).
Draw units: smd/staticmesh_draw.cpp, cd/compartment_draw.cpp (both 0 in isolation, one_unit VERIFIED) -> landing as staticmesh_draw + compartment_draw.
Probe harness for port_unit without touching config/: strip/probe.py <id> <src> [port_unit args] (drafts into strip/config, runs strip_objects).
LANDED d637ce32 staticmesh_draw + compartment_draw (556 + 860, .bss pooled).
- CCompartment::Init (2628): ci/hd_best.cpp = raw 622 / struct 147 (sd.py; draft was struct 315). Frame now 64 = orig, r20..r31 saved.
  Fixed: block views up to g_TextureSwapCache (base+0x240) incl. @541/@542 chains; CRenderBinData mem-init ctor (priority 3) so the
  three vptr stores follow the field stores; ShapeHeader 'unsigned m_type:8; int m_paletteOffset:24' (rlwinm/srawi.); short width/height/
  count (lha); '((int)header & 0x10) != 0x10'; palette check 'if (!(t==0x20||t==0x21||t==0x30||t==0x31)) { if (t != 0x32) palette = 0; }'
  (only the final 'bne; b' differs); texture switch with the CI cases first; GX enums + '(m_flags & 8) ? GX_REPEAT : GX_CLAMP' (gives
  neg/or + rlwinm like orig); display list 'char* p', int size; swap +0x2b as bitfields (m_enabled 0x20, m_numAlternates low 3) and
  'swap->m_enabled = entry.m_active != 0'; LinkBins 'last' computed first.
  Left (mostly scheduling): GX format ternary diamond is scheduled before the call setup in orig (after in ours; locals/inline helpers
  don't move it, locals blow up regs); stride diamond (orig real if/else 'beq; li 4; b; li 8', CW makes ours a select); placement-new
  result 'mr r4,r26' before the null check (orig uses new's result; 'bin = new (bin) ..' gives it but moves bin's register);
  block base register r30 (ours r28), base r28 (ours r29); command-byte store scheduling. Tools: sd.py (reloc-masked struct diff),
  ci/hill.py (decl-order hill climb: no gain).
- CCompartment::Init round 2: ci/BEST.cpp (= e2) struct 79 / raw 578 (sd.py / sc.py). Keys found: Load is CALLED FROM THE CONSTRUCTOR
  ('CCPTMatBin(int base) { Load(base); }', loop 'bin = new (bin) CCPTMatBin(base); if (previous) previous->m_next = bin;
  bin->m_prev = previous; previous = bin; if (flags & 2) ...; bin++;') - explains the null check skipping Load and the 'mr r4,r26';
  GX format as a local inside braced case blocks ('case 0x19: { GXCITexFmt format = type == 0x18 ? GX_TF_C4 : GX_TF_C8; ... }')
  with 'unsigned int type = header->m_type;'. WATCH the inline budget: many small changes make CW stop inlining Load in one loop
  (struct jumps to ~359, len -6..-192); check sd.py 'len' after every change.
  Left: callee registers rotated (orig this r31, block base r30, &g_GlobalCpt r29, base r28; ours base-block r31, gc r30, base r29,
  this r28; decl-order hill climb: no effect), stride diamond, palette final 'bne; b', display-list store scheduling (orig loads all
  command constants first), LinkBins scheduling.
  Also tried (no change): RestoreSwaps / FixUpHeader as inline members (rotation unchanged), decl-order hill climb (ci/hill.py).
  Next idea: permuter needs a C-parsable harness (big); or study why 'this' drops below the hoisted block-base temp (z3 vs hd_best:
  the inline-budget-sensitive Load changes flip it).

## Agent 2 handoff (2026-11-11 ~22:25)
LANDED: 5ef1c047 tooling (port_unit kept_span: linker-stripped records never kept by address; test StubKeptSpan);
8efb7975 compartment_fastvec_tri += HandleExplosion (736); d637ce32 NEW staticmesh_draw + compartment_draw (556 + 860, .bss pooled).
All pushed. Verified 1,227,792 bytes.
WIP: CCompartment::Init ci/BEST.cpp struct 79 / raw 578 (draft 315/639); notes above. __sinit_compartment_cpp untouched (claimed then
released; the sinit would construct the .bss block objects the compartment.cpp unit places - check ownership before trying).
Released claims: HandleExplosion, Draw(staticmesh), Draw(compartment) (landed); Init__12CCompartmentFv, __sinit_compartment_cpp (parked).

## Agent 3 (2026-11-11 ~22:40) CCompartment::Init
ci/BEST8.cpp = struct 5 / raw 131 / len equal (was BEST.cpp struct 79 / raw 578). Tools: ci/vs.py, ci/vsr.py (variant sweeper, prints
struct + register-kept struct via sdr.py: KEEPREG=1), ci/cmp.sh <src> <off0> <off1> (orig|ours raw side by side).
Fixes, in order: (1) Load declared 'inline' explicitly (lifts the auto-inline budget, lets bigger forms inline both times);
(2) palette tail 'if (palette->m_type == 0x32) palette = (PaletteHeader*)((char*)header + paletteOffset); else palette = 0;'
inside the 4-way check = the 'bne; b' AND it un-rotated all callee regs (this r31, block r30, gc r29, base r28);
(3) LOAD macro evaluates the value first: '{ unsigned int v = value; p[off] = 8; p[off+1] = reg; *(u32*)(p+off+2) = v; }';
(4) 'p[48] = 0; { unsigned short v = list->m_count; p[49] = 0x98; *(u16*)(p+50) = v; }'; (5) loops 'i++, ptr++' in the for
clause (i increments first); (6) LinkBins written in place (no inline): count load inside the if; (7) stride
'switch (list->m_flags & 1) { default: stride = 4; break; case 1: stride = 8; break; }' (keeps the real if/else; every if/ternary/
type/enum/bitfield form becomes a select).
Round 2 (same agent): ci/BEST_ci.cpp (= BEST15) = RAW 2 lines, struct 0, frame and length exact. Further keys:
(8) display-list writer as in the landed staticmesh_init.cpp: 'WriteCP(char* p, char reg, unsigned long v)' + 'WriteVCD(list, p)'
+ 'FinishDL(list, p, stride)' + 'BuildDL(list, int stride) { char* p = (char*)list->m_commands; WriteVCD; FinishDL; }', and the CALLER
block does 'CPTDisplayList* list = chunk->m_list; if (list) { 5x offsetPtr; int stride; switch...; BuildDL(list, stride);
DCFlushRange(list->m_commands, 52); }' (same in the swap loop). Stride as a param copy = r0 like orig; p ranks low = callee r27.
A single inline holding all of it (FixUpDisplayList) or a VertexStride() helper: regs 112-180 or blows the inline budget.
(9) fresh loop vars: 'for (int n ...)' for the m_swaps offset loop and 'for (int m ...)' for the restore loop (gives the separate
'li 0' IV inits); alpha bin loop uses its own counter 'n' (function-level, decl order base, previous, bin, i, n, chunk, node, swap, j, k).
LEFT (2 lines): swap-loop expansion only, 0x7c4/0x7cc: orig 'lwz r22,12(r29); clrlwi r22,r22,2' (normals raw + masked share r22),
ours 'lwz r23; clrlwi r22,r23,2'. The chunk expansion matches. Invariant to: WriteCP param types (16 combos), normals temp
forms, WriteVCD/FinishDL param order, j/k/i/stride/size types, decl orders (120 x 2 sweeps), list/stride scope (block/function),
'if (T* list = ...)' forms, a 550-sample random knob search (ci/rs/rs.py, rs2.py). Next: permuter (needs a C harness: placement
new + reference casts in the body) or accept as WIP. __sinit_compartment_cpp untouched.

## Agent 4 (2026-11-11) CCompartment::Init SOLVED
Fix: swap-loop copy of the writer (WriteSwapVCD/BuildSwapDL) masks normals through a POINTER local:
'signed char* normals = list->m_normals; normals = (signed char*)((unsigned long)normals & 0x3fffffff); WriteCP(p + 12, -95, (unsigned long)normals);'
Same form in the shared WriteVCD breaks the chunk copy (2), so the copy stays separate. ci/FINAL.cpp = src/game/compartment_init.cpp.
Integer forms ('v &= mask', unsigned int/int temps, <<2>>2) coalesce raw+masked but swap the 12/-95 constants (7).
Permuter harness: pci/P.cpp (macros PNEW/OFFV/OFFG for placement new + reference casts, this-> on members, C89 loop counters),
pci/perm* (perm/perm4 whole function, perm2/perm3 PERM_RANDOMIZE on the swap loop; base.c has a typed PERM_PRETEND with real C
structs - empty typedefs made most mutations crash). ~15k permuter iterations found nothing; the manual sweep (pci/tv.py + v*.py,
'swap' scoped replacements) found it. Fix 1 (explicit inline) on the near-misses fao/gpl/dtc/ed/dtcf/sd: no change (their helpers
are in-class, already inline; pci/../fix1/).
__sinit_compartment_cpp still open (draft research/drafts/compartment_init.cpp, which still holds an old Init too).

## Agent 5 (2026-11-11) Rising Sun helper-boundary pass (TRICKS mid1) on the parked near-misses: NO landings
Tools: airender/rs/bl.sh ADDR SIZE (bl targets of an RS function), rs/both.py (scores DTCF + DTC from ONE compile, font_render.json).
RS counterparts (../moh-rising-sun/orig/GR8E69/MOH3RDVD.ELF) and outcome:
- FlagAsOccupied / GetPeekLocation: RS CoverPoint is a different design (no peek-type bitmask/GetTypes table; FlagAsOccupied() takes
  no type, calls IsOccupied; GetPeekLocation calls GetDirectionVector(CVector3&) const, GetStyle (TriggerObject field), GetModifiedPosition).
  Tried GetDirectionVector as an inline CCoverPoint member in gpl/x1 (rs/gpl/v.py): 3 (same) or 11. Nothing new.
- CSoldierObject::Draw: RS RemoveWeapons is out of line with the same shape as our inline (detaches, IsNodeInScene/Remove/DestroyWeapon
  loop, mmg weapon); rest is RS-only (ShouldDrawFlag, AddCommand). Nothing new.
- CScene::Draw: RS takes a ready CDrawContext (EAGL), Lock/Unlock, GetNodesBegin/End iterators. Named camera->m_tm192 / GetTM192 temps
  before the screen call (rs/scd/v.py): all 49. Nothing new.
- SweepExtents: RS has an out-of-line SExclusionEntry(const ISceneNode*, const ISceneNode*, float) ctor that orders the pair, a stack key +
  operator== search, a UNodePair ordering ctor, list iterators for the outer loop. All 22 combos (rs/sx/v.py) and 5 iterator forms
  (rs/sx/w.py) compile to the IDENTICAL code (39). Left knot = outer-loop rotation (orig node r31/sceneNode r29/iter r28; ours r29/r28/r31).
- ConstrainVelocity (CAnimObject), CStaticMesh::DrawNode: no RS counterpart (RS uses CClsnConstraintsBase / EAGL DrawMesh).
- EndDraw: RS is EAGL (enddraw has no colour code); RS CFont::GetColor is out of line. Landed font_render GetColor/CColor copy-ctor forms
  in ed/BEST11 (rs/ed/v.py): 11-21, no gain.
- DrawTextCentered: RS has GetScale(float&, float&), ComputeJustification, GetRenderedSize (none match Frontline's direct FONT_getrecta).
  FINDING: DTC goes 7 -> 3 when the inlined DrawText declares scaleY BEFORE scaleX ('float scaleY; float scaleX; scaleX = ..; scaleY = ..;'
  or with GetScale, rs/dtc/g2.cpp, r1.cpp). NOT landable: the same change takes DTCF 3 -> 7 and the landed out-of-line DrawText
  (font_dtor) 0 -> 4. DTC's own scale order is the opposite of DTCF's/DrawText's with every form tried (rs/dtcf/c.py: 8 forms);
  DTCF with DTC inlined into it (rs/dtcf/n1.cpp, deferred-style nesting) = 11. So something ELSE in DTC's body sets the colouring;
  remaining DTC/DTCF lines are the conv*var fmuls operand order (unchanged).
Released all claims.
