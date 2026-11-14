# buddy3 (2026-11-13): x,y,z GiveBuddyOrders body vs the landed users -- no all-zero form, nothing landed

Tools: ms.py (one compile, line-diff score of every function incl. aliases NAME_ZZtag), big.py (one file per shared
config with ~430 per-function context aliases), one.sh/summ.py (grid driver/summary), blk.py (the 12 x/y/z block
forms: decl permutation x assignment order), dd2.py/side.py (alias diffs), gp.py, ub.py, ab2-5.py, ubc2.py.
Scores below are line diffs (unitcmp/ms metric, about 2x cmpall).

Model (empirical): each x/y/z block's FPRs AND GPRs follow its declaration order, mirrored by the block's
colouring order in the function; the colouring order is set by inline depth (deeper = coloured later, own
code (depth 0) last; ties: g-depth >= dump-depth+1 swaps). Every context lever found (wrapper depth, iterator
overload, DumpAttached depth) flips FPR and GPR orders TOGETHER; the original needs them flipped separately
between UpdateBuddyCheckpoints and AddBuddy.

- GBO body sweep (12 forms, everything else landed): only xyz_yz gives AddBuddy 0, only zyx_yz gives
  UBC/UBP 0; exact mirror. Hoisting GBO's decls (720 orders of path/object/x/y/z/index): floats decide, GPR
  order only via 'index first'; no order gives both.
- Grid of shared configs (DBV form x DRB form x DumpAttached struct x GBO form, 288 + 392 configs, each with
  per-function aliases: UBC GBO depth 1-4 x dump depth; UBP top decl order x assign order x SetBuddyProgress
  form x DumpReady depth; Died/GI/BA/AddBuddy own-block form x call depths):
  * landed world (GBO zyx, DBV zyx, DRB xyz): UBC 0, UBP 0, Died 0, GI 14, AddBuddy 14, BA 48.
  * xyz world A (GBO xyz, DBV xyz, DRB zyx): UBP 0 (direct SetBuddyProgress code or an extra DumpReady
    level), Died 0 (extra DumpReady level), AddBuddy 0, BA 32, GI 28, UBC 26 (GBO deeper than DBV: FPRs all
    right, GBO path/index and DBV soldier/index GPRs swapped). GPR decl sweeps (gp.py) break Died.
  * hoisted float decls in GBO/DBV = the nested forms (inert).
- AddBuddy in the landed (zyx) world: UsingBuddy block in an inline helper (any param form) + GBO one level
  deeper (G2 wrapper) + argument 'g_attached_buddies.m_data + i' (or &m_data[i], G2r(m_data[i])):
  FPRs right, path/index right, ONLY the GBO 'buddy' pointer register left (ours r25, orig r29): 26 lines.
  Pointer routing (slot locals at 3 positions, hoisted, vector/int/ref wrappers): no gain.
