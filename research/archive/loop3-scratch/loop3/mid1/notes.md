# mid1 (geometry slot -> middle of the scripting lane), 2026-11-11

LANDED 70aad029 static_obj_scale += CStaticObject::RotateFromScript (512) + TranslateFromScript (308) = 820 B.
  Key: Rising Sun's binary keeps CalculateMovementTime__13CStaticObjectRC8CVector3T1 out of line; as an inline member with two
  ternaries both went 24/4 -> 0 at once (TRICKS.md entry). Source: scratch/loop3/mid1/land/static_obj_scale.cpp.

Method worth repeating: list RS (../moh-rising-sun/orig/GR8E69/MOH3RDVD.ELF) callees of the RS copy of a function
(objdump -d range | grep bl). scratch symbols: /tmp scratchpad rs_syms.txt (regenerate with objdump -t). Checked so far:
- UpdateProximityTriggerStatus: RS calls UpdateProximityTriggerState x5; FL draft already has UpdateStatus inline (no news).
- CheckForObstructions: RS members TestRay/ShouldHeightCheckBeFlipped. PARKED (cfo/): as members they change nothing.
  Best = cfo/a.cpp / n_if.cpp raw 9 (if-form sign: regs right, 'li r31,1' too early + TestLine#1 g_scene addi/li r3,0 schedule).
  Ternary sign (RS form 'ShouldHeightCheckBeFlipped(node) ? -1 : 1') places li right but sign r28 instead of r31 (this r31):
  25. Inert: int/long/const/ESign types, decl position (9 spots), Sign(bool) inline, const copy, scene ref/world local in TestLine,
  'return volume->TestCollision(...)' forms. TestRay returning '!= 1' + early returns adds 3 instrs (orig = nested ifs).
- PlayerScreenFlash: RS calls CFader::Flash(CColor, float, float); FL forms with named arg locals + CColor(r,g,b,a) ctor,
  Set(from,to)/Flash(by value)/SetRef: 40-81 (draft 42). Orig: colour A (args 1-4) built at sp+16, copy +20; B (5-8) at +8, copy
  +12; arg 6 lands in r30. PARKED (psf/).
- BIFunc_GetObjInfo: RS splits it (BIFunc 256 B + GetObjInfo(int, BSObject*, float, ISceneNode*, bool&, int&, float&) 6228 B).
  In FL, CW 1.3.2 does NOT inline a 6 KB 'inline' GetObjInfo (stays a call); only '#pragma always_inline' / inline_max_size
  inline it (784, length 1522 vs 1556). So FL had no such split. PARKED (goi/s1.cpp).
- DoTargetCheck / CreateDumpObject / CreateRootHierObject: RS versions have different signatures; no helper lead.
All claims released.
