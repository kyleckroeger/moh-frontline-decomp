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
