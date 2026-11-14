# dtor1 (2026-11-13): dwi ~list / DoWeOwnThisMemory local-order rule study
Rule (research/notes/loop3-tricks-2026-11-12.md, "An inline's locals: reversed declaration order"):
an expanded inline's locals are numbered in reverse declaration order relative to the out-of-line
compile; put the locals in an inline the function calls (clear(), OwnsElement) so both copies expand.
Landed: c96ab0db alarm.cpp (6 fragments), 0e5984f0 bsutil.cpp (4 fragments).
Open:
- include/game/DwiContainers.h ~list -> clear(): code-neutral for 105 units (vh.py, incA/) but
  renumbers @N labels; scene_dtor.json stripped_objects "@177" then fails reconstruct. Needs the
  manifest label updated in the same commit.
- world_volume: 27/27 exact with clear() (wv/c1.cpp; c2.cpp has reserve in-class -> ctor,reserve
  order right) but ~list must come right after ~CWorldVolume, before the ctor. Not solved.
- pumhandler: list pair exact with clear() (pw/pumhandler_lr.cpp); whole-file draft not written.
- world_object: CCompartment pair with clear(): ~list exact, reserve node/next one register low
  (pw/wov.py, wov2.py). Open.
Tools: pr.py + p/gen.py (probe), vh.py (header check over units.txt), bsu/var2.py, land3.sh/mrg.sh.
