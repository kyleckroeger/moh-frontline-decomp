# Overnight backlog (2026-10-23 night)

Do not stop when your own target list runs out: keep going down this backlog until the coordinator tells you to stop.
Stop only if a whole hour has passed with no landing AND no draft has improved.

## Claiming
Before you start on any function, check it is not claimed: `grep -i <name> scratch/par/CLAIMS.txt`.
Then claim it: `echo "<mangled-or-short name> <NAME> $(date +%H:%M)" >> scratch/par/CLAIMS.txt`.
Also skip anything already covered (re-check project units) or logged as parked/blocked in AUTOPILOT.md, unless you have a new idea.
Full list of uncovered functions never mentioned in AUTOPILOT.md (regenerated 2026-10-27, size + address + name): scratch/par/untried_now.txt
(158 functions, 166 KB; at 75.66%). Re-run `python3 scratch/uiw/untried.py 150 6000` for a fresh copy.

## Lanes (take from your own lane first, then any unclaimed row in untried_now.txt, smallest-to-mid first)
- lines: collision and geometry: CCDB*, CVol*, CTriangle, FindIntersection, GetNearestPoint, TestCDBBranchNodeCollision<...>, CPlane, CQuaternion/CMatrix, the CObstacleAvoidance math.
- parks: the EA/sound/library C code: SND*, saems*, PATHI_*, PROFILE_*, decodemut/filter/form_component_prediction (UTALK, MPEG),
  AnimFile*, _Anim*, _unpack_*, plus remaining parks from scratch/uiw/parks.py.
- soldier: AI and soldier: CSoldierObject*, CAIObject*, CAISplinePath*, CAIFilter*, hierobject, CExplosion/CImpact Visit,
  aim_cb/floco_cb (CAnimCallbacks), BIFunc_* script functions.
- bigfiles: player/objcreate/scene/UI/render: CPlayerObject*, objcreate Create*, CScene*, CStaticObject*, CParticleSystem*,
  UserInterface*, PopUpMessageHandler*, CFont/CSprite Link, BSFile*, Tib*, SoundSchedule.

