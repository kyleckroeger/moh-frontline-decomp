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

**Where it stops (2026-11-17 02:30):** the image boots through `__start`, OSInit (banner, arena),
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
