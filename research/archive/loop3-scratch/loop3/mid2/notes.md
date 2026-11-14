# mid2 (airender slot -> middle of the scripting lane), 2026-11-11/12

No landing. One big draft improvement (parked), one re-park.

## CreateRootHierObject 0x8003dde8 (3416, objcreate_swap) -- real 44 -> 2 (scratch/loop3/mid2/CRH_BEST2.cpp)
- Score with config/GMFE69/objcreate_swap.json (fast.py counts relocs; use research/tools/score.py real: (2, 0)).
- KEY: the box allocation written as 'box = (CVolBox*)g_pVolumeAllocator.GetFreeElement(true); new (box) CVolBox;'
  plus top decls 'BSGO_Tank* go; CHierObject* object; CTankObject* tank; CVolBox* box; TriggerCoreView* core; CVolBox* world;'
  (core assigned after the decls) gives EVERY callee register of the original (core r31, flag r30, trigger r29, go r28,
  object r27, tank r26, box r25, world r24). Left: the two ctor vtable stores go through r25 (box) where the orig uses r3
  ('mr. r25,r3; beq; stw r0,0(r3) x2').
- The direct form 'box = new (alloc) CVolBox' gives the orig's exact shape there (mr. + stores via r3) but box always takes a
  HIGH callee reg (r29): all 720 decl orders (gen3.py on crh0.cpp), box declared at use, const core/param copies
  (gen4.py, 5040 orders): best 76-92.
- Any void*->CVolBox* cast copy (inline AllocateBoxVolume() returning void*, '(CVolBox*)(void*)new ...', IVolume* temp)
  gives the right ranking but the box copy lands after the null-check join ('cmplwi r3,0 ... mr r25,r3', +1 instr,
  fast 7: crh7.cpp). 'void* memory = alloc; box = (CVolBox*)memory; new (memory) CVolBox;' = 'cmplwi r3,0; mr r25,r3'
  (+1 instr, m_O.cpp). 'if (box) new (memory) CVolBox' = an extra beq (m_BA.cpp).
- Rising Sun's CreateRootHierObject AND CreateSubHierObject both call AllocateBoxVolume (+ CStaticMesh::GetBasis/GetCenter,
  AddObjectToList, __nw__9BSGO_Tank). FL's AllocateBoxVolume (0x8003c254, objcreate_volumes) is earlier in the same file,
  so it would auto-inline; but CreateSubHierObject (landed, direct form, box r31 in the orig) goes to 57 with it.
  An inline box-setup helper is not inlined (too big; 848). -Cpp_exceptions off / deferred inlining: no help.
  The orig extab record has no actions (408a0000), so no EH temp is involved.
- Next idea: the two vtable stores through r3 while box is r25: an inline CVolBox ctor called through a reference/
  pointer param? CGerman's class 'operator new' trick does not apply (hides placement new for the landed CreateSub).

## PublishEvent 0x800d7718 (568) -- stays 2 (pub7/e.cpp)
- Loop-2 callback local in every shape (plain, const, inline Notify helpers for loop 2 or both loops, const param copies of
  subject/event at function or loop scope): 88-93 (the it/subject/event rotation). Inline Notify without callback local
  (const SItem&) = 2. pe_*.cpp, pq_*.cpp, pr_*.cpp.

## Not taken (checked history)
- AllocateStaticObject (real 0): needs Init's .rodata string first (Init unlanded) -> gap/unlanded duplicate, no.
- BSInitBIFuncs: needs unlanded DisplayHint + multiplayer UpdateFragCount as duplicates, no.
- AIStartCoverPointWalkToStored: scripting already did 19 forms this run.
