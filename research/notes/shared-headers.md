# Shared class headers: what each move kept and lost (PortReadiness step 3.3)

`research/tools/try_header.py CLASS` moves every unit with its own view of CLASS onto
`include/game/CLASS.h` in a copy and verifies it as `one_unit.py` does. Tables:
`research/layout/header_moves/CLASS.tsv`. CVector3 (2026-11-11): see docs/PortReadiness.md.

## CMatrix (2026-11-12, agent hdr1)

`include/game/CMatrix.h`. Evidence used:

- Original out-of-line members (`nm`, matrix.cpp 0x8008ebf0..0x80090bb0): EndianSwap,
  GetGAMECUBEMatrix34, TibToMOHFL, ToEulerXZY/XYZ, Ident, Orthonormalize, Transpose,
  Inverse(const CMatrix&), FastInverse, TransformToLocal, TransformVector, TransformPoint,
  Transform(CVector4&, CVector3), Orthographic, Perspective, Multiply, Scale (float and
  CVector3), RotateX/Y/Z (long and float), Rotate, Translate, PreTranslate, SetPos/Up/Front/
  Right, BuildTrans, BuildRotZ/X, BuildRot, BuildScale (vector and float), `__as__`,
  InitClass; statics s_TempMat (.bss, 16-aligned) and s_ClassInit (.sbss).
- Original weak copies: only `__defctor__7CMatrixFv` (0x80095b40: `if (!s_ClassInit)
  InitClass();`, so the default constructor takes a defaulted argument) and
  `PostMultiply(const CMatrix&)` (0x8009b91c: `Multiply(*this, m)`).
- Rows named after SetRight/SetFront/SetUp/SetPos at +0/+0x10/+0x20/+0x30, the offsets every
  naming view agrees on (`research/layout/classes.tsv`). The views spell the storage
  `float m[4][4]` (74), rows right/forward/up/position (34), right/front/up/pos (17),
  `float m[16]` (5), m_right.. (5), and five odd ones. Moving a file rewrites `m[i][j]` with
  constant indices to `row.x..w`, `m[i]` to `&row.x`, any other `m` to `((float(*)[4])&right)`
  (`float*` for `m[16]` views), and forward/position/m_* to the header's names.
- Inferred, marked in the header: the constructor argument (int, Ident() when non-zero), the
  copy constructor through operator=, the GetRight/GetFront/GetUp/GetPos/GetRow getters
  (by value, the most common form; no weak copies) and the in-place `Inverse()`.

Result: 148 units with their own view. **53 kept** (moved in the landing commit), 4 lost,
90 didn't compile, 1 control failure.

- 87 didn't compile because the unit still has its own CVector3 view (or its own sqrtf):
  CMatrix.h includes game/CVector3.h, and those units lost their match on the CVector3 move.
  They keep both views until their CVector3 view is reconciled.
- `AISoldierDoodad` (GetTranslation) and `Soldier_object_cleanup` (SetAxes, SetTranslation):
  invented inline helpers the header doesn't have; the code would have to change.
- `hierobject_tree`: has its own CVector3 view.
- `animmorph`, `animskinned`, `dmesh` (lost, "Stripped object is not one compiler object"):
  the header's inline functions shift the compiler's anonymous label numbers (@18 -> @38),
  and the unit config names its stripped extab objects by number. **Moved 2026-11-12 (hdr2):**
  their `stripped_objects` renamed to the header compile's labels (animmorph and animskinned
  @18/@19 -> @38/@39, dmesh @281/@282 -> @291/@292), each confirmed the same object by its
  relocations (the extabindex entry points at the same discarded destructor, __dt__9CAnimatedFv
  or __dt__10CRenderBinFv, and at the renamed extab entry); `include` added to include_dirs.
- `player_ctor` (lost, .text differs): its CPlayerObject view places matrices at 8-aligned
  offsets (m_matrix518 ...); with the 16-aligned header they move by 8. Without the
  alignment player_ctor keeps its match but 13 others lose theirs (the original realigns
  the stack for local CMatrix variables and 16-aligns them in sections), so the alignment
  stays and CPlayerObject's layout is the open question.
