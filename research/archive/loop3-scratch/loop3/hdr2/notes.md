# hdr2 (shared headers follow-up, secondary work)

Landed (all pushed, complete image identical, 1,261,812 bytes unchanged):
- b9bf12de ISceneNode: 52 units, weak copies listed as weak_duplicates (redraft.py).
- ac0e8fe7 CMatrix: animmorph, animskinned, dmesh, stripped_objects labels renumbered.
- 4e927077 ISceneNode: compartment_ctor, labels renumbered + weak copies listed.

Tool: redraft.py CLASS UNIT... = try_header move + port_unit.draft (CONFIG redirected) with
--weak-duplicate for every new weak function; merged config = old manifest + draft's
weak_duplicates/strip_unused/stripped_sections/sections/include_dirs, externals from draft
(+ old hand-resolved ones the draft can't resolve). Output rd/<unit>/unit.json, src/.
reltargets.py OBJ LABEL... compares relocation targets of labels.

Left:
- collisionvolume_ctor: verifies with weak copies listed (rd/collisionvolume_ctor), but
  tests/test_data_strip.py pins its stripped_sections == ['.data']; needs the test updated.
- 8 .text differs: AIFilter_queries Tank_object_update compartment_render explosion_dtor
  objcreate_german player_bullet player_draw thrown_obj_cleanup.
- light_animmanager, light_propertyanim_update: non-weak __dt__18CPropertyAnimLightFv.
- decal_dtor: .data differs even with labels + weak copies.
- player_ctor (8-aligned matrices), AISoldierDoodad (GetTranslation), Soldier_object_cleanup
  (SetAxes, SetTranslation): untouched per ruling.
