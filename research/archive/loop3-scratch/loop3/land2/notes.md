# land2 notes (2026-11-12)

Tools: tools/sc.py (real score of files), tools/slots.py (r1 stack-offset sequence ours vs orig), gen_ss.py (SoundSchedule
inline-grouping generator).

- Durability: research/drafts/pending/ (026b942d, pushed): UBB_BEST56 (q10), RS_BEST514 (q10),
  PATH_event_unit_autoinline.c (q5), DTC_BEST0 (q8), DoTargetCheck DISC0_ctorcast + copy_bool (q11).
- CalculateLevelStats: the 1 line is REAL (li r29,0 vs li r3,0; mr r29,r3), not a relocation. MostHit inline gives the
  mr but location r30 (9). Default-param inline, decl positions, weapon-loop inline, named format: inert/worse.
  Variants cls_*.cpp. Parked (shared-zero family, 60+ forms over sessions).
- TLT_LoadFromRepository: 13 = r7/r8/r9 rotation in the inlined find; p-first decl, &m_data[i], unnamed it: no gain.
- GetInitialBuddyOrders: NOT landable: without DumpBuddyVector 7 -> 39 (gi_nodbv.cpp); without the vec_iter
  GiveBuddyOrders overload 7 -> 19 (gi_noov.cpp). Needs the audit's likely-declined DumpBuddyVector. Not worked further.
- SoundSchedule::Update: 50 -> 0 (ss_final.cpp; merged unit soundschedule_sound.cpp). See TRICKS (stack-slot order).
- LANDED a842943c (pushed). Lock released.
