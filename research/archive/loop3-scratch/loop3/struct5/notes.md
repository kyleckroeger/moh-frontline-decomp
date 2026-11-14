# struct5 (continuing struct4), 2026-11-12
Harness: var.py (tlt: `RD=1 python3 var.py tlt base.cpp out.cpp 'old' 'new' ...` -> fast score, structural, register diff),
gvar.py (gcw, same idea), tlt/sd.sh, gcw/s.sh, sct/s.sh (all write only under struct5/). struct4 tools: ../struct4/tools/{asm,rdiff}.py.

## No landings this session.

## GetClosestWaypointByType (784) - structural 9 -> 2: gcw/GCW_BEST2.cpp (fast 238)
- SOLVED the unfolded index 0: start the caller at 'int index = -1;' and fetch the FIRST waypoint through NextWaypoint
  ('waypoint = NextWaypoint(leaf, index, 0, type, all);', Next does index++ then FindWaypoint). Gives the orig
  'li r31,0; slwi r6,r31,1; cmpw r31,r8; bge' exactly. Partial step on the way: index inside a local struct/array
  ('struct { int index; } it;' or 'int it[1]') gave structural 5 (slwi still mr).
- Left (structural 2): the two 'li r3,0' on the return-0 paths of the first find - follow the register colouring.
- Registers: orig index r31, type r30, leaf r29, position r28, all r27, base r26, waypoint r25, closest r24; ours
  base r31, index r30, waypoint r29, closest r28, all r27, type r26, position r25, leaf r24 (params always r24-r27).
  Inert: all 120 inline parameter orders (both helpers), caller decl orders (closest/nearest/waypoint first), for-loop
  caller. Worse: full CWaypointIterator class (object goes to memory, 269), conditional 'if (!current) index = 0' (302),
  whole body wrapped in an inline (not inlined, too big).

## TLT_LoadFromRepository (572) - still fast 22 (TLT_BEST from struct4), parked
- KEY FINDING: writing find's code out in the CALLER (tlt/TLT_h1_regs.cpp: caller 'it' is the loop pointer, count==0 and
  not-found assign 'it = g_Repository.end()', found path 'goto found') gives EXACTLY the orig registers (data r7, last r8,
  p r9) - only structural diff left is the found branch: orig 'bne +8; b found' (an inline return's copy that got
  coalesced) vs ours 'beq found'. So the knot is the inline RETURN TEMP: when p, the return temp and the caller's it
  coalesce, the group colours first (r7). A separate caller copy ('it = p; goto found', h7) does not coalesce (extra mr).
- Inert (all 22, same p r7/data r8/last r9): iterator class (with/without copy ctor, landed PathNodeList_disable form),
  late-assign 'it; it = find()', 'it' declared at top, const / const& / ctor-init caller forms, iterator-object bucket
  walk (it.m_p++), data/last named locals, return-temp style body ('iterator it; ... return it;' = 20 but count==0 end
  merges with last), nested inline search(), contains()/InRepository() wrappers, out-of-class inline find, out-param
  find (void/bool/iterator& return: beq structure). 400-sample random search over 12 knobs of find's spelling
  (tlt/gen.py): all 22.
- Ideas not tried: something that leaves an EMPTY block in the caller form (h1) without a dead store; a by-value
  iterator class whose copy ctor does real work; different inlining depth for the caller's end().

## CScene ctor: looked only (162, structural 15). Orig 0/1/2 in r27/r28/r29 with no call before them (pressure-driven);
  ours puts 1/2 in r12/r11 and this+144 in r28. Not worked.
