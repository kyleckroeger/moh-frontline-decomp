# hdr1 (shared headers, secondary work)

- CMatrix landed 8881a8a4 (53 units moved; table research/layout/header_moves/CMatrix.tsv,
  notes research/notes/shared-headers.md). Pushed.
- Tooling: scratch copy of try_header.py here = the committed one plus ISceneNode PROVIDES
  (bases + enum EClsnId removal). Run with TRY_HEADER_ROOT/DIR/WORK/OUT env (see log lines).
- ISceneNode: draft header include/game/ISceneNode.h here (hierarchy from RTTI, bodies from
  Moh2.cpp's matched weak copies). Eligible = 125 units with no own CVector3/CMatrix view
  (isn_elig.txt). Results: isn1/ISceneNode.tsv.
- ISceneNode: 12 kept, landing (see research/notes/shared-headers.md). Declaration-only
  variant in include_decl/ keeps 42 of the 63 weak-copy losses (isn2/). Open question there.
