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
- ~03:20 The image is in step with the original's trace into level 1_2 (update 2790 matches the control).
  Fixed: HUD vertex arrays (transcribed), verified weak copies made strong, pure_data_check.py.
  Still: a runaway read in Dolphin's EFB range soon after the player appears (the game, not the probe).
  Parked for an interactive debugger session; moving on to item 4 (W3.1 matching).
- ~03:30 W3.1: CPartBin::Link (5 natural helper/initialisation variants, all worse) and DrawNode (an FPR swap)
  give nothing new tonight. Built entry_trace.py and dolphin_direct.py for W1. The failure isn't
  layout-dependent. Next: W2.3 shared headers (try_header.py), which serves the port more than more near-misses.
- ~04:10 IVolume.h written; 4 units moved onto it (verified); 34 lost and 40 no_compile, analysed
  in shared-headers.md. pure_link drops the drafts' duplicate .ctors (route 1_2 still fails).
- ~04:30 More header moves (IVolume 18, BSGO_Basic 17, CPlayerObject +1). entry_trace fixed (read
  probes): the route 1_2 failure starts right after the level's first subtitle (sound.cpp's 'LUMS'
  chunk > DisplaySubtitle > PopUpMessage). See pure-link.md.
- ~05:05 Narrowed further: the read is most likely MEM_allocalign walking a corrupt heap chain (probes on
  strlen and DWI_allocalign). Lead for a heap-check probe recorded in pure-link.md.
- ~05:15 Heap check probe: heap intact; not the allocator. Narrowed to the pop-up/subtitle draw path
  (pure-link.md). W1 parked here with probes saved in research/probes/.
- ~05:25 W2.2 done in the port repo (moh-frontline-pc 501317b: tools/hazards64.py, pc/hazards64.tsv, tests).

## Questions for the morning

(none yet)

## Morning summary (final, 08:50)

**W1 pure-source image: the level-1_2 failure is found and nearly explained.**
- Dolphin 2609's GDB stub works. research/tools/dolphin/gdbpeek.ps1 (Windows side; the firewall blocks
  WSL) attaches, runs to a log line, and dumps registers and stack, or logs every hit of a write
  watchpoint. This answers last night's question 1 without you.
- The runaway read is not the subtitle code. It's DMMorphBlendDisplayLists (the speaking soldier's
  face morph). The morph object's count is overwritten every update by LLCharObjCompositeMorph
  decoding garbage frame data (target index 63 writes weights[64] = count).
- **With the count restored by a probe, the pure image plays route 1_2 for the whole 400 s run**
  (trace update 106290) with no other failure.
- The garbage comes from the animation frame reads from ARAM. The upload to ARAM is identical to
  the original's and lands (direct readbacks deep in it return real data). The game's ARQ reads reach
  ARStartDMA with the right arguments, but the cache slot ends up holding 0x486e486e, written after
  boot by something not yet identified. Next steps are in research/notes/pure-link.md: a watchpoint on
  a frame-cache slot, then the ARQ/AR callback order.
- Probes: research/probes/{morph63,aram_upload,aram_alloc,sndaram,ardma,frame_read}_probe.cpp.

**W2.1 SDK struct views: done.** All 17 game/EA files that declared their own OS, DVD, PAD, CARD, GX,
ARQ or AX structs now take them from the SDK headers (include/dolsdk2001), with bytes unchanged.
research/notes/sdk-views.md.

**W2.2 64-bit hazard inventory: done, in the port repo** (moh-frontline-pc 501317b,
tools/hazards64.py, pc/hazards64.tsv: 589 candidate rows, with tests).

**W2.3 shared headers:** new IVolume.h (18 units), CQuaternion.h (35), CRenderBin.h (21),
CAIFilterRealPosition.h (20), BSGO_Basic.h (17), CLine3.h (7), CVolSphere.h (6), CVolCapsule.h (2),
CVolFiber.h (2), plus CColor +14, ISceneNode +2, CPlayerObject +1 and CPlane +1: about 150 units
moved, each verified. The rest are mostly blocked by member-name spellings and result types (int vs
bool); research/notes/shared-headers.md.

**Drafts:** ShellInit's vertex colour fixed (0x80808080) and proven again; the compartment draft's
statics fixed and proven again.

**W3.1 near-misses:** nothing new. The death_cb permuter ran 135k iterations without beating its base
score (120). The DrawNode and CPartBin::Link harnesses don't compile (autoperm's reference-to-pointer
rewrite breaks recursive calls).

## Questions for the morning

1. Header moves are now mostly blocked by member-name spellings (the same field named differently
   per file). A rename tool would move many more units, but it rewrites a lot of matched source. OK
   to go ahead in a later session?
2. W1 is close: one more watchpoint session should find what writes 0x486e486e into the animation
   frame cache. Continue W1 next session, or switch to the port as planned?

## Decisions (user, 2026-11-17 morning) — the plan for the next session

Both questions answered "yes", in this order, starting next session (paused for today):
1. **W1, capped at one session.** Run the write watchpoint on a g_frame_cache slot (pure_l12fr.elf),
   then check the ARQ/AR callback order. If the writer of 0x486e486e isn't found in that session,
   write down where it stands in research/notes/pure-link.md and move on.
2. **Rename tool for header moves.** One class at a time, landed and verified before the next.
   Use the shared header's member name, or the most common one if there is no header yet. Rename
   data members only, never anything that appears in a symbol.
3. **Then switch to the PC port** (moh-frontline-pc). Port work runs in its own session opened in
   the port repo, not from this one; a decomp session stops at the hand-off and says so.
