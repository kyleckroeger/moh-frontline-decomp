# Matching rules found in the matching-plan run (2026-11-17)

Compiler behaviours that closed a queue row; each was tried on the other rows of its group.

- **An empty member destructor changes a constructor's registers.** A class member whose type has
  `~T() {}` (even empty and inlined) makes CW build the constructor's code differently: in
  RCMP::DECODER's constructor the vtable address and the shared zero got one register (r5, the
  vtable stored before `li r5,0`) only once CODEC_IDATA had its destructor; without it the zero was
  hoisted above the vtable store into r5 and the vtable went to r6. Check other units' weak copies
  (here avplayer's `__dt__Q24RCMP11CODEC_IDATAFv`) for destructors a class view is missing.
- **A weak inline function lands where its first referencing function is emitted.** CW emits a
  referenced inline/weak function right after the function that first references it (queued in
  reference order); a reference made only by `__sinit` puts it after `__sinit`. If the original has
  the weak copy before `__sinit` and no kept function references it, a stripped function did: use
  an accepted gap function (`--linker-stripped`). A `delete p` through a virtual destructor counts
  as a reference. Such a gap function also makes its class's vtable and RTTI emit at that point;
  with `-inline auto` that put RCMP_SYSTEM's records before DECODER's, and `-inline deferred,auto`
  (source last to first) gave the original's order (rcmpbase.cpp).
- **A `.bss` destructor-chain record numbered far below the file's first extab label** (`@106` vs
  `@173`) means the global was defined near the top of the file.
- **A class `operator new` that returns through a named local puts the new-expression's result in
  the highest callee register.** `void* p = alloc(...); return p;` instead of `return alloc(...);`
  (same code wherever the result is only returned): in RCMP::MOHFL_VD_create the GCHW_VD pointer
  moved from r29 to r31 and the constructor's inlined parameters fell into the original's order
  (17 -> 0, with the frame height declared before the width). swvideodisplaylist_dtor's
  GCSW_VD_create, which shares VIDEO_DRIVER's operator new, is unchanged by it. Of the other near
  rows only MOHFL had a class operator new.
- **A constructor inlined into its only caller can still be a linker-stripped function.** If a
  weak copy (VIDEO_DRIVER's destructor) sits between the previous function and the caller, define
  the constructor out of line just before the caller: it emits there, pulls the weak copy after
  it, and `strip_unused` drops it (accepted gap function; hwvideodisplaylist.cpp).
- **Register priority comes from Chaitin sweep levels, not only declaration order** (regA,
  BIFunc_GetInitialBuddyOrders). CW's allocator (described in the mwcc-debugger README,
  github.com/cadmic/mwcc-debugger): phase 1 sweeps the variables repeatedly and removes any with
  at most 28 remaining neighbours (r0/r3-r12 count as neighbours for values live across calls;
  coalesced copies stay in the graph as neighbours); a variable removed in a later sweep gets a
  higher priority than every variable of an earlier sweep. Phase 2 assigns by priority: the lowest
  numbered callee register already in use that no assigned neighbour holds, else a new one below
  (calibrated: `int a = g(); int b = g(); h(); k(a, b); int c = g(); ...` gives a r31, b r30, c r30).
  Declaration order therefore ranks locals only inside one sweep; a busy caller can push some of an
  inline's locals into a later sweep and invert their rank. Lever: put the loop body in an inline
  with parameters (the entry and slot) and declare the locals inside it; GI's ready-list dump went
  from the soldier ranked above the node (any of 900+ declaration orders, a 23k-iteration permuter)
  to the original order. Re-sweep the float declaration orders of the other inline blocks after it.
  Same lever closed CPlayerObject::SetFinishedLevelData (regA3): `this` ranked above the loop
  index (lifted into a later sweep); writing the loop in the caller with its body in a free inline
  `SaveWeaponEntry(CWeapon* weapon, int i, int mission, int stage)` gave the original i r29 > this r28
  (the same body inside a member loop inline stayed 11 lines off).
