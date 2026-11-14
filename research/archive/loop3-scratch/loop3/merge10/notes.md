# merge10 (2026-11-13): whole-file merges, continued from merge8/merge9

Tools here: merge8's (land3.sh/mrg.sh with lock name merge10, chk.py, cc.py, fd.py, wd.py, vt.py,
vtchk.py, mt.py, mtp.py, pool.sh, orig.py, funcs.py, split.py) plus
- ao/gen.py + ao/head.cpp: anim_obj's merged source = head.cpp + every fragment's function
  bodies with per-fragment renames, written last to first. ao/wdargs.txt: the weak duplicates.
- src/AnimObjFactory.cpp, src/saems.c, src/pathxSND.c: the landed sources (built by inline
  python from aof/, sa/, px/ copies of the fragments).
- px/ord.sh SRC: compile with tpl_px.json and print the text order.
- land_ao.sh, land_aof.sh, land_sa.sh, land_px.sh: the landings.
- Templates: tpl_ao.json (RTTI on, deferred,auto, include dirs incl. "include"), tpl_aof.json,
  tpl_saems.json, tpl_px.json (exceptions on, lmw on, -str reuse,readonly).

Landed
- ecdb630e anim_obj (11 fragments, 72 functions, 9604 B, __vt__11CAnimObject 82 slots exact).
- 6ce9ef7b AnimObjFactory (5 fragments, 14 functions, 5720 B).
- 223b162e saems (3 fragments, 47 functions, 14380 B); f36e2c1f pathxSND (6 fragments, 18 functions, 4972 B); 0ba7cb33 source_link 54/103/47 (unchanged) + PortReadiness.

Tricks learned
- A template copied from a fragment config may lack "include" in include_dirs (chk.py/fast
  adds it, port_unit does not): the first land failed with "cannot open game/...". Check
  include_dirs of the template before landing.
- anim_obj: all 11 fragment views fold into one CAnimObject : IMovingSceneNode (shared header)
  with animplayer.cpp's virtual order; whole file deferred,auto + RTTI on (RTTI @N below the
  extab @N); written last to first; every weak in-class virtual defined __declspec(weak)
  out of class in its image slot; 69/72 exact first compile.
- Vector assignment forms: the shared CVector3 operator= (struct pair cast) stores each double
  right after its load; three anim_obj assignments load both doubles first (a 16-byte union
  copy). A file helper CopyWords(to, from) through a {double[2]; float[4]} union fixes them
  without touching the shared header.
- A literal the fragment pooled from another file's .sdata2 (anim_obj_update's 0.015625f at
  0x80350ba8) is a global constant: CONTACT_EPSILON (orig_syms). Grep orig_syms for the address.
- pathxSND: the fragments were built with exceptions off, but the original has an extab record
  per function: build the merged file with exceptions on (code identical, extab anchored).
  '-str reuse,readonly' for the whole file works when only one function has strings; no
  #pragma needed.
- CW non-deferred '-inline auto' POSTPONES some function definitions (here PATHX_queuefile and
  PATHX_createtrack) until the next inline function definition (static inline helper) or the
  end of the file. In a single fragment it is invisible when the function is last. Fix: place
  an inline definition right after the postponed function (moved ClearTrack up; an inferred
  TrackOverhead inline before PATHX_overheadtrack). Anonymous unions in a struct copied
  whole were not the cause.
- saems: SNDAEMSI_destroyinstance ends with li r3,0 -> int (callers ignore r3).

Not merged
- light (ruling: stays split), Gecko_ExceptionPPC (two compilers), world_volume/world_object
  (merge9's).
