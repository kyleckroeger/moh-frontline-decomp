# Vtable order: the original's tables against the source's class views

PortReadiness steps 2.3 and 3.2 (agent vt1, 2026-11-13). A compiler regenerates a `__vt__`
symbol only from a class declaration whose virtuals come in the original's order, and a
view's virtual calls use the slots its own declaration order gives.

## The original's tables (`research/layout/vtables.tsv`)

`research/tools/vtables.py --tsv` writes one row per word of every `__vt__` symbol, from the
ELF's relocations: 113 tables (92 global, 21 weak), 2,951 words: 115 RTTI pointers, 115
offset words and 2,721 slots. Columns: vtable, class, address, bind, word offset, part,
role (`rtti`, `offset`, `slot`), slot index within the part, target symbol (or value) and
its demangled name (`dtk demangle`).

MWCC's layout, as the tables show it:

- A part is the RTTI pointer, the subobject's offset (0 for the primary part), then one
  word per virtual. Base slots come first, an override keeps its base's slot, and a new
  virtual is appended in declaration order.
- 30 slots are 0 with no relocation: pure virtuals (MWCC writes no `__pure_virtual_called`).
  CParticleSystem 18, `RCMP::CODEC` 5, `RCMP::VIDEO_DRIVER` 4, CScreen 2 (+0x4c, +0x58),
  `dwi::IVisitor<ISceneNode>` 1.
- Two classes have a second part. CParticleSystemManager is CRenderBin first and ISceneNode at
  +32 (offset word -32). CBulletDecalManager is ISceneNode first and CRenderBin at +36. The
  secondary part's overridden slots are thunks (`@32@Draw__22CParticleSystemManager...`).
  The class's own overrides of the secondary base's virtuals then get new slots after
  the last part, in declaration order. For CParticleSystemManager these are Draw,
  IsVisible, IsDrawEnabled and BeginUpdate. For CBulletDecalManager they are IsUsed and Link.
- `research/tools/vtables.py CLASS` prints one table with parts and slot numbers. `--json`
  keeps the old `{class: [every word]}` format.

## The views (`research/tools/vtable_order.py`)

MWCC 1.3.2's DWARF 1 has no member functions, so the tool reads the views as text. It
covers every class body in `src/` and `include/game/`, and the drafts with `--drafts`.
Quoted includes are followed. From each view it builds the whole table: the primary base's
table, then each secondary base's part, then new virtuals and overrides of secondary-base
virtuals. An overload declared in the same class is a new slot. The tool then compares
the view's table word by word with the original's. Kinds:

- `order`: a named virtual sits where the original has another method, and the original has
  that name at another slot.
- `rename`: the name isn't in the original's table. The view guessed a name for the right
  slot. The code is unaffected, but a header move must rename the calls.
- `placeholder`: an `unknown024`/`vf24`/`Function024` name labels a different offset from
  the one it gets. A few views count in decimal (`unknown012` = +12); those pass.
- `beyond`: the view declares more virtuals than the original's table has.
- `signature`: the right method, but a different constness or argument count.

Validation: the views of the multiple-inheritance managers in particlesystem.cpp (64 words)
and decal.cpp (62 words) reproduce the original's tables word for word, including the
appended overrides. `include/game/ISceneNode.h` reproduces all 53 slots of
`__vt__10ISceneNode`, and its IDestructible, ISubject and IObserver reproduce their 3 slots.

## Result (2026-11-13)

110 of the 113 classes have views: 1,173 views, of which 1,029 agree with the original in
every slot they declare. The 144 that differ:

| Kind (first by severity) | Views | Of them, the file says its table is kept from being emitted |
| --- | --- | --- |
| order | 102 | 52 |
| rename | 18 | 1 |
| signature only | 20 | 5 |
| beyond | 3 | 0 |
| placeholder | 1 | 0 |

**`include/game` headers: no disagreement.** ISceneNode.h (with its three bases) is the only
shared header with virtuals, and it matches. CVector3.h and CMatrix.h have no virtuals. So
there was nothing to fix there, and no `src/` view was changed. Every row below is a
verified unit's view. Changing one would change the unit's compile (a moved key function
emits the table, a renamed call changes nothing but has no evidence of its own), so they
are left for the header step, class by class.

What the differing views are:

