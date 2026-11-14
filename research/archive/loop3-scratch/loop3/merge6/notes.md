# merge6 (2026-11-13): whole-file merges, continued from merge5

Tools here: merge5's (land3.sh/mrg.sh with lock name merge6, chk.py, rev.py, vt.py, vtchk.py,
mt.py) plus
- pool.sh TPL SRC IDX: compares the compiled .sdata2/.rodata/.data/.sdata bytes with the
  original file record's (orig/GMFE69/Moh2RelGC.elf); run it before landing (a .data DIFF of
  relocated words, e.g. switch tables, is expected).
- split.py: splits a fragment into declarations and top-level function definitions (used to
  assemble merged files; filter out its 'inline'/'template' chunks).
- batch1.sh / batch2.sh: landings 60 s apart through land3.sh; tpl_*.json are copies of the
  fragment configs (mrg.sh git-rm's the originals before port_unit).
- lt/: light.cpp experiments (exp.sh builds a head/body variant and prints .data order;
  pos.sh inserts a probe line at a body position).

Merged files (source in src/): threads, ipad, saramman, stagpat, eaxadecf, saemsstr, sbasync,
ssysinit, sst (batch1); IStudio, IStudioFuncs, bigyuvswizzler, jaw_controller, MathFun,
animchan, renderlist, bsmessage, rcmp_main, bsevents, bulletfactory, weaponfactory (batch2).
See the final report / git log for which landed.

Tricks learned
- Two fragments compiled with different -inline modes: if one side has no calls the other
  mode would inline, use the stricter mode for the whole file (sbasync: -inline noauto for
  all; the read helpers match either way). Deferred for the whole file + reversed source when
  a later function is inlined into an earlier one (MathFun, bsevents, weaponfactory): the
  non-deferred fragments matched unchanged.
- A function defined `inline` in one fragment and out of line (global) in another
  (BSEventDecrimentReferenceCount, GetWeaponPropertiesByCRC as a file duplicate): with
  deferred inlining one plain definition is both inlined into the earlier callers and emitted
  at its place.
- Template helpers emitted by explicit instantiation (bulletfactory): one class template with
  all members, the members defined __declspec(weak) once, and `template class X<A>; template
  class X<B>;` at the end gives the original order (member declaration order per instance).
- Return type mismatch between fragments (int definition, bool declaration read as a byte):
  making the definition bool kept its code (bsmessage); a function-pointer cast at the call
  site turns the bl into bctrl (don't).
- Member views that differ in type, volatility or signedness: anonymous unions with both names,
  renaming the second fragment's uses (sst, renderlist, bsmessage, rcmp_main, sbasync).
- rcmp_main: the merged .sdata2 pool comes out in another order; --pool .sdata2 (the
  fragments already used pool_references). chk.py compiles in a temp dir, so a source-dir
  include ("rcmp_main_block.h") needs a probe copy with "../src/game/rcmp_main_block.h".

light.cpp (still parked; lt/)
- Finding: the .data position of a weak vtable of a class with only inline virtuals depends
  on whether its inline virtuals are generated in the "function phase". With the manager's
  ~CAnimLightManager/Destroy written in-class (lt/head2_inl.inc + body_n.inc, i.e. no
  out-of-class definitions) AND any function-phase use of the class's vtable - a function that
  placement-news a CAnimLightManager, calls p->~CAnimLightManager() qualified, or does
  `delete p` (a plain virtual call through a CAnimLightManager*) - the data order becomes
  exactly the original's (Instanced, manager RTTI + vtable, [weak ISceneNode/IMovingSceneNode,
  dropped], Property, CLight) and the weak ~CAnimLightManager/Destroy come out right after
  InitClass in the original order, wherever the probe function is placed (probe at body
  positions 0/26/33/266 all work). The RTTI label number stays late (@759) as in the original
  (@1451), so the original also generated the table at the end.
- Not enough: the static object alone (its sinit is generated after the vtable pass), an
  explicit or out-of-line inline constructor, copy-initialisation, extern + late definition,
  __declspec(weak) out-of-class virtuals (one or both), direct calls g_AnimLightManager.Destroy()
  (devirtualised), dead `if (0)` uses, unused inline/sizeof uses.
- None of the 45 original functions references the manager's vtable or makes a virtual call
  through a CAnimLightManager* (orig relocs: only ~CAnimLightManager and __sinit), so the
  trigger in the original is unknown. A probe function would add code. Options for the
  coordinator: (a) find a function-phase use that leaves no code (the original had one); (b)
  accept a discarded helper function (like rcmp_main_block.h's RcmpMainBlockStrings, listed in
  discarded_functions) that does `delete (CAnimLightManager*)p` - a layout device, a policy
  question.

Not done (with what I found)
- anim_obj, alarm, pumhandler, world_object (task 2): not attempted (time went to the 21
  simpler merges); merge3/merge5 notes still apply.
- AnimObjFactory (5): five different CVector3/CAnimObject/LevelFile views, ~1000 lines.
- ConfigGC (3): dualconfig has its own CScreen view, init includes game/CScreen.h.
- rcmp_mpc_codec (4): create is -RTTI off non-deferred, codec deferred with RTTI.
- projectilebullet (3): dtor/link -RTTI off, main RTTI on.
- pathxSND (6): mixed -str reuse / -inline on flags.
- saems (3, 47 functions), camera (frustum's own CVector3), animskinned, hearingvolume: not
  attempted.

rcmp_main (parked after a failed landing, nothing committed): all 9 functions and the pools
match, but .sbss: the original has g_fw at 0x8034edc8 (4), then 4 unexplained bytes, then
Play's FirstTime$703 at 0x8034edd0 and init$704 at 0x8034edd4. The merged compile puts the
function statics first (port_unit: "Inconsistent address for .sbss"); with g_fw defined first
(src/rcmp_main.cpp now) it gives g_fw, FirstTime, init with no gap. The gap is probably a
static of a function the original linker removed (the same removed code whose strings
rcmp_main_block.h recreates); filling it would extend that layout device (policy question).
