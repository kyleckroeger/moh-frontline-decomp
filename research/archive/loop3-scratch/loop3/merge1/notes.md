# merge1 (2026-11-13): whole-file merges that own their vtables

complete_files.tsv: 80 original files whose functions are all verified but split into
fragment units (30 own a global vtable, 21 more have fragment-local symbols).

Landed (each "Merge X.cpp: N fragments into one unit"): AIPlayerDoodad 4295539d,
AIPlayerObject bb097d2e, AISoldierObject 1854ea4d, AITankDoodad 518faaf1, AITankObject 69d3895f,
AISoldierDoodad 97c1d83e, csg_volume 57181464, locatbig 4281c105, collisionvolume cdf1a650,
bullet d1575a57, memalloc 87adde24, printstr a40e4bed.

Recipe (scratch/loop3/merge1/): write src/<file>.cpp, check with chk.py (scores every
function of the old fragments, text order, section sizes), then under the lock merge.sh
<main> <tpl.json> <orig file> <index> <others...> -- <port_unit args>.
- RTTI on (game units were ported with -RTTI off where no table was emitted). RTTI records
  are LOCAL per file (__RTTI__ of every base in the chain), names in .rodata (<=8 bytes go
  to .sdata2), base lists in .data before the vtable.
- Key function: declare the destructor (or the first own out-of-line virtual) first.
- DEFERRED: when the file has other .data (switch tables) or several classes' RTTI, the
  original's order only comes out with -inline auto,deferred and the source written in
  REVERSE (rev.py). Non-deferred puts weak base vtables / jump tables first.
- Header inlines (ISceneNode, IMovingSceneNode) -> --weak-duplicate for every extra weak
  function and the weak __vt__ the compiler emits (chk.py's "extra text syms").
- '#pragma auto_inline off' around a static callee that the original calls (locatbig,
  printstr): one TU lets -inline auto inline it otherwise.
- tests/test_data_strip.py used collisionvolume_ctor as fixture -> now compartment_ctor.

Not done (candidates, in order): camera (frustum's 16-aligned CVector3 view vs header),
animskinned (two unrelated CAnimated views), hearingvolume (CBullet weak inlines), fader,
sprite, animated, animated_volume, Tank_object, weapon, attachobject, thrown_obj, sphere,
fiber, hierobject, light, thrownbullet, animplayer; FL: smixfx, sdspmix, stretch, animseq,
SearchNode, filesys(EA), filedev, dvdStatus, Gecko_ExceptionPPC, targimpl, UISUtils, UIStudio,
snddrv, bsmachin, animmot. hdr6 files (world_object, world_volume, anim_obj, collision,
bsutil, soundschedule, alarm, pumhandler, player, propdat) left alone.
