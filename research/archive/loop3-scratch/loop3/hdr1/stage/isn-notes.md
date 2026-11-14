
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
  objects had those copies too and the linker kept one, but the unit configs don't list
  them, so the views declare the virtuals without bodies. Measured: with a variant of the
  header whose ISceneNode virtuals are only declared, 42 of these 63 keep their match
  (54 in all). That variant misstates the original (the functions are weak there), so it
  is not committed. **Question for the maintainer:** list the weak ISceneNode copies in
  those units' `discarded_functions`, or accept a declaration-only header?
- 2 lost to label renumbering of stripped objects (as for CMatrix); 9 others lost with
  bytes or sections differing (mainloop_game: .text).
- 39 didn't compile: 15 use `AIDoodadView` members through GetAIDoodad() (the real class is
  CAIDoodad; renaming the view would change code), 7 BSObject/BSObjectView the same way,
  4 the scene's `SExtent` nested in their own ISceneNode view, 5 name the data members
  (m_link8, m_prev/m_next, m_sceneNext), 2 their own sqrtf, the rest single cases.
