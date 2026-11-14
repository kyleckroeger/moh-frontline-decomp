# buddy2 agent (2026-11-13)

## LANDED
- 5ed2ccfa buddy.cpp tail unit src/game/buddy.cpp (0x80058ccc..0x800590ec, 1,056 B: Destroy, __sinit_buddy_cpp (+224 B new),
  ~vector<BuddyPathInfo>, ~vector<BuddyInfo>). Replaces buddy_destroy, buddy_vec_path_dtor, buddy_vec_info_dtor. Owns .bss block
  0x801c6c20 (real containers -> CW makes the chain records @562..@564) and .sbss (g_bInitialized, g_player_checkpoint).
  port_unit --like buddy_head.json (include dirs!), 7 --weak-duplicate (begin/end x2, vec_iter dtors x2, ~list).
- Check(state) ruling applied to buddy_head, buddy_list_reserve (re-verified) and the 4 open drafts (scores unchanged).
- source_link refresh: 54 / 102 / 45 / 0.

## Create (0 by score) still NOT landable alone
- Whole-file probes (full4.cpp = buddy_head.cpp + globals + tail of drafts; scored with tmpl_pf.json, which puts
  inc/ (DwiContainers.h with push_front through begin().m_node) first -- AddBuddy needs that push_front: 22 -> 6):
  Create's bss-before-rodata base order appears iff an EARLIER function in the TU references g_attached_buddies
  (any of UpdateBuddyCheckpoints..AddBuddy; SetBuddyPathGroup's g_buddy_path_base alone is not enough; unused inlines
  that reference it are not enough; swapping the if/else is not enough). So Create can only land with AddBuddy
  (contiguous before it) -> AddBuddy must match first. (A gap function would also do it: on hold.)
- Shared header: AddBuddy needs push_front via 'begin().m_node' (header has m_head.m_next); check all 115 header units
  before changing include/game/DwiContainers.h.

## AddBuddy 6 (x/z FPRs of the inlined GiveBuddyOrders block)
- GiveBuddyOrders' own decl order is the lever: 'float x = 0.0f; float y = x; float z = x;' -> AddBuddy 0, but then
  UpdateBuddyCheckpoints 6, UpdateBuddyProgress 6, GetInitial 13 (all landed/close). zyx (current) the reverse.
  Call-site wrappers (iterator overload, SetBuddyProgress direct, extra inline levels, ref/ptr/const params, vector+index
  overload, identity inlines) are INERT for AddBuddy (ab/grid.py, sig.py, par.py, gen2.py). First-block decl order inert.
  UsingBuddy dump in an inline (BuddyInfo& / const& / int i order): FPRs right, block-2 GPRs one lower (17).
- Need: something in AddBuddy's context (not GiveBuddyOrders' body) that flips its coloring. Untried: different loop form
  for AddBuddy (find slot then act), giveOrders handling, return/break structure.

## BIFunc_AddBuddy 22-24, GetInitial 7: no gain this round
- BA: 11 order variants of info setup (ba/gen.py), debug block in helper (188). GI: soldier/it decl forms (gi/g.py).
