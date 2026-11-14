# pnew1 (placement-new knot: CreateRootHierObject + CreateDestructable), 2026-11-12

No landing. Both stay parked: CRH at 1 line (v/R1.cpp, extra beq) / 3 (v/O1.cpp, M1) / 7 / 8 (CRH_BEST2.cpp);
CD unchanged (systems5 cd_o4 = 1, cd_i6 = 3).

## Tools
- var.py <base> <func> <template json> <variants.py>: applies replacement lists, writes v/<name>.cpp, prints score + reg map
  (uses rm.py/d.py copied from late1; set T=<json>). v1..v9.py = variant batches. gen.py = decl-order sweep of the CRH header.
- tiny.py <file.cpp>: compile a small test file with the objcreate_swap flags and print the disassembly.
  t1..t20.cpp are the small experiments below.

## What the small experiments showed (t*.cpp)
1. `mr. rD,rS` only fuses when the compare is on the COPY (rD): `b = (T*)m; if (b)` fuses, `if (m)` gives `cmplwi r3; mr` (t1).
2. CW ALWAYS null-checks placement new: global or class operator new, throw() or not, reference arg (`new (*p)`) (t2).
3. Cast form `box = (T*)new (...) T` (or `(T*)Alloc()`) fuses (`mr. rB,r3`, ctor stores via r3, box ranked by declaration)
   ONLY IF box is dead before the next conditional branch (t15/t16: any later `if`, even inside an inline, breaks it;
   box dead before the if = fused). That is why CRH's `world = (CVolBox*)AllocateBoxVolume()` matches (world dies
   before AttachScript's `if (tank)`) and box (live across `if (tank)`) cannot. Copy chains, nested blocks, const
   copies: no help (t18).
4. Direct form `box = new (...) T` (also `box = (T*)x; box = new (box) T`, inline helpers returning the new, class op new)
   makes box the new-expression temp: always fused, ctor via r3, but the temp ranks above all locals and even params
   in small functions (t14/t17/t19/t20), whatever the declaration order or an earlier dead def (Z1-Z3).
5. `if (box) new (mem) T` with box = (T*)mem: the second compare is dropped (CR0 reused) but its beq stays (R1, Q3-Q7 = 1).

## CRH results (base CRH_BEST2.cpp, objcreate_swap.json)
- const box declared at use (`CVolBox* const box = new ...`), helper forms, H3/H6/N6/N8, AllocateBoxVolume for box (W1):
  7 (all registers right; cmplwi r3 + late mr r25). gen.py over 720 header orders of W1: 7 at best.
- custom placement new `operator new(size_t, void* p, CVolBox*& out) { out = (CVolBox*)p; return p; }` (O1) and
  `void* mem = ...; box = (CVolBox*)mem; new (mem) CVolBox;` (M1): 3 (`cmplwi r3,0; mr r25,r3` instead of `mr.`).
- R1 (`void* mem; box = (CVolBox*)mem; if (box) new (*(CVolBox*)mem) CVolBox;` with operator new(size_t, CVolBox&)): 1, extra beq.
- every box-form x world-form combination (v7.py): nothing below 7 apart from the above.

## Open
- Compiler 1.3 (t13.json): worse everywhere (BEST2 16, R1 9, K1 15).
- The orig fuses box while box is live across `if (tank)`, ranked below object/tank. None of the known forms give that.
  Untried: the vtable-pointer store hack
  (`if (box) *(void**)mem = ...`), which is probably a declined form (needs the user's say).
