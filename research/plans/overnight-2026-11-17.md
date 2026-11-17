# Overnight run, 2026-11-17 00:40 to 09:00 EST (maintainer asleep)

Asked: "keep working on this until 9am ... put yourself in a loop". Each wake-up reads this file,
does the next item, updates "State", lands with research/tools/land.sh (lock name s1116) and
pushes. **Stop at 09:00 EST.** Write the morning summary and stop the loop.

Rules: research/plans/endgame.md (W1-W4, D1-D4); PC-only work goes in moh-frontline-pc;
reversible calls are made and listed under "Questions for the morning"; anything else is queued
there. If an item stalls for about 90 minutes with no landing, park it with notes and take the
next one. Use ninja -j4 / --jobs 4 and ulimit -v. Don't run more than one Dolphin at a time.

## Queue (in order)

1. **W2.1a: survey the SDK struct views.** List every game/EA file that declares its own OSThread,
   OSMessageQueue, OSAlarm, OSMutex, OSCond, DVDFileInfo, DVDCommandBlock, GXTexObj, GXTlutObj,
   CARD*, ARQRequest or _AXVPB (or a sized byte array in their place), and the SDK header each
   should take instead. Write it to research/notes/sdk-views.md. Also list pointers passed or
   stored as `unsigned long` / `u32` (ARQ, DMA records).
2. **W2.1b: move the views onto the SDK headers**, file by file, gated by one_unit.py and verified
   bytes. Start with the most common struct. A file whose bytes change keeps its view and gets
   listed.
3. **W1.2: the pure-source link tool** (research/tools/pure_link.py). It links the raw compiled
   objects with GNU ld at a fresh layout, globalizes the 101 fragment-local symbols, keeps one
   copy of each of the 45 duplicates, and builds _rom_copy_info, _bss_init_info and
   _eti_init_info. Goal: an ELF that links with no errors. Booting it in Dolphin comes after.
4. **W3.1: near-miss matching**, if time remains: the drafts within 10 lines (research/drafts/
   index.tsv), in this order: CPartBin::Link, CStaticMesh::DrawNode, AddAmo, then the rest.

## State

- 00:40 Loop started. Verified 1,285,620 B (90.88%), source_link 0 undefined (stopgap).
- ~01:30 Item 1 done (research/notes/sdk-views.md: 17 files, 6 address casts). Item 2 started:
  mutex2, inittmr, signals, threads, timerthread on dolsdk2001's os.h. The 2001 OSThread header
  was fixed to the game's 784 bytes (+error) and given its missing prototypes.
- ~01:15 filesys_ea, filedev, pad, ipad, game filesys moved (GAME_SDK_NO_LIBC opt-out in types.h;
  extern "C" guards added to nine 2001 headers). Next: snddrv.c/sdspmix.c (ARQ, _AXVPB, address
  typedef), MemoryCardGC, staticmesh_init, animemot, Moh2's PADStatus.
- ~01:30 snddrv.c and sdspmix.c moved onto os/ar/dsp/ax/ai.h (AXVPB fields by their SDK names).
  Next: MemoryCardGC, staticmesh_init, animemot, Moh2, then the address typedef.
- ~01:15 Items 1-2 DONE: all 17 files on the SDK headers; GameAddr typedef for the ARQ address
  casts. Next: item 3, the pure-source link tool.
- ~01:20 Item 3: research/tools/pure_link.py links the pure-source image with no errors
  (build/pure/Moh2RelGC_pure.elf). It doesn't boot: Dolphin calls __start and nothing more is
  logged. Working on the boot.
- ~02:30 The image boots through OSInit, MEM_init, InitCORE and the static constructors into
  CShellMenu::ShellInit, then crashes: compartment's draft static initializer has the wrong
  .bss layout (research/notes/pure-link.md). Fixed along the way: SDA21 relocation offsets (MWCC
  +2), segments, file-static merging, section anchors, per-file .bss layout, vtable checks.
- ~02:25 compartment draft statics fixed (re-proven on route_menu): the pure-source image boots like the
  original (no exception, 3 movies). pure_link --probe added. Route 1_2 reaches the level, then a runaway
  read during level load. Next: bisect that.

## Questions for the morning

(none yet)
