// GameAddr: a memory address passed as an integer, as the SDK's DMA calls take it
// (ARQPostRequest's source and destination, ...). On the GameCube it is the 32-bit
// `unsigned long` the original code used, so the bytes are unchanged. A 64-bit build
// (the PC port) defines GAME_ADDR_TYPE as uintptr_t before including this, so these
// casts don't truncate. See research/notes/sdk-views.md.
#ifndef GAME_GAMEADDR_H
#define GAME_GAMEADDR_H

#ifdef GAME_ADDR_TYPE
typedef GAME_ADDR_TYPE GameAddr;
#else
typedef unsigned long GameAddr;
#endif

#endif
