# Undefined symbols of the pure-source link: survey (2026-11-13, agent undef)

Start: 54 undefined (`research/layout/source_link.tsv`, `source_link.py --jobs 4`). End: 38
(36 vtables, `axDspSlave`, `axDspSlaveLength`). Commits 9d46be0d, 36657ee7, 29f602fe.

## Non-vtable names (16 -> 2)

| Name | Original | Why undefined | Done |
| --- | --- | --- | --- |
| `C_VECDotProduct`, `C_VECNormalize`, `PSVECCrossProduct`, `PSVECNormalize` | not in the original (it links no `vec.c`) | referenced only by `mtx.c`'s dead-stripped `C_MTXReflect`, `C_MTXRotAxisRad`, `C_MTXLookAt` | listed as `stripped` |
| `__CARDIsWritable` | not in the original | only `CARDFastDeleteAsync` (dead-stripped) calls it | `stripped` |
| `__double_max`, `nextafter` | not in the original | only ansi_fp's dead-stripped `__dec2num` uses them | `stripped` |
| `ShellMenuBlockItem`, `RcmpMainBlockItem`, `UserInterfaceBlockItem`, `BsbifuncBlockItem` | not in the original | the accepted strings-only block headers call them from a discarded function | `stripped` |
| `_eti_init_info` | `extabindex` 0x80016e44, 32 B | MWLD writes it (exception-table index), like `_rom_copy_info` | link-script name |
| `PATHI_choosebeat__FP12PATHNODEVIEW...` | `PATHI_choosebeat__FP12PATHFINDNODE...` (0x8010e254, unit path_choosebeat) | the `PATHI_eventstart` draft named its node view `PATHNODEVIEW` | view renamed `PATHFINDNODE`; code unchanged (160 lines). Its file hash changed, so the route_1_2 proof mark goes stale: re-prove |
| `Render__15CUcodeRenderBin` | not in the original | particlesystem's view gave `CUcodeRenderBin` a `Render`; the manager's table (0x801848d4) lists `CRenderBin::Render` | view now declares the real weak `Link` instead; unit VERIFIED |
| `axDspSlave` (.data 0x8018e2a0, 6,624 B), `axDspSlaveLength` (.sdata 0x8034e4e8, 2 B) | SDK DSP microcode for AXOut | data not in source: PortReadiness step 2.2 lists it as platform-owned (the port replaces it), not transcribed | **open: question** |

`stripped`: `source_link.py` now models MWLD's dead-stripping. A reference made only from a
unit's `discarded_functions` (found from the relocations) is listed under the new kind
`stripped` and not counted as undefined. The SDK's `__copy_rom_section`/`__init_*` references
from discarded code moved there too. The units that still reach them from kept code (OSAudioSystem,
OSError, OSSync, SISamplingRate, __start) stay fragment-local.

## Vtables (38 -> 36)

Every vtable's file has at least one open function. light.cpp (4 tables) and world_volume.cpp
(1) are complete but on hold (gap-function question), so they were skipped. In the remaining
files, the vtable's `.data` holds the RTTI base lists, the table and sometimes jump tables. A unit
can own those bytes when it compiles the key function with RTTI on and covers a contiguous run of
fragments with no open function inside. "Run" below means that run of fragments.

