# merge7 (2026-11-13): whole-file merges, continued from merge6

Tools here: merge6's (land3.sh/mrg.sh with lock name merge7, chk.py, rev.py, vt.py, vtchk.py,
mt.py, pool.sh, split.py) plus
- land3.sh tolerates (and restores on revert) the coordinator's uncommitted edit of
  research/plans/three-day-loop.md; land.sh sweeps it into the next commit.
- pool.sh prints the whole section bytes (no cut) so a diff position can be computed.
- wd.py OBJ IDX: lists the compiled weak symbols (candidates for --weak-duplicate).
- mtdbg.py / mtfix.py: mt.py printing which relocation anchors extab/extabindex; mtfix.py runs
  port_unit_fix.py (the tools/port_unit.py fix below) read-only.
- diag.sh: lock, merge, reconstruct/docs, run the tests verbosely, revert, release (to see
  which test a failed landing hit).
- batch1-4.sh: the landings (60 s apart through land3.sh).
Work dirs: pb/ (projectilebullet), mpc/ (rcmp_mpc_codec), cfg/ (ConfigGC), as/ (animskinned),
cam/, wv/ (world_volume, merge3's draft copied, not worked).

Landed: rcmp_main f22f4de7, projectilebullet 824da889, animskinned 517777d8, ConfigGC
91345f25, rcmp_mpc_codec 9bd2207e (with the tools/port_unit.py fix); source_link: 59 undefined,
104 fragment-local, 49 duplicate (was 61/108/49).

Tricks learned
- rcmp_main's "unexplained" .sbss gap was the global CurentOPPlayer (research/data/bss/
  rcmp_main.cpp defined it): define it after g_fw in the merged file and drop the data line
  (EDITCMD). Before inferring a gap static, grep orig_syms.txt for globals at the address.
- port_unit needs --weak-duplicate for every compiled weak function whose kept copy is in
  another file (projectilebullet: __dt__10CRenderBinFv, __dt__7IVolumeFv); without it the
  .text anchors disagree by the copy's size.
- projectilebullet: deferred + reversed source put the RTTI names first in .rodata (original
  order). A `#undef fabs; inline double fabs()` placed mid-file in the fragment became a
  problem after reversal: moved to the head, and Penetrate (which used math.h's macro) spells
  __fabs directly.
- A pair-copying CVector3 copy constructor adds a temporary to `CVector3 v = s * w;`: build
  the vector directly (projectilebullet GetVerts).
- A weak out-of-class template member definition (`template<class T> __declspec(weak)
  TDLNODE<T>::~TDLNODE()`) must come right after the class template, before any class
  derives from the instance; defined later, the explicit instantiation comes out global (T).
- ConfigGC: research/tools/check_data.py, source_link.py, source_symbols.py and
  research/data/config_keymap.cpp took their default settings from ConfigGC_dualconfig.json;
  deleting that config failed the tests (check_data). The landing repoints them to
  ConfigGC.json (EDITCMD).
- port_unit match(): a discarded function's exception records between kept ones (deferred
  mode emits unreferenced weak ~TDLIST and CODEC::operator delete right after
  MPC_CODEC_create) shifted the extab anchor by their size. Fix (landed with rcmp_mpc_codec 9bd2207e): positions in data sections close up over dropped
  spans (foreign records and weak-duplicate objects); the 'sources' anchoring uses the packed
  position. Checked: Tank_object's anchors now equal its config's (the old code gave
  0x18 low values on a fresh compile).
- animskinned: one CAnimated with the constructor's members renamed by the skeleton code's
  offsets; anonymous unions where the frame's first word is the count and where the bone
  offset pointer is the storage pointer; a file-private double-pair union for
  ExtractTranslation (the shared CVector3Pair struct schedules the copy differently).

Not done / parked
- camera: the frustum fragment's `CMatrix m;` local has no constructor call; the shared
  CMatrix (needed by rows/dtor with ISceneNode.h) adds the s_ClassInit check. Needs a
  constructor-free CMatrix for the whole file and the shared ISceneNode chain on it.
- hearingvolume: CHearingVolume : ISceneNode (shared header) with the rows view's members at
  +0x0c..+0x1c mapped onto ISceneNode's data; CBullet's weak Penetrate/SetExclusionPair are
  emitted here, which suggests CBullet's weak vtable was generated in this file (all its other
  inline virtuals would be weak duplicates). Not attempted beyond reading.
- saems (C, 3 fragments): conflicting struct views under the same names (SNDAEMSVIEW,
  AEMSMODULEVIEW, MODULEBANK...) and SNDAEMSI_destroyinstance int vs void; needs one set of
  views with anonymous unions. Not attempted.
- pathxSND: pathx_create is -str reuse,readonly, the others -str reuse.
- world_volume: merge3's ordering problem (reserve emitted last, ~list after the
  constructor); the original's ~list comes right after ~CWorldVolume although only the
  constructor's extab references it.
- alarm, pumhandler, world_object, anim_obj, AnimObjFactory: not attempted (time). alarm's
  weak list members (reserve, ~list, the iterator constructor) are the same dwi family as
  world_volume.
- light (ruling: stays split), bsutil (inline out-of-line vs inlined local order: stays split
  per ruling), Gecko_ExceptionPPC (two compilers).
