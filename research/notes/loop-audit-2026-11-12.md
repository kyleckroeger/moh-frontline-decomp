# Audit of the three-day loop's landings (2026-11-12, agent audit1)

Independent read-only review of the 36 source landings in `9eb9cac2..6b511f5e` against
docs/Remaining.md "Source forms". For each form that looked like it only steers output, the
auditor removed it in a copy and re-ran the unit's verify: **load-bearing** = the unit stops
verifying without it; **not needed** = it still verifies. Variants and tools:
`scratch/loop3/audit1/` (vtest.py, mkvar.py, variant_results.txt, diffs/, var/, work/).
Line numbers are at 6b511f5e.

Summary: 7 likely declined, 7 need a maintainer look, 12 depend only on the `.bss` question
(State q1), 10 clean. Several likely-declined forms were approved by the coordinator during the
run (the "write it twice" block in CCompartment::Init, the Mul/SubV value inlines); the auditor
disagrees, so they are listed for the maintainer.

## Likely declined

1. **6e3c8d59 CCompartment::Init (2,628 B).** `WriteSwapVCD` (`compartment_init.cpp:208`, used
   :225) is a second copy of `WriteVCD` that differs only in masking one pointer through a local
   to move a register: the shape declined for UpdateTargetPosition (duplicate inline).
   Load-bearing. Also load-bearing: the palette pointer recomputed into itself (:98) and
   `bin = b;` writing the placement-new result back (:420, :432). Header comment (:1) lacks an
   address, does not mark the CPT/Shape views inferred, and says it "creates the file's globals".
2. **9b775aa0 CThrownObject::Init, 85412b69 PlayScriptedSound: chained zeroing**
   (`thrown_obj_init.cpp:191-192, 218-219`, `sound.cpp:1435`). **Not needed**: plain stores in
   v[2], v[1], v[0] order still verify. Mechanical fix.
3. **00b269c6 CVolCapsule::GetExtents.** `SubV`/`AddV` (`capsule_plane.cpp:187-188`) are
   one-operator wrappers only for the schedule. Load-bearing (328 B without them vs 304).
4. **2e4ac3cd CFont::DrawTextCenteredF.** `Mul` (`font_centeredf.cpp:86`) only sets operand
   order; load-bearing. `CurrentScreen()` (:82) is a forwarding wrapper, **not needed**.
5. **d1ba9bc7 BIFunc_BuddyDied.** `DumpBuddyVector` (`buddy_head.cpp:439`) load-bearing;
   `GiveBuddyOrders(vec_iter&)` (:427) **not needed** (its comment is wrong).
6. **8efb7975 HandleExplosion.** `SwapCollisionDisabled`/`SwapTexture`
   (`compartment_fastvec_tri.cpp:135, 142`) copy the real `TextureSwapCollisionDisabled`/
   `TextureSwap`; the unit verifies with the real names. Free fix.

## Needs a maintainer look

- **fc2de588 DetermineNewObjectivePosition** (q12): besides the duplicated IsClear test
  (`ObstacleAvoidance.cpp:203`), `ShouldAvoid` splits its switch into two `return false` arms
  (:207-222); merging them loses 36 B the original has.
- **5e72bb35 CreateGerman**: inline `SelectFirstWeapon` has an unreachable
  `else { if (weapon) {...} }` (`objcreate_german.cpp:521-530`) reproducing 72 B of the
  original's dead code (q12 family). Also q1.
- **8efb7975**: the fast_vec constructor stores `m_size` twice (:21-24, the original's
  redundant store); one-element member views `m_sphere[1]` (:55), `unknown00[1]` (:48).
- **e959464b MoveStatsIntoProperAreaForIstudio**: multiplayer stats code (q6); `CountWindows`
  (`ShellMenu_sku.cpp:76`) is an invented name for the real `Get_NumPlayers`, which verifies.
- **72ffb0fc TriggerEnterAndLeaveMessages, d637ce32 CCompartment::Draw: plan rule 10.** New
  units with local CVector3 views (`proxtrig_messages.cpp:30`, `compartment_draw.cpp:14`) that
  still verify with the shared `game/CVector3.h`. In d637ce32, `m_frustum[1]` one-element
  members serve as address markers (`compartment_draw.cpp:48`, `staticmesh_draw.cpp:64`).
- **9de067ca debug_prof**: `(void)0;` at `debug_prof_register.cpp:169` (carried from
  debug_prof_dump) controls inlining depth; load-bearing (DumpRecurse 732 -> 928 B without it).

## Depend only on the `.bss` question (q1)

5c8d9fb5, c6d755db, 9256cc51, a3c88065, 69455560, 056f3dde, ea91be6d, f07db75a, 471a7662,
4fbde63f, fbe4be84 (local CVector3 justified there: the header does not build), plus 5e72bb35
and 6e3c8d59. a3c88065's DestroyBuddyDataStructures is the blocker docs/Remaining.md names; the
constructor-less views get around `__sinit_buddy_cpp` rather than solving it.

## Clean

5d836f39, 176f30c6, b9dfe832, 95aae29e, 47e3ee8e, 70aad029, eff49cf3, e47e2460, 9ea3edcd,
9aa74087, 13057d9d, f12a2e09.

## Follow-up during the run

The coordinator assigned the **not needed** items (chained zeroing, invented names,
CurrentScreen, GiveBuddyOrders(vec_iter&), and the rule-10 header switches that still verify)
to a cleanup agent; each change must keep the complete image identical. Load-bearing items are
left for the maintainer: reverting them loses their matches.
- (natural1) CCompartment::Init: the palette self-recompute (:98) is now the sibling units' `GetPalette(header)`
  inline (staticmesh_init.cpp, texture_load.cpp: one five-type test, `return palette` / `return 0`), and
  `bin = b;` is gone: `bin = new (bin) CCPTMatBin(base);` with `bin` used throughout. Unit verifies, image identical.
  `WriteSwapVCD` still load-bearing (4 lines, one register: the swap loop's normals load coalesces with its mask in
  the original, not in the chunk loop); ~40 natural variants inert (decl orders, list/stride/swap/j/k scopes,
  shared helpers, WriteCP/mask spellings). Drafts: scratch/loop3/natural1/cpt/.