- **Same final code, different pre-RA graph: try the loop form of an unrolled search** (regrules,
  AnimStProcess). An if/else chain that CW could also have produced by fully unrolling a small
  constant-count loop ('int last = 3; if (!s[3]) { last = 2; if (!s[2]) ... }') compiles to the same
  instructions as 'for (last = 3; last >= 0; last--) if (s[last]) break;', but the allocator sees a
  different graph (more values alive with the loop's induction variable before unrolling), which
  changes which values are still blocked in the first simplify sweep. Here it moved owner above the
  strength-reduced slot pointer (r31/r30, 23 -> 0). Try it whenever a long-lived value ranks one
  place too low and the code contains a hand-unrolled compare chain.
- **The allocator, read from the compiler binary** (mwcceppc.exe 1.3.2, x86; Coloring.c strings
  locate it): colorinstructions 0x5077b0 builds the graph (0x579cf0), simplifies (0x507b50) and
  colours (0x507a30). Simplify sweeps the virtual registers in INDEX order and removes every node
  with degree < K at once (neighbours decremented immediately), repeating sweeps; when a sweep
  removes nothing it pushes the node with the smallest cost/degree and sweeps again. Colouring pops
  the last pushed first and takes the lowest-numbered register among the volatiles and the
  nonvolatiles already obtained that no coloured neighbour holds; otherwise it obtains the next
  nonvolatile (r31 down). Measured index order (highest index is coloured first when nothing is
  blocked): optimizer temps (IV pointers/offsets, hoisted constants and addresses) > inline-expanded
  variables > outer locals (first declared highest) > outer params (LAST param highest); inline
  params rank first-param-highest (in3(k(1),k(2),k(3)): x r31 > y r30 > z r29). Probes and the
  replay tool (scratch/loop4/regrules/sim.py, calib.py: 1139 of 1396 landed functions with DWARF
  reproduced at K=28) are in scratch/loop4/regrules.
- **Value-returning inline arguments are evaluated into temporaries right to left** (regA3,
  CInputManager::Update). A plain expression argument of an inline is forwarded to its use, so
  the inline's body order decides the instruction and register order. Wrapping the arguments in
  small value-returning inlines (`Both(b, Lo(input), Hi(input))`, `Hi(v) { return v >> 8; }`) makes
  CW compute them as temporaries at the call, last argument first: the original's high button
  number computed first (r6) and the low one tested first. Local copies, parameter types, parameter
  orders and body orders (70+ forms) were inert. Try it when the original computes an operand
  earlier than its first use.
- **The loop form of an unrolled chain also fixes volatile registers** (regA3, AnimStStartState).
  _AnimStGetUnusedStateID's four `if ((u16)id == slots[k].state)` compares (state in r0, id in r3
  for every spelling, operand orders and helper forms) match as a fully unrolled pointer loop
  `for (int j = 0; j < 4; j++, slot++) if (slot->state == (u16)id) { ...; break; }` (state r3,
  id r0); the indexed loop `slots[j]` is not unrolled (231 lines off).
  Also closed _find_aim_blend_values (regA3): one of four TriangleBlend expansions had two volatile
  FPRs swapped (orig -y below bx - x); a second copy for that call writing the negations as
  `Neg(x)`, `Neg(y)` (value-returning inline) matches, while Neg in the shared copy breaks the others.
  Also closed AddAmo (regA4): to make the original evaluate a call's arguments in a different order
  (here the int argument loaded before the bool's compare), route the call through an inline wrapper
  whose parameters are in the opposite order and pass value-returning inlines (an accessor
  `GetType()` instead of the plain field `m_type`): the last argument is computed first. Plain
  field arguments are forwarded to their uses and did nothing (21 forms before).
- **A statement moved into a small entry inline recolours a shared inline it calls** (on2,
  TLT_LoadFromRepository 13 -> 0). The shared DwiHashMap find (fixed body) coloured p/end/data
  r7/r9/r8 in every caller spelling with a named iterator; 'static inline bool InRepository(char*
  data) { iterator it = g_Repository.find(data); return it != g_Repository.end(); }' gives the
  original's r9/r8/r7. Passing find's result as a value-returning ARGUMENT ('end() != find(data)')
  also fixes the colours but makes the found iterator operator!='s argument, which swaps the two
  subf operands: the original compares 'it != end'. Same lever family as the loop-body entry
  inlines (BIFunc_GetInitialBuddyOrders, SetFinishedLevelData).

