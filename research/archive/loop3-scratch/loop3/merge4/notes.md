# merge4 (2026-11-13): whole-file merges of vtable owners, continued from merge3

Tools here (merge3's plus): land3.sh now saves mrg.sh's full output to last_mrg.log and also
reverts tools/ on failure; mrg.sh prints 8 lines of one_unit. vtchk.py OBJ __vt__X compares a
compiled vtable's slot relocations with the original's (run it before landing: a missing
override shows up only as a port_unit "Compiled code refers to X where the original refers to
Y"). mt.py OBJ FILE IDX DUPFILE runs port_unit.match read-only (section anchors).
hier_static.inc: CStaticObject : IMovingSceneNode on the shared headers, every override and new
virtual in __vt__13CStaticObject order, the weak inline bodies from the units that keep them,
members by offset (mesh +0x30, matrix +0x40, script +0x118, collision id +0x1dc, flag byte
+0x1e0 with a bool draw bit, light +0x24c, size 0x280). CAttachableObject (slots +0x280..0x580)
and CHierObject are in src/attachobject.cpp and tank_fwd.cpp.

Landed: Tank_object 74d43ce2 (with a port_unit fix), attachobject 15476542, sphere 518301a8,
fiber b68a92a4; source_link 0e680648: 66 undefined, 109 fragment-local, 50 duplicate.

Tricks learned
- port_unit: a weak duplicate with an exception table (CHierObject::Init, CStaticObject::
  TriggerScriptEvent) put its extabindex entry first; that entry's relocation pinned extab to
  the previous file's records (0x38 too low). Fixed in tools/port_unit.py (74d43ce2): the
  records of dropped code (foreign_records) are dropped spans in match().
- The weak copies of a class's inline virtuals come out in the order the class DECLARES them
  (overrides of an inherited slot can be declared anywhere): attachobject needed IsDrawEnabled
  declared after AsStaticObject.
- A vtable's GetWorldBoundingVolume__13CStaticObject... contains "10ISceneNode" in its
  parameter mangling: don't filter vt.py output on class names, use vtchk.py.
- Volume files (sphere, fiber): a file-private CVector3 = the shared header's form (copy
  through an 8-aligned base) but with the implicit assignment. Pair-copying copy constructor
  breaks GetCenter (lfd/stfd interleaved), an implicit one breaks the line tests, the shared
  pair-cast operator= breaks SetCenter.
- -inline auto,deferred + reversed source puts the RTTI name string (<= 8 bytes, .sdata2)
  before the float constants, as the original (sphere, fiber).
- fiber's visibility box: vector members as plain 16-aligned records (CVector3Base), else a
  CVector3 member array adds __construct_array and loses the 16-byte stack alignment.

Not done (next agent)
- weapon (4 fragments): weapon_bullet and weapon_head use a private CVector3 (union d) and
  CMatrix (float m[4][4], GetRow by reference) plus placeholder ISceneNode/CHierObject/CBullet
  views; weapon.cpp a CStaticObject view. Needs the hier_static.inc chain with a file-private
  CVector3/CMatrix, i.e. a private copy of ISceneNode/IMovingSceneNode (the shared ISceneNode.h
  includes CMatrix.h). Not attempted.
- thrown_obj (5): its CThrownObject view reuses CStaticObject's offsets under other names
  (m_owner at +0x1dc = the collision id, bounds at +0x0c inside ISceneNode's data) and a
  12-byte VECTOR3VIEW CVector3. Not attempted.
- hierobject (9), light (11; three vtables, __sinit, .bss statics), thrownbullet (8),
  animplayer (5), anim_obj, alarm, pumhandler, world_object: not attempted (time).
