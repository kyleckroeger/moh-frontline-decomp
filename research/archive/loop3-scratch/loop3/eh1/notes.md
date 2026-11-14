# eh1 notes (2026-11-12): extab records as type evidence

Tools: eh.py (decode CW extab: `eh.py orig ADDR`, `eh.py cmp ADDR TMPL SRC FUNC`; committed as
research/tools/extab_records.py), scan.py (all research/drafts/index.tsv rows -> records.tsv), cmpall.py
(diff orig vs ours records for named drafts), sc.py (real score of pum variants).

## Scan result (records.tsv; columns orig_actions / ours_actions / same)
Only 17 drafts have an action record in the original; no draft has one the original lacks.
- Header-only originals (no cleanup actions at all): CreateRootHierObject, CreateDestructable,
  CreateDumpObject, AnimStStartState, CPathNodeList ctor, list<BuddyInfo> ctor. So the placement-new knot
  gets no help from EH: the original has no DELPTR for `new (box) T` (no placement operator delete) and
  no stack objects with destructors; our drafts agree.
- Exact already (yes): TestCDBBranchNodeCollision<CLine3>, AllocateStaticObject, CCDBTriangle-capsule,
  CheckForObstructions, GetInitialBuddyOrders, CreateBuddyDataStructures, CCompartment::DrawNode,
  UserInterface::Init.
- Same actions, register/offset only: MOHFL_VD_create (new'd GCHW_VD in r31 orig, r29 ours: matches
  sweep1's finding), MemoryCardBootUpPortCheck (range shift), CStaticObjectFactory::Init (DELPTRCOND
  cond/obj regs: orig distinct cond regs 28,27,27,26,25 / objs 26..22, ours cond r24 for all / objs
  31..28,22), BSInitBIFuncs (gprs 5 vs 4), GenerateTestSplinePath (fprs 16 vs 10, draft 'differs'),
  __sinit_compartment_cpp (DBASE ISceneNode on r28 vs r29: types right; the gap is the .data/.bss pool
  base registers (vtables via one base r29 in orig) = whole-file layout), PopUpMessageHandler ctor
  (DMEMBERARR PopUpMessage[20] + 4 DELPTR ::delete: types right; obj=31 vs 30 = the this/pool swap).

## Fixes from the evidence
- Play (OPTION_PARSING_PLAYER): orig DLOCAL at r1+8 with __dt__15DISP_OLD_FRAMESFv; draft had no
  destructor. FrameTimes -> DISP_OLD_FRAMES with inline empty ~DISP_OLD_FRAMES() {}: extab now
  identical, code still exact (5 relocation-only lines). The weak dtor is emitted right after Play
  (0x80103e58 = src/game/rcmp_main_delete.cpp), so Play lands as a prepend to that unit once the q9
  rodata-head question is answered.
- ShellInit: our CSprite view declared a class operator delete; the original's DELPTRs call
  ::operator delete (no __dl__7CSprite symbol exists). Removed; action set now identical (135 unchanged).
- PopUpMessageHandler ctor 45 -> 35 (not EH): Load's locals declared text, index, pos (any order with
  pos last or text first... perms 3-5) fixes r26/r27. Left: this r30 vs orig r31 (pool base swap) and
  the loop's volatile colouring (entry/extra/length). Inert: entry/extra/length forms, i outside, screen
  decl, fonts/fontData order, ::umTable, declared dtor.
