# The pure-source GameCube image (endgame W1, 2026-11-17)

`research/tools/pure_link.py` links every unit, data file (with the vtable stopgap) and draft with
GNU ld at a fresh layout and no original bytes, into `build/pure/Moh2RelGC_pure.elf`, and
`dolphin_boot.py run` boots it. What it takes, in the order the problems were found:

1. **Fragment statics.** One original file's fragments each compile a copy of the statics they
   use. Writable statics defined by several objects of one file merge into one global symbol
   (`name.FILE`, or `name` when a copy is global). Every copy but one is weak: one with the
   original's size, then a data file's, then a unit's, then a draft's. Function statics (`x$NN`)
   are left alone. Statics another fragment reaches with extern are renamed there too.
2. **Section anchors.** MWCC reaches statics as `...bss.0` + offset. Before merging, each such
   relocation is pointed at the static's own symbol, or the merge redirects nothing.
3. **One .bss layout per file.** Code that loads the anchor once and reaches several statics by
   register offset needs the original file's layout. Each object's `.bss` is placed so that its
   named statics keep their original offsets (shifted by one constant into the image's `.bss`).
   The fragments of a file then overlay and share storage (`--no-check-sections`). This works
   for 258 sections, mostly units (verified code has the original layout). `.sbss` isn't split
   per object, because GNU ld wants SDA21 targets in an output section named `.sbss`.
4. **SDA21.** MWCC records `R_PPC_EMB_SDA21` at the instruction's low halfword (+2), and GNU ld
   wants it at the instruction. Without the fix, every small-data access corrupts the next
   instruction (OSInit hung before its banner).
5. **Segments.** One PT_LOAD per section, each 32-byte aligned, as in the original. A single
   segment from 0x80000000 overwrote low memory.
6. **Vtables.** After linking, every table's slots are checked against the original
   (research/layout/vtables.tsv). Where the linker kept a weak copy compiled from a reduced class
   view (`__vt__15CUcodeRenderBin`: Render in the destructor's slot), the copy that matches is
   kept, or the original's is generated. RTTI and offset words aren't compared: fragments built
   with RTTI off write 0 there, and only RTTI and dynamic_cast read them.
7. **Dead-stripped references.** A name that only dead-stripped code reaches gets its local
   definition made global (`__start.c`'s `__init_registers`, reached from inline asm), or else a
   trap stub.

**Where it stopped at 02:00:** the image boots through `__start`, OSInit (banner, arena),
`MEM_init`, `InitCORE` and the static constructors, then crashes in `CShellMenu::ShellInit` >
`WaitRenderList` > `CRenderList::Finish` > `FinishBin`: `g_CptSimpleBin` was never constructed. Its
constructor is in `__sinit_compartment_cpp`, which exists only in the draft
`research/drafts/compartment_init.cpp`. That draft's statics have another order and other sizes
than the original file (its render-bin views are 48 bytes, the original's 36), so its static
initializer constructs a private block that nothing else uses. The fix belongs in the draft:
declare compartment.cpp's statics in the original's order with the units' class views. Other
drafts that hold a file's static initialization need the same check (`pure_link.py` writes
`build/pure/bss_placed.txt`; a draft missing from it has a layout unlike the original's).

Debugging aids:
- `scratch/s1116/direct.py ELF SECS` boots an ELF with `Dolphin.exe -e` (Dolphin then knows the
  symbols, and its MI log shows invalid MMIO writes).
- `research/tools/boot_probe.py` checks whether execution reaches a symbol before OSInit (BAT-write
  probe).
- The crash log's register dump and stack (`dolphin_boot.py run`), symbolised with `nm -n`.

**02:20: it boots like the original.** The compartment draft's statics now follow the original's
order and sizes: render-bin views of 36 and 32 bytes instead of 48 and 44, a 104-byte 8-byte-aligned
CCompartment view, and the two matrices it lacked. Its `.bss` then overlays the units' block.
The draft is still `consistent` and was proven again on route_menu (8,887 trace lines identical,
`__sinit_compartment_cpp` entered). The pure-source image now boots with no exception and plays the
three intro movies, with the same 72 log lines as the original ELF.

**Probes.** `pure_link.py --probe P.cpp ... --output X.elf` links test harnesses as link_drafts.py
does: each probe replaces the functions its `// replaces:` lines name (the game's copies are
weakened), and its own statics merge with its file's (the clock probe's `g_bSwap` is Screen.cpp's).

**Route 1_2 (02:25).** With the route_1_2, clock and level probes, the image plays the menu into level
1_2 (94 trace lines, the player at -5.1 21.1 0.5 at update 2790). Then, while the level loads (after
FRAME 2850), a runaway read walks through Dolphin's EFB range ("Unimplemented Z+Color EFB read"
millions of times; the log grew to 2.5 GB and was deleted). FRAME 1470 already differs from the
control (an intro movie frame; 1530 is the known timing-dependent one). The all-drafts link_drafts
image plays this route to the end, so the cause is in the pure link: a static still split between
fragments, a `.data` copy that differs, a draft's data view, or layout. Next: bisect it. Swap units'
and drafts' copies of the file statics they touch while loading a level, or link with
`--probe` plus a reach probe at the loader functions.

**03:20: route 1_2, continued.**
- The image is **in step with the original into level 1_2**. At update 2790 its trace matches the
  control's exactly (position -5.112 21.053 0.520, yaw 3.1210, flags d08002b8, weapon 6). Only the
  player-object hash differs, and it depends on the layout. It reaches the level three trace steps
  later than the control because loading takes more emulated time.
- `research/tools/pure_data_check.py` compares every named initialised data object of the original
  with the pure image's, pointer words masked. It found seven HUD vertex arrays (CompassVerts and
  others) that only a multiplayer draft defined, zero-filled. They are now transcribed, exact
  (`research/data/init/UserInterface.cpp`). Left: the RTTI offset words of the two two-part manager
  tables.
- Weak functions: 6 of 604 have copies that differ between fragments (GetWorldLinearVelocity is 20
  bytes in some and 44 in others). pure_link now makes each unit's verified copy, the one placed at
  the original address, strong.
- **Still failing.** A little after the player appears (after update 2790 with the clock probe,
  3210 without it), something reads sequentially from 0x88800000 up, Dolphin's EFB range ("Unimplemented
  Z+Color EFB read", millions of lines). Without the clock probe it fails the same way, so the game
  does it, not the probe. The original never reads there. The pure image contains GXPeekARGB/GXPeekZ
  (GXMisc's functions the original linker stripped), but no game source calls them. Next: find the
  reader with boot_probe-style probes on the level-start path (CPlayerObject::Init, the HUD's
  first draw, the level's texture and display-list set-up), or with Dolphin's debugger
  interactively.

**03:30: tools for the next session.**
- `research/tools/entry_trace.py`: logs the first entry of every game function, for an image
  linked with `pure_link.py --probe-space N`. Each traced function branches to a stub that writes
  once to 0xCC010000 + 4k, which Dolphin logs. `--decode MAP LOG` prints the order. It works
  (3,811 functions traced), but the stubs change timing: without the clock probe, the traced
  image stalled in Gameloop_Init (last entries SIGNAL_create, SIGNAL_wait, AlarmHandler) instead of
  reaching the level, so traces aren't comparable with untraced runs.
- `research/tools/dolphin_direct.py ELF SECS`: boots with `Dolphin.exe -e`.
- The route 1_2 failure isn't layout-dependent: with 512 KB reserved after .text (every data
  address shifted) it fails at the same update, with the same log size (the 2.5 GB cap). It is
  deterministic.
- Next: an interactive Dolphin debugger session (break on the first EFB read and read LR and the
  stack), or entry_trace with the clock probe, accepting slips.

**04:10.** Also ruled out tonight: (1) the drafts' extra static initialisers. Each draft holds a
whole fragment, so 8 of the 10 drafts with .ctors re-ran their file's initialiser after the unit's.
pure_link now keeps .ctors only in the drafts of a `__sinit` function (53 entries, as in the
original), and the failure stays. (2) Private copies of other files' globals: only 2, both SDK.
(3) Merged statics are all .bss (57), .sbss (24) or .sdata (7), and every object holding a merged
.bss static is placed in its file's layout. The failure is deterministic and still unexplained.

**04:55: the failure located.** entry_trace.py now probes with a *read* in Dolphin's logged EFB range
(0xC8A00000 + 4k). The first version wrote to 0xCC01xxxx, which Dolphin decodes onto real hardware
registers, and that changed the run. With the clock probe, the traced image reproduces the failure exactly
(same last trace line). The functions entered for the first time just before the first runaway read
(at 0x88800000) are the level's first scripted voice line and its subtitle:
PlayScriptedSound > CSoundTable::GetSoundEvent > EndianSwap(SOUND_MULTISUBTITLEDATA) >
CShellMenu::Get_LanguageSubtitleIndex > DisplaySubtitle > PopUpMessageHandler::AddPopUpMessage >
PopUpMessage's setters, Scroll (a draft, proven on m1_opening), UpdateDisplayTime, Get_screenLocX,
Get_a. Then the reads start. All of them are verified units except Scroll. In sound.cpp the subtitle
text is `strings + header->offsets[language]` inside sound data loaded at run time ('LUMS' chunks). A
wrong language index or a wrong chunk pointer would give exactly a long sequential read (strlen or
the text draw). The statics on this path (g_popUpMessage, g_pPopUpMessageHandler, g_HintTextArray)
have the original's sizes and relative layout. Next: probe deeper (entry_trace on SDK/EA code too:
font drawing, strlen) or read g_Shell's language field and the 'LUMS' chunk in Dolphin's memory view.

**05:00.** A probe that replaces strlen (reporting any pointer past main memory, with its caller)
saw nothing before the flood, so DisplaySubtitle's text pointer is valid (it calls strlen first).
The reader is further on: pumhandler.cpp's subtitle queue (g_subtitle_queue.reserve(20) on first
use, then the line splitting) or the text draw. Next: a probe replacing DWI_allocalign or reporting
g_subtitle_queue after reserve, or Dolphin's memory view at that point.

**05:05: the best lead so far.** A probe replacing DWI_allocalign (memory.cpp's body, reporting the
subtitle queue's block and any block outside main memory after MEM_allocalign returns) printed
nothing before the flood, although DisplaySubtitle had been entered and its first call reserves the
queue through DWI_allocalign ("Subtitle Queue"). So the runaway read most likely happens inside
MEM_allocalign: the REAL memory manager walking a heap chain that leads past main memory, that is,
heap corruption earlier in the run. Candidates: an object allocated with one class size and written
with another (a view smaller than the real class), or a latent overflow that the original layout
hides. Next: a probe on MEM_allocalign/MEM_free that checks the heap chain (the REAL memory manager's
block headers) after every call and reports the first bad link with its caller.

**05:15.** Heap ruled out too. `research/probes/heap_check_probe.cpp` is memalloc.cpp with a walk of
every memory class's block chain at each allocation. The chains stay intact through all ~3,100
allocations up to the failure. The last allocation before it is the "Subtitle queue" (call 3096,
update ~2760), and it succeeds. No allocation happens between it and the flood, so the runaway reader
isn't the allocator. With the probe's timing the trace matches the control even at 2730 (the
pre-spawn state). What's left: the pop-up/subtitle draw path in its first frames (pumhandler.cpp's
UpdateSubtitles and PopUpMessageHandler_Loop, popup.cpp's draft Scroll, the font draw). Next:
entry_trace extended to the font/SDK text functions, or a probe around PopUpMessageHandler_Loop
reporting the message's string pointers. `research/probes/strlen_check_probe.cpp` is the strlen probe.
- Tried and dropped: tracing SDK and library functions too. With the OS/EXI/SI/cache functions the image
  doesn't boot; without them it crashes in MSL's printf path (some library functions don't survive a
  moved first instruction). entry_trace.py stays game-only.

**Draft fix (05:40).** ShellInit (flagged `differs`): the loading screen's four vertices have the colour
0x80808080 in the original (128 is full intensity in GX vertex colours), not 0xffffffff. The draft's
vertex table is now byte-identical to the original's (.rodata 0x801784a8) and the draft was proven
again on route_menu (8,829 lines). check_draft still says `differs` because the two versions point
into the table at different words, which is codegen; the data is the same.

**Located with Dolphin's GDB stub (06:30, 2026-11-17).** Dolphin 2609 has a GDB stub (`[General]
GDBPort` in Dolphin.ini). The Windows firewall blocks WSL from it, so the client is a PowerShell script
on the Windows side: research/tools/dolphin/gdbpeek.ps1 attaches, runs until a log line matches, stops
the CPU and dumps registers and the back chain (optionally logging every hit of a write watchpoint).
The game runs about 10x slower under the stub (route 1_2 to the failure takes ~25 minutes).

The runaway read is **not** in the pop-up code. At the first EFB read (TRACE 2790, same point as
always) the stack is:

    DMMorphBlendDisplayLists+0xc4   (draft, consistent with the original)
    CAnimMorph::Draw+0x184
    CSoldierObject::Draw+0x2f0
    CScene::Draw+0x1e4
    GameLoop+0x770 / main / __start

It is the speaking soldier's face morph (hence the timing with the first subtitle). The weights loop
runs `for (i = 0; i < morph->count - 1; i++)` (unsigned, as in the original) and morph->count, at
DMMorphObject_T+0x100, holds 0xBDC80001: a float (about -0.098), not a count. So a weight was written
one past `weights[64]`. LLCharObjCompositeMorph's decoder writes `weights[(entry & 63) + 1]`, which
reaches +0x100 when a target index is 63, in the original too. So either the frame data reaching it
differs in the pure image, or another writer is at fault. Next: a write watchpoint on that word
(0x81358a40 in pure_l12ct.elf's run) to catch the writer.

**Watchpoint result (07:10).** gdbpeek.ps1 with `-Watch 81358a40` (DMMorphObject count of the
speaking soldier's CAnimMorph, base 0x81357c70, m_morph at +0xcd0) logs every write:

1. boot clears (apploader, __start's .bss clear, a heap memset): 0;
2. CAnimSkinned::BindSkeleton / DMMorphInitObject: 0x40 (64), correct;
3. then, every update from CAnimMorph::UpdateAnimation: **LLCharObjCompositeMorph** (the draft,
   `consistent` with the original) storing decoded weights there: 0x3FBFC000, 0x3FB40000,
   0xBE800000, ... and finally 0xBDC80000 (= 2*(206/1024)-0.5), which DMMorphBlendDisplayLists then
   reads as the count. (One hit reported AnimFileGetStateAnimList as the PC; the value is the
   decoder's, so that PC is stale, a JIT memcheck artefact.)

The decoder writes `weights[(entry & 63) + 1]`, so these are entries whose target index is 63. The
original's code is the same, so in the original no entry has target 63: the frame data reaching the
decoder differs in the pure image. Suspects, upstream: the AnimMotFrameList_t frames' `morph` pointers
(+0x4c of each 0x50-byte frame, filled from animmot.cpp's motion decoding) or the motion file data they
point into (loader fixups, drafts). Next: a probe replacing LLCharObjCompositeMorph that OSReports
the frame list (count, each frame's weight and morph pointer, and data[0..4]) when an entry has
target 63, then follow that pointer back to its writer with another watchpoint.

Tool notes: Dolphin answers every packet with a bare `+`, so the client waits for `$` before reading
a stop reply. A watchpoint run hits boot clears first, so read past them. Restore Dolphin.ini
(GDBPort) after a session: dolphin_boot runs fail to start while the stub waits for a client.

**Root of the morph overwrite, narrowed (07:20-08:30).** Probes in research/probes/ (each compared on
the pure image and on the original-layout link_drafts image, route 1_2):

- morph63_probe.cpp (CAnimMorph::UpdateAnimation copy): the pure image's frame morph data claims
  thousands of entries (garbage); the control's has 0-3. **With the morph count put back to 64, the
  pure image runs on past update 2790 to the end of a 400 s run (trace update 106290), no EFB reads.**
  So this is the only failure on route 1_2 so far. The frame list, cache offsets (g_frame_cache +
  39948, ...) and fill level are identical in both images; the bytes differ.
- aram_upload_probe.cpp (UploadFramesToARAM copy): the one animation upload is identical (ARAM
  0x800000, 4,162,784 bytes, source 0x8236ca70, the same checksum for each 256 KB chunk), and a
  readback of ARAM 0x800000 right after it returns the source's words. The upload works.
- sndaram_probe.cpp: the sound ARAM pool is 0x4100..0x800000 in both, below the animation pool.
- ardma_probe.cpp (ARStartDMA copy): no other main-memory-to-ARAM transfer into 0x800000..0x1000000
  in the pure image.
- frame_read_probe.cpp (AnimFileGetFrame copy): the reads use the same ARAM addresses, sizes and
  frame numbers in both images, but **every read in the pure image returns 0x486e486e repeated**
  (the control's: 0x36000000, 0x38000000, ...). Invalidating the cache afterwards changes nothing.
  g_frame_cache, g_bDMAToMRAM and g_fromARAM_request overlap nothing.

So the ARAM-to-main-memory reads return a fill pattern in the pure image although the upload
landed. Open: whether the ARAM contents are overwritten after the upload some other way (ARInit's
__ARChecksize/__ARClearArea run again, which bypasses ARStartDMA; the DSP or the sound streamer
writing ARAM), or whether the reads complete (callback) without moving data (ARQ state, the read
request's type or addresses). Next checks: read back ARAM 0x800000 again just before the first frame
read; probe ARInit's callers (animdb.cpp, snddrv.c) for a second ARInit; find what fills memory with
0x486e486e.

**08:45: ARAM is intact; the ARQ read is what fails.** frame_read_probe.cpp now also reads ARAM
directly (ARStartDMA type 1 + ARGetDMAStatus poll + DCInvalidateRange) at the first frame reads:
0x800000, 0x804000 and 0xaf4e20 all return real animation data (0xaf4e20: 18000300 9eff0300 ...).
ardma_probe.cpp shows the game's own read reaching ARStartDMA with the right arguments (main memory =
the g_frame_cache slot, ARAM 0xaf4e20, 192 bytes, from ARQ). Yet DMAFrame returns with the slot
holding 0x486e486e, and invalidating the cache doesn't change it. So DMAFrame's wait
(`while (g_bDMAToMRAM)`, cleared by DMAFromARAMCallback through ARQ) ends before the transfer lands,
or the transfer's completion is routed elsewhere. The slot then holds memory never written (the pure
image's g_frame_cache is not zeroed? 0x486e486e looks like a fill pattern).
Next, in order: (1) log, in DMAFrame, the slot's first word again a few hundred ticks after the wait
ends (does the data land late?); (2) check that g_frame_cache's .bss is inside a range __start
clears; (3) compare the AR DMA callback registration order (ARQInit's ARRegisterDMACallback vs the
sound driver's ARInit(0, 0) in snddrv.c) in both images. Fixing this should let route 1_2 play
through: with the morph count patched the pure image already does.
(2) checked: __start's first .bss range (0x801d8ce0..0x80458d80) covers g_frame_cache, so the
0x486e486e in it was written after boot. Find that writer with a gdbpeek.ps1 write watchpoint on a
frame-cache slot (e.g. the first read's, g_frame_cache + 0x220 in pure_l12fr.elf).

**Found and fixed (2026-11-17, afternoon): an alignment bug, not a stray writer.** A probe dumping
the whole slot next to a direct ARStartDMA read of the same ARAM range
(research/probes/frame_async_probe.cpp) shows the slot holding the right frame data **shifted by 16
bytes** (slot[i] = aram[i + 16]); 0x486e486e is ordinary frame data at the wrong offset. The DMA
ignores the low 5 bits of the main-memory address, and the pure image's `g_frame_cache` sat at
16 mod 32 (0x80446570; the original's is 0x801c6e20). It is defined in
research/data/bss/unassigned.cpp with no alignment, so the pure link placed it on 8/16 bytes.
research/probes/ardma_seq_probe.cpp ruled out the ARQ/AR callback-order theory first: one DMA per
read, finished (`busy 0`) before DMAFromARAMCallback, nothing else in between; even the uncached
mirror read 0x486e486e.
- Fix: `__attribute__((aligned(32)))` on the research/data objects the original has 32-aligned:
  g_frame_cache, axDspSlave (DSP microcode, also a DMA source), FONTstandfile, masterEventTable,
  movementText, sndss, vgPadStatus. check_data.py: still 527 exact.
- Result: route 1_2 on the pure image (route, clock and level probes, no patch) runs the whole 400 s
  (4,492 trace lines, update 134,730), no exception, no EFB reads. Its trace, hash left out, is
  identical to the control (cm63.log) for all 3,632 of the control's lines.
- `research/tools/pure_align_check.py ELF` lists objects that lost 32-byte alignment. Left after the
  fix, all defined by verified units and none a known DMA buffer: pfstates, DCTtab0a,
  g_theCollisionTable, two vtables, aemscompinfo, ShellLibraryInfo, GXNtsc480IntDf, sndlibauthor.
- Lesson for the port: any buffer that hardware DMA touches needs explicit 32-byte alignment in
  source; the original got some of it from link order alone.
- The 25-minute watchpoint run missed: a probe edit moved the slot. Re-derive watch addresses from
  the exact ELF being run.

**W1.3 on the pure image (2026-11-17, late afternoon).** Each route linked with its input probe and
the clock probe (route 1_2 also with level_1_2.cpp), traces compared with the hash left out:
- route_1_2: identical to the control (cm63.log) for all 3,632 control lines; runs 400 s.
- route_menu: identical to the control (rp_menud_ctl.log) for all 4,414 control lines.
- m1_opening: identical to the control (rp_m1b_ctl.log) for all 748 control lines; the pure run
  reaches update 22,560 on the beach.

**pure_link now aligns data sections like the original.** `align_like_original` raises a data
section's alignment to 32 when an object of 32+ bytes in it was 32-aligned in the original and
sits at a 32-multiple offset in the section. This also aligns ICON_TEXTURE_DATA/ICON_CLUT_DATA,
g_skyBoxVertIndices/g_skyBoxUV and other GX inputs. 42 objects are still off 32 in the pure image,
all at non-32 offsets in their own sections (string literals, small tables: 32-aligned by chance
in the original). Route 1_2 re-run with it: still identical (4,440 trace lines, no EFB reads).

**Open: frame hashes differ while the logic matches.** On route_menu every FRAME hash from 1890 on
(the main menu) differs from the control; on m1_opening, 1470/1500, the menu frames, and most
in-level frames from 5640. The original-layout draft image differs only at the known 1530. The
visible difference on the menu: the pure image draws the bottom-left button help text ("BROWSE",
"SELECT", grey and fuzzy) next to the button icons, and the control shows only the icons. The
section-alignment pass doesn't change it. pure_data_check.py finds nothing new; with the worst copy
of each name instead of the best, only `Type` (4 of 16 bytes) and `g_HeightScale` (2 of 4) besides
the known RTTI words. Next: find what draws the help text (shell menu, the szLib*/images tables,
the font draw) and compare its colour/alpha inputs between images, e.g. with a gx_capture-style
probe on that draw call, or a frame-1890 GX capture of both images.

**Menu frame divergence found (2026-11-17, evening): a test-rig artefact, not a pure-image bug.**
- Probes on TibTextDraw and CFont::Link (scratch only) showed both images give the help text the
  same string, font, colour, glyph records, texture/TLUT objects and bytes, and the same GX shadow
  state (gxData); forcing every unshadowed GX state still left them different, and solid quads in
  place of the texture drew identically. So the GPU sampled different palettes from the same
  descriptors.
- Cause: the shell font (PopupDisplay.gfn, a C4 texture with an IA8 TLUT) sits at 0x826517e0,
  because Dolphin runs with 64 MB of MEM1 and the main heap reaches 0x827fce00. A TLUT load's
  source address is masked to 25 bits (Dolphin LOADTLUT1: `addr & 0x01FFFFFF`, as on hardware), so
  the palette was read from 0x80655a00: heap, laid out differently in each image. Dolphin's EFB
  copy and texture caches were ruled out (EFBToTextureEnable off, safe texture cache: no change).
- Fix: research/probes/arena_probe.cpp (replaces mallocinit) ends the main heap at 0x82000000;
  dolphin_boot.py keeps it in control images (PROBES). A 32 MB console doesn't work: mallocinit
  keeps 24 MB of an arena over 24 MB out of the heap, and the game hangs at boot.
- route_menu with the cap: pure and control identical for every line of the control's run
  (6,466 trace lines, every FRAME hash, the intro-movie frames 1470/1500 included).
- m1_opening with the cap: TRACE identical for 1,321 lines (player hash left out: it holds heap
  pointers); intro and menu frames now match. **Still open:** in-level frames from 5640 differ.
  Lead: the same wrap for data outside the main heap (the 24 MB above it that mallocinit holds
  back, or the OS heap), or another address-sensitive GPU input; next step is the probe approach
  above on a level draw.