- (pnew, CreateRootHierObject 2 -> 0) Placement new where the original compares on the copy ('mr. rB,r3')
  but stores the vtables through r3: that is the DIRECT form 'box = new (alloc()) T' (the box web IS the
  new-expression's temporary; copy forms build through box, 'void* m' forms give 'cmplwi r3; mr', 'if (box)'
  forms keep a redundant beq). When the temporary ranks too high, don't fight it: make the variables that
  must rank ABOVE it compiler temporaries created earlier. A single-use inline that returns through a
  NAMED LOCAL ('T* t = call(); return t;', not 'return call();') turns the caller's variable into one, and
  a class operator new written the same way does it for a 'new T' result. Temporaries rank above all user
  locals, first-created highest.
- **A copy made inside an expression keeps a variable that a copy statement loses** (on3,
  CBulletDecalManager::Init__NotTheVirtualOne). `resource = call(); m_resource = resource;` lets
  CW fold `resource` into the call's result temporary (the DWARF lists the variable in r0): the
  value is then numbered as a temporary and ranks below the loop counter. Writing the member copy
  inside the first use, `if ((m_resource = resource)->data->m_count > 13)`, gives the same
  instructions but keeps `resource` a user variable, which stays blocked in the first simplify
  sweep and takes r31 as in the original. Check DWARF (research/tools/varmap.py) for a variable in
  r0 whenever a call result is copied into a local and a member. Same row: an `(unsigned int)`
  cast on part of an array index, `(unsigned int)(i * 6) + 5`, gave that offset its own induction
  variable created first (the preheader's first `li`); other cast placements kept or broke it.
- **Fold an index into its address, name a reloaded pointer** (on4, SND::decodexac 28 -> 0). Two
  opposite uses of the folded-local lever in one loop. A `float* table = &tab[k * 16]` local ranked
  as a user variable and took the wrong register; an `int ix = k * 16` index used as
  `(&tab[ix])[b >> 4]` is folded by CW into the address temporary (varmap shows `ix` in r0), which
  takes the original's register. The other way round, the per-statement reload of `packet->out`
  coloured before the loop's byte load as a temporary; `float* out = packet->out; out[0] = ..;
  out = packet->out; out[1] = ..;` (same loads) makes it a user variable and swaps the two
  volatile registers to the original's. Also: `*p->in++` used inside an expression increments
  before the load; the original's `lbz` then `addi rX,rX,1` (same register) is the increment as
  its own statement after the reads.
- **Struct-promoted fields rank below every inline local** (ba, BIFunc_AddBuddy 3 -> 0). MWCC numbers
  register candidates (0x435c60): params, user locals, then '@' compiler locals in list order; the
  fields of a struct local that CW splits into registers (a `BuddyInfo info` filled field by field)
  are '@' locals created AFTER inlining, so they come after every inline-expanded local and are
  coloured after them. To put a value below the caller's promoted fields, read it through a struct
  copy that is promoted the same way (`BuddyInfo b = buddies[i]; soldier = b.soldier;` or a
  function-scope `BuddyInfo b; ... b = *node;` declared before the iterator); to put a value above
  all of them, give it an inline's named local. Compiler truth: scratch/loop4/ba/tr.py (ptrace on an
  int3-patched mwcceppc: per function, every vreg's index, '@' name, degree and the colouring stack;
  cl.py prints BA's callee-saved webs in colour order). The colour rule then explains each register:
  a node takes the lowest obtained callee register no coloured neighbour holds, else the next new one.
- **Colour a CSE temp LATER by making it a user local declared last** (tr3, SNDI_precalcaztospkrvol
  4 -> 0). The tracer showed the original's sum register (r30, while ours r31) means the sum was coloured
  after the third branch's b had obtained r30 (lowest free obtained callee wins). A CSE sum is an '@'
  temp (high index, pushed late in sweep 1, popped first); written as a user local `int s = e + d;`
  declared AFTER the locals that must colour before it (d, e, and a, b moved to function scope so
  they are declared first) it gets the lowest index, is pushed first and coloured last. User locals
  index in reverse declaration order, so "declare it after X" = "colour it after X". Declaring s
  before d/e coloured it before them and it took a free volatile (7).
- **Move a user local above an IV by making it an inline local** (tr3, CSoldierObject::Draw 8 -> 0).
  The original's RemoveWeapons weapon pointer (a loop-optimizer '@' temp) shares mesh's r24, so it is
  coloured after mesh; ours ranked mesh (a plain user local, low index) below it. Moving the block that
  declares mesh into a single-use inline member (DrawAttached) makes mesh an inline '@' local created at
  expansion, numbered above the loop optimizer's later temps: it is pushed after the IV, popped and
  coloured before it, and the IV takes the lowest obtained register no neighbour holds (r24).
- **Push an inline's locals later by nesting them one level deeper** (tr1, quick_sort_ 6 -> 0). The tracer
  showed CW creates the '@' locals of the inlines a function calls directly before the locals of the
  inlines THOSE call (depth 1 in source order, then depth 2): the two insertion_sorts' backward-loop
  locals (i, prev) were created before the final loop's unguarded_linear_insert (last, next), so they
  had higher indices, coloured first and took r28/r27, and the final loop's next took the lowest
  obtained register r27 instead of the original's r29 (free once first is dead). Moving the backward
  pass into its own inline (backward_pass, called from insertion_sort) makes its locals depth 2, created
  after the final ULI: next now colours right after first and reuses r29. Writing the swap out inside
  backward_pass (instead of iter_swap at depth 3) keeps the swap temporaries' stack-slot order. Try it
  wherever an inline local that the original colours early is out-ranked by sibling inlines' locals.
- **A block keeps its pre-allocation schedule unless the allocator edits it** (sched, structural
  run 2026-11-24, CCoverPoint::GetPeekLocation 3 -> 0). MWCC schedules each block twice: before
  allocation (virtual registers) and after, but the second pass skips blocks the first one did
  unless a pcode was inserted or deleted there in between; deleting a coalesced copy counts. After
  allocation the real registers add anti/output dependences and the block can come out in another
  order (here `or r5,r5,r4` moved above `lwz r3` because it frees the `and r4` that reuses r4). So
  when the original equals our pre-allocation order (research/tools/alloc/sched.py prints both
  passes and the deleted copies), remove the copy from that block: here
  `CoverPointTypes types = GetTypes();` copied the inline's result into the caller's variable;
  an out parameter assigned at field level, `out.m_bits = types.m_bits & mask.m_bits;
  types.m_bits = out.m_bits;` with `CoverPointTypes types; GetTypes(types);`, defines the
  variable directly (`out = types & mask` through operator= still copies). CoverPoint_modified's
  GetAvailablePeekPosition was respelled the same way (still exact) so every fragment shares one
  GetTypes body.
- **A dead instruction can steal an issue slot from the one the original issued** (sched,
  SFILTER_rsf 5 -> 0). Before allocation a ready instruction that makes a successor ready (its
  only remaining predecessor) beats one that does not. `state->position &= 0xffffLL` on an unsigned
  position builds a dead high-word pair `li rH,0; and rX,rH,rH`; the li made the and ready, so it
  took cycle 1's second slot and the mask's `addi` slipped behind the fraction's `srwi`, which
  changed the allocation (fraction r0 instead of r4). `state->position = (int)state->position &
  0xffffLL` makes the high word a `srawi` of the position (still dead), so the li no longer frees
  anything and the addi issues in cycle 1 as in the original. research/tools/alloc/ssim.py replays
  the choice; read the picks in a sched.py dump before trying spellings.
- **A constant initialisation chain stores the last target first** (big, structural run
  2026-11-24, CProfileInitClass's constructor 202 -> 0). `a = b = c = 0;` (accepted: constant
  initialisation chain) stores c, b, a and, in a store-heavy initialiser, also changes how the
  constants are materialised: CProfileInitClass's 31 inline `Set` calls got the original's
  stores 16, 12, 8 and its li/addi grouping (stmw r23, 48-byte frame) only with
  `m_field08 = m_field0c = m_field10 = 0;`; three separate `= 0` statements in that order stored
  the same but spread the constants over more callee registers (stmw r17, 80-byte frame). Try it
  when a run of zero stores is right but the function saves too many registers.
- **Loop-invariant code motion skips expressions of variables assigned inside the loop** (big,
  structural run 2026-11-24, MemoryCardBootUpPortCheck 958 -> 269, draft). MWCC hoists an invariant
  expression such as `(int)top` or `line + 22.0f` to the loop preheader (and then shares it between
  branches) when its operands are set before the loop. The original converted `top` again in each
  branch and added 22 inside the loop: with `float top = 50.0f * scale;` written inside the do-loop
  the multiply is still hoisted (computed once before the loop, as in the original) but the
  dependent conversions stay; `float y = line; ...; y += 22.0f;` inside the loop likewise stays
  where `float y = line + 22.0f;` was hoisted. Symptom: one fctiwz/fadds at our function head
  against several in the original's loop body.
