# systems5 (scripting lane tail: objcreate), 2026-11-11

## LANDED
- 5c8d9fb5 objcreate_collvol: CreateCollisionVolume (1236, .bss pooled). Fix for the 78-line GPR permutation:
  'TriggerCoreView* core;' declared FIRST, then 'int const c_param = param; TriggerObject_struct* const c_trigger = trigger;
  int z; int y; int x;', then 'core = c_trigger->core;' (direct field, NOT the GetCore() inline). Steps: const copies +
  direct field 72 -> 28; core before the copies 14 -> 8 (header perms); assigning core after the copies -> 0.
  Manifest by hand as objcreate_nonvis: extab/extabindex = last compiled record (dups compile first, image order),
  .data/.sdata stripped, RTTI/vtable as file duplicates + local externals.
- 5e72bb35 objcreate_german: CreateGerman (1732, .bss pooled), systems4's og_best + the CCV form + the .bss block views
  (source scratch/loop3/systems5/objcreate_german.cpp). See commit after 5c8d9fb5.

## CreateDestructable (0x8003c93c, 2028): PARKED, best 3 (cd_i6.cpp, needs a cast copy) / 1 (cd_o4.cpp, double beq)
- Base cd_a1.cpp = 22 (param r30 / box r28 swapped). Header ('int const c_param' BEFORE 'c_trigger', then 'int y; int x;
  int z; CVolBox* box;') + 'box = (CVolBox*)new (...) CVolBox;' gives ALL registers right (cd_i6.cpp, 3 lines) but the
  cast adds 'cmplwi r3,0 ... mr r28,r3' instead of 'mr. r28,r3'. Without a conversion (plain 'box = new ...', any decl
  position, 720 header perms in k.res) box is a new-expression temp and always ranks r30 (just below script).
  'void* mem = ...; box = (CVolBox*)mem; if (box) new (mem) CVolBox;' (cd_o4.cpp) = 1 line: an extra beq (the new's own
  null check is not merged). Probably the orig's form is plain and the ranking comes from something else.
- Permuter harness: permcd/ (macros NEW_SCRIPT/NEW_BOX/ALLOC_OBJ/VEC_INIT/VT3/VT0 in perm_unit.cpp, perm_func.c with
  typedef struct lines); base 55, ~21k iterations, nothing.
- AllocateBoxVolume() (defined before CD, auto-inlined) puts box LOWEST (r26) with an extra copy; inline Impl wrapper is
  not inlined (too big).

## Tools: rm.py <file> [func] (score + orig->ours reg map), d.py (diff), cdk.py/ccvk.py (structured searches), ex.py/syms.py.
