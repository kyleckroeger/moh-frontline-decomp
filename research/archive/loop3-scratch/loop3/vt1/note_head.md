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
     (the original has them at +0xe8/+0x108 of IMovingSceneNode). bullet_ctor.cpp's CBullet
     declares `Init` right after ISceneNode (+0xdc), without IMovingSceneNode's 18 slots.
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

This is a policy question for the maintainer, not decided here. The 3 weak tables need a
unit that constructs the class: BSGO_Dummy's constructor is in a draft that doesn't see
a full declaration, and CFinishPartBin and CAnimLightManager have no constructor in
source yet. The units also define 2 tables the original doesn't have,
`__vt__16CScreenFrameMode` and `__vt__19CScreenFieldModePAL` (unit Screen, stripped in its
verification).

## All differing views

Generated by `vtable_order.py --md`. "Kept off" means the file says its table is not
emitted there.

