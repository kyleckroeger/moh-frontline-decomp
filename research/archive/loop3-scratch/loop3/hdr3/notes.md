# hdr3 (header re-moves after the CVector3 base form, d8cf803c)

Landed (complete image identical, 1,261,812 bytes unchanged):
- 8c84b851 CMatrix: 31 units (try_header, sources only).
- bb9ad8be ISceneNode: 8 units (4 plain, 4 weak copies listed via redraft.py).
- 0253a542 CVector3: 4 units (hierobject, skybox, static_obj_dtor, staticobjfactory_lookup), stripped labels relabelled.
- 7f0aa96a CMatrix: hierobject, skybox, static_obj_dtor (relabelled).
- 9da917fa ISceneNode: staticobjfactory_lookup (relabelled), static_obj_dtor (redraft).

Tools here: relabel.py CLASS UNIT... (RELABEL_WORK=try dir from try_header: matches stripped_objects
labels control->moved by section/size/bytes/relocations, verifies, writes try/<u>/unit.json);
apply.py TRYDIR UNIT... (copy moved source, add include); mergetsv.py OLD NEW NOTE.
Results: out1 (CMatrix), out2 (ISceneNode), out3 (CVector3), rd1/rd2.tsv (redraft).

Left:
- CVector3: 0 kept as is of 110. 9 function-set losses are code-level (port_unit draft says the
  calling functions differ: default ctor / Normalize inlined differently); AIFilter_linechecks,
  hierobject_tree, sound still differ after relabelling; 33 no_compile.
- CMatrix: 54 still own CVector3; quaternion_matrix (own sqrtf vs msl_sqrtf.h), CoverPoint_modified
  (out-of-line CVector3 member via pair cast), matrix_ps (own Row helper), AISoldierDoodad/
  Soldier_object_cleanup (invented helpers), player_ctor (alignment).
- ISceneNode: 37 own CVector3 + 1 own CMatrix; skybox .rodata pool; bsbifunc_attach and
  light_propertyanim_dtor .text after weak copies; collisionvolume_ctor held (test).
