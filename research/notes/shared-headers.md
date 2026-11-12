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
  and the unit config names its stripped extab objects by number. Recoverable by renaming
  `stripped_objects` in their configs; not done (config, not view).
- `player_ctor` (lost, .text differs): its CPlayerObject view places matrices at 8-aligned
  offsets (m_matrix518 ...); with the 16-aligned header they move by 8. Without the
  alignment player_ctor keeps its match but 13 others lose theirs (the original realigns
  the stack for local CMatrix variables and 16-aligns them in sections), so the alignment
  stays and CPlayerObject's layout is the open question.
- `attachobject_ctor`: control failure (doesn't verify unchanged in this harness).

Variants tried: no alignment (40 kept of the 61 compilable), `bool` vs `int` constructor
argument (same result on the units it touches).
