# systems lane notes

## Agent 1 (2026-11-11)

Landed:
- fbe4be84 particlesystem_active += CParticleSystemManager::Draw (628): `--pool .bss`.
- f12a2e09 particlesystem += 5 @32@ thunks, decal += 3 @36@ thunks (64).
- 4fbde63f NEW UserInterface_timestamp: ResetTimeStampStuff + InitTimeStampStuff (464), `--pool .bss`
  (scratch/loop3/systems/ts/UserInterface_timestamp.cpp, both 0 in fast.py).

KEY TRICKS
- `.bss` block-base functions (`lis/addi ...bss.0` then +offset): define the block's objects in the
  fragment (size views are fine, NO constructors so no __sinit is compiled), and port with
  `--pool .bss`. NOBITS pool items compare as zeros. Globals are fine too (each unit links alone).
  Other parks blocked only by "(b) .bss base" may fall the same way (CStaticMesh::Draw,
  Gameloop_Restart, DestroyBuddyDataStructures, bsbifunc built-ins) IF the fragment needs no sinit.
- Adjustor thunks: CW emits a class's table + thunks only in the TU defining its key function (first
  non-inline virtual DECLARED). Declare a leaf member first (IsUsed), define it with its real body,
  `--file-duplicate` it and the `__vt__`. A key function with extab (Init) breaks pairing.

Parked:
- RCMP::DECODER ctor (0x80105560, 4 lines: vt in r6 vs r5; zero 'li r5,0' scheduled before the vt store).
  Tried here: 4-arg CODEC_IDATA ctor with defaults inlined, mem-initialisers, member-wise copy, local
  pointer copy, trailing return: all 4 (v2 10). RCMP_SYSTEM dtor + __sinit_rcmpbase_cpp wait on it.
  Order idea for the weak dtor (orig emits it BEFORE __sinit): define it out of line with
  __declspec(weak) between the ctor and rcmp_sys (as particlesystem_active's IsActive). Untested.
- PopUpMessageHandler ctor (0x800ba3d0, 988; appends to pumhandler.json, then __sinit_pumhandler_cpp
  0x800ba7ac): the ctor INLINES CUserMessageTable::Load (umTable.Load(stringFile, lengthFile)). Draft
  research/drafts/pumhandler_ctor.cpp 133; scratch/loop3/systems/pum/d2.cpp ('inline' Load from bigfiles'
  umt2/z0t1.cpp + ChangeEndian) 113: frame and byte-swap now right; left = callee reg rotation (Load's
  own 10-off problem), the CColor temps, and the DebugMsg string at ...rodata.0+300 (ours +0) = the file's
  .rodata string order (strings of earlier functions first) -> needs those strings in the unit. Plain
  (non-inline) Load is not auto-inlined (d1 209). Not landable soon.
- Not retried (history says exhausted): SNDPKTPLAY_start, CInputManager::Update, CUserMessageTable::Load,
  TLT_LoadLevelContentsFile (needs Repository in the same unit for a literal position).
- Released claims at stop: RCMP_SYSTEM dtor, pumhandler ctor/sinit are PARKED (free for the next agent).
Next for this lane: scan remaining rows for other '...bss.0' users (scan script idea in TRICKS.md); the
lane's remaining rows are long-worked near-misses. Figure after agent 1: 1,220,428 (86.27%).

## Agent 2 (2026-11-11)

Landed:
- 9de067ca debug_prof_register = register + dump units merged, += DebugProfTimerVerifyNew (552) + DebugProfTimerDumpAll (164)
  = 716 new code bytes (+ the CProfHistRenderBin table/base lists .data 60 and RTTI records .sdata 24). Source
  scratch/loop3/systems/dp/final.cpp; flags like.json (deferred + RTTI on). See TRICKS 'loop systems 2'.
  VerifyNew's research draft had an inverted loop test (orig breaks on a USED slot) that check_draft missed.

Scan result: in this lane the only remaining '...bss.0' user is __sinit_pumhandler_cpp (needs the parked ctor); no
adjustor thunks/vtable-adjacent rows left. '...rodata.0' users: TLT_LoadLevelContentsFile, ShellInit, Play
(OPTION_PARSING_PLAYER, 523), UserInterface::Init (398), pumhandler ctor (133), CProfileInitClass ctor, MemoryCardBootUpPortCheck.

Parked / questions:
- TLT_LoadLevelContentsFile (code-exact; only @512 "data/%d/%d_%d/%s" must sit at +268, before LLC's messages,
  scratch/par/parks/s15/llc_a.cpp scores 2 = offsets). Probed: a file-scope 'static const char* const' puts it at
  the right place (scratch/loop3/systems/tlt/d.cpp: LLC +288 exact) but the const pointer is loaded from .sdata
  in the 4 loaders (3 off each), so it is not the original form. Most likely the original LLC had a compiled-out
  sprintf(name, "data/%d/%d_%d/%s", ...) path (the literal created during LLC's codegen, shared by TLT_Init via
  -str reuse). QUESTION for the maintainer: is a dead branch inside a real function, holding a literal the
  original proves was created there, acceptable? (Remaining.md declines invented dead FUNCTIONS.) Else needs
  TLT_LoadFromRepository (115 off) in the same unit.
- CProfileInitClass ctor: draft 396 off, and its strings (+1344) sit behind unreferenced @991-@1040 (stripped
  code) -> gap-function question, not worked.
- RCMP::DECODER ctor: retried with deferred/RTTI flag flips and 6 body forms (rc2/var.py), still 4 (vt r6/r5).
- research/drafts/index.tsv row for __ct__17CProfileInitClassFv still names config/GMFE69/debug_prof_dump.json
  (removed by the merge) -> point it at debug_prof.json (fixed in 443f2ebd).
