# struct2 (structural matching of far-off drafts), 2026-11-12

Tools: sc.sh/sbs.sh/hill.py/frame.py copied from struct1; sd.sh <src> (sdiff -v for RenderSystem), ours.py <src>
(dumps our RenderSystem asm to <src>.s), cp.py <src> <func> (asm of any probe), var.py base out 'old' 'new'... (substitute + score).
rs_orig.s / rs_orig_full.s = original RenderSystem (masked / with addresses).

## CParticleSystem::RenderSystem (4532) -- PARKED, best RS_BEST_pinned.cpp (structural 1069 -> 943, fast 1691 -> 1381)
Draft rebuilt as a full particlesystem_link unit (head.cpp + types.inc + bss.inc + body): real virtuals (link unit's
table with return types bool/int/float), CColor default ctor (0,0,0,0x80), CMatrix ctor + copy ctor '{*this = other;}'
(__as__), member Multiply(toWorld, toCamera), GX inlines from <dolphin/gx.h> (GXPosition3f32/GXColor4u8/GXTexCoord2f32
with float colour args -> per-vertex u8 conversions), .bss block defined as size views (base r29 = ...bss.0, +112
g_WorldToCamera, +188 g_ParticleSystemManager; port with --pool .bss). Findings:
- switch source order 'default, case 0, case 1' (orig falls into default body); 'bool rotate = HasRotation();
  rotate &= g_bAllowRotation;' (deferred clrlwi); Get_currentMission returns unsigned; count >>= 1; endTime 'if
  (!m_emitting && endTime > m_maxTime)' (bit 6 of +136); vfn order GetLocalToWorld, now, Delay(72)->period,
  Rate(68)->count, ParticleLifetime(80).
- STACK SLOTS: all match now (toWorld 688, toCamera 624, gc 572, rot matrix 536, then CVector3s 520..104 in this decl
  order: sizeLow sizeHigh sizeRange spawnLow spawnHigh spawnRange velLow velHigh velRange pull accel drift birth deathBase
  birthRange deathRange right up offset force spawn vel color size centre a b; X/Y/Z by-value param copies 88/72/56
  (inline 'RotateByRows(CVector3& out, const CMatrix&, CVector3 v)'); 4-byte: growth c0..c3 rotLow rotHigh a0..a2).
  Rule seen: CMatrix/arrays first in decl order, then the rest in decl order (first declared = highest address); the
  ctor-less vectors are hoisted to the top, CMatrix/CColor declared where their ctors run.
- BLOCKER (policy): the original keeps all 18 named vectors + the 3x3 matrix in MEMORY (stores, some dead: spawn/vel
  are stored and never loaded) while 'axis' (Z) stays in f27-f29. MWCC 1.3.2 scalar-replaces these locals unless a NAMED
  pointer/reference is bound to them ('CVector3* p = &v;', 'V& t = *this' inside an inline). Probed and NOT pinning:
  inline/auto-inlined helpers with ref or pointer params, operator[], accessors, base class, array member, union with
  double pair, out-of-class ctor, copy ctor/assign, returning *this, header CVector3. A dtor pins but adds EH actions
  (orig extab 4c8a0000 00000000 has none). RS_BEST_pinned.cpp uses 'CVector3* pin_x = &x;' aliases = declined form.
  -> Question for the maintainer: is there a natural form that pins? Without one RenderSystem can't land.
- Remaining after pinning: setup-block scheduling (colour conversion order c0 r,g,b; c1 b,g,r; c2 r,g,b; c3 b,g,r
  suggests a different colour-vector construction), frame 1456 vs 1504 (6 fewer spill slots), one extra frsp on endTime.

## Other targets this session
- DMMorphBlendDisplayLists: int/long/uint/'!=' inner-loop counter forms all keep the CTR loop (orig: non-CTR 'cmplw r25,r9;
  blt'); not pursued further (history: 10-20 park).
- TRKSuppAccessFile: skipped (lane-parks: pure r29/r30 allocator swap at struct 0 after exhaustive tries).
- random (EA, ipad): skipped; same unsolved stage-1 'addi r7,r4,0' as iSNDrandom (lane-parks 450/533); best structure is
  scratch/par/parks/s16/rn/p1.c (iSNDrandom).
- PopUpMessageHandler ctor: PUM_BEST.cpp (from systems/pum/d2.cpp) structural 19 -> 1 (the 1 = relocated -3092 offset,
  not real): 'SetColor(CColor(63, 63, 63, 128))' byte-ctor TEMPORARIES + CSprite class 'operator new(size) { return
  DWI_alloc(0, size, 0x400); }' + 'unsigned char unknown[100]'. Left: this r31/rodata-base r30 swap and r26/r27 (Load's).
  NOT landable: pumhandler.cpp's .rodata block = dwi primes (108) + 188 B of float/colour templates that NO code in the
  file references (orphans) before the ctor's strings at +300 -> gap-function question.
- YUV_WriteRow: 14 structural = scheduling of the second 4-word group; writing it like the first group (a,b,c,d interleaved)
  is worse (64). Permuter 20 min -j2 (perm_yuv/): 1860 -> 1490 only via a junk 'new_var = 6 * 2' index device. Parked.
- PlayScriptedSound (1336): WRONG TEMPLATE in research/drafts/index.tsv: it is sound.cpp's FIRST text function
  (0x800e9ba8, right before sound.json's 0x800ea0e0), not pathxSND. With sound.json's flags (t_pss_sound.json: lmw on,
  fp_contract on, inline auto,deferred, Cpp_exceptions on) the draft goes 133/31 -> 98/14. PSS_BEST.cpp ('int* data =
  g_pGenericSoundMemory;' moved before 'int type = 7;') 68/12 real-ish: left = the two CVector3 by-value copy slots swapped
  (104 vs 120), orig computes the data address after the pitch MathFunRandomI64 call and keeps it in r22 (one more saved
  GPR), ours computes it early ('mr r23,r3'); moving the decl before pitch gives fast 46 but recomputes it (struct 16).
  The 'li r3,0' vs 'addi r3,r13,-29148' (g_SoundSchedule) and stfs/lfs r13 lines are relocation-only. Landing = prepend to
  the sound unit (deferred inlining: last in source). Coordinator: fix the index.tsv template to config/GMFE69/sound.json.
