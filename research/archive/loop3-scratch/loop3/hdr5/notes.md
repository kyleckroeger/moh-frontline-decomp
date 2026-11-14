# hdr5 (boot-path shared headers: CScreen, CCamera, CInputManager, CScene, CPlayerObject)

Landed (complete image identical, verified 1,281,040 bytes unchanged), all pushed:
- db01ee97 CScreen: 29 units (+ CColor.h, CRect.h).
- 4c5b6224 CCamera: 2 units (+ IMovingSceneNode.h, CPlane.h).
- 710cb78c CInputManager: 20 units (+ CDeviceManager.h; device manager is a member, not a base).
- 3eec12a4 CScene: 74 units (containers as byte ranges).
- 874c45dc CPlayerObject: 41 units.

Tools here (copies of the committed ones with scratch paths): try_header.py (committed version
= this with ROOT from __file__), redraft.py, relabel.py, apply.py. Env: TRY_HEADER_DIR=
scratch/loop3/hdr5/include, TRY_HEADER_WORK/OUT per class (try_*/out_*), REDRAFT_WORK, RELABEL_WORK.
New in try_header: EARLY_INCLUDE, OFFSET_NAMES/OFFSET_BITS + _offset_names (from
build/layout/views.json, the 11-12 lint), _virtual_names (slot renames from vtables.tsv).

Not moved (reasons in research/notes/shared-headers.md): mostly units still on their own
CVector3/CMatrix/ISceneNode view; table-kept-off: camera_dtor, player_dtor; scene units waiting
for a dwi container header; code-level differences listed per class.
