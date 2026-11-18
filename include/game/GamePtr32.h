// 32-bit pointers in loaded game data. The game uses many loaded files in place,
// through structs that mirror the file's layout, and relocates their offset fields
// into pointers (offsetPtr, `(int)base + (int)field`). Those fields are 32 bits wide
// in the file.
//
//   GAME_PTR32(T)              a pointer field that is 32 bits in the file
//   GAME_ADDR32(p)             a pointer as the 32-bit integer the relocations add
//   GAME_FROM_ADDR32(T, a)     that integer back as a T*
//
// On the GameCube these are a plain T*, `(int)p` and `(T*)a`, so the bytes are
// unchanged. The PC port defines them first (its pc/game_types.h): there a 32-bit
// address is an offset from the start of game memory, so these fields keep the
// file's layout on a 64-bit host.
//
// GAME_LITTLE_ENDIAN is defined by the PC port. Files the game byte-swaps itself
// (ChangeEndian) are little-endian on the disc, so the swaps are skipped there.
#ifndef GAME_GAMEPTR32_H
#define GAME_GAMEPTR32_H

#ifndef GAME_PTR32
#define GAME_PTR32(T) T*
#define GAME_ADDR32(p) ((int)(p))
#define GAME_FROM_ADDR32(T, a) ((T*)(a))
#endif

#endif
