# tool1 (port_unit kept_span fix) - done 2026-11-12, 9d94f2c9
- regress.py UNIT...: redraft from committed manifest args into scratch config, diff extab/extabindex/stripped fields (--tools DIR picks a port_unit copy).
- sweep.py units.txt: old vs new port_unit over the 158 stripping GC units with extab; results sweep.out.
- Remaining differences (equivalent kept bytes): scene_cmppairs, impact_dtor, AIObject_visionseek list a trailing dropped record in stripped_objects; the tool trims it.
- attachobject_ctor, matstack: regeneration from manifest args fails before kept_span (old and new alike).
