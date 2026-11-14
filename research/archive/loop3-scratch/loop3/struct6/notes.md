# struct6 (continuing struct5), 2026-11-12
Harness: t.sh (TLT: fast score + structural), v.sh base new 'old' 'new'... (TLT), mk.sh/mk2.sh (TLT caller bodies on h1 prefix),
g.sh / gv.sh (GCW same idea), regs.sh (GCW prologue registers), gcw/gen.py (caller decl orders with const param copies,
BASE=file env), gcw/search.py, gcw/pperm.py (inline param orders). All write only under struct6/.

## Drafts commit d5baeeb8 (pushed): GCW 195 -> 94 real, TLT 104 -> 53 real (h1 form). No landings.

## GetClosestWaypointByType (784): structural 2 -> 0, fast 238 -> 222 (gcw/GCW_BEST_S0.cpp = gcw/k5.cpp)
- FIX for the two missing 'li r3,0': in FindWaypoint's 'index == triggerCount' branch hold the result in a local
  initialised to 0 and re-zeroed after the exhausted loop:
    TriggerObject_struct* found = 0;
    if (all) { for (...) if (match) return trigger; found = 0; }
    return found;
  Orig 'li r3,0; beq join' (the !all path) + 'li r3,0; b join' (exhausted path). 'if (!all) return 0;' gives bne/li/b;
  'found = trigger; break;' keeps an uncoalesced mr; 'return 0' inside the if gives an extra li/b.
  check_draft: consistent, similarity 1.0. NOTE for the maintainer: 'found = 0;' re-assigns the value found already holds
  (no storage, emits the orig's li) - flagged in the final report as a possible policy question.
- Registers now: ours closest r24, waypoint r25, index r26, base r27, leaf r28, position r29, type r30, all r31;
  orig closest r24, waypoint r25, base r26, all r27, position r28, leaf r29, type r30, index r31. Orig looks like
  spill-cost order (index > type > leaf > position > all); ours is params-on-top in parameter order.
- Inert on k5: const/non-const param copies (59 orders, before/after index), enum->int type copy, all 120 Find and
  120 Next inline parameter orders, int* index, while-form first loop, the distance test moved to an inline
  Consider(). Caller local orders: closest/nearest before index = 268 (worse).

## TLT_LoadFromRepository (572): still fast 3 (h1 caller form, 1 branch-shape diff) / 22 (inline form, regs)
- Three families, nothing between: (a) no copy in the found path -> 'beq found' (3); (b) a USER-variable copy in the
  found path ('it = p; goto found', 3-var hand-inlined 'r = p', p/r/it in any declaration order) -> right
  'bne +8; b' shape but the copy is NEVER coalesced (19, extra mr); (c) inline find (return temp) -> coalesced,
  structurally exact, but the group colours first (22).
- Inert/worse: loop-condition found test (e2 6), break + retest (e1/a3 5-6), found arm first / else arms (a1/a2 3),
  goto to a shared 'missing' label (e3 43), self-assigns 'it = it' / '&*it' / 'it + 0' (dropped early, 3),
  find bodies 'p = end(); return p;', count!=0 wrapper (22-75), caller via repo ref/pointer (69), no named it (75).
- Not tried: making the inline's return-temp group colour LATER (it is coloured before the base/last CSE temps).
