# merge2 (2026-11-13): whole-file merges, continued from merge1

Tools here (copies of merge1's, adapted): mktpl.py <unit> <out.json> <inline> [incdirs]
(RTTI on, -inline set), chk.py (now also scores __sinit_* against the probe's sinit),
cc.py <tpl> <src> (compile, print first errors), orig.py <file.cpp> (the original's local
symbols of one file record + globals in its data sections), vt.py <__vt__X> (each slot of an
original vtable with its symbol), merge.sh (as merge1's, sources from merge2/src), rev.py.

Step 1: 39a20dca, source_link/source_symbols/layout_lint/check_data append include/ to a
source's include dirs (as fast.py): 75 undefined, 149 fragment-local, 53 duplicate.

Landed: fader 42acea64, sprite 909ae39f (research/data/bss/sprite.cpp removed), animated
7060e48d, animated_volume 3b744322, smixfx b126c2be, sdspmix 21a1781c, animseq 8644e7e9,
animmot 5a1179a4, stretch 9f9d8f22 (first try failed: .sdata2 pool order, fixed by setting
crossfade's weight before its step), SearchNode 25359b64 (unit SearchNode, source now
src/game/SearchNode.cpp). Final source_link: 70 undefined, 137 fragment-local, 53 duplicate.
landone.sh / batch.sh / sn.sh: lock, merge, verify, land, push, release (revert on failure).

Tricks learned
- A struct with a user constructor (CColor) can change a function's frame even where only
  its copy is used (sprite Link: +16 bytes, one more saved GPR). Keep it an aggregate and
  initialise the members in the owner's constructor body.
- port_unit "Compiled code refers to X where the original refers to Y" on a vtable slot: the
  intermediate base must declare the override (animated_volume: CVolSphere's TestVisibility
  and GetExtents).
- --weak-duplicate on a vtable that the TU does not emit fails in data_strip ("not one
  compiled weak function or data object"): check `nm` for D/W vs U before listing it.
- Forward order + -inline auto matched every function of animated.cpp but put the RTTI name
  after the strings in .rodata; deferred + reversed fixed it (check .rodata order with
  objdump -s before landing).
- Two unrelated views of one struct (sdspmix _AXVPB): map one view's fields onto the other's
  by offset (the fragment comments say which are offset-named).
- Static helpers the original calls out of line in a file compiled -inline auto: '#pragma
  auto_inline off' / 'reset' around them (stretch, fader's MathFunClamp).
- merge.sh is set -e and git-rm's the other fragments first: when port_unit fails, finish by
  hand (cp src, port_unit, pop adapted_from, one_unit) instead of re-running it.
- Never release the lock before land.sh has committed (wait for the task notification).

Not done (next agent), with what I found
- Tank_object (4 fragments): __vt__11CTankObject has 91 slots (vt.py lists them with owner
  classes: ISceneNode/IMovingSceneNode header inlines, CStaticObject, CAttachableObject
  (CommitAI, IsVisible), CHierObject, CTankObject). Needs CStaticObject : IMovingSceneNode with
  the new virtuals from slot 0x124 (Init ... InitAI) in order, padding so CTankObject's
  members land at the fragments' offsets (flag bit at +0x1e0, doodad at +0x740, look
  sub-object +1940, spheres). Tank_object_weapons uses its own CVector3/CMatrix views
  (GetFront/GetPos by value) that conflict with the shared headers: rematch it on the headers
  first. RTTI chain IDestructible..CTankObject (nine records).
- weapon, attachobject, thrown_obj, sphere, fiber, hierobject, light, thrownbullet,
  animplayer: all 600-2500 lines, most with fragments on private CVector3/CMatrix views
  (triage: weapon 2/4 fragments own a vector view, sphere 2/6, fiber 5/9, hierobject 3/9,
  thrownbullet 4/8). Same pattern as Tank_object: put the class on the shared headers first.
- smixfx: MIX_getwetbuffer (0x801611d0) is inside smixfx.c's .text range but sits in the
  smixfram unit; moving it is a separate change to smixfram.
