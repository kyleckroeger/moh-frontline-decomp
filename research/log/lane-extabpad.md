# extabpad notes
Task: extab padding patch tooling (user-approved 2026-10-27) + land blocked fns.
- Staged tooling: scratch/par/extabpad/stage/ (tooling.patch for project_build+Progress.md; extab_patch.py, test, config json, one_unit.py patched copy).
  NOTE: I briefly (~3 min, ~18:15-18:18) wrote these into tools/config/docs/tests without the lock; reverted at once, not in soldier's 773ad5f5.
- Field: 0x8b DELETEPOINTERCOND pad at action+6; 0x90 CATCHBLOCK_32 pad at action+2.
- spline mgr ctor: record +0x16 066c->2500 (orig 0x8000a9dc). draft spm/new.cpp (0 diff all 3 fns).
- AddChildren: record +0x1a ff7f->2500 (orig 0x8000a80c) - NOT 0025.
- Tools: origx.py <addr> [n] = orig extab record; trypatch.py.
## Landed
- c27aa295 tooling; 870fdbb3 SplinePathManager ctor; bf0eca3b AddChildren; b2bbd91f LoadWeaponObjects; 6a4d17b1 CAnimObjectFactory::Init
## Pending
- dmmorph: script dmm/land1.sh (rename dmmorph->dmmorph_buffers, new unit dmmorph = SetMaxActiveTargets+InitObject+__sinit, drop dmmorph_initobject), then Game.md rows + land.sh.
- ComputeAStarPath: cap/new.cpp (int result variant: 21 sbs lines; bool result: 25 = goal/list reg swap). Patches would be +0x2a and +0x3e (0x90, ff7f->2500). Permuter cap/perm (delete/new -> perm_delete/perm_new inline helpers), base 460.
- 8efcd348 dmmorph landed.
- ComputeAStarPath PARKED: scratch/par/extabpad/cap/new.cpp = whole AIObject_searchnodes unit + CAP, sbs 8 lines: only the removal loop (list walk deleting closed nodes) has node/next in r26/r27 vs orig r27/r26. Keys found: 'while (queue.m_count > 1)' loop with break; bool result + if (found); 'CSearchNode* node;' hoisted to function top (permuter); IsClosed() in the found branch; separate nearestNode + float distance temp. Permuter (cap/perm, delete/new wrapped in inline helpers) only finds junk after ~30 min. Extab patches ready when it matches: +0x2a and +0x3e, 0x90, ff7f->2500.
