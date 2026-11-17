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