- `attachobject_ctor`: control failure (doesn't verify unchanged in this harness).

**Re-run 2026-11-12 (hdr3, after the CVector3 base form d8cf803c):** 92 units still had their own
view. 31 kept and moved (no config change: all had `include` already). 54 still have their own
CVector3 view; 5 other no_compile: AISoldierDoodad and Soldier_object_cleanup (invented helpers),
quaternion_matrix (its own sqrtf clashes with msl_sqrtf.h), CoverPoint_modified (defines a CVector3
member out of line through a pair cast), matrix_ps (`CVector3 forward = Row(1)`: an own `Row`
getter); player_ctor lost (alignment, above), attachobject_ctor control_fail.

Variants tried: no alignment (40 kept of the 61 compilable), `bool` vs `int` constructor
argument (same result on the units it touches).

## ISceneNode (2026-11-12, agent hdr1)

`include/game/ISceneNode.h`, with its bases. Evidence:

- `__RTTI__10ISceneNode` lists IDestructible, ISubject and IObserver, all at offset 0, so the
  chain is ISceneNode : IObserver : ISubject : IDestructible. The vtables of the bases
  (5 words each) give their three virtuals: MarkForDestruction(int) (IDestructible's and
  ISubject's override, both out of line), the destructor (~IDestructible weak, ~ISubject
  and ~IObserver out of line) and Destroy (weak, empty). The bases' destructors store only
  the vtable pointer: no data. ISceneNode is 0x24 bytes, so +4..+0x20 are its own.
- `__vt__10ISceneNode` (0x80181c78): after the three base slots, 52 slots in the order the
  header declares them (AsMovingNode right after HandleBulletCollision, each As* non-const
  before const). Every ISceneNode virtual and the destructor are weak in the original
  (0x8001ada0..0x8001b064), so they are inline in the header, with the bodies of
  src/game/Moh2.cpp, the matched unit that emits those weak copies.
- Inferred: the pointer return types (Moh2.cpp's; the real `CAIDoodad` and `BSObject`
  names are in other mangled names), `bool` for IsVisible/IsDrawEnabled (the derived views
  override with bool), and the member names data04..data20 (the matched constructor views'
  names; which of them are the scene's list links is not resolved).

Result: 179 units with their own view. 54 weren't tried: they still have their own CVector3
or CMatrix view, which the header's include of game/CMatrix.h redefines. Of the 125 tried,
**12 kept** (moved in the landing commit), 74 lost, 39 didn't compile.

- 63 lost with "unexpected compiler function set": MWCC 1.3.2 emits a weak out-of-line copy
  of an inline virtual in every unit that calls it (AIFilter_queries gets
  GetForward/GetPosition just for calling them through an ISceneNode*). The original's
  objects had those copies too and the linker kept one (Moh2.cpp's), so the units list them.
  **Follow-up (2026-11-12, agent hdr2, maintainer's ruling: list them):** each unit's config
  names the copies in `weak_duplicates` (externals -> the original's retained weak copy),
  which turns on `strip_unused` where it was off, as port_unit does for any dropped function;
  sections/stripped_sections come from port_unit's own draft (`--weak-duplicate`), every other
  field stays as it was (objcreate_collvol/nonvis keep their hand-resolved local RTTI
  externals). 30 units also gained `include` in include_dirs. Tool:
  scratch/loop3/hdr2/redraft.py. **53 of the 63 kept; 52 moved**; the rest:
  - collisionvolume_ctor verifies but is held back: tests/test_data_strip.py uses it as the
    weak-vtable fixture and pins its stripped_sections to [.data]; the copies add .sdata2.
    Moving it needs that assertion updated (a test change, left to the maintainer).
    Moved 2026-11-12 (q17 accepted, agent unlockA): the assertion now requires `.data` and allows
    `.sdata2` beside it.
  - 8 .text differs (real code differences, not the weak copies): AIFilter_queries,
    Tank_object_update, compartment_render, explosion_dtor, objcreate_german, player_bullet,
    player_draw, thrown_obj_cleanup.
  - 2 emit a non-weak `__dt__18CPropertyAnimLightFv` with the header (light_animmanager,
    light_propertyanim_update): the derived destructor is no longer implicit; not resolved.
- 2 lost to label renumbering of stripped objects (as for CMatrix); 9 others lost with
  bytes or sections differing (mainloop_game: .text). **Follow-up (hdr2):** compartment_ctor
  moved with `stripped_objects` @69/@70 -> @83/@84 (the extabindex entry still points at
  __dt__10ISceneNodeFv and its extab entry) plus @87/@88, the extab pair of the new
  ~IDestructible weak copy, and its 54 weak copies listed (it constructs an ISceneNode, so
  it emits every inline virtual). decal_dtor, with the same treatment, differs in .data.
- **Re-run 2026-11-12 (hdr3, after the CVector3 base form and the CMatrix re-run):** 114 units
  still had their own view. 4 kept and moved (animplayer, animplayer_draw, decal_reset,
  objcreate_swap); 4 more with their weak copies listed (redraft.py: anim_obj_bullet,
  hearingvolume_rows, hierobject_constrain, particlesystem_visible). 37 still have their own
  CVector3 view, 1 its own CMatrix view, 43 other no_compile (the groups below). Still lost:
  the 8 .text differs above plus bsbifunc_attach and light_propertyanim_dtor (.text, after the
  weak copies), hierobject_draw, mainloop_game, objcreate_destroy, particlesystem_beginupdate,
  thrown_obj_ctor/dtor/init (.text), anim_obj (compiler dependencies), bsbifunc_explosion and
  particlesystem (.sdata2 pool), decal_dtor, the two CPropertyAnimLight destructors and
  collisionvolume_ctor (held, test; moved later, see above).
- 39 didn't compile: 15 use `AIDoodadView` members through GetAIDoodad() (the real class is
  CAIDoodad; renaming the view would change code), 7 BSObject/BSObjectView the same way,
  4 the scene's `SExtent` nested in their own ISceneNode view, 5 name the data members
  (m_link8, m_prev/m_next, m_sceneNext), 2 their own sqrtf, the rest single cases.

## CVector3 re-run (2026-11-12, agent hdr3)

try_header.py CVector3 over the 110 units still on their own view after the base form (d8cf803c):
none kept as is. 4 kept after renaming their stripped extab/extabindex labels (the header's
inlines renumber them): hierobject, skybox, static_obj_dtor, staticobjfactory_lookup, each label
matched to the moved compile's object with the same section, size, bytes and relocation targets
(scratch/loop3/hdr3/relabel.py), then verified; moved. The other 3 with renumbered labels
(AIFilter_linechecks, hierobject_tree, sound) still differ after the rename. The 9 "unexpected
function set" units define or call the original's weak copies (default constructor, Normalize,
...) in a way the inline header changes (port_unit's draft: the calling functions differ; 
particlesystem_active gains a non-weak __sinit), so listing weak copies doesn't help. The other
lost units differ in .text or sections (code-level differences), 33 don't compile (own methods
or spellings). Table: research/layout/header_moves/CVector3.tsv.
Follow-up: hierobject, skybox and static_obj_dtor (their CVector3 view now shared) also move onto
CMatrix.h, again after renaming their stripped labels (same matching); staticobjfactory_lookup
has no CMatrix view. On ISceneNode.h: staticobjfactory_lookup moves after renaming its stripped labels;
static_obj_dtor (it constructs an ISceneNode, as compartment_ctor) with its weak copies listed and its
labels from port_unit's draft (redraft.py), including the new ~IDestructible extab pair; skybox
loses (.rodata pool item size differs).

## CScreen (2026-11-13, agent hdr5)

`include/game/CScreen.h` (with CScreenFieldMode), plus `include/game/CColor.h` and
`include/game/CRect.h`, which it needs by value. Evidence:

- `__vt__7CScreen`: 22 slots in the header's order; slots 17 and 20 are 0 (pure), and
  `__vt__16CScreenFieldMode` names them Rebuild and SetDraw. Real names from the symbols:
  GetHeight (+0x24) and GetWidthScale (+0x30), which some views called GetY and GetScale.
- Every CScreen function is out of line in Screen.cpp except GetHeightScale/GetWidthScale,
  which are weak (inline in the header). No CScreen constructor symbol: it is inline.
- `g_screen` (0x110 bytes, Moh2.cpp) is a CScreenFieldMode: moved views' `extern CScreen
  g_screen;` become `extern CScreenFieldMode g_screen;` (CScreen is abstract). Calls on the
  object stay direct, as in the original.
- GetVSyncs returns `int` (mainloop.cpp converts it to float signed; Screen.cpp had
  `unsigned long`, which the moved definition now spells `int`, same code).
- Inferred, marked: return types, field names (Screen.cpp's), SetRect, CColor's and CRect's
  constructors (Screen.cpp's), the render mode record (the SDK's, used when the file doesn't
  include the SDK).

Tool changes (try_header.py): PROVIDES may now carry types the file uses before the class
view; with `EARLY_INCLUDE` the include goes where the first of them was. View rewrites:
fields m_scaleX/m_scaleY/m_scale/m_frontBuffer to the header's names, virtual calls renamed
by slot from `research/layout/vtables.tsv` (`_virtual_names`).

Result: 38 units with their own view. **29 kept** (26 as is, 3 with the weak
`GetWidthScale` copy listed in weak_duplicates: UserInterface_score, UserInterface_weaponname,
font_centeredf), all moved. Not moved:

- 5 still have their own CVector3 view (Moh2, TibText, camera_frustum, player_respawn, sound).
- UserInterface_init calls `CScreen::GetDepthBufferMax()` as a static (a member in Screen.cpp):
  code change.
- rcmp_main_delete (its own `_GXRenderModeObj` with other field names) and viewport (its
  `m_mode` is a CScreenMode view): their code names fields the header spells differently.
- sprite_draw: its CColor is a plain struct without constructors; with CColor.h's
  constructors `CSprite::Link` compiles differently.
Table: `research/layout/header_moves/CScreen.tsv`.

## CCamera (2026-11-13, agent hdr5)

`include/game/CCamera.h`, with `IMovingSceneNode.h` (its base) and `CPlane.h` (it holds ten).
Evidence:

- `__vt__7CCamera`: IMovingSceneNode's 71 slots and no new ones. `__vt__16IMovingSceneNode`:
  ISceneNode's 53 (AsMovingNode overridden) plus 18 new ones; every IMovingSceneNode virtual and
  its destructor are weak in the original (Moh2.cpp's copies), so they are inline with Moh2.cpp's
  bodies. IMovingSceneNode has no data (0x24 bytes).
- CCamera's overrides, UpdateWorldToCamera, SetPerspective, IsPointVisible and ASPECT_4x3 are
  out of line (camera_dtor, camera_rows, camera_frustum). The constructor is inline (no symbol).
  The header declares the destructor first, so the table is emitted with it.
- CPlane: CPlane(CVector3, float), Transform and GetIntersection out of line; the default
  constructor and destructor weak (inline, empty); 0x20 bytes, 16-aligned.
- Fields: +0x140 m_perspective (SetPerspective sets it), +0x141 cleared by every move and set by
  UpdateWorldToCamera, +0x142 m_projectionValid (cleared by SetPerspective), +0x150 the planes,
  +0x290 the script object (camera_dtor destroys it). Moh2.cpp named +0x141 m_perspective; the
  moved files are rewritten by offset (try_header.py `_offset_names`, from the layout lint's
  build/layout/views.json).

Result: 14 units with their own view. **2 kept and moved**: player_weapon_draw (as is) and
player_dtor (its 18 weak IMovingSceneNode copies listed in weak_duplicates). Not moved:

- camera_dtor: **table kept off on purpose** (defines ~CCamera but declares Draw first so the
  table, outside its fragment, isn't emitted; with the header it would be). Kept on its view.
- 5 still have their own CVector3 view (camera_frustum, player_beginupdate, player_commit,
  player_weapon_object_init; Moh2) and 4 their own ISceneNode view (anim_obj, bsbifunc_ai,
  mainloop_game, player_ctor), which the header's includes redefine.
- camera_rows: invented inline row getters (UpRow ...) and the ISceneNode words +0xc..+0x18
  used as `CameraFollowerView*`; box_visibility: its planes as `FrustumPlane`: code changes.
Table: `research/layout/header_moves/CCamera.tsv`.

## CInputManager (2026-11-13, agent hdr5)

`include/game/CInputManager.h` and `include/game/CDeviceManager.h` (CDeviceManager, CGCDevice,
the actuator record; devicemgr.cpp's view). No virtuals. Evidence:

- Out of line: Init (input_init), Update (a draft), Reset, VerifyAnalog, IsReady, SetFeedback,
  ResetActuatorState, GetValueAnyPlayer, GetValue, SetKeymap, Shutdown (input_manager); the
  destructor is weak (input.cpp, with g_inputMgr, 0x2450 bytes).
- **The device manager is a member, not a base.** input_init.cpp and input_manager.cpp view
  it as a base, input.cpp as a member at +0. The weak destructor destroys it with `li r4,-1`;
  as a base MWCC passes 0 (tried: input.cpp loses its match with the base form). The header
  keeps the destructor implicit: an explicit `~CInputManager() {}` renumbers impact_dtor's
  stripped labels.
- VerifyAnalog returns bool: Moh2.cpp, the only caller, tests it as a bool (Moh2 loses its
  match with int); input_manager.cpp's definition said int (same code for `return 1`).
- +0x244a m_inverted and +0x244b m_zooming[4] (one byte per player) are player_beginupdate.cpp's;
  bsbifunc_vibration.cpp's m_blockVibrations is m_feedbackOff (+0x2446), renamed by offset.

Result: 23 units with their own view, **20 kept and moved** (Moh2, input and mainloop_game among
them). Not moved, all code changes:
- Pause: calls `g_inputMgr.IsReady(controller)`, CDeviceManager's IsReady(int) through the
  manager (as a base); with the member it is `g_inputMgr.m_deviceMgr.IsReady(...)`, same code.
- input_init and input_manager: call CDeviceManager's members as a base's
  (`CDeviceManager::IsReady(i)`, `CDeviceManager::Shutdown()`), and input_manager reaches the
  key table through its own InputDeviceView/GetKeyTable helper; input_init clears
  +0x244a..+0x244e as one 5-byte array.
Table: `research/layout/header_moves/CInputManager.tsv`.

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

## CPlayerObject (2026-11-13, agent hdr5)

`include/game/CPlayerObject.h` (includes CCamera.h: the player holds its camera at +0x7d0).
Evidence:

- `__vt__13CPlayerObject`: IMovingSceneNode's 71 slots plus SetScript (+0x124). Real names for
  the views' renames: GetScriptObject (+0x60, "GetInventory" in two views), SetScript
  ("SetScriptObject" in three); moved files' calls are renamed.
- Weak in the original, so inline with the matched bodies: Destroy, both AsPlayerObject,
  GetWorldLinearVelocity, both GetAIDoodad, SetCollisionId, GetScriptObject. Every other member
  (the 49 non-virtual ones in the symbols) is out of line; constness from the mangled names
  (CanPlayerSwitchWeapon is not const).
- 0x12d0 bytes (mainloop.cpp's `new CPlayerObject`). Fields at the offsets the matched views
  agree on, named by the most common view; the five flag bytes +0x394..+0x398 as one-bit
  bitfields (most significant first, the views' semantic names). Moved files are rewritten by
  offset and by bit (`_offset_names`, OFFSET_BITS).
- Layout only (byte ranges, commented): the AI doodad (+0x24), the two boxes and two capsules,
  the contact list (dwi::fast_vec<CPlayerObject::CClsnInfo>), the hearing volume, the script
  object (a BSGO_Player) and the user interface (+0xa70..+0x1234).

Result: 103 units with their own view (1 control failure, attachobject_ctor). **41 kept and
moved**: 37 as is, bsbifunc_mgflag and proxtrig_messages with their weak IMovingSceneNode
copies listed, bsbifunc_playerammo with ISceneNode::TriggerScriptEvent dropped from its
weak_duplicates (CPlayerObject overrides it, so the unit no longer emits it), and
player_switchweapon. Not moved:

- 40 still have their own CVector3, CMatrix or ISceneNode view, which CCamera.h's includes
  redefine (Moh2, mainloop_game, player_ctor, most player_* and bsbifunc_* units ...).
- player_dtor: **table kept off on purpose** (it defines ~CPlayerObject; on the header its
  destructor compiles differently and the table would be emitted). Kept on its view.
- player and objcreate_playerscript define the weak copies themselves (`__declspec(weak)`
  out of line); with the header's inline bodies they lose (extab, .text). bsbifunc_random:
  compiler dependencies differ.
- 17 need code changes: fields used as other types (player_crouch and player_setweaponenable
  read m_weapons as CWeapon*/WeaponView*, player_constrain and player_fastvec use the contact
  list's members, player_init the doodad's, objcreate_collvol the script object's address as a
  BSGO view, player_lightvolume calls through m_weapons), GetScriptObject's result as a
  CInventory* (UserInterface_drawcompass, UserInterface_update), an int stored into
  m_collisionId (player_death), the flag byte +0x396 read whole (Pause), invented helpers
  (player_move PositionRow, player_rows UpRow, ShellMenu_levels GetTeam), a field the header
  doesn't have (pumhandler_loop m_hideMessages), m_soldier's members (bsbifunc_mpplayer), and
  GetCurrentWeapon's result as a CWeaponFlagsView* (player_weapon_object_head).
Table: `research/layout/header_moves/CPlayerObject.tsv`.

## dwi containers (2026-11-13, agent hdr6)

`include/game/DwiContainers.h` (list_node, list_iter, list, vec_iter, vec_const_iter, vector,
fast_vec, min/max) and `include/game/DwiHashMap.h` (hash_map and the prime table). There was no
container header in this repo before (Rising Sun has one for list_iter; its member names differ).
Evidence:

- Names from the original's weak copies and local template symbols: list/vector `reserve(int,
  char*)`, the list range constructor `list(int, list_iter, list_iter, char*)` (local 't', like
  dwi::copy), fast_vec `reserve(T*, int)` (0x800770a4, the views' SetStorage/UseBuffer),
  `fast_vec(int)`, `push_back(T)` by value, vector `erase(vec_iter)`, `vec_iter::operator+(int)
  const`, hash_map `erase(hash_map::iterator)`.
- dwi_prime_list is a local 27-word table with 25 copies in the original, many in files that never
  use a hash map, so it is a `static const` in the hash map's header, not in DwiContainers.h (every
  file including that would emit it). The 18 files that only carried the table now include
  DwiHashMap.h for it.
- Member names: the matched views agree almost everywhere (m_count/m_capacity/m_pool/m_free/m_head/
  m_owned for list; m_data/m_unknown4/m_size/m_capacity/m_owned/m_growth for fast_vec). Where views
  had different bodies for one member the header takes the most common (fast_vec::erase, list
  push_back, list clear); list::erase returns the next node (pumhandler_subtitles uses it; the other
  callers ignore it, same code). min/max take and return references (AISplinePath_segment's dwi::min);
  the invented Min/Max of cdbgeom_triangle_line, cdbgeom_volbox_ctor and scene_update are now
  dwi::min/dwi::max (same inline body).
- vector::reserve/erase are declared only: their bodies call dwi::copy, a local template the
  original emits in its own fragment (scene_copy, buddy_copy_*), so the placing units keep them.

Tool: research/tools/try_dwi.py (removes the covered templates, their out-of-class members the
header defines, the unit's placement new, min/max and the prime table from `namespace dwi` blocks,
includes the header where the first block was; verifies like try_header.py). Table:
`research/layout/header_moves/DwiContainers.tsv`.

Result: 114 units with a dwi view, the prime table or Min/Max. **54 moved**: 41 as is, 3 after
renaming stripped labels (cdbobject_csg/leaf/trigroups, relabel.py), 10 with weak copies listed or
labels from port_unit's draft (redraft.py: scene_cmppairs, scene_dtor, scene_fast, scene_query,
scene_reset, scene_update, pumhandler, pumhandler_subtitles, toilet,
PathNodeList_disable). The scene units among them are on the header's containers but still on
their own CScene view. Not moved:

- The weak-copy placing fragments (about 40: *_list, *_list_reserve, *_list_ctor, *_vec_*,
  *_fastvec*, compartment_weak, scene_veciter_*, publisher_erase, PathNodeList_erase/hashmap,
  toilet_hashmap, world_object_node_dtor): they emit their copies with `template class X<T>;`,
  which instantiates every member of the header (find needs ==, the list constructor needs T(),
  hash_map::erase a default V), or their declared-only members are inlined from the header
  (.text, weak duplicates no longer emitted). Moving them means changing how they instantiate.
- Code-level: alarm_register (AlarmInfo has no default constructor; the inline reserve needs one),
  scene_rmextents (push_back returning T*), world_volume_remove (a two-argument push_back),
  player_ctor (invented Allocate), buddy_destroy and alarm (a new __sinit), pumhandler_umt_dtor
  (emits the local range constructor), AIFilter (fast_vec(T*, int) with growth 0),
  world_volume_ctor (.text), world_object_dtor (the inline list reserve copies add .bss beyond its
  range), PathNodeList_closest and publisher_setsubject (their file's prime
  table is in another fragment; the header adds it).
