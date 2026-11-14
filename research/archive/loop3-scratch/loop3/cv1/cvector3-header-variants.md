# CVector3 header variants: one header, or several? (cv1, 2026-11-12)

Question: did every translation unit of the original see one CVector3 with a user copy
constructor, or do the units that only matched with a constructor-free view show that some
TUs saw a different CVector3 (no copy constructor, a C struct, an out-of-line one)?

Short answer: one header with a user copy constructor explains the evidence, as long as
the copy constructor copies a plain 8-byte-aligned subobject (an implicit-style block copy)
rather than casting `this` to a pair of doubles. With that form, 41 constructor-free or
own-view units take the shared header unchanged and all 168 units already on it still
match (2 after a one-line assignment form). Four units are still unexplained by any
header with constructors (below).

Scratch: `scratch/loop3/cv1/` (drv.py, inj.py, chk.py, res_*.tsv, the variant headers h*/).

## 1. Where the original's weak CVector3 copies sit

Weak copies are kept from the first object in link order that emitted them out of line,
and MWCC emits one only where a call was not inlined (big functions) or the address was
taken. So they show TUs that certainly saw a definition; they say nothing about TUs that
inlined every use.

| Weak copy | Address | Unit (original file) | Out-of-line callers |
| --- | --- | --- | --- |
| `__ct__8CVector3Fv` (blr) | 8005480c | ObstacleAvoidance (ObstacleAvoidance.cpp) | none: address taken (array construction) |
| `Normalize` | 8006c224 | cdbgeom_vector (cdbgeom.cpp) | CCDBTriangle/CVolBox TestCollision, CPlayerObject::BeginUpdate |
| `DotDotDot` | 8006c340 | cdbgeom_vector (cdbgeom.cpp) | TestCollision (triangle, box) |
| `Length` | 8006c3a4 | cdbgeom_vector (cdbgeom.cpp) | TestCollision, BeginUpdate |
| `__ct__8CVector3FRC8CVector3` | 80074bd8 | compartment_volbox (compartment.cpp) | CCompartment::DrawNodeShadows (compartment.cpp), CHierObject::DoBeginUpdate (hierobject.cpp) |
| `Scale` | 800a4cec | player_vec_add (player.cpp) | BeginUpdate, CVolBox::TestCollision (box.cpp) |
| `Add` | 800a4d14 | player_vec_add (player.cpp) | BeginUpdate |
| `Sub` | 800a4dac | player_vec_sub (player.cpp) | BeginUpdate, CVolBox::TestCollision |
| `__as__` (by value) | 800a4de0 | player_vec_sub (player.cpp) | BeginUpdate |
| `__ct__8CVector3Ffff` | 800a4e7c | player_vec_sub (player.cpp) | BeginUpdate |
| `Cross` | 800c5008 | box_ivolume (box.cpp) | CVolBox::TestCollision |
| `Abs` | 800c50a4 | box_ivolume (box.cpp) | CVolBox::TestCollision |

(`Constrain` at 8009213c is a strong function of vector.cpp.) So compartment.cpp and
hierobject.cpp certainly saw a user copy constructor, ObstacleAvoidance.cpp a user default
constructor, player.cpp the (fff) constructor and a by-value `operator=`. The copy
constructor's body is `lfd f1,0(r4); lfd f0,8(r4); stfd f1,0(r3); stfd f0,8(r3)`, and
`__as__` copies `o` into `this` and then `*this` into the return slot the same way (so the
copy constructor is a two-double block copy).

That already rules out "TUs that need a constructor-free view had no copy constructor":
`compartment_intersect` (compartment.cpp) and `hierobject_script` (hierobject.cpp) only
matched without a copy constructor, but their own TUs call the out-of-line one.

## 2. The two groups and the flags

With the header of 666d079d (copy constructor `*(CVector3Pair*)this = *(const
CVector3Pair*)&other`): 168 units on the header keep their match, 117 own-view units lose
it, 35 don't compile. Without any copy constructor (variant A): 136 of the 168 keep, 32
lose (they need the user copy constructor: e.g. animplayer_initai builds the by-value
`SetCenter(CVector3)` argument in place only when the class has one; without it the
compiler adds a 16-byte temporary), and 20 own-view units gain.