1. **Key-function order (most `order` rows; 61 of them are a swap at +0x8).** MWCC emits a class's
   table in the unit that defines the first non-inline virtual the class declares. A
   fragment that defines the destructor but mustn't own the table declares some other virtual
   first (`virtual void ChooseTarget(float); virtual ~CAIFilterObject();`), so the table
   isn't emitted there. Many of these files say so in a comment. These views are right
   for their unit and wrong for a header. The same trick also works through an
   override declared first, which keeps every slot in place (camera_dtor.cpp declares
   `Draw` before `~CCamera`). That is why the tool can't see all of them: slot order
   doesn't show which override a class declares first.
2. **Reduced views.** A view declares only the virtuals its unit names, so later ones
   shift. light_animmanager.cpp's ISceneNode has MarkForDestruction and two AsLight, and
   says no virtual call is made. These are harmless where no virtual call goes through the
   shifted slots, and they are dropped on a header move.
3. **Mislabelled slots in matched code** (the rows to fix when the unit moves to a header):
   - thrown_obj_cleanup.cpp's ISceneNode/CStaticObject/CThrownObject call the slot at
     +0x84 `AsStaticObject`, +0x8c `AsWorldObject` and +0x9c `AsSoldierObject`. The
     original has AsWorldObject, AsAnimObject and AsPlayerObject there. The calls are
     right by offset, so these are wrong names.
   - Renames: CBullet +0x14c `Deflect`/`PassThrough` is `Penetrate`, CPlayerObject +0x60
     `GetInventory` is `GetScriptObject`, CPlayerObject +0x124 `SetScriptObject` is
     `SetScript`, CSoldierObject +0x134 `AttachWeapon` is `AttachObject`, CScreen +0x24
     `GetY` is `GetHeight` and +0x30 `GetScale` is `GetWidthScale`, CAnimated +0x20
     `SetSkeleton` is `BindSkeleton`, CCompartment +0x34 `GetCDBObject` is
     `GetLocalBoundingVolume`, CStaticObject +0x148 `GetOwnerNode` is `AsHierTankObject`,
     ISceneNode +0x6c `GetAnimController` is `AsMovingNode` (collision_head.cpp) and +0x94
     `GetOwner` is `AsSoldierObject` (thrownbullet_tail.cpp), and CInstancedParticleSystem
     +0x70 `GetVelocity` is `GetParticleAcceleration`.
   - player_bullet.cpp's CBullet placeholders `vf0e0..vf130` are each labelled 4 bytes
     high (vf0e0 sits at +0xdc). GetDamage and the slots after it are right.
   - Flattened bases: attachobject_ctor.cpp, attachobject_update.cpp and player_commit.cpp
     declare IMovingSceneNode's SetTMLocalToWorld/SetPosition inside their `ISceneNode`
     (the original has them at +0xe8/+0x108 of IMovingSceneNode). bullet_ctor.cpp's
     IMovingSceneNode declares none of its 18 virtuals, so CBullet's `Init` comes at +0xdc
     (the original has it at +0x124).
4. **Signature only.** `CRenderBin::Render()` and `CParticleSystem::Render()` are declared
   without arguments (the original takes `CDmaPacket&, void*`). `GetPosition`,
   `GetScriptObject` and `CBullet::GetVelocity` differ in constness. `MPC_CODEC_INTERNAL::Init`
   differs in its argument count.

With `--drafts` there are 1,321 views and 164 that differ. The drafts' rows aren't listed
here (agents g1b, g2 and g3 are matching drafts).

## What this means for the 67 undefined vtables (pure-source link)

The fresh link (docs/PortReadiness.md, step 2) leaves 67 `__vt__` symbols undefined: 64
global, plus the weak BSGO_Dummy, CFinishPartBin and CAnimLightManager. For every one of the
64 global tables, the method that would be the key function (the class's first
out-of-line slot that it owns) is defined by a verified unit or draft
(`scratch/loop3/vt1/vt_keys.tsv`). 21 of those units' views show a key-function swap
(group 1). The other 43 do it through an override declared first, as camera_dtor.cpp does.
The tables are absent on purpose. Each sits in its original file's `.data` outside the
fragment the unit verifies, and the unit's config resolves it as an external at its
original address (`"__vt__7CCamera": "0x8018460c"`).

So a correct header alone won't define them. Something has to emit each table, and there
are two ways:

- (a) the unit that owns the table's `.data` range declares the class in the original
  order. Today no unit owns those bytes.
- (b) a `research/data/` file per original file emits the tables. It would have to declare
  the class in original order and define the key function, which duplicates the unit's
  definition. So (b) needs either a merge of the file's fragments or a link rule that
  takes only `.data` from that object.

