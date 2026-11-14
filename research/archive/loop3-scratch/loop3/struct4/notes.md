# struct4 (structural matching, continuing struct3), 2026-11-12
Tools: tools/asm.py <tmpl> <src> <func> [from to] (our asm with indices), tools/rdiff.py <tmpl> <src> <func> <addr> <size>
(structure-aligned diff that also shows register-only differences). Per-function dirs dno/ tlt/ with s.sh, var.py.

## LANDED
- DetermineNewObjectivePosition 0x80053738 (1276) -> ObstacleAvoidance.cpp prepended, fc2de588. port_unit needed no hand trims.

## Parked / WIP
- TLT_LoadFromRepository (572, toilet.cpp, would be a new fragment at 0x8001cca8 before TLT_LoadLevelContentsFile; block_orphans: no
  orphans): tlt/TLT_BEST.cpp fast 22, structural 0 (the 2 sdiff lines are the r2 "" / "%s" relocations). Use template tlt/td.json
  (toilet's -inline deferred,auto; t.json gives the same). Keys: the body is the file's own TLT_LoadFileFromLevelBigFile (copied from
  toilet_files.cpp: strip block, LoadFromBig, TLT_LoadFileNormal path[256]) inlined, then a named 'iterator it = g_Repository.find(data);
  *cached = it != g_Repository.end();' (the direct compare hoists end() and the loads) and PathNodeList_disable's hash_map find/insert
  bodies ('++p == m_data + m_size', 'return end()'). Left: find's else path colours data/last/p r8/r9/r7 vs orig r7/r8/r9 (count==0
  end in r9). Inert: find statement order, &m_data[i], index local, const K&, iterator class, end() spellings, caller forms
  (p2/p4), (long)/(K) -1 tests.
- GetClosestWaypointByType (784): gcw/GCW_BEST.cpp fast 245 -> 240, structural 17 -> 9. The float-wise copy into memory is SOLVED:
  vec1's alias-free pin ('float& operator[](int i) { return ((float*)this)[i]; }' on CAIFilterRealPosition, used once as
  'point[2] - position.z') + member-wise copy in the order y, x, z ('point.y = ...; point.x = ...; point.z = ...;') gives the orig's
  lfs z,y,x / stfs y,x,z exactly. Left: the unfolded initial index 0 (orig 'li r31,0; slwi r6,r31,1; cmpw r31,r8' and the first
  find's result in r3 ('li r3,0' on both return-0 paths); ours folds to 'li r6,0; cmpwi r8,0; ble'). Inert: 'int index; index = 0;',
  a FirstWaypoint inline that zeroes index, int* index, while-form first loop, non-inline/static FindWaypoint (auto-inlined
  identically), unsigned int/long index; single-call-site loops (w1/w3) lose the second inline copy; short types worse.
- CScene ctor: fast_vec ctor body/default-arg growth, list_node(const T& = T()) default, by-value list_node, list m_owned in body:
  all inert (162/15); CScene plain members assigned in the body: worse (186/49). Not worth more without a new idea.

Session end 2026-11-12: 1 landing (fc2de588, 1276 B). Nothing claimed, no permuter run, lock released.
YUV_WriteRow not attempted (struct2 + lane-parks history: scheduling of group 2, permuter junk).
