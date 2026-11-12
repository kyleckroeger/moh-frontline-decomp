# Layout conflicts: which view is right (layout_lint, 2026-11-12)

Full recompiling run of `research/tools/layout_lint.py` (agent lint1, three-day loop, under
the landing lock; 1 minute with `--jobs 4`): 1,185 files, 0 failed, 1,304 classes,
**170 conflicts: 163 member, 6 width, 1 vptr**; 18 rows where a draft holds a layout no
matched file has (11-10: 164 member, 6 width, 24 draft-only). Rows that went away since
11-10 are drafts that landed (Visit, TranslateFromScript, RotateFromScript, objcreate
soldiers, ...); the new vptr row is `src/game/decal.cpp` (below). The tables are
`research/layout/conflicts.tsv` and `classes.tsv`. After the two draft fixes below the committed tables show **168 conflicts (163
member, 4 width, 1 vptr), 16 draft-only**.

Method: the original's loads and stores (offset and width) decide. For a verified unit the
code is byte-exact, so its accesses are the original's; a conflict between two verified
units is either two different objects sharing an invented name, or a misnamed/mis-sized
member that the code never touches. Original disassembly: `scratch/loop3/lint1/od.py
<addr> <len> [regex]`.

## Width conflicts (all 6)

| Class | Verdict | Evidence |
| --- | --- | --- |
| `AnimTransition_t` +0x4 | **Two different records, one name.** Draft renamed (fixed). | `src/game/animemot.cpp`: an 8-byte transition-list entry `{u16 state; AnimBlob_t* blob}`, sized by its copy (`(count-1)*sizeof + 12`). `_AnimTranDoTransition` draft: the record `AnimTranEntry_t::transition` points to (u16 at +4/+6, float +8, u16 +0xc/+0xe); `check_draft` offsets 100%. Renamed the draft's struct to `AnimTranData_t` (static inline helpers only, no mangled name changes; score 185 unchanged). |
| `CColor` +0x0 | **Bytes r,g,b,a are right; the draft's `unsigned int m_rgba` was wrong.** Fixed. | PopUpMessageHandler ctor (0x800ba3d0) at 0x800ba6c8: `stb r0,16..19(r1)` four byte stores, then `lwz r0,16(r1); stw r0,20(r1)` (the by-value copy). The word view compiled a single word store (`check_draft`: access sizes at 0x0 [1,2,4] vs [1,4]). Draft replaced by struct2's `PUM_BEST.cpp` (byte ctor temporaries): real 167 -> 45, `check_draft` offsets 95.7% -> 100%. 44 matched files use bytes. (`UserInterface_init` was already fixed before this run.) |
| `AnimStateSlotView` +0x8 | **Same memory, frames shifted by 4; harmless.** | `animst.cpp`: owner = `slots[4]` of 0x68 with a 4-byte head in each slot (state +4, flags +8, delay +0xc). `animst_transprecheck.cpp`: owner = 4-byte head + `slots[4]` (state +0, flags +4, delay +8). Absolute offsets agree (`owner+4+0x68i` = state). transprecheck's `memcpy(&slots[k], &slots[k+1], 0x68)` is the original's copy, so its framing (slot starts at owner+4) is the likelier real one; animst.cpp and the drafts AnimStProcess/AnimStStartState and `_AnimTranDoTransition`'s `AnimStSlot_t` use the other framing. A header step should adopt transprecheck's. |
| `AEMSLINKDEFVIEW` +0x4 | **saems.c is right (`u16* targets; short count`); saems_system.c's `int to` is never accessed.** Harmless. | saems.c reads +4 as a halfword count and +0 as the target pair list. saems_system.c only relocates +0 (`links[k].from += bank`: it is the targets pointer) and never touches +4. |
| `AEMSSTATEVIEW` +0x0 | **Intended union; not a conflict.** | saems_counter.c declares it as a union of int, pointer and `unsigned short step` (big-endian high half of the state word, `state0.step = step` and `phase += state0.step` are halfword accesses in the original). The lint compares union members like struct members. |
| `MessageView` +0x4 | **Two different objects, one name**, plus misnamed members in bsmessage_group.cpp. Harmless. | Moh2/mainloop/player_health: an 8-byte UI message-table entry `{char* text; u8 type}`. bsmessage_group/bsmessage_remove: the 36-byte `g_MessageQueue[1024]` record. For the queue, bsmessage_remove's names are proven by its stores (`QueueMessage`: +0 sender, +4 target, +8 object, +0xc `registration->thread`, +0x10 trigger, +0x14 data, +0x18 value, +0x1c entry, +0x20 registration value). bsmessage_group calls +0 `arg1`, +4 `arg2`, +0xc `target`, +0x10 `arg3`, +0x14 `arg0`, +0x1c `key`, +0x20 `sender`: its "target" (dereferenced for `handlers` at +0x18) is the queued **thread**, and its "sender" is the registration value. |

