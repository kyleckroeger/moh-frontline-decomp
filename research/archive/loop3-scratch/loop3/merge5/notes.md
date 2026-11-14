# merge5 (2026-11-13): whole-file merges of vtable owners, continued from merge4

Tools here (merge4's plus): land3.sh/mrg.sh as merge4's (lock name merge5); mt.py now loads
tools/port_unit.py (merge4's fix is in it). Per-file work dirs: hier/, wpn/, thr/, tb/, ap/, lt/,
each with head*.inc (declarations), body.inc (definitions in FORWARD emission order) and
build.sh (cat -> rev.py -> chk.py with -inline auto,deferred). The body is reversed by rev.py,
and deferred inlining reverses it back, so body.inc order = text order. extra.txt in each dir =
the --weak-duplicate list (every compiled weak symbol the original's file lacks; check each
exists in merge1/orig_syms.txt or one_unit fails "Weak duplicate lacks one retained original
copy" (hierobject's __dt__17CHierObjectBuffer: drop it, it is a discarded inline)).
File index for port_unit = line number of the file record in merge1/orig_syms.txt minus 4.

Landed: hierobject f1f51e14, weapon 56e764ae, thrown_obj 0605808c, thrownbullet 82142e7f,
animplayer a60776ff; source_link: 61 undefined, 108 fragment-local, 50 duplicate.

Tricks learned
- CVector3 copies: shared header + `operator=` through the CVector3Data union (loads both
  doublewords before storing) fixes the fragments whose views copied a union (hierobject,
  animplayer). Constructor-free vectors (no user ctor at all) keep return-value temporaries and
  stop stores to vector locals from forcing reloads of unrelated members (thrown_obj
  ConstrainVelocity/BeginUpdate, thrownbullet explosion); then ISceneNode/IMovingSceneNode need
  private copies (their GetPosition and GetWorldLinearVelocity bodies touch the vector), and a
  lone `x = CVector3(a, b, c)` temporary becomes `x = CVector3Value(a, b, c)` with
  `struct CVector3Value : CVector3 { ctor }` (thrownbullet Init).
- Predefining include guards (GAME_CVECTOR3_H, GAME_CMATRIX_H, GAME_ISCENENODE_H,
  GAME_IMOVINGSCENENODE_H) before a file-private class lets the shared headers use it (weapon).
- A view's "non-virtual" base call written `((Base*)this)->F()` becomes a virtual call once
  the class is real: write `Base::F()` (animplayer UpdateAnimation/Init, light ISubject).
- Fragment views named slots by guess; use the shared header's names by offset (thrown_obj:
  0x84 AsWorldObject, 0x8c AsAnimObject, 0x9c AsPlayerObject; thrownbullet GetOwner = 0x94
  AsSoldierObject; weapon's holder slots = AsPlayerObject/GetCollisionId).
- BSGO_Basic: declare its three words BEFORE its virtuals (vptr at +12).
- An override with a different return type silently becomes a new slot: AsAnimatedPlayerObject
  must return CAnimatedPlayerObject* (the shared slot's type). Run vtchk.py.
- Weak inline copies: emission order is not simply declaration order; for const/non-const
  pairs declare the non-const first (the const copy comes out first); otherwise try both.
- A weak inline whose kept copy is in another file still has to be declared inline here
  (thrownbullet GetFiredBy, kept by static_obj) or the vtable slot names the base's function.
- Data files: when a merged unit defines a global that research/data/ also defines, git rm the
  data file in the landing (RMFILES), e.g. research/data/init/thrownbullet.cpp.

Parked
- light.cpp (draft lt/, all 45 functions match, order ok, 4 vtables match; land would need
  RMFILES=research/data/bss/light.cpp and EDITCMD removing g_DefaultLights/g_DefaultLightVolume
  and their BPDLight views from research/data/init/compartment.cpp, which the original places at
  the start of light.cpp's .data). Blocker: .data order. Original: ..., __vt__19CInstancedAnimLight,
  @1451 (CAnimLightManager RTTI) + weak __vt__17CAnimLightManager, __vt__18CPropertyAnimLight,
  __vt__6CLight. Compiled: Instanced, [weak ISceneNode/IMovingSceneNode vtables, dropped],
  Property, CLight, then the manager's RTTI + vtable last (mt.py: "Inconsistent address for
  .data: 0x80183e38 vs 0x80183d38"). With inline manager virtuals the weak ~CAnimLightManager/
  Destroy also come out Destroy-first; explicit __declspec(weak) definitions at the end of body
  fix the text order but not the data. Tried without effect: static object defined mid-file or
  at the end (with an extern declaration first), manager class before CLight, an out-of-class
  inline constructor mid-file, out-of-class inline virtuals. SetDefaultLight needs a private
  double-pair direction view (lt/head2.inc LightDirectionView) - the shared vector swaps f13/f31.
- anim_obj, alarm, pumhandler, world_object: not attempted (time). animplayer's CAnimObject /
  CSoldierObject declarations (ap/head2.inc, members by offset to +0x44b0) are a start for
  anim_obj; its fragments mix deferred and non-deferred compiles.
