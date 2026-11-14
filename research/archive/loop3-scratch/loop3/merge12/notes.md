# merge12 (2026-11-13): stopped early at the coordinator's wrap-up; nothing landed
## TestCDBBranchNodeCollision<CLine3> (1 line)
Not matched. Only the __construct_array argument registers differ. scratch/par/lines/notes.md s17/s18
already tried this exhaustively. New probes in v1.cpp (run with scratch/par/lines/s18/pv.py) all give OURS:
- a 2-D [1][20] array
- a typedef'd array type
- a pointer alias
- s+0 / &s[0]
- a 4*5 size
- a padding array ahead of it
- an inner scope
- the array declared before the OSGetTick
## cdbobject stepping stone (coordinator's ruling: one key-function fragment owns the file's .data)
WIP, tested only by compiling (not landed): cdbobject_dtor.cpp + tpl_cdbdtor.json (cdbobject_dtor flags, -RTTI on).
- The full IVolume view (17 virtuals; ~IVolume is inline, the rest non-inline).
- CCDBObject declares ~CCDBObject first, then the overrides, then the new TestCollision(CCSGVolume) last.
- chk.py: the dtor scores 0.
- Emitted sections match the original:
  - .data 92 B (@837 + __vt__10CCDBObject, at 0x80183a00)
  - .sdata 16 B (__RTTI__7IVolume, __RTTI__10CCDBObject, at 0x8034db84, after g_bDisablePassThru)
  - .rodata 11 B ("CCDBObject", at 0x80170790)
  - .sdata2 8 B ("IVolume" @838, at 0x8034fd90)
Next steps:
1. Add the file-comment header.
2. Land with a copy of merge11/mrg.sh: nid = frags = cdbobject_dtor, tpl = tpl_cdbdtor.json, --original-file cdbobject.cpp,
   file index from cdbobject_dtor.json; maybe --pool .sdata2.
3. Run one_unit, then source_link.py --jobs 4 (expect 38 -> 37).
4. Note the stepping-stone ruling in docs/Remaining.md in the same landing.
## debug_prof (planned, not started)
- debug_prof_bins defines both Render keys.
- debug_prof_register already owns CProfHistRenderBin's table (as file duplicates).
- Plan: merge bins with the cdbgstatsrenderbin_dtor fragment that follows it (contiguous, 0x800f451c..0x800f4608), RTTI on.
  - CDbgStatsRenderBin declares Render first; its dtor is defined __declspec(weak).
  - CProfHistRenderBin declares its dtor first and does not define it here, so its table is not emitted.
- Expect .data 0x8018b54c (52 B), .sdata __RTTI__18CDbgStatsRenderBin.
- Open risk: the CRenderBin/CRenderBinData base RTTI records are emitted again (register owns the originals). Try --pool .sdata.