## 2026-10-27 update (coordinator)
untried.py checks addresses only; many "untried" rows were already worked by name. untried_now.txt is now name-filtered
(python3 scratch/par/namefilter.py on the address-only list scratch/par/untried_addr_only.txt; drops rows whose name appears in
AUTOPILOT.md, notes.md, TRICKS.md or CLAIMS.txt): 10 fns, 9.8 KB. The untried frontier is essentially gone.
Main work is now PARKED NEAR-MISSES, closest first: python3 scratch/uiw/parks.py, plus each lane's notes.md drafts.
Always grep AUTOPILOT.md and every scratch/par/*/notes.md BY NAME (not only by address) before starting on a function.

## 2026-10-27 17:20 refresh (coordinator)
At 77.62% (1098008/1414572), HEAD 6511c531. untried_now.txt regenerated + name-filtered: 9 fns, 9.5 KB (CAISplinePathManager ctor,
list<BuddyInfo> range ctor, Get_Hdr, RFPuts, TestCDBBranchNodeCollision<CVolBox>/<CLine3>, quick_sort_<UNodePair>, CEAXABLKDecf::Decode, aim_cb).
Priority: lowest-diff parks first (python3 scratch/uiw/parks.py): CPlane::GetIntersection 0 off, CEAXABLKDecf::Decode 2 instr,
CShellMenu ctor-ish 4-line items, AddAmo 4, BIFunc_AIStartCoverPointWalkToStored 7, ~IObserver/~ISubject (no draft yet), sbam CHierObject LerpTo twin.
lines had no landing last session: also take any unclaimed park from other lanes.
- 2026-10-27 19:20: extab padding patches are LIVE (c27aa295, config/GMFE69/extab_patches.json, scratch/par/extabpad/addpatch.py). Open item: ComputeAStarPath (released, see CLAIMS) - soldier lane.

## 2026-10-29 refresh (coordinator)
At 79.53% (1125076/1414572), HEAD e13a1390 = origin (all pushed). Stale LANDLOCK (bigfiles, 10-27 19:29, bsutil_update committed) released.
untried_now.txt regenerated + name-filtered: 7 fns, 8.6 KB (list<BuddyInfo> range ctor, Get_Hdr, TestCDBBranchNodeCollision<CVolBox>/<CLine3>,
quick_sort_<UNodePair>, CEAXABLKDecf::Decode, aim_cb). Main work stays the parked near-misses (python3 scratch/uiw/parks.py) + each lane's notes.md.
Closest open items: SNDPLATFORM_outputset raw 5 (parks, s13/os/o8.cpp), AnimMotCalcFloatingBone raw 9 (parks), ComputeAStarPath 8 lines
(soldier, extab patches ready), BIFunc_GetObjInfo (soldier), MathLLAngleInit rd 2 (bigfiles; the 'int i = 0' index-local fix is a register
device = POLICY QUESTION, do not land), CAnimObject::ConstrainVelocity 35 (lines). Whole-file (e) merges keep paying off for bigfiles
(mainloop, skybox, Pause, bsutil): look for more files where one or two fragments are missing.
- 2026-10-29 12:30 (soldier finding, ALL LANES): most '...rodata.0' blocks of blocked game files START with the header's
  static dwi::dwi_prime_list[27] (108 B). Define the prime list in the unit + port with --pool .rodata (AllocateGenerateBuffers matched
  first compile, 3d1e56c9). Candidates: TLT_LoadLevelContentsFile, FindAllMGPointsInBPDFile (parks); AllocateStaticObject, CScene ctor
  (bigfiles); ShellMenu block 0x801783c8; CSoldierObject::FireWeapon (primes + 164-B int table + RTTI names before its strings).
  Survey: scratch/par/soldier/bpe16/rodata1.py.
- 2026-10-29 13:10 (soldier, ALL LANES): FireWeapon landed (44a2a210) by merging the whole Soldier_object range into one unit:
  in '-inline deferred,auto' + RTTI files, rodata.0 = prime list, file-level table initialisers, RTTI class names, THEN strings
  (strings are created at codegen, after the vtable/RTTI). Hint: a file whose string @N numbers are higher than its RTTI names'.
  Candidate: AIObject.cpp SetupAStarPathWalk+StartAStarPathWalk (needs CheckFieldOfViewModeList + ChooseCoverPoint in range). See TRICKS.md.
- 2026-10-29 17:45 (coordinator, ALL LANES): '#pragma dont_inline' in GAME code is still UNDECIDED (AUTOPILOT Waiting-on-user 5(b)).
  41b619ec and e8cf43ee already used it; do NOT use it in any new game-code landing until the user decides (library/TRK code is fine, upstream precedent).
- 2026-10-29 21:30 USER DECISIONS (see AUTOPILOT Decisions 2026-10-29): '#pragma dont_inline' in game code now ALLOWED (supersedes 17:45 note).
  Approved: size-0 asm label tooling, Screen.cpp stubs (tooling agent builds both), GetLevel-style single-use inlines, byte-offset loops,
  inferred ctors. DECLINED: register devices (STREAM_kill alias, MathLLAngleInit index local, staticobjfactory offset spelling,
  GetObjInfo (int)(filter = 0)) and ShellMenu's invented dead function.
- 2026-10-29 21:50 (coordinator): soldier lane PAUSED (down to 3 lane agents + tooling). Open soldier items for any agent with spare time:
  AIObject.cpp merge blocked ONLY by ChooseCoverPoint (ccp/b.cpp 23; CheckFieldOfViewModeList now 0 in soldier/s24/CFOVML_BEST.cpp,
  Setup/StartAStarPathWalk 0 in soldier/sap/SAP_BEST.cpp); player.cpp merge blocked ONLY by player OnCollision (bigfiles/pzD0.cpp 7;
  InitMultiplayer 0 in soldier/pim/IM_BEST.inc). See scratch/par/soldier/notes.md Session 24.
- 2026-10-29 22:30 (tooling agent DONE): label_functions (size-0 asm labels) and linker_stripped (Screen.cpp stubs) tooling landed
  (386ae88b, 676e2ae1) + targsupp, gc_idct, __exception vector table, Screen.cpp whole file -> 84.76%. Lesson: ss.py missed a 2-word
  diff in a whole-file draft - always confirm whole-file drafts with one_unit before landing.
- 2026-10-29 22:25 (coordinator): parks lane PAUSED too (every anim/EA/RCMP/sound file is blocked by register colouring; Session 20 in
  parks/notes.md). ChooseCoverPoint now 18 (scratch/par/parks/s20/ccp/CCP_BEST18.cpp, FPRs fixed; only this/from/type rotation left) -
  still the single blocker of the AIObject.cpp merge. Lanes running: lines, bigfiles.
- 2026-10-29 22:45 (coordinator): lines lane PAUSED (survey: no data-base-only blockers left in its files; all register colouring).
  Improved: CVolBox::TestCollision(CPlane) bpl_BEST.cpp 448->239 (exact only with an invented Id() copy helper = policy question if it
  gets close), tcap 25, CCDBVolBox::TestCollision(CTriangle) 495. Only bigfiles keeps running.
- 2026-10-29 23:00 (coordinator): 'T* const self = this;' (02723aaa, 8b4aca90) is an OPEN user question - do not land NEW uses until decided.
  Next unclaimed item: AIObject.cpp merge for SetupAStarPathWalk/StartAStarPathWalk (plan in scratch/par/bigfiles/notes.md).
- 2026-10-30 01:00 SESSION END (coordinator): all lanes paused at 85.06%, HEAD 61f98ea8 pushed. Decisions + next PLAN in AUTOPILOT
  "Decisions (user, 2026-10-30)": named-copy rule (const self / index local OK), InitMultiplayer + UpdateTargetPosition dup DECLINED.
