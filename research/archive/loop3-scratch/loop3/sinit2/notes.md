# sinit2 notes (2026-11-12): __sinit_buddy_cpp + list<BuddyInfo> range ctor (the BuddyInfo() temp knot)

Result: no landing. Both still stuck on the same knot; nothing in research/ or src/ changed.

## Tools (all in this dir)
- sc.py TMPL ORIGFUNC OURPREFIX [-v] src...: score one function (inlines `#include "x.h"` from this dir).
- st.sh x/f.cpp...: wraps a BuddyInfo/BuddyState definition file into the sinit probe (base_s.h) and the
  range-ctor probe (pre.h + base_l.h, = scripting/lc/state.cpp minus the types) and prints both scores
  plus OUR stack stores (r1 offsets, saves excluded). Target: S: 20 24 16, L: 20 24 16 (moving, active, path).
- scb.py TMPL ORIGFUNC PREFIX files...: batch scorer (score, length delta, stack stores).
- gen2/gen3/gen4.py: combinatorial BuddyState/BuddyInfo/list_node shapes (results r2/r3/r5/r5l.txt).

## Scores (fast diff lines; scripting's metric halves these)
- range ctor: best 6 here = scripting's "3" (lc/state.cpp: nested state(BuddyState()) with a self-> default ctor;
  temp at 8 instead of 16). Also 2 extra-line forms: BuddyState() self-> member + `BuddyInfo() { soldier = 0; progress = 0; }`
  (v5/self_body_body_bsp_impl_l.cpp): stores 20,24,16 exactly right, then 8,12 extra -> everything else in the function matches.
- sinit (minimal probe s0.cpp, globals + list() + vectors only): best 2 = the same two extra stores (8,12).
- extab: sinit 0 ranges (gprs 1), range ctor 0 ranges (gprs 3): no dtor evidence; BuddyInfo/BuddyState have no destructor
  (Create's extab only has the two vec_iter DLOCALs, none for the BuddyInfo temp).

## What the original does (all three users: sinit, range ctor, CreateBuddyDataStructures)
The temp is a 20-byte BuddyInfo slot whose state part (path/moving/active at +8/+12/+16) is in memory, stored in
moving, active, path order, while soldier/progress (+0/+4) are never stored and reach the copy as constants.
Create confirms the slot: temp state at 48..60 and 40..48 (soldier/progress) never touched; after the out-of-line
end() call the orig reloads moving/active/path but writes soldier/progress as literal 0. So it is PARTIAL
scalarisation of one var (or an unexplained 8-byte var in front of a 12-byte BuddyState temp in all three frames).

## What CW 1.3.2 does with our shapes (~2,000 compiles)
- Any address of the temp or of a sub-object (self = this in BuddyState() or BuddyInfo(), &state, BuddyState& s = state,
  array subscripts incl. constant ones, reinterpret of a char[12], pointer-to-member, placement new, reference helper):
  the WHOLE temp is memory and all five fields are stored.
- No address: fully scalarised (no slot at all), regardless of union/bitfield/array/const/auto-inline forms.
- The sinit context is worse: init-list soldier/progress + a member BuddyState ctor makes the whole temp memory even
  without self (m4 vs m3: body-assigned soldier/progress scalarise).
- Partial scalarisation DOES exist (it is what sinit1 hit in pumhandler): BuddyInfo { soldier; progress; char st[12]; }
  with init-list soldier/progress and the array NEVER written: soldier/progress no stores, the array region stays in the
  slot and is loaded by the implicit copy (x/t1, x/z1). As soon as the region is written by typed stores the whole temp
  goes to memory again (t3, y1-y3, o1-o3, bf4).
- Uninitialised soldier/progress (b2, k_self_*): exactly the 3 stores at 16/20/24, but the copy then LOADS soldier/progress
  from the slot (or, scalarised, holds them in callee-saved regs: c5, k_body_*). Orig saves no extra GPRs, so not that.
- Not it: default args (BuddyState/BuddyInfo), list_node by-value or const&-pointer params, head default-constructed in
  place, operator= forms, base-class or member Head{soldier,progress}, Ptr/Int wrapper members, bitfields, unions,
  out-of-class (auto-inlined) ctors, explicit this->, Clear()/static helper forms.

## Partial scalarisation found (diagnostic, not natural yet)
- A BLOCK copy into the state region keeps just that region in the slot and scalarises soldier/progress, exactly the
  original's frame shape: x/pb1.cpp (POD BuddyState, `state(BuddyState())` = value-init, copied as three words from a
  static zero) and x/pa2.cpp (state copied from a global): stack stores only at 16/20/24, head soldier/progress constants.
  But the region stores are WORDS of loaded values (lwz from the static), not the original's typed stw/stb/stw zeros.
- x/pc2.cpp: that block copy PLUS typed body writes (state.moving = 0; state.active = false; state.path = 0;) gives the
  original's three typed stores at 20/24/16 and no soldier/progress stores, but the block-copy stores stay too (no DSE).
- So the original needs a non-scalarisable access to the state region that leaves no code: candidates are a block READ
  of the region that CW forwards completely, or a block write it kills. Typed writes alone, self->, subscripts,
  unions, bitfields, Flag-struct members all failed (whole temp or fully scalar).

## Next ideas (not tried)
- A shape where the state region is written by something that is NOT an address-taking access and not a matching member
  access (the pumhandler array is read only); maybe a struct member copied by an implicit copy from a memory temp that is
  itself built with self (two vars sharing the slot?).
- Check the CW 2.x/1.3.2 IR scalariser rules if a compiler decomp turns up; nothing local (no IroVars sources in ~).
- Landing needs a buddy.cpp-named unit for __sinit_buddy_cpp (sinit1 rule) with the .bss block 0x801c6c20 defined in order
  (g_attached_buddies vector, g_ready_buddies list, g_buddy_path_base vector + dtor records); buddy_head currently has
  the globals extern and no __sinit.
