# late2, 2026-11-12

## LANDED b9dfe832 scene_cmppairs += SweepExtents (712)
- sx/clean.cpp: list walked with dwi::list<SExtent>::iterator (RS list_iter). Pairs loop written out (as inline: 44).
- Landing gotcha: the fast_vec<ISceneNode*> dtor is a --file-duplicate whose extab/extabindex entries sit right after
  SweepExtents' (scene_fastvec_node owns them); port_unit took them into the ranges -> "Source ranges overlap". Fix: give
  extab/extabindex size = own bytes plus linked_offset 0, linked_size = compiled size, stripped_objects [@188]/[@189].

## BIFunc_AIStartCoverPointWalkToStored (3 diff lines, c/base.cpp = late1 c_i3.cpp)
- Dichotomy holds over ~1700 forms (cgen*.py, cv2-cv7.py; both objcreate t.json and the real bsbifunc_cover.json flags):
  a NON-const named bool (any init/type/struct wrapper/inline param from an implicit conversion) gives the orig's
  'srwi r0; mr rX,r0' copy but ranks BELOW the stack param (swap, 12-13); const / '!= 0' / const-ref inline param gives the
  right ranks but converts straight into r31 (3). Tried: inline Arg() returning bool/int/uchar x 6 bodies x 4 param kinds x
  4 call forms x plain/local/const-local; register; direct-init; struct BArg/SP wrappers; one var reused for flag+result;
  if/else assignment; int& / int* top copies; const stack copies.
- Also: '!= 0' / explicit forms with a NON-const var give right ranks, no copy (3); only implicit int->bool into a non-const var
  makes the copy, and that copy always demotes the var below the stack param (same as landed SetCoverPoint/SetPlayerFallingDamage,
  where the param outranks the copied bool). The orig has the copy AND flag above the param: no source form found. Parked, released.
  Untried: permuter on c/base.cpp with the real bsbifunc_cover.json flags (late1's permcw used objcreate flags).
