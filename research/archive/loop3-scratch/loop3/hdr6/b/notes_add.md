
### CScene re-run on the containers (2026-11-13, hdr6)

CScene.h now types its containers (dwi::fast_vec<CLight*>/<CCharacterShadow*>/<UNodePair>/
<SExclusionEntry>/<SModCommand>, dwi::vector<CCollision>, dwi::list<ISceneNode::SExtent> x2) and
defines SExclusionEntry and SModCommand; ISceneNode.h gained the nested SExtent (16 bytes, with
an empty default constructor, as the matched list views have it). CScene.h does not include
DwiContainers.h itself: the typed members are under `#ifdef GAME_DWICONTAINERS_H` (byte ranges
otherwise), because scene_fastvec_pairs and scene_fastvec_commands include CScene.h for the element
types and place the fast_vec destructor with their own fast_vec view (`template class`, see the dwi
section; MWCC 1.3.2 rejects an explicit instantiation of the destructor alone). The extent lists
also need GAME_ISCENENODE_H. All 74 units already on CScene.h and the 68 other ISceneNode.h users
verify unchanged with the new headers.

try_header.py: the trial header directory now goes before the unit's own include dirs (units that
already had `include` were compiled against the committed header, not the draft); CScene's offset
names extend to the containers (+0x50..+0x130).

Result: 19 units still with their own CScene view (1 control failure). **2 moved**: scene and
objcreate_nonvis. Not moved (code changes): scene_cmppairs (SExclusionEntry::Matches), scene_visit
(FindExclusion), scene_update (ProcessCommands), scene_fast and scene_query (InvolvesBullet),
scene_now (m_players as ISceneNode*, inline Link/Unlink/IsNodeInScene), scene_reset (m_world[i]),
scene_dtor (its constructor names 20 fields the header doesn't have), scene_rmextents (push_back
returning T*, m_extentNodes); the 5 GetPlayer-as-ISceneNode units, bsbifunc_explosion/explosion_dtor
(VisitAllNodesInVolume with their own ISceneNode view) and proxtrig_messages (Acquire), as before.
The ISceneNode guard stays: 48 of the 74 CScene.h files (and those two) do not include ISceneNode.h, most because they have their own view of it.
