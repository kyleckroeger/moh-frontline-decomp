
## CScene (2026-11-13, agent hdr5)

`include/game/CScene.h`. Evidence:

- No virtuals; g_scene (Moh2.cpp) is 0x148 bytes. Every member function is out of line (scene*
  units and two drafts); names and parameters from the symbols (the `*_now` functions take
  ISceneNode*; DetectSlowCollisions takes `float, float&`).
- Fields at the offsets every matched view and the layout lint agree on: the node list
  head/tail, four players, world, camera, the static/anim/light lists and their counts, the
  updating flag (+0x48), the player count (+0x4c), then the containers. Moved files are rewritten
  by offset (m_firstNode/m_node0 -> m_head, m_worldObject -> m_world, ...).
- The containers' types are in mangled names (dwi::list<ISceneNode::SExtent>,
  dwi::fast_vec<CScene::UNodePair/SExclusionEntry/SModCommand>), but no shared dwi header
  exists yet: they are byte ranges in the header, commented with their types (inferred
  layout only). UNodePair is defined (scene_fast.cpp's union); SExclusionEntry and SModCommand
  are declared only.
- GetPlayer returns CPlayerObject* (41 views; 9 others said ISceneNode* or IMovingSceneNode*).
  VisitAllNodesInVolume takes ISceneNode's nested EVolumeType, so the header declares it only
  when game/ISceneNode.h was included first (`#ifdef GAME_ISCENENODE_H`): the header doesn't
  include ISceneNode.h, so the 42 CScene units still on their own ISceneNode view can move.

Result: 94 units with their own view (1 control failure, attachobject_ctor). **74 kept and
moved**: 70 as is, 4 after renaming their stripped extab labels (AIFilter_linechecks,
impact_dtor, projectilebullet, sound: each label +1, matched by relabel.py). Not moved:

- 10 scene units (scene, scene_cmppairs, scene_dtor, scene_fast, scene_now, scene_query,
  scene_reset, scene_rmextents, scene_update, scene_visit) use the containers (or nested
  command/exclusion types) by member: they wait for a dwi container header. scene_reset's
  view also indexes +0x18 as an array (`m_world[i]`).
- bsbifunc_explosion, explosion_dtor: call VisitAllNodesInVolume with their own ISceneNode view.
- 6 use GetPlayer's result as an ISceneNode (bsbifunc_grenade, bsbifunc_setobjinfo,
  objcreate_collvol, objcreate_german, objcreate_nonvis, propdat_restart): the header's
  CPlayerObject* would need CPlayerObject's definition there.
- proxtrig_messages: an invented inline helper (`Acquire`).
Table: `research/layout/header_moves/CScene.tsv`.
