# data1 notes (secondary work: initialised data for source_link undefined symbols)

Drafts in scratch/loop3/data1/init/ -> land into research/data/init/<original file>.cpp
(one file per original file per build/audit/file_map.json; '?' files -> unassigned.cpp
or file inferred from evidence, marked). Checked with check_data.py on the scratch path.

Tools: dump.py (orig symbol section/size/file/bytes/relocs), rd.py (bytes at address),
decls.py (existing extern declarations), syms.tsv / small_vals.txt.

## Skipped (policy / later)
- 84 __vt__ symbols: PortReadiness step 2.3 says vtables come from step 3 (class decls).
- 15 names not in the original (C_VEC*, PSVEC*, Multiply__7CMatrix..., __dt__..32/36, etc.):
  those are source naming bugs, not data.

## Landed
- d5d1282e: 73 game globals, research/data/init/ (24 files + unassigned.cpp); check_data
  accepts same symbol+addend relocation (SPR_END one-past-end).
- 18f0a9a4: 35 library data symbols (EA, MSL). Pushed by the coordinator after a usage-limit stop.
- (landing) shell export tables, 44 symbols.

## Next
- shell export tables drafted: init/ispauselibrary.cpp, isShellLibrary.cpp,
  ispausegroup.cpp, isShellGroup.cpp (44 symbols exact, incl. 40 not-yet-referenced
  szLib*/i* arrays the four roots point at).

## Left undefined (not initialised game data; for the maintainer)
- vtables (70 now): step 3.
- 15 names the original lacks (source naming bugs in units/drafts).
- _eti_init_info (extabindex, runtime exception index: linker/runtime-generated, policy).
- axDspSlave + axDspSlaveLength: platform-owned per PortReadiness (DSP blob).
- Render__15CUcodeRenderBinFR10CDmaPacketPv: new since run start, from another lane's landing.
## Side effect
- g_promptBuffer is now fragment_local (init/bsbifunc.cpp points g_promptCursor at the
  bsbifunc unit's file static); resolves when bsbifunc's fragments merge.

## layout_lint --reuse (2026-11-12, not acted on)
Reused build/layout/views.json from 2026-11-10 (stale: no recompile), so the tsvs came out
unchanged. 1198 files, 1299 classes, 170 conflicts (164 member, 6 width), 24 draft-only
leads. Saved in lint_conflicts.tsv / lint_classes.tsv here. A full lint (recompile) is
needed for current numbers.
- [landed] d0f84b89 shell export tables (44).
