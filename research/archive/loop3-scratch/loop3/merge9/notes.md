# merge9 (2026-11-13): DwiContainers ~list through clear(), pumhandler merge, world_volume/world_object

Landed
- c7f196ff include/game/DwiContainers.h: ~list calls clear(). hv.py checked all 115 units that
  include game/Dwi*.h or game/CScene.h (units.txt; moved = incA/ prepended): 114 kept, scene_dtor
  relabelled @177/@178 -> @179/@180 (relabel.py logic: section, size, relocation targets).
- 9962fe5e pumhandler.cpp merged (12 fragments, 9,084 B, 16 functions). src/pumhandler.cpp, pw/
  (pre.cpp + funcs.cpp chunks, gen.py fwd|rev, try.sh NAME "INST"), tpl_pum.json (deferred,auto).
- source_link re-run (this landing's last commit; numbers in docs/PortReadiness.md).

Tools: hv.py (header check + relabel, 4 jobs), users.py, fd.py OBJ FUNC (diff vs original),
mrg.sh/land3.sh (dtor1's, lock name merge9), land_hdr.sh, land_pum.sh, wv/v.sh SRC [TPL] (order of
the four + scores), wv/lab.sh SRC TPL (extab label -> function in our object).

Tricks learned (also in research/notes/loop3-tricks-2026-11-12.md)
- Deferred inlining emission order: functions come out in reverse source order; a weak copy of an
  IN-CLASS inline member comes out right after the first function (in output order) that needs it
  out of line, followed by its own needs (pumhandler: DisplaySubtitle, reserve, ~list, range ctor,
  UpdateSubtitles). Out-of-class template members (and member templates defined out of class) go
  to the END of the file. Explicit 'template class X<T>;' places the non-template members at that
  point but not member templates.
- Label evidence: function statics and dtor-chain records (.bss 0xc) numbered below every extab
  label = deferred (pumhandler had no RTTI to show it).
- sinit with uninitialised floats in f31: QueuedSubtitle's colour set in its ctor BODY
  (m_color.r = 0 ...) instead of a CColor(0,0,0,128) initialiser.
- A word copy of a 4-byte CColor (lwz/stw to the stack, then lbz from the copy) = the colour passed
  BY VALUE (here to a small inline ShowSubtitle); 'CColor c = x;' with CColor.h copies member-wise.
- world_object reserve rank (r26/r25 vs r27/r26): split the range constructor into the inline
  helpers init_free() and link_range() (as pumhandler/alarm), with 'const T& value = *it;' before
  the placement new: reserve<CCompartment> and ~list<CCompartment> both exact (wo/h3.cpp).

Open
- world_volume: 27/27 exact (wv/c1.cpp, c2.cpp) but ~list must come out right after ~CWorldVolume.
  The original is deferred + RTTI (RTTI labels @364/@365 below extab @386; dtor1's tpl_wv_def.json).
  Original ~CWorldVolume INLINES ~list (no call), so by the emission rule above something in
  ~CWorldVolume must still request the out-of-line copy. Tried (all inert): explicit
  'template class' after ~CWorldVolume, ': m_subVolumes()' in the ctor, ~list out of class
  (plain/weak/inline, also after ~CWorldVolume), deferred+reversed (r2.cpp: TestCollision<X> then
  differ by 63 each, TCWSV inlined). Label counts per function in the original are ~3x ours
  (~CWorldVolume 89, ~list 85, ctor 19, reserve 113), suggesting a different inlining history.
- world_object: whole-file merge not written. The original is deferred + RTTI on (RTTI labels
  @964..@972 below extab @1067), like hearingvolume; 8 fragments (explosion 3.9 KB, ctor/dtor on
  ISceneNode). The list pair is solved (above). Next agent: build it like merge8's hearingvolume.
