# g3 (day2 group3) notes, 2026-11-13

Tools: s.sh KEY [src] [score] (sbs/score with include dirs), v.py KEY base vars.py (variant scorer),
slots.py (r1-offset store/load sequence ours vs orig), cd.py KEY src (check_draft on a scratch file).

## GenerateTestSplinePath (draft was 'differs' 817/len 824 vs 736)
gtsp/k.cpp: consistent, 92.3% similar, 731 real, length 737 vs 736. Found:
- real CVector3 header members (Sub, *=, -=, Scale, Add) for delta/tangent sweeps;
- 'if (reversed)' loop first; copy loops through a 'CVector3 v' temp, field stores interleaved
  (u_vol form) = orig x4 unroll and load/store order;
- back loop with the unsigned i ('i != -1' -> addis/cmplwi 65535, not unrolled);
- segment Set(float step, a, b, c, d) inline with '*(float*)&step' = the original's three dead
  float slots (20/12/8) + sqrtf y (16); 'float step = 0.25f / sqrtf(e)' before Set;
- hoisted 'CVector3 d, c, b, a;' then zero (chain zero.x = zero.y = zero.z = 0) = orig slots;
- statement order c = d0; a ops; b ops; d = p0 (o2) -> frame 416.
Left: one callee FPR too many (f15 saved) in the segment loop -> every r1 offset shifted 16.

Committed 8ab1da79 (pushed): GTSP consistent (k.cpp), TRK (trk/draft.c, C 14 / C++ 97), YUV 45 (yuv/b.cpp).

## Parks / tried this session (no change)
- RCMP::DECODER ctor (2): member chains impossible (different member types); store order inert. No extab (leaf).
- PublishEvent (3): callback local in loop 2 (any position) = 46 (it -> r31); member Notify, const item, slot local inert.
- Scroll (6): if/else inversion, early return forms worse; the 6 lines are the inlined Reset's schedule.
- CProfileInitClass ctor (202): orig materialises strings in 3 groups (bins 0-11, 12-22, 23-30); static inline
  helpers per group don't inline (calls). Blocked anyway: strings behind the stripped-code gap (+708..+1199).
- ChooseCoverPointByType (20): header Sub/LengthSq/Normalize form 57 (worse).
- AnimStProcess (23): indexed owner->slots[i] inert; loop-in-inline variants worse.
- LLCharObjCompositeMorph (18): value/target statement orders inert (CW canonicalises).
- UpdateAnimation (16): header Normalize body inert.
- DrawNode (58): IsOutside with header members (DotDotDot/Abs/Dot, extents vector) puts every op in the orig's
  operand order but colours worse (75-80); kept the old draft. dn/v_absdirect.cpp has e0..e2 in f0..f2 like orig.
- iSNDrandom: loop form fully unrolls but goes branchless (84); orig stage 1 has two compares.
- SNDMIXI_volramp (20): orig keeps &sndmix in three callee regs (r30 alignedWork, r27 value0f/scalemix/alignedMix,
  r31 loop bound) - no form found.
- ShellInit (135): placement '::new (sprite) CSprite' after a cast DWI_alloc gives 49 and the orig's single 'mr.',
  but loses the extab DELPTR (orig: DELPTR __dl__FPv on r26 at the ctor call). The class inline operator new
  (other units' form, e.g. CDualConfig::Init orig 'mr r30,r3; mr. r27,r30') gives the double copy here. Open:
  which new-expression gives one copy AND a ::delete cleanup. si/v_pl3.cpp.
- TRKSuppAccessFile: decl order (C mode) completely inert; read/done swap remains.

Committed 4ca0418b (pushed): GTSP 731 -> 211 (p.cpp): RIGHT-NESTED arc-length sum (freed the extra FPR, frame
416), m_segments[i].Set per branch, 'last' declared first, 'float* g = &gamma[last]', back loop from last - 1,
segment statement order via gtsp/ord.py (random swap/move climb with order constraints). Left (colour only):
delta[last] block volatile regs (orig offset r3, &points[last] r4, &delta[last] r9, &tangents[last] r3) and the
segment loop IVs (orig r6/r7/r8, ours r5/r6/r7), pair copies 'lfd,lfd,stfd,stfd' vs ours interleaved (header op=;
implicit/base op= far worse).
- RenderSystem: check_draft 'differs' = the operator[]-pinned vectors addressed as 'addi rX,r29,96; lfd 8(rX)' where
  the orig folds 72/88/104; not a logic bug. Not worked further.
- hist.py (mnemonic histogram ours vs orig over all drafts): no other draft shows an fmadds/fadds count mismatch
  (the right-nested-sum signature).
- DrawFragCount (MP, block user): looked only; r17/r18 IV init swap and f29/f30 - not worked (MP hold).
- Not attempted: UpdateTargetPosition (declined form), __sinit_buddy_cpp (known blocker, sinit2),
  __sinit_compartment_cpp (needs the whole compartment.cpp file), PATHI_seeknextnode (partial SROA of a 232-byte
  PATHTRACK copy; solo/pred notes), SetFinishedLevelData (rank1: degree promotion), CreateDestructable (pnew1 knot).
