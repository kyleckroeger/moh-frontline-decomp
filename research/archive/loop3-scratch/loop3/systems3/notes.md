# systems3 (systems lane dry -> scripting lane tail, bottom-up), 2026-11-11

- NonVisDestructFarthestSoldiers (objcreate.cpp 0x8003bf88, 552): NEW unit objcreate_nonvis, source
  scratch/loop3/systems3/objcreate_nonvis.cpp (0 in fast.py, offsets included). Layout: objcreate's ...rodata.0 block =
  primes + RTTI names "BSGO_Player","BSGO_Basic" + NonVis strings. BSGO_Player::Destroy (objcreate_volumes) compiled as
  --file-duplicate creates the names; BSGO_Basic::Destroy must be an IN-CLASS inline '{}' (an out-of-line weak def makes
  Basic's name come first / Player's after the strings). Deferred,auto; source reverse order (Player::Destroy, NonVis).
  Code fixes vs draft: real virtual calls (ISceneNode table as AIFilter_visit), 'for (...; g = g->m_next, found++)' with
  'found--; continue;' for marked soldiers, float distance[] declared before nodes[], limit = best before 'if (farthest)',
  'removed++' before the DebugMsg, CScene sized so g_scene is not SDA.
- LANDED 9ea3edcd objcreate_nonvis (552). LANDED c6d755db bsbifunc_promptinteger (832): copy of bsbifunc_prompttext with
  the integer body -> 0 at once (scripting's appendMessageToPrompt form fixed the inlined registers). Figure 1,233,388.
  (Before that form, sweeps pi/sw1.py, sw2.py of displayPromptBuffer decl orders found nothing.)
- Other tail rows: list_iter<BuddyInfo> ctor = question (g) (local member-template instance); __sinit_buddy (leave alone);
  CreateGerman (.rodata.0, needs NonVis strings first -> now easy layout-wise via NonVis as file duplicate) but draft 442 off;
  CreateCollisionVolume (.bss.0, block in objcreate_destroy) draft 311 off.