| File | Undefined tables | Open functions (lines) | Run with the key function | `.data` the run would own | Verdict |
| --- | --- | --- | --- | --- | --- |
| font.cpp | `CFont` | DrawTextCentered (8) | font_dtor, font | all (RTTI lists + table) | **merged 36657ee7** |
| animmorph.cpp | `CAnimMorph` | LLCharObjCompositeMorph (18) | animmorph_weight, _dtor, animmorph | all | **merged 29f602fe** |
| capsule.cpp | `CVolCapsule` | FindIntersection (266) | dispatch, capsule, dispatch2, create, dtor (5) | all (12 B list + 80 B table) | feasible; dispatch, dispatch2 and create still use private `CVector3` views, which must move onto the shared header first |
| cdbobject.cpp | `CCDBObject` | TestCDBBranchNodeCollision<CLine3> (1) | 9 tail fragments (fastvec_push .. leaf), key `~CCDBObject` | all | feasible but large; better to match the 1-line function and merge the whole file |
| cdbgeom.cpp | `CCDBVolBox` | TestCollision(CTriangle) (449) | volbox_line, fiber, volbox, volbox_ctor (key `TestCollision(CVolSphere)` in cdbgeom_volbox) | prefix 0x80183958..0x801839c8 (lists + table); the trailing jump table @2358 is cdbgeom_triangle_line's | feasible |
| box.cpp | `CVolBox` | GetExtents (37), TestCollision(CVolCapsule) (102), TestCollision(CVolSphere) (13) | ivolume, setters, create, dtor (4) | suffix 0x801894e8..0x80189588 (list, table, box_ivolume's 64 B); `remapCodes` stays box_visibility's | feasible |
| debug_prof.cpp | `CDbgStatsRenderBin` | PROFILE_UPDATE (82), CProfileInitClass ctor (no draft) | register, timers, stats, bins, cdbgstatsrenderbin_dtor, debug_prof (6) | all 112 B (debug_prof_register already owns the first 60) | feasible |
| static_obj.cpp | `CStaticObject` | OnCollision (3) | head run of 8 (type .. collisionid), key `MarkForDestruction` in static_obj_type | all (5 lists + 356 B table) | feasible, large (ISceneNode chain); or match OnCollision |
| staticmesh.cpp | `CStaticAnimMesh`, `CMSHMatBin`, `CMshSimpleBin` | DrawNode (5) | head run of 7 (head .. draw) | prefix 0x8018b400..0x8018b4f0 (`g_GCFogType` first; `__vt__11CStaticMesh` is staticmesh_ctor's) | feasible, large; or match DrawNode |
| player_weapon_object.cpp | `CPlayerWeaponObject` | AddAmo (2; ~40 forms tried) | 6 tail fragments (donereload .. player_weapon_object) | all: 6 RTTI lists, 82-slot table, UpdateWeaponTransforms' jump table; `.sdata` starts with `g_fNearMultiplier` (now in research/data/init) | feasible; needs CAnimObject's 82 virtuals in order (anim_obj has them) |
| AIFilter.cpp | 5 (`CAIFilterObject`, `...Tank`, `...Player`, `...Soldier`, `MechanicCollisionChecker`) | GetClosestWaypointByType (88), ChooseCoverPointByType (20) | head run of 16 (queries .. visit) holds every key | all 400 B (soldier_dtor owns 164 B inside) | feasible only as a 16-fragment merge |
| particlesystem.cpp | `CParticleSystem`, `CPropertyParticleSystem`, `CInstancedParticleSystem`, `CParticleSystemTextureBin` | RenderSystem (no draft), UpdateBoundingBox (56) | keys split: three in the middle run (rotation .. visible), `~CParticleSystem` after UpdateBoundingBox | one block with the manager table and a 112 B table @1024 ahead | blocked: needs UpdateBoundingBox |
| dmesh.cpp | `CDMRenderBin`, `CPart`, `CRemapBin`, `CPartBin`, `CFinishPartBin` (weak) | CPartBin::Link (4) | `CPartBin`'s key is before Link, the others' after; the tables are interleaved | no contiguous range | blocked: match CPartBin::Link, then merge the whole file |
| compartment.cpp | `CCPTMatBin`, `CCompartment`, `CCptShadowBin`, `CCptSimpleBin`, `CCptShadowCleanUpBin` | DrawNode (58), `__sinit_compartment_cpp` (no draft) | keys on both sides of DrawNode | one block | blocked |
| objcreate.cpp | `BSGO_Dummy` (weak, no key) | CreateDestructable (11), CreateRootHierObject (2), CreateDumpObject (56) | the weak table comes from the code that constructs a BSGO_Dummy (CreateDumpObject's area) | interleaved with units' data | blocked |
| player.cpp | `CPlayerObject` | SetFinishedLevelData (11), InitMultiplayer (multiplayer, declined form) | run of 21 fragments between the two | interleaved with player_crouch's and player_bullet's data | blocked (multiplayer last) |
| light.cpp | 4 | none | — | — | on hold |
| world_volume.cpp | `CWorldVolume` | none | — | — | on hold |

The same recipe works for both merges: RTTI on, the key function declared first, one member view per
class, `--pool .sdata2`, and weak or file duplicates for inline copies (see the commits).

## Questions

1. axDspSlave: transcribe the 6,624-byte DSP microcode into `research/data/` (a check_data
   file, like the built-in font) so the link has no undefined data, or keep it platform-owned
   and leave 2 undefined for good?
2. Minimal ownership: may a single key-function fragment (for example capsule_dtor or
   cdbobject_dtor) turn on RTTI and own its file's `.data` without merging its neighbours? It
   is byte-exact and resolves the table, but it is not a merge.