This is a policy question for the maintainer, not decided here. The 3 weak tables are
referenced by units that resolve them at their original address (BSGO_Dummy by
thrown_obj_ctor and others, CFinishPartBin by dmesh, CAnimLightManager by light). No source
emits the weak copy yet, because no view that constructs them declares the whole class. The units also define 2 tables the original doesn't have,
`__vt__16CScreenFrameMode` and `__vt__19CScreenFieldModePAL` (unit Screen, stripped in its
verification).

## All differing views

Generated by `vtable_order.py --md`. "Kept off" means the file says its table is not
emitted there.

| Class | File | Kinds | First difference (view at offset: original) | Kept off |
| --- | --- | --- | --- | --- |
| CAIFilterObject | `AIFilter_caifilterobject_dtor.cpp:8` | order 2 | `ChooseTarget` at +0x8: `__dt__15CAIFilterObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterObject | `AIFilter_player.cpp:181` | order 2 | `ChooseTarget` at +0x8: `__dt__15CAIFilterObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterObject | `AIFilter_player_ctor.cpp:181` | order 2 | `ChooseTarget` at +0x8: `__dt__15CAIFilterObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterObject | `AIFilter_player_update.cpp:184` | order 2 | `ChooseTarget` at +0x8: `__dt__15CAIFilterObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterObject | `AIFilter_soldier_dtor.cpp:141` | order 2 | `ChooseTarget` at +0x8: `__dt__15CAIFilterObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterObject | `AIFilter_tank_ctor.cpp:25` | order 2 | `ChooseTarget` at +0x8: `__dt__15CAIFilterObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterObject | `AIFilter_tank_dtor.cpp:194` | order 2 | `ChooseTarget` at +0x8: `__dt__15CAIFilterObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterObject | `AIFilter_tank_update.cpp:199` | order 2 | `ChooseTarget` at +0x8: `__dt__15CAIFilterObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterPlayerObject | `AIFilter_player.cpp:191` | order 3 | `ChooseTarget` at +0x8: `__dt__21CAIFilterPlayerObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterPlayerObject | `AIFilter_player_ctor.cpp:191` | order 3 | `ChooseTarget` at +0x8: `__dt__21CAIFilterPlayerObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterPlayerObject | `AIFilter_player_update.cpp:194` | order 3 | `ChooseTarget` at +0x8: `__dt__21CAIFilterPlayerObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterSoldierObject | `AIFilter_soldier_dtor.cpp:150` | order 2 | `ChooseTarget` at +0x8: `__dt__22CAIFilterSoldierObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterTankObject | `AIFilter_tank_ctor.cpp:35` | order 2 | `ChooseTarget` at +0x8: `__dt__19CAIFilterTankObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterTankObject | `AIFilter_tank_dtor.cpp:203` | order 3 | `ChooseTarget` at +0x8: `__dt__19CAIFilterTankObjectFv (ChooseTarget at +0xc)` | yes |
| CAIFilterTankObject | `AIFilter_tank_update.cpp:208` | order 3 | `ChooseTarget` at +0x8: `__dt__19CAIFilterTankObjectFv (ChooseTarget at +0xc)` | yes |
| CAnimMorph | `animmorph.cpp:49` | order 2 | `UpdateAnimation` at +0x8: `__dt__10CAnimMorphFv (UpdateAnimation at +0xc)` |  |
| CAnimMorph | `animmorph_dtor.cpp:103` | order 2 | `UpdateAnimation` at +0x8: `__dt__10CAnimMorphFv (UpdateAnimation at +0xc)` | yes |
| CAnimObject | `Soldier_object_ctor.cpp:37` | order 1 | `~` at +0x8: `MarkForDestruction__11CAnimObjectFi (~ at +0xc)` | yes |
| CAnimSkinned | `animskinned.cpp:48` | order 2 | `UpdateAnimation` at +0x8: `__dt__12CAnimSkinnedFv (UpdateAnimation at +0xc)` |  |
| CAnimSkinned | `animskinned_dtor.cpp:19` | order 2 | `UpdateAnimation` at +0x8: `__dt__12CAnimSkinnedFv (UpdateAnimation at +0xc)` | yes |
| CAnimated | `AnimObjFactory_head.cpp:92` | rename 1 | `SetSkeleton` at +0x20: `BindSkeleton__9CAnimatedFP10CharSkel_t` |  |
| CAnimated | `animated_dtor.cpp:8` | order 2 | `UpdateAnimation` at +0x8: `__dt__9CAnimatedFv (UpdateAnimation at +0xc)` | yes |
| CAnimated | `animmorph.cpp:34` | order 2 | `UpdateAnimation` at +0x8: `__dt__9CAnimatedFv (UpdateAnimation at +0xc)` |  |
| CAnimated | `animmorph_dtor.cpp:90` | order 2 | `UpdateAnimation` at +0x8: `__dt__9CAnimatedFv (UpdateAnimation at +0xc)` | yes |
| CAnimated | `animskinned.cpp:33` | order 2 | `UpdateAnimation` at +0x8: `__dt__9CAnimatedFv (UpdateAnimation at +0xc)` |  |
| CAnimated | `animskinned_dtor.cpp:13` | order 2 | `UpdateAnimation` at +0x8: `__dt__9CAnimatedFv (UpdateAnimation at +0xc)` | yes |
| CAnimatedPlayer | `animplayer_ctor.cpp:17` | order 2 | `~` at +0x8: `MarkForDestruction__11CAnimObjectFi (~ at +0xc)` | yes |
| CAttachableObject | `attachobject_type.cpp:16` | order 2 | `~` at +0x8: `MarkForDestruction__13CStaticObjectFi (~ at +0xc)` |  |
| CAttachableObject | `hierobject_tree.cpp:90` | order 1 | `BeginUpdate` at +0x8: `MarkForDestruction__13CStaticObjectFi (BeginUpdate at +0x14)` |  |
| CBullet | `bullet_ctor.cpp:40` | order 2 | `Init` at +0xdc: `Reset__16IMovingSceneNodeFv (Init at +0x124)` |  |
| CBullet | `player_bullet.cpp:155` | placeholder 21, rename 1, signature 2 | `vf0e0` at +0xdc: `Reset__16IMovingSceneNodeFv` |  |
| CBullet | `static_obj_setlight.cpp:128` | rename 1 | `Deflect` at +0x14c: `Penetrate__7CBulletFRC10CCollision` |  |
| CBullet | `world_object_explosion.cpp:190` | rename 1, signature 1 | `PassThrough` at +0x14c: `Penetrate__7CBulletFRC10CCollision` |  |
| CBullet | `animplayer_draw.cpp:92` | signature 1 | `GetVelocity(CVector3 &) const` at +0x144: `GetVelocity__7CBulletFR8CVector3` |  |
| CBulletDecalManager | `decal_link.cpp:130` | order 2 | `Link` at +0xf8: `IsUsed__19CBulletDecalManagerFv (Link at +0xf0)` | yes |
| CBulletDecalManager | `decal_reset.cpp:177` | order 2 | `Link` at +0xf8: `IsUsed__19CBulletDecalManagerFv (Link at +0xf0)` | yes |
| CCDBObject | `cdbobject_ctor.cpp:32` | order 1 | `TestCollision` at +0xc: `Create__7IVolumeCFv (TestCollision at +0x14)` |  |
| CCDBObject | `cdbobject_dtor.cpp:19` | order 1 | `TestCollision` at +0xc: `Create__7IVolumeCFv (TestCollision at +0x14)` | yes |
| CCDBObject | `compartment_ctor.cpp:49` | order 1 | `TestCollision` at +0xc: `Create__7IVolumeCFv (TestCollision at +0x14)` | yes |
| CCamera | `player_ctor.cpp:182` | order 1 | `Draw` at +0x10: `Destroy__13IDestructibleFv (Draw at +0x30)` | yes |
| CCamera | `player_dtor.cpp:96` | order 1 | `~` at +0x8: `MarkForDestruction__8ISubjectFi (~ at +0xc)` |  |
| CCharacterShadow | `compartment_intersect.cpp:66` | order 1 | `IsDrawEnabled` at +0x8: `__dt__16CCharacterShadowFv (IsDrawEnabled at +0x10)` |  |
| CCharacterShadow | `compartment_nodeshadows.cpp:75` | order 1 | `IsDrawEnabled` at +0x8: `__dt__16CCharacterShadowFv (IsDrawEnabled at +0x10)` |  |
| CCharacterShadow | `compartment_volbox.cpp:41` | order 1 | `IsDrawEnabled` at +0x8: `__dt__16CCharacterShadowFv (IsDrawEnabled at +0x10)` | yes |
| CCompartment | `world_object_explosion.cpp:288` | rename 1 | `GetCDBObject` at +0x34: `GetLocalBoundingVolume__12CCompartmentCFQ210ISceneNode11EVolumeType` |  |
| CCptShadowBin | `compartment_bindtors.cpp:40` | order 3 | `Render` at +0x8: `__dt__13CCptShadowBinFv (Render at +0xc)` | yes |
| CCptShadowCleanUpBin | `compartment_bindtors.cpp:34` | order 3 | `Render` at +0x8: `__dt__20CCptShadowCleanUpBinFv (Render at +0xc)` | yes |
| CCptSimpleBin | `compartment_bindtors.cpp:46` | order 3 | `Render` at +0x8: `__dt__13CCptSimpleBinFv (Render at +0xc)` | yes |
| CDMRenderBin | `dmesh_cdmrenderbin_dtor.cpp:27` | order 1 | `Link` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv (Link at +0x14)` | yes |
| CFaderBin | `fader_dtor.cpp:22` | order 1 | `Link` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv (Link at +0x14)` | yes |
| CFont | `font_render.cpp:127` | order 1 | `Link` at +0x10: `Init__10CRenderBinFv (Link at +0x14)` |  |
| CHearingVolume | `player_ctor.cpp:166` | order 1 | `~` at +0x8: `MarkForDestruction__8ISubjectFi (~ at +0xc)` | yes |
| CHearingVolume | `player_dtor.cpp:89` | order 1 | `~` at +0x8: `MarkForDestruction__8ISubjectFi (~ at +0xc)` |  |
| CHierObject | `hierobject_playback.cpp:213` | order 1 | `~` at +0x14c: `AttemptScriptMotionUpdate__11CHierObjectFf (~ at +0xc)` | yes |
| CHierObject | `hierobject_tree.cpp:116` | order 1 | `BeginUpdate` at +0x8: `MarkForDestruction__13CStaticObjectFi (BeginUpdate at +0x14)` |  |
| CInstancedAnimLight | `light_animmanager.cpp:61` | order 4 | `AsLight` at +0xc: `__dt__19CInstancedAnimLightFv (AsLight at +0xb4)` |  |
| CInstancedParticleSystem | `particlesystem_cinstancedparticlesystem_dtor.cpp:37` | order 3 | `DeActivate` at +0xc: `Render__23CPropertyParticleSystemFR10CDmaPacketPv (DeActivate at +0x1c)` | yes |
| CInstancedParticleSystem | `particlesystem_cpropertyparticlesystem_dtor.cpp:208` | order 2, rename 1 | `DeActivate` at +0xc: `Render__23CPropertyParticleSystemFR10CDmaPacketPv (DeActivate at +0x1c)` |  |
| CInstancedParticleSystem | `particlesystem_create.cpp:140` | order 2, rename 1 | `DeActivate` at +0xc: `Render__23CPropertyParticleSystemFR10CDmaPacketPv (DeActivate at +0x1c)` |  |
| CLight | `light_animmanager.cpp:43` | order 2 | `AsLight` at +0xc: `__dt__6CLightFv (AsLight at +0xb4)` |  |
| CLight | `light_visible.cpp:62` | order 1 | `~` at +0x8: `MarkForDestruction__6CLightFi (~ at +0xc)` |  |
| CMshSimpleBin | `staticmesh_cmshsimplebin_dtor.cpp:27` | order 1 | `Link` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv (Link at +0x14)` | yes |
| CParticleSystem | `particlesystem_cinstancedparticlesystem_dtor.cpp:25` | order 1 | `DeActivate` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv (DeActivate at +0x1c)` | yes |
| CParticleSystem | `particlesystem_cparticlesystem_dtor.cpp:19` | order 1 | `DeActivate` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv (DeActivate at +0x1c)` | yes |
| CParticleSystem | `particlesystem_cpropertyparticlesystem_dtor.cpp:142` | order 1 | `DeActivate` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv (DeActivate at +0x1c)` |  |
| CParticleSystem | `particlesystem_create.cpp:94` | order 1 | `DeActivate` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv (DeActivate at +0x1c)` |  |
| CParticleSystem | `Soldier_object_attach.cpp:483` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` |  |
| CParticleSystem | `Tank_object_draw.cpp:136` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` | yes |
| CParticleSystem | `animplayer_draw.cpp:57` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` |  |
| CParticleSystem | `bsbifunc_hier.cpp:52` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` |  |
| CParticleSystem | `player_commit.cpp:283` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` |  |
| CParticleSystem | `player_weapon_object_init.cpp:264` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` |  |
| CParticleSystemManager | `particlesystem_beginupdate.cpp:170` | order 1 | `BeginUpdate` at +0x28: `__dt__22CParticleSystemManagerFv (BeginUpdate at +0x30)` |  |
| CParticleSystemManager | `particlesystem_reset.cpp:105` | order 5 | `Render` at +0x8: `__dt__22CParticleSystemManagerFv (Render at +0xc)` |  |
| CPlayerObject | `UserInterface_drawcompass.cpp:81` | rename 1 | `GetInventory` at +0x60: `GetScriptObject__13CPlayerObjectCFv` |  |
| CPlayerObject | `UserInterface_update.cpp:30` | rename 1 | `GetInventory` at +0x60: `GetScriptObject__13CPlayerObjectCFv` |  |
| CPlayerObject | `objcreate_collvol.cpp:87` | rename 1 | `SetScriptObject` at +0x124: `SetScript__13CPlayerObjectFP8BSObject` |  |
| CPlayerObject | `objcreate_createplayer.cpp:95` | rename 1 | `SetScriptObject` at +0x124: `SetScript__13CPlayerObjectFP8BSObject` |  |
| CPlayerObject | `objcreate_german.cpp:155` | rename 1 | `SetScriptObject` at +0x124: `SetScript__13CPlayerObjectFP8BSObject` |  |
| CPlayerObject | `player_draw.cpp:174` | order 1 | `~` at +0x34: `GetLocalBoundingVolume__13CPlayerObjectCFQ210ISceneNode11EVolumeType (~ at +0xc)` |  |
| CPlayerWeaponObject | `player_weapon_object.cpp:32` | order 1 | `IsVisible` at +0x10: `Destroy__19CPlayerWeaponObjectFv (IsVisible at +0x50)` |  |
| CProjectileBullet | `bulletfactory_projectile_ctor.cpp:116` | order 1 | `~` at +0x8: `MarkForDestruction__17CProjectileBulletFi (~ at +0xc)` |  |
| CProjectileBullet | `bulletfactory_projectile_pool.cpp:109` | order 1 | `~` at +0x8: `MarkForDestruction__17CProjectileBulletFi (~ at +0xc)` |  |
| CProjectileBullet | `bulletfactory_thrown_ctor.cpp:116` | order 1 | `~` at +0x8: `MarkForDestruction__17CProjectileBulletFi (~ at +0xc)` |  |
| CProjectileBullet | `bulletfactory_thrown_pool.cpp:111` | order 1 | `~` at +0x8: `MarkForDestruction__17CProjectileBulletFi (~ at +0xc)` |  |
| CPropertyAnimLight | `light_animmanager.cpp:53` | order 3 | `AsLight` at +0xc: `__dt__18CPropertyAnimLightFv (AsLight at +0xb4)` |  |
| CPropertyAnimLight | `light_propertyanim_dtor.cpp:253` | order 2 | `BeginUpdate` at +0x10c: `SetBasis__6CLightF8CVector38CVector38CVector3 (BeginUpdate at +0x14)` | yes |
| CPropertyParticleSystem | `particlesystem_cinstancedparticlesystem_dtor.cpp:31` | order 2 | `DeActivate` at +0xc: `Render__23CPropertyParticleSystemFR10CDmaPacketPv (DeActivate at +0x1c)` | yes |
| CPropertyParticleSystem | `particlesystem_cpropertyparticlesystem_dtor.cpp:169` | order 2, rename 1 | `DeActivate` at +0xc: `Render__23CPropertyParticleSystemFR10CDmaPacketPv (DeActivate at +0x1c)` |  |
| CPropertyParticleSystem | `particlesystem_create.cpp:102` | order 2, rename 1 | `DeActivate` at +0xc: `Render__23CPropertyParticleSystemFR10CDmaPacketPv (DeActivate at +0x1c)` |  |
| CRenderBin | `compartment_bindtors.cpp:19` | order 3 | `Render` at +0x8: `__dt__10CRenderBinFv (Render at +0xc)` | yes |
| CRenderBin | `particlesystem_reset.cpp:31` | order 1 | `Render` at +0x8: `__dt__10CRenderBinFv (Render at +0xc)` |  |
| CRenderBin | `Soldier_object_attach.cpp:472` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` |  |
| CRenderBin | `Tank_object_draw.cpp:125` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` | yes |
| CRenderBin | `animplayer_draw.cpp:46` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` |  |
| CRenderBin | `player_commit.cpp:272` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` |  |
| CRenderBin | `player_weapon_object_init.cpp:253` | signature 2 | `Render()` at +0xc: `Render__10CRenderBinFR10CDmaPacketPv` |  |
| CScreen | `font_centeredf.cpp:34` | rename 1 | `GetScale` at +0x30: `GetWidthScale__7CScreenFv` |  |
| CScreen | `pumhandler_loop.cpp:44` | rename 2 | `GetY` at +0x24: `GetHeight__7CScreenFv` |  |
| CSoldierObject | `Soldier_object_ctor.cpp:76` | order 2 | `~` at +0x8: `MarkForDestruction__11CAnimObjectFi (~ at +0xc)` | yes |
| CSoldierObject | `animplayer_ctor.cpp:11` | order 1 | `~` at +0x8: `MarkForDestruction__11CAnimObjectFi (~ at +0xc)` | yes |
| CSoldierObject | `bsbifunc_givegrenade.cpp:38` | rename 2 | `AttachWeapon` at +0x134: `AttachObject__14CSoldierObjectFP13CStaticObjectPci` |  |
| CSoldierObject | `bsbifunc_setobjinfo.cpp:37` | rename 2 | `AttachWeapon` at +0x134: `AttachObject__14CSoldierObjectFP13CStaticObjectPci` |  |
| CSprite | `sprite_draw.cpp:143` | order 2 | `Link` at +0xc: `Render__7CSpriteFR10CDmaPacketPv (Link at +0x14)` |  |
| CStaticAnimMesh | `staticmesh_animdraw.cpp:34` | order 2 | `Draw` at +0x8: `__dt__15CStaticAnimMeshFv (Draw at +0xc)` |  |
| CStaticAnimMesh | `staticmesh_cstaticanimmesh_dtor.cpp:22` | order 2 | `Draw` at +0x8: `__dt__15CStaticAnimMeshFv (Draw at +0xc)` | yes |
| CStaticMesh | `staticmesh_animdraw.cpp:25` | order 2 | `Draw` at +0x8: `__dt__11CStaticMeshFv (Draw at +0xc)` |  |
| CStaticMesh | `staticmesh_cstaticanimmesh_dtor.cpp:16` | order 2 | `Draw` at +0x8: `__dt__11CStaticMeshFv (Draw at +0xc)` | yes |
| CStaticMesh | `thrownbullet_misc.cpp:120` | rename 1 | `UsesLightVolume` at +0x14: `IsLightingEnabled__11CStaticMeshFv` | yes |
| CStaticObject | `attachobject_type.cpp:11` | order 1 | `~` at +0x8: `MarkForDestruction__13CStaticObjectFi (~ at +0xc)` |  |
| CStaticObject | `hierobject_tree.cpp:81` | order 1 | `BeginUpdate` at +0x8: `MarkForDestruction__13CStaticObjectFi (BeginUpdate at +0x14)` |  |
| CStaticObject | `static_obj_pathbegin.cpp:191` | order 2 | `Draw` at +0xa0: `AsPlayerObject__10ISceneNodeCFv (Draw at +0x30)` |  |
| CStaticObject | `static_obj_setlight.cpp:322` | rename 1 | `GetOwnerNode` at +0x148: `AsHierTankObject__13CStaticObjectFv` |  |
| CStaticObject | `static_obj_type.cpp:120` | order 2 | `Draw` at +0xa0: `AsPlayerObject__10ISceneNodeCFv (Draw at +0x30)` | yes |
| CStaticObject | `thrown_obj_cleanup.cpp:123` | order 3 | `AsStaticObject` at +0x84: `AsWorldObject__10ISceneNodeFv (AsStaticObject at +0x74)` |  |
| CStaticObject | `weapon_ctor.cpp:31` | order 1 | `~` at +0x8: `MarkForDestruction__13CStaticObjectFi (~ at +0xc)` |  |
| CStaticObject | `objcreate_thrown.cpp:37` | signature 1 | `GetScriptObject()` at +0x60: `GetScriptObject__13CStaticObjectCFv` |  |
| CStaticObject | `thrown_obj.cpp:321` | signature 2 | `GetLocalBoundingVolume()` at +0x34: `GetLocalBoundingVolume__13CStaticObjectCFQ210ISceneNode11EVolumeType` | yes |
| CTankObject | `Tank_object_draw.cpp:261` | order 1 | `~` at +0xec: `PreTransform__16IMovingSceneNodeFRC7CMatrix (~ at +0xc)` | yes |
| CThrownBullet | `bulletfactory_projectile_ctor.cpp:30` | order 1 | `~` at +0x8: `MarkForDestruction__13CThrownBulletFi (~ at +0xc)` |  |
| CThrownBullet | `bulletfactory_projectile_pool.cpp:27` | order 1 | `~` at +0x8: `MarkForDestruction__13CThrownBulletFi (~ at +0xc)` |  |
| CThrownBullet | `bulletfactory_thrown_ctor.cpp:30` | order 1 | `~` at +0x8: `MarkForDestruction__13CThrownBulletFi (~ at +0xc)` |  |
| CThrownBullet | `bulletfactory_thrown_pool.cpp:29` | order 1 | `~` at +0x8: `MarkForDestruction__13CThrownBulletFi (~ at +0xc)` |  |
| CThrownBullet | `static_obj_setlight.cpp:214` | rename 1 | `Deflect` at +0x14c: `Penetrate__13CThrownBulletFRC10CCollision` |  |
| CThrownBullet | `thrownbullet_tail.cpp:374` | order 3 | `Draw` at +0xc: `__dt__13CThrownBulletFv (Draw at +0x30)` |  |
| CThrownObject | `thrown_obj_cleanup.cpp:149` | order 4 | `AsStaticObject` at +0x84: `AsWorldObject__10ISceneNodeFv (AsStaticObject at +0x74)` |  |
| CThrownObject | `objcreate_thrown.cpp:120` | signature 1 | `GetScriptObject()` at +0x60: `GetScriptObject__13CStaticObjectCFv` |  |
| CThrownObject | `thrown_obj.cpp:326` | signature 2 | `GetLocalBoundingVolume()` at +0x34: `GetLocalBoundingVolume__13CStaticObjectCFQ210ISceneNode11EVolumeType` | yes |
| CUcodeRenderBin | `compartment_bindtors.cpp:28` | order 3 | `Render` at +0x8: `__dt__15CUcodeRenderBinFv (Render at +0xc)` | yes |
| CVolSphere | `Tank_object_update.cpp:81` | order 1 | `~` at +0xc: `Create__10CVolSphereCFv (~ at +0x8)` | yes |
| CWeapon | `weapon_ctor.cpp:39` | order 2 | `~` at +0x8: `MarkForDestruction__13CStaticObjectFi (~ at +0xc)` |  |
| CWorldVolume | `world_volume_ctor.cpp:59` | order 1 | `TestCollision` at +0xc: `Create__7IVolumeCFv (TestCollision at +0x14)` | yes |
| CWorldVolume | `world_volume_dtor.cpp:64` | order 1 | `TestCollision` at +0xc: `Create__7IVolumeCFv (TestCollision at +0x14)` | yes |
| ISceneNode | `attachobject_ctor.cpp:65` | beyond 1 | `SetTMLocalToWorld` at +0xe8: `-` |  |
| ISceneNode | `attachobject_update.cpp:28` | beyond 1 | `SetTMLocalToWorld` at +0xe8: `-` |  |
| ISceneNode | `collision_head.cpp:118` | rename 2 | `GetAnimController` at +0x6c: `AsMovingNode__10ISceneNodeFv` |  |
| ISceneNode | `light_animmanager.cpp:36` | order 2 | `AsLight` at +0xc: `__dt__10ISceneNodeFv (AsLight at +0xb4)` |  |
| ISceneNode | `player_commit.cpp:177` | beyond 2 | `SetTMLocalToWorld` at +0xe8: `-` |  |
| ISceneNode | `thrown_obj_cleanup.cpp:71` | order 3 | `AsStaticObject` at +0x84: `AsWorldObject__10ISceneNodeFv (AsStaticObject at +0x74)` |  |
| ISceneNode | `thrownbullet_tail.cpp:282` | rename 1 | `GetOwner` at +0x94: `AsSoldierObject__10ISceneNodeFv` |  |
| ISceneNode | `bsbifunc_makenoise.cpp:71` | signature 1 | `GetPosition(CVector3 &)` at +0x40: `GetPosition__10ISceneNodeCFR8CVector3` |  |
| ISceneNode | `bsbifunc_makenoise_bi.cpp:105` | signature 1 | `GetPosition(CVector3 &)` at +0x40: `GetPosition__10ISceneNodeCFR8CVector3` |  |
| ISceneNode | `thrown_obj.cpp:302` | signature 2 | `GetLocalBoundingVolume()` at +0x34: `GetLocalBoundingVolume__10ISceneNodeCFQ210ISceneNode11EVolumeType` | yes |
| MPC_CODEC_INTERNAL | `src/ea/getpic.cpp:169` | signature 2 | `Init()` at +0xc: `Init__18MPC_CODEC_INTERNALFPQ24RCMP7DECODERPQ24RCMP5CHUNK` |  |