No pattern by flags: the own-view units are 132 `-inline auto -RTTI off`, 7 `deferred,auto`,
5+5 RTTI on, 3 `auto,deferred`; the header units 157 / 6 / 4+1. Nor by library vs game (all
are game code), nor by file group: the same original files (compartment.cpp, hierobject.cpp,
player.cpp, matrix.cpp) have units in both groups.

## 3. Variants tried (try_header-style moves, the whole set, 4 jobs)

The `own` columns are units that still have their own view (152); `hdr` the 168 on the header.

| Variant | copy constructor | hdr kept | own kept |
| --- | --- | --- | --- |
| 0 (666d079d) | `*(CVector3Pair*)this = *(const CVector3Pair*)&other` | 168 | 0 |
| A | none (implicit) | 136 | 20 |
| S | `{ *this = other; }`, implicit `operator=` | 112 | 2 |
| T1 | fields in an `aligned(8)` POD base, `: CVector3Base(other) {}` | 166 | 40 |
| T2 | same base, `{ *(CVector3Base*)this = other; }` | 119 | 0 |
| T3 | base without `aligned(8)`, `: CVector3Base(other) {}` | 119 | 0 |

Single-unit injections into the own views (`inj.py`): two-double bodies written as a pair
cast, two indexed doubles, loads-then-stores, four floats and 4-float POD casts all lose
compartment_intersect, matrix_ps and thrown_obj_init; `{ *this = other; }` with the implicit
`operator=` keeps compartment_intersect and matrix_ps (Orthonormalize 0 instead of 96). The
common factor: the copy must be a whole plain-object copy, not an access through a cast
pointer. T1 gets that while keeping a user copy constructor and the pair-cast `operator=`
that the header units need (S drops it and loses 56). An out-of-line declared copy
constructor (variant "decl") turns copies into calls and can't match.

T1's two losses, AIObject_mmgpoint and bsbifunc_waypointinfo, had contrived copies
(`CVector3 forward(*&row)`, `CVector3 direction = matrix.GetFront()`); under T1 the
natural `CVector3 forward; forward = matrix.GetRow(1);` / `CVector3 direction; direction =
matrix.GetFront();` match (the returned temporary is the dead 16-byte copy).

## 4. Still unexplained by a header with constructors

- thrown_obj_init (`CThrownObject::Init`): a user default constructor alone (171 lines) or
  any copy constructor (158 with the pair form) breaks it. With any user constructor the
  three `CVector3 x = frame->x` copies are scalarised (lfs straight from the frame) instead
  of block-copied to the stack. Either this TU saw a constructor-free vector type, or the
  frame vectors are another plain type (the original has a weak `__ct__8CVector4Fv`, so
  CVector4 had constructors too).
- impact_dtor (`CImpact::Visit`): a user default constructor alone breaks Visit (616). Its view
  also has invented `Add(float,float,float)`/`Sub(fff)` overloads, so it can't take the header
  as is.
- vector (vector.cpp, `CVector3::Constrain`, the class's own implementation file) and
  projectilebullet_link match with no copy constructor and not under T1.
- staticmesh_draw (own copy constructor `d = other.d`) loses under every variant tried.

These are the only evidence left for a second CVector3 declaration. Since a default
constructor provably existed (ObstacleAvoidance's weak copy), a constructor-free view can
only be right if those files declared their own struct, which is unusual for EA's code; more
likely their reconstructed code differs from the original's (different local types or
copies). Not settled.

## 5. Landed

include/game/CVector3.h takes T1 (the field names, layout and every mangled name are
unchanged; the (fff) constructor assigns in its body since a base's members can't be in a
mem-initialiser). 41 own-view units move onto it (40 by try_header plus matrix_ps by hand,
its `v.d.v` arguments rewritten to `&v.x`), and AIObject_mmgpoint and bsbifunc_waypointinfo
take the assignment forms above. Every one of the 209 units that include the header verified
(chk.py) before the landing; the table is research/layout/header_moves/CVector3.tsv.
