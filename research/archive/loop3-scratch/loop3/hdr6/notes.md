# hdr6 (dwi container headers)

Landed, pushed: 156ad714 DwiContainers.h + DwiHashMap.h, 54 units; 7c29fadf CScene.h typed
containers (guarded on GAME_DWICONTAINERS_H), ISceneNode::SExtent, scene + objcreate_nonvis.
Tools here: try_dwi.py (committed as research/tools/try_dwi.py), redraft.py/relabel.py (hdr2/hdr3
copies taking HDR=scratch include dir), apply.py, b/recheck.py (verify units with a draft header dir
first), b/try_header.py (now = committed). Tables: dwi.tsv, b/CScene.tsv.
Not moved: see research/notes/shared-headers.md (dwi containers; CScene re-run).
Lock note: merge1 re-acquired the lock instantly after each release; a 0.05 s mkdir poll won.
