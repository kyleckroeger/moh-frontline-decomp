# undef (2026-11-13)
Part A/B landed (see log): undefined 54 -> 40. PATHI_eventstart draft renamed view PATHNODEVIEW -> PATHFINDNODE:
code identical (check_draft consistent, 160 lines) but the file hash changed, so its route_1_2 proof mark goes stale
on the next check_draft --all: needs a reprove on route_1_2.
Landed: 9d46be0d (A/B: 54->40), 36657ee7 (font.cpp tail merge, 40->39), 29f602fe (animmorph.cpp tail merge, 39->38), 3723dc5b (survey).
Next candidates (research/notes/undefined-survey-2026-11-13.md): debug_prof run (6 frags), cdbgeom tail (4), capsule tail (5) and box tail (4) after moving their private CVector3/CMatrix views to the shared headers.
Recipe: scratch/loop3/undef/tpl_*.json (fragment flags with -RTTI on), scratch/loop3/merge1/chk.py, port_unit --pool .sdata2 + weak/file duplicates.
