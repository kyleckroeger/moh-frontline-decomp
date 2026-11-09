# Initial audit

Target: **Medal of Honor: Frontline**, GameCube, USA, disc ID `GMFE69`,
disc 0, revision 0. Findings below come from the pinned executables only.

## Disc executables

| File | Size | Role |
| --- | --- | --- |
| `boot.dol` / `Moh2BootRel.dol` | 168,544 | Boot program (identical bytes); loads the game ELF |
| `Moh2RelGC.elf` | 2,866,920 | Game executable; the decompilation target |
| `Moh2StubRelGC.elf` | 278,096 | Loader stub at `0x80680000` (`loadelf.cpp`, `loader.cpp`, Dolphin SDK) |

This mirrors Rising Sun's `boot.dol` → `MOH3RDVD.ELF` / `STUBRDVD.ELF` layout.

## `Moh2RelGC.elf`

- Entry `__start` at `0x80007140`; `_SDA_BASE_` `0x80355960`; `_SDA2_BASE_` `0x80357520`.
- Allocated sections: `.init`, `extab`, `extabindex`, `.text`, `.ctors`,
  `.dtors`, `.rodata`, `.data`, `.bss`, `.sdata`, `.sbss`, `.sdata2`, `.sbss2`.
- Executable bytes (progress denominator): `.init` 9,452 + `.text` 1,405,120
  = **1,414,572**.
- 18,811 symbols: 5,005 functions, 10,560 objects, 557 file records.
- `.rela.*` sections are preserved for code and data. `.debug` and `.line` are
  empty: there is no DWARF type information.

### Compiler

Metrowerks CodeWarrior, from: `extab`/`extabindex` exception tables,
`.mwcats.text`, `__sinit_<file>_cpp` static initializers, CodeWarrior C++ name
mangling, and the MW linker-generated `_rom_copy_info`, `_bss_init_info` and
`_eti_init_info` tables. This audit did not establish the exact compiler
version and flags; [Game.md](Game.md), [Dolphin.md](Dolphin.md) and
[Runtime.md](Runtime.md) have the profiles units use. Rising Sun is a useful reference: its Dolphin SDK units match
with CodeWarrior `GC/1.2.5n`, but its game code is ProDG, which does not apply here.

### Rough composition

Heuristic grouping by file name, using dtk's preliminary `.text` ranges (see
the caveat below). Use it for orientation only.

| Group | Files | Share of `.text` |
| --- | ---: | ---: |
| C++ (`.cpp`): game code and EA C++ libraries | 240 | ~72% |
| Dolphin SDK | 90 | ~11% |
| EA audio (`s*.c`, `SNDI_*`) | 129 | ~8% |
| Other C (UI studio `UIS*.c`, `mainloop.c`, `DSPCode.c`, …) | 15 | ~4% |
| MSL / MW runtime | 36 | ~3% |
| MetroTRK debugger | 22 | ~2% |
| MSL math | 25 | ~1% |

`gc_idct.asm` is a hand-written assembly file record.

## Baseline relink

`tools/baseline.py` reproduces the complete ELF-derived DOL byte for byte:

1. `dtk elf2dol` produces the reference DOL (SHA-1 `0e96c0be…`, 1,641,760 bytes).
2. `dtk elf config` exports the original symbols. `_ctors`/`_dtors` are dropped
   because dtk recreates them for DOL inputs and rejects duplicates.
3. `dtk dol split` produces **one** object covering every allocated section.
4. GNU `ld` relinks it at the original section addresses. Symbols that dtk leaves
   undefined (MW linker symbols such as `_stack_addr`, `__ArenaLo`) are pinned to
   their unique original definitions.
5. dtk omits the bytes of MW linker tables. These are included from the original:
   `_rom_copy_info` + `_bss_init_info` at the end of `.init` (164 bytes),
   `_eti_init_info` at the end of `extabindex` (32 bytes), and the null
   terminators of `.ctors` and `.dtors` (4 bytes each). The script fails if
   anything other than these pinned tables is missing.

The relink proves the toolchain and layout. It earns no source credit.

## Per-file splits

`dtk elf config` alone attributes every global without a file-local anchor to
the last file record (`sdfx.c`), which makes the link order cyclic. Its other
problems are empty ranges, ranges that run into the next section's padding, and
a range wrapping to `0x0` at the end of `.sbss2`.

`tools/file_map.py` recovers ownership from the original instead
(`build/audit/file_map.json`):

| Rule | Evidence | Symbols placed |
| --- | --- | ---: |
| local-symbol | Private symbols follow their file record | 10,282 |
| verified-unit | The item lies inside a byte-verified unit (`config/GMFE69`) | 662 |
| verified-unit (no file record) | Inside a verified unit whose file has only global symbols, so no record | 40 |
| exception-index | A function's private `extabindex` entry points to it | 2,230 |
| private-reference | A global's bytes refer to one file's private symbol | 257 |
| same-address | An object starts at a file's private section marker | 11 |
| table-target | A `.ctors`/`.dtors` word goes with the routine it calls | 51 |
| between-same-file | Contributions follow file-record (link) order in every section | 1,057 |
| same-class (inferred) | A C++ method goes with its class's other methods | 134 |
| between-same-file (inferred) | Link order around inferred placements | 85 |
| unresolved |  | 953 |

The link-order rule holds without exception: file contributions appear in
file-record order in every section except `.ctors`/`.dtors`, where the MW
runtime's entries come first (`__init_cpp_exceptions.cpp`,
`global_destructor_chain.c`; renamed to `.ctors$10`, `.dtors$10`/`$15` as in
other CodeWarrior decompilations). Against the functions of every verified unit,
the strong rules have placed every function correctly. `file_map.py` and the
progress snapshot's code map also take the verified units as evidence (a
contradiction is an error); `baseline.py`'s split does not, so it depends on
the original only. The remaining unresolved code is mostly EA's sound and system
C libraries, whose files have only global symbols and so no record; file
boundaries leave no padding in `.text`. A sole-caller rule was
tried and rejected, scoring 2 of 6.

`baseline.py` now splits the image into **535 objects**: 357 per-file objects
plus 178 dtk auto-units for unowned stretches. GNU `ld` relinks them into the
identical DOL. For the split only, unowned stretches that cannot start on the
section alignment are attached to the preceding file. These are listed as
`split_guesses` and do not affect the code map. Before linking, global names
containing `@` (MW thunks, globalized `@N` labels) are renamed, because GNU `ld`
would read them as symbol versions. `--whole-image` keeps the previous
single-object check.
