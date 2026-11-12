# buddy.cpp merge notes (2026-10-21)
Target unit: one merged unit 0x80056728 (buddy_head) .. 0x80058508, owning buddy.cpp .rodata (base 0x8016fdd0, dwi_prime_list first) and
.bss 0x801c6c20 (g_attached_buddies vector @? , g_ready_buddies list (count, cap, pool, free @+0x34?, head @0x801c6c58 prev/next), g_buddy_path_base).
Weak templates inside the range: vector<BuddyInfo>::end/begin (0x80056b6c/84), vec_iter<BuddyInfo>::~vec_iter (0x80056b90): folded into buddy_head (2026-11-11), which now covers 0x80056728..0x80057164.
Inline helpers seen in every BIFunc (all inlined):
- GetObjectInfo(obj): pos = (*(obj+8))->+4 node: floats +0x10/+0x14/+0x18, short +0x38 path id; defaults _988 (0.0?) / 0; then obj+0xc->+0xc vtbl+0xc call -> object with +0x5c field. Used for "[%3d @(%4d,%4d,%4d) %4d]" debug prints.
- GiveBuddyOrders(BuddyInfo* b): "Attempting GiveBuddyOrders"; if b->progress(+4) < g_player_checkpoint { if (!b->moving(+0xc)) { BSObjectTriggerEvent(b->soldier->vf60(), 0xd3, b->path(+8)+b->progress+1, g_pBSObject, 0); b->moving=1; print advancing } else "Already moving" } else { "Stopping buddy"; b->moving=0 }
- DumpAttachedList / DumpReadyList (debug prints).
BuddyInfo (20 bytes): soldier*, progress, path, moving, bool active(+0x10).
Ready list node: prev,next, BuddyInfo data (+8..+0x18).
BIFunc stack idiom: as buddy_head (firstArgument/argumentCount).
Decompiles: re-run Ghidra decompile on 0x800567e8, 0x80056bd4, 0x80057164, 0x80057640, 0x80057b30, 0x80057eac, 0x8005823c.

## 2026-10-21 (late) full-file draft: scratch/buddy/buddy_full.cpp (score: python3 scratch/buddy/cmpall.py config/GMFE69/buddy_head.json <src> [fn]; V=N shows diffs)
- EMISSION ORDER SOLVED: begin/end, reserve, list dtor, range ctor (in-class member template), copy (static inline) all defined INLINE
  (in-class or 'inline') -> CW emits each weak instance right after its first user, exactly the original order 0x80056728..0x800590ec.
  Out-of-line non-inline template defs go to the end of the file instead.
- dwi_prime_list (header static, 27 primes) must lead .rodata (string offsets +108).
- BuddyInfo has a USER COPY CTOR: ': soldier(o.soldier), progress(o.progress) { moving = o.moving; active = o.active; path = o.path; }'
  (node copies 0,4,12,16,8; operator= stays implicit layout order). Range ctor = init list incl. m_head(&m_head,&m_head,T()) + inline insert(end(), first, last).
- Matching now: SetBuddyPathGroup, Checkpoints, Destroy, all reserves/dtors/begin/end (0). Left: Progress 113, Died 16, GetInitial 27,
  BIFunc_AddBuddy 217, AddBuddy 22, Create 81, list reserve 7 (r4/r5 swap), range ctor 81, sinit 31.
- UNSOLVED KEY: the original keeps the BuddyInfo() temporary's BODY-assigned fields (moving/active/path) in stack memory (dead stores in
  list()/range ctor; reloaded after end() in Create) while soldier/progress (init list) are constants. Every probe (scratch/buddy/probe/*.cpp:
  this->, Reset(), static Clear(ptr), nested struct, unions, by-value/default-arg, iterator temp) either SROAs all five or stores all five.

## 2026-11-11 (loop scripting) UpdateBuddyProgress matched
- 'Already past there' first: 'if (it->progress >= checkpoint) { msg } else { msg; ...; result = it->moving; } found = true; break;'
  (result is a bool assigned straight from the int field: 'srwi r0; mr r25,r0').
- Ready-list loops through dwi::list_iter (begin()/end() inline) instead of raw node pointers; DumpReadyBuddies with its locals
  declared at the top (node, soldier, object, x, y, z, index, i, state).
- FP colouring of the inlined GiveBuddyOrders: the "Haven't reached yet" updates sit in an inline taking the vector iterator BY
  REFERENCE ('SetBuddyProgress(dwi::vec_iter<BuddyInfo>& it, int checkpoint)'); a BuddyInfo* parameter or direct code mirrors x/z.
- Rule seen in every buddy BIFunc: the FIRST object-info block of a function has x in f31, every later one x in f29.
  BuddyDied (12) has exactly that left: its DumpAttached/DumpReady blocks come out x=f31.
