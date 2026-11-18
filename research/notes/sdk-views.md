# SDK struct views in game and EA code (endgame plan W2.1, survey 2026-11-17)

The PC port links game code against Aurora and its own SDK layer (`moh-frontline-pc/pc/sdk`).
There, SDK structs have 64-bit layouts and sometimes extra fields: Aurora's DVDFileInfo carries
`auroraHandle`, GXTexObj is larger, and so on. A game file that declares its own copy at
GameCube size (often a sized byte array) breaks that link or corrupts memory. Each of these files
must take the struct from the SDK header instead (`include/dolsdk2001/dolphin/...`, the release the
verified SDK units are built from (see 2026-11-17 below), which the
port swaps for Aurora's), with the GameCube bytes kept.

## Own declarations (17 files)

| File | Declares | SDK header |
| --- | --- | --- |
| src/ea/filedev.cpp | OSThread (`unsigned char[784]`), OSMessageQueue, DVDFileInfo | os.h, dvd.h |
| src/ea/threads.cpp | OSAlarm, OSMessageQueue | os.h |
| src/ea/inittmr.cpp | OSAlarm | os.h |
| src/ea/filesys.cpp | OSMutex | os.h |
| src/ea/mutex2.cpp | OSMutex | os.h |
| src/ea/signals.cpp | OSMessageQueue | os.h |
| src/ea/timerthread.cpp | OSMessageQueue, OSThread (`unsigned char[784]`) | os.h |
| src/ea/snddrv.c | OSMutex, OSThread (`[784]`), ARQRequest, _AXVPB / AXVPB, own ARQPostRequest prototypes (3x) | os.h, ar.h (arq), ax.h |
| src/ea/sdspmix.c | _AXVPB (GameCube offsets: `char field000[24]; unsigned long index; unsigned long sync;`) | ax.h |
| src/ea/ipad.cpp, src/ea/pad.cpp | PADStatus | pad.h |
| src/game/Moh2.cpp | PADStatus | pad.h |
| src/game/filesys.cpp | DVDFileInfo | dvd.h |
| src/game/MemoryCardGC.cpp | CARDFileInfo, CARDStat | card.h |
| src/game/animemot.cpp | ARQRequest (incomplete typedef), own ARQPostRequest prototype | ar.h (arq) |
| src/game/staticmesh_init.cpp | GXTexObj (`data[32]`), GXTlutObj (`data[12]`) | gx.h |

15 game files already include `<dolphin/...>` with `include/dolphin-sdk` in their include path
(e.g. compartment_render, staticmesh_render, font_render, texture_load); they follow that pattern.

## Pointers passed as integers

The SDK's own prototypes take addresses as `u32`: ARQPostRequest's source and destination,
ARStartDMA, and others. On a 64-bit PC, Aurora takes `uintptr_t`. On Windows `unsigned long` is
32-bit, so `(unsigned long)ptr` truncates. Call sites:

- src/game/animemot.cpp:344, 381 (`(unsigned long)cache`, `(unsigned long)aligned`)
- src/ea/snddrv.c:592, 1019, 1499, 1851 (`(unsigned long)source`, `(unsigned long)dest`)

Plan: a project typedef for "an address as an integer", identical to `unsigned long` on GameCube
(same bytes) and defined as `uintptr_t` by the port. The call sites cast to it, and the moved SDK
headers declare those parameters with it. Other `(unsigned long)` casts of pointers (UIStudio.c
17, compartment_init 9, staticmesh_init 4, MathFun 4, ...) are offset-pointer fix-ups of loaded
data (the port's `OffsetPtr` work) and need checking one by one; most are not SDK calls.

## Order

1. The OS structs in EA's thread, timer, file and mutex code, which are on the boot path: filedev,
   threads, inittmr, filesys (ea), mutex2, signals, timerthread.
2. DVDFileInfo (ea/filedev, game/filesys) and PADStatus (ipad, pad, Moh2).
3. snddrv.c and sdspmix.c (ARQ and _AXVPB), with the address typedef.
4. CARD (MemoryCardGC), GX objects (staticmesh_init) and animemot's ARQ.

Each move is verified with one_unit.py. A file whose bytes change keeps its view, and the reason
is recorded here.

## Progress

- 2026-11-17: the game's SDK headers are `include/dolsdk2001`, the release the verified SDK units
  are built from, not `include/dolphin-sdk`. The original's OSThread objects (IdleThread,
  DefaultThread, TimerThread, ReadFileThread) are all 784 bytes. The 2001 header lacked `s32 error`
  at 0x30C, the newer one adds `specific[2]` (0x318), so the 2001 header gained `error` and the
  thread prototypes it was missing (OSYieldThread, OSCreateThread, ...). All OS units still verify.
- Moved onto `<dolphin/os.h>` (dolsdk2001), each verified: mutex2, inittmr, signals, threads,
  timerthread.
- The 2001 SDK headers needed two fixes for C++ users. (1) `dolphin/types.h` pulled in libc
  (stdio, string, ctype, cmath), which the real SDK's doesn't. Those includes now sit behind
  `#ifndef GAME_SDK_NO_LIBC`: game/EA files that declare their own C library define it before
  including SDK headers, and the SDK units are unchanged. (2) dvd.h, pad.h, ar.h, ax.h, vi.h, si.h,
  dsp.h, dtk.h and exi.h had no `extern "C"` for C++, so the names were mangled.
- Moved and verified: filesys_ea (OSMutex), filedev (OSThread, OSMessageQueue, DVDFileInfo; the
  call casts its path to `char*` as the SDK prototype takes), pad and ipad (PADStatus), game
  filesys (DVDFileInfo).
- Sound: snddrv.c (OSMutex, OSThread, OSThreadQueue, ARQRequest, _AXVPB; 109 lines of private
  views and prototypes removed) and sdspmix.c (_AXVPB with its parameter block: the mixer writes
  `pb.mixerCtrl`, `pb.mix` and `pb.ve` and flags them in `sync`), verified. The driver's voice
  index is `(int)userContext`, its state `pb.state`, its play position
  `pb.addr.currentAddressHi/Lo`. ax.h now includes dsp.h (it names DSPTaskInfo).
  research/tools/dedupe_protos.py deletes a file's own prototypes of functions the headers declare.
- MemoryCardGC (CARDFileInfo, CARDStat; the 2001 card headers have the same layouts), staticmesh_init
  (GXTexObj, GXTlutObj; the GX init calls cast their ints to the SDK's enum types), animemot (ARQ, VI)
  and Moh2 (PADStatus), all verified. **All 17 files in the survey now take their SDK structs from
  the headers.**
- `include/game/GameAddr.h`: `GameAddr`, an address passed as an integer, is `unsigned long` on the
  GameCube (same bytes) and `GAME_ADDR_TYPE` where defined (the port defines it as uintptr_t). The
  five pointer casts to ARQPostRequest in animemot and snddrv use it.
- Left for the port: Aurora's ARQPostRequest prototype must take the same type. The other
  `(unsigned long)` pointer casts (UIStudio.c, compartment_init, ...) are offset-pointer fix-ups of
  loaded data, the port's OffsetPtr work, not SDK calls.
- 2026-11-17 (port session): the port compiled the 16 surveyed files against Aurora's SDK headers
  (clang, 64-bit, `TARGET_PC`). Aurora defines `s32`/`u32` as `int`/`unsigned int` on PC, so
  game code that spells SDK types as `long`/`unsigned long` no longer matches the SDK's
  prototypes. On the GameCube they are the same types, so respelling them keeps the bytes (all
  verified with one_unit.py):
  - filedev: `AyncDVDCallback(s32, DVDFileInfo*)`, as `DVDCallback` declares it.
  - signals: SIGNAL_wait receives into a `void*` (the 2001 SDK has no `OSMessage` typedef).
  - MemoryCardGC: CARD results and out-parameters are `s32`/`u32`; its own prototypes of
    CARDFormatAsync and CARDSetAttributes (not in the 2001 card.h) take the SDK's signatures.
  - animemot, snddrv: `ARQRequest` is in `ar.h` in the 2001 SDK but in `arq.h` in later SDKs and
    Aurora. `include/dolsdk2001/dolphin/arq.h` forwards to `ar.h`, and both files include it.
  Left for the port, not fixable here: Aurora's `CARDInit` takes the game and maker codes on PC;
  Aurora has no `ax.h` (snddrv, sdspmix); filedev and MemoryCardGC declare their own C library
  (`FILE`, stdio prototypes), which clashes with the host's.
- 2026-11-17 (port session): the port's gen_sources.py now checks that every game file is built
  with signed char and, for .c files, `-lang c++` (MWCC's plain char is signed, and all 149 game
  and EA .c units are C++). Two EA drafts, SNDtimeremaining and iSNDrandom, took their settings
  from ShellMenu_ctor.json (a game .cpp unit: exceptions and RTTI on, no `-lang`). They now take
  them from their neighbours' units, sover.json and srandom.json (EA settings, `-lang c++`).
  score_draft.py gives the same scores (3 and 80); the pure-source image was not re-run.
