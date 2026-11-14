# buddy agent (2026-11-13)

## LANDED 44540024 buddy_list_reserve += list<BuddyInfo> range ctor (360, 3 -> 0) + ~list (buddy_list merged in)
- BuddyInfo partial scalarisation rule: nested POD BuddyState + a dead whole-struct read of state BEFORE the typed
  writes in BuddyInfo() ('BuddyState unused = state;'); also works: 'state(state)', empty by-value inline Check(state),
  discarded GetState(). BuddyState needs a typed operator= (layout order). Probes: p/ (sinit2's st.sh), v/ w/ u/.
- buddy_head converted to the same definition and re-verified.

## Drafts
- Create: 0 by score (research/drafts, 'int first'); landing blocked by pooled base order in a fragment
  (final_create.cpp + port_unit flags: --pool .bss, five --weak-duplicate, see the draft header). Land with AddBuddy.
- BIFunc_AddBuddy 22 (ad/p09_0312_1.cpp): soldier/progress r26/r27 vs r27/r28, x/z FPRs. Helper inline: 188.
- AddBuddy 6 (FPRs, late2 notes), GetInitialBuddyOrders 7: unchanged with the new definition.
- __sinit_buddy_cpp: sinit2's probe gives 0 with this form; still needs the .bss-block unit decision.
