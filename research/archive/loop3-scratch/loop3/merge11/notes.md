# merge11 (2026-11-13): world_object whole-file merge, world_volume ~list placement

Tools: land3.sh/mrg.sh (merge9's, lock name merge11), land_wo.sh. wo/: pre_head.cpp + views.inc +
classes.inc + funcs.cpp (chunks split at '//@@ name', forward = image order), gen.py OUT [fwd|rev]
[funcs], c.sh SRC (compile + score the 8 units), d.sh SRC FUNC... (diffs), var.py (list template
variants on classes_v1.inc), tpl_da.json (deferred,auto + RTTI on), wd.txt (weak-duplicate args).

Landed
- world_object (8 fragments, 8,020 B, 27 functions, __vt__12CWorldObject + RTTI + statics):
  4a6b4854 (pushed). source_link 396817a4: 54 undefined, 103 fragment-local, 47 duplicate.

Tricks
- One template body for two element types (reserve<SRequest> POD, reserve<CCompartment> with copy
  ctor): range ctor 'link_range(end(), first, last)' with 'head = end.m_node; stop = last.m_node;
  tail = head->m_prev;' and end() computed from this+16. head from &m_head was hoisted (SRequest:
  r0 instead of r7); tail from m_head.m_prev loaded frame-relative (CCompartment 128(r1) vs 16(r28)).
- A class that really derives from ISceneNode (CCompartment) makes 'x.~T()' a VIRTUAL call; the
  original's direct call comes from destroying the node ('node->~list_node<T>()'), whose implicit
  dtor destroys the member directly.
- A struct with an array member (SRequest's 'unsigned char m_reuse[4]') is not scalarised: Acquire
  builds it on the stack and copies it word by word; with a bool member it was scalarised (79 diffs).
- .bss order (zero-request temps before the dtor-chain records): global object definitions at the
  END of the source (deferred, written last to first).

Open
- world_volume: PARKED, needs a gap function (policy). With '-inline deferred' (no auto) + RTTI on
  and the source written last to first (merge9/wv/rc1.cpp) all 27 functions are exact, but the weak
  ~list comes out after the constructor. Adding any function between the constructor and
  ~CWorldVolume in the source that needs ~list out of line (wv/gap1.cpp: a Probe() with a local
  list and a call) gives the original order ~CWorldVolume, [Probe], ~list, ctor, reserve. The
  original's extab label gaps (~CWorldVolume +89, ~list +85, RemoveSubVolume(iter) +147, Add +101
  vs ours 17/10/53/58) fit functions the original linker dead-stripped between them, so the
  original file had more functions than the image keeps. Question for the user: may a reconstructed
  unit contain such a gap function (stripped at link, like port_unit --linker-stripped)?
