# 32-bit pointers in loaded data (for the PC port, 2026-11-17)

The game uses many loaded files in place: it reads a file into memory and works through structs
that mirror the file's layout, after relocating the file's offset fields into pointers
(`offsetPtr(m_file->m_keys, base)`, `(char*)((int)bank->modules + (int)bank)`). On a 64-bit host a
pointer field is 8 bytes, so every such struct would stop matching its file.

The PC port's decision (moh-frontline-pc, 2026-11-17): those fields stay 32 bits, and on PC a
32-bit address is an offset from the start of game memory (Aurora's MEM1). The decomp marks them
with `include/game/GamePtr32.h`:

| Macro | GameCube | PC port |
| --- | --- | --- |
| `GAME_PTR32(T)` | `T*` | a 4-byte field holding an offset from game memory |
| `GAME_ADDR32(p)` | `(int)(p)` | the pointer's 32-bit game address |
| `GAME_FROM_ADDR32(T, a)` | `(T*)(a)` | back to a `T*` |
| `GAME_LITTLE_ENDIAN` | undefined | defined: skip the game's own byte swaps |

On the GameCube the preprocessed source is unchanged, so a converted file must still verify with
one_unit.py. The port's tools/hazards64.py stops listing an `offsetPtr` call once its field is
declared `GAME_PTR32` in the same file or in `include/game/`.

## Byte order

Two kinds of loaded file:
- **Swapped by the game** (it calls `ChangeEndian`/`EndianSwap` after loading): little-endian on
  the disc. Level contents (TLT_LoadLevelContentsFile), skeletons, propdat, soundtable,
  script schedules. The host is little-endian too, so the port only needs the pointer fields and
  `#ifndef GAME_LITTLE_ENDIAN` around the swap helpers' bodies.
- **Read raw**: big-endian on the disc. compartment, cdbobject (collision), staticmesh, dmesh,
  texpack, EA's AEMS banks (saems). These will also need a big-endian field type for the fields
  the CPU reads; bulk vertex and texture data goes to the GPU layer (Aurora) as on the GameCube.

## A format is converted only when every view of it is

Files declare their own views of a format, and each view's pointer fields need `GAME_PTR32` too.
soundtable, for example, reads the level contents file's entries through its own
`LevelResourceView` (the relocated data pointer, then two relocated name strings in the data).
So the order is: move a format's structs into one shared header (endgame W2.3), mark its pointer
fields there, then convert its loader and its users.

## Converted

- soundtable (2026-11-17): the key, parameter and entry tables, its `ChangeEndian` helpers, and
  its view of the level resource entry (`LevelResourceView`, `LevelResourceNamesView`; the level
  contents format's other views are not converted yet).
