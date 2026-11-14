# merge8 (2026-11-13): port_unit regression check + whole-file merges, continued from merge7

Tools here: merge7's (land3.sh/mrg.sh with lock name merge8, chk.py, cc.py, fd.py, rev.py, vt.py,
vtchk.py, mt.py, pool.sh, wd.py, split.py) plus
- sweep.py / sweep2.py / sweep3.py: tool1's sweep, old (oldtools/ = 9bd2207e^) vs HEAD / vs
  fixtools/ / HEAD vs fixtools; units.txt = the 302 strip_unused units. Results sweep.out,
  sweep2.out; summary regress.txt.
- mtp.py OBJ ORIGFILE IDX WEAKFILE [pooled sections...]: mt.py with pooled sections.
- dtv.py SRC TPL FUNCS: tries ~list body variants (alarm).
- land_hv.sh, land_cam.sh, landfix.sh: the landings. Work dirs hv/, cam/.

Landed
- 4f1fc15d tools/port_unit.py fix: 9bd2207e's close-up also subtracted LEADING dropped records
  (16 units' extab/extabindex drafted 8..0x30 high, e.g. alarm_list_ctor, Tank_object, bstimer);
  now only dropped spans after the section's first placed object close up. regress.txt.
- b5ba70e7 hearingvolume (5 fragments, 2692 B, __vt__14CHearingVolume).
- 1adffd9d camera (3 fragments, 4632 B, __vt__7CCamera, ASPECT_4x3).
- a5ad9727 source_link: 57 undefined, 104 fragment-local, 49 duplicate (was 59/104/49).

Tricks learned
- RTTI label numbers tell the compile mode: if the original's @N for the class name/base lists
  is lower than every extab @N (hearingvolume @724 vs @779, camera @557 vs @642), the file was
  compiled deferred: use -inline deferred(,auto) and write the source last to first. Non-deferred
  puts the weak base vtable (ISceneNode) and its RTTI first in .rodata/.data.
- camera: '-inline deferred' WITHOUT auto when deferred auto-inlining would absorb a callee the
  original calls (AttemptUpdate -> UpdateWorldToCamera); the fragments had no other auto-inlines.
- camera: a member array whose destructor is called after an address check (addic. rX,this,off;
  beq) = a member OBJECT with an inline destructor holding the array; here CCamera's planes are a
  CFrustum (the file's own class of ten planes).
- camera: CMatrix local without constructor call: file-private CMatrix = the shared header with
  `CMatrix() {}` and `CMatrix(int identity)` (keeps CCamera.h's m_tm(true)); include guard
  predefined before the shared headers.
- UpdateWorldToCamera on the shared CVector3: `CVector3 right = m.right; right *= -1.0f;` then
  copy-construct forward/up/position (declaring them first and assigning gave 88).
- An unused-looking data object right after a file's vtable may belong to the NEXT file record:
  check the next record's section start (g_idSet is mainloop.cpp's, not camera.cpp's).
- wd.py lists the file's own global weak functions too (they are global elsewhere in
  orig_syms): subtract the fragments' functions. A weak duplicate that the merged compile does not
  emit (hearingvolume's __vt__7IVolume in deferred mode) must be dropped from the list
  (data_strip: "Weak duplicate is not one compiled weak function or data object").
- Shared ISceneNode words +0x0c..+0x18 are the sweep-extent record pointers (hearingvolume and
  camera store a float at +8 of each): cast dataNN through a small view.

Not merged (with findings)
- alarm, pumhandler, world_object: the same dwi::list destructor case as world_volume (ruling:
  stay split). Merged alarm draft (scratch/loop3/merge8/src/alarm.cpp, shared ISceneNode.h,
  CAIFilterRealPosition : CVector3, one AlarmInfo view): 10 of 11 functions exact, order ok;
  the out-of-line ~list needs `next` declared before `node`, the copy inlined into reserve needs
  `node` first (r4/r5 swapped, 14 lines). 18 body forms tried (dtv.py: for/while, scoped next,
  end().m_node, free_node helper, out-of-class/weak/inline definitions, reserve forms): every
  one fixes one side only. pumhandler_list/world_object_list vs their reserve fragments have the
  same two orders.
- anim_obj (11 fragments, 73 functions, 6 deferred + 5 non-deferred, weak CBullet copies,
  __defctor__7CMatrix, fast_vec<CClsnInfo> dtor): not attempted (time).
- AnimObjFactory (5 fragments, 14 functions, all '-inline auto'): views read (AnimObjectEntry vs
  CAnimObjDef for the same 0x50-byte records; three LevelFileInfo views, two of them in
  AnimObjFactory_weapon; a private CVector3 struct in AnimObjFactory.cpp); not attempted (time).