## vptr conflict

`CRenderBin`: vtable pointer at +0x1c in 77 matched views, at +0 only in `src/game/decal.cpp`
(virtuals declared before `int data04[7]`; CW places the pointer by declaration order). The
77 are byte-exact units that use the class (e.g. CFont's views in Pause.cpp). decal.cpp only reads
`CBulletDecalManager::m_active`; the class is 0x20 bytes either way, so the derived
offsets, thunks and vtables are unchanged. Wrong but harmless (maintainer list).

## Most frequent member conflicts (all between verified units)

| Class.member | Verdict | Evidence |
| --- | --- | --- |
| `CQuaternion` w/x/y/z (33 xyzw vs 16 wxyz) | **wxyz (scalar at +0) is right.** | quaternion.cpp `Rotate` (byte-exact): the +0 component appears only in the cross products (`2*w*x`...), never squared; the squares use +4/+8/+0xc. `EndianSwap` swaps +4,+8,+0xc then +0. The xyzw files only copy or pass whole quaternions; naming only. |
| `list::m_owned` (+0x2c / +0x28 / +0x1c) | Apparently different container classes sharing the name `list` (buddy list, scene lists, world volume); not checked access by access. Byte-exact either way; header step. | |
| `fast_vec::m_capacity` (+0xc in 39, +4 in scene.cpp) | **+0xc is right.** scene.cpp names its unused +4 word `m_capacity`. Harmless. | AIFilter.cpp's ctor stores the capacity argument at +0xc; scene.cpp's `push_back` reads only +0 and +8. |
| `CAIObject::m_position` (+0x18 in 17, +0x38 in bsbifunc_movewalk) | Different fields. movewalk's +0x38 is another CAIFilterRealPosition (the walk target), misnamed. Harmless. | AIObject.cpp's +0x18 is the object's own position; movewalk only passes +0x38 to StartWalkToArbitraryPoint. |
| `CPlayerObject` m_controller, m_weapons, m_currentWeapon, m_position | Not checked access by access (the bsbifunc_* views put m_weapons/m_currentWeapon past +0x4000, so they view a different block). Byte-exact either way; header step. | |
| `SClsnContact::m_object`, `CCollision::m_time`, `UNPACKINITPARAMS::getframe`, `UISInfo_t` | Neighbouring fields named alike in different views; byte-exact either way; header step. | |

## Draft-only rows (16 after the fixes)

Unchanged in kind from the 11-10 check (all names reused for different fields, see
docs/PortReadiness.md), apart from the two fixed above. The rest: CFog in the Draw draft,
CharSkel_t, LightPoolView (draft view based 0xc04 into the compartment), PATHTRACK as one
bitfield word in two PATHI drafts (bytes +1..+3 in the matched files: same bits), SNDMIXVIEW
in MIX_audioslice, TriggerCoreView, _PropBSPLeaf and BSObject::m_owner in the multiplayer
draft. None was edited (MIX_audioslice and the BSP-leaf drafts are worked by other agents).

## For the maintainer (verified units, not edited)

Wrong but harmless views, for a header step that keeps the image identical:
`decal.cpp` CRenderBin vptr (+0 vs +0x1c); `saems_system.c` AEMSLINKDEFVIEW `from`/`to`
(should be `targets`/`short count`); `bsmessage_group.cpp` MessageView names (+0xc is the
thread, +0x20 the registration value); `scene.cpp` fast_vec `m_capacity` at +4;
`bsbifunc_movewalk.cpp` CAIObject `m_position` at +0x38; animst.cpp's slot framing;
the 33 xyzw CQuaternion views.
