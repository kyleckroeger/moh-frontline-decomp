# systems4 (scripting lane tail, bottom-up), 2026-11-11/12

## CreateGerman (objcreate.cpp 0x8003f1ec, 1732): CODE EXACT, layout proven, BLOCKED on CreateCollisionVolume landing
- Draft: scratch/loop3/systems4/og_best.cpp = merged TU (CreateGerman, CreateCollisionVolume, CreatePlayer,
  BSGO_Player::Destroy, NonVisDestructFarthestSoldiers, in reverse image order). fast.py (template t.json = objcreate_nonvis
  flags, deferred,auto): CreateGerman 0 INCLUDING the r31-relative .rodata offsets 784/812/896; CreatePlayer/NonVis/Destroy 0.
- Layout: objcreate's ...rodata.0 = primes, RTTI names, NonVis strings (132..556), CreatePlayer's "Unknown difficulty" (556),
  the two Mechanic strings (604, 688; FIRST USED BY CreateDestructable, but CreateCollisionVolume uses the same two strings, so
  with -str reuse CCV creates them), then CreateGerman's (784..988). So the coordinator's "NonVis as dup is enough" was not
  quite right: CreatePlayer AND CreateCollisionVolume (or CreateDestructable) must be file duplicates too. Without CCV the
  strings land at 604 (verified, og1.cpp). => land CCV first, then CreateGerman with dups NonVis, Player::Destroy,
  CreatePlayer, CCV (pool .rodata + .sdata2; .sbss guysLostCount/init guard is the function's own static).
- CreateGerman code fixes vs the research draft: real virtuals (ISceneNode table + placeholders to 0xd8; CSoldierObject slots:
  SetCollisionId 0x5c, Reset 0xdc, Yaw 0x104, SetPosition 0x108, SetScript 0x12c, AttachObject 0x134, DetachObject 0x138,
  InitAI 0x148); class operator new for BSGO_Soldier (stores via r30); 'static int guysLostCount = 0;' (CW guards it);
  queue: 'g_iNumGermansQueuedThisFrame++' BEFORE the entry stores; tail wrap as 'x = x >= 15 ? 0 : x' / 'x < 0 ? 14 : x';
  switch case bodies ordered 1,2,0,default; position filled field by field; 'int z, y, x;' at function top;
  weapon part = inline CSoldierObject::SelectFirstWeapon() whose else-branch RETURNS (explains the dead 'if (weapon)' block
  placed after the Update call); aim = 'position.Set(0,0,1); m_aim->SetDirection(position)' (by-value param -> copy at 32)
  + SetPitchLimits/SetYawLimits inlines (one m_aim load per pair).

## CreateCollisionVolume (0x8003ed10, 1236): 78 lines, PARKED (pure GPR permutation)
- Best: scratch/loop3/systems4/ccv_best.cpp (standalone, defines objcreate's .bss block with size views as objcreate_destroy).
  Everything else is exact: '/ 16.0f' (not '* 0.0625f') gives the conv*const fmuls; placement new for sphere/box (stores via
  r3), class op new for BSGO_Destructable, node as 'node = (CCollisionVolume*)alloc.GetFreeElement(true); node = new (node)
  CCollisionVolume;' (gives 'mr r0,r3; mr. r26,r0'), trigger->GetCore() inline accessor (core r26 -> r27).
- Remaining: orig object r31, core r30, param r29, trigger r28, base(+sphere/box) r27, node r26; ours core r27 below param
  r30 / trigger r29 / base r28. INERT: every decl order (core/object/node/volume/sphere/box), const params, const copies,
  T* const self copies, core as reference, accessors on the core (GetSize/GetShape/GetChildScript), AddObjectToList inline
  (RS has it out of line), AllocateSphere/BoxVolume() calls (auto-inlined, file's own functions), switch, nested if, inline
  volume builders. Diagnostics: removing the volume branches or the base use for the node allocator reorders everything
  (core rises), so the knot is around the base register/volume branches. Permuter (permccv2, helpers for new/CVector3)
  only finds logic-changing moves. Rising Sun's copy (TriggerObject accessors) gives no FL helper boundary that helped.

## Other tries (no gain)
- CThrownObject::Init (fast.py 32, not 16 as status.tsv says): RS has GetBasis/GetCenter on CStaticMesh; as FL inlines
  (ti1base.cpp, v40*) 32 -> 32..82; Length() nesting/temp/ref forms (v41-v43) no gain. Released.
- CreateDestructable (scripting cd/a1.cpp = 22 in fast.py with t.json; param r28 vs box r30 swap, same family as CCV):
  AllocateBoxVolume() call (217, not inlined?), reassign box form, RS GetBasis/GetCenter inlines: no gain. Released.
  NOTE: if CreateDestructable lands first, it can replace CCV as the Mechanic-string file duplicate for CreateGerman.

## Tools here
- s.sh / d.py (normalized difflib diff; T=<template> env), var.py (string-replacement variants from a dict on stdin),
  sig.py (CCV register signature), vt.py (vtable slot -> symbol, needs syms.txt from objdump -t).
