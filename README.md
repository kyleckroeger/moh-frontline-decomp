# Medal of Honor: Frontline

An early matching decompilation of **Medal of Honor: Frontline** for GameCube, targeting the USA release **GMFE69, revision 0**. The game-code target is the disc's `Moh2RelGC.elf`.

[![Verified progress snapshot](https://github.com/kyleckroeger/moh-frontline-decomp/actions/workflows/progress.yml/badge.svg)](https://github.com/kyleckroeger/moh-frontline-decomp/actions/workflows/progress.yml)

The project follows the structure and verification rules of the sibling [Medal of Honor: Rising Sun decompilation](https://github.com/lifewillbeokay/moh-rising-sun), whose CC0 tooling it adapts. Accepted source is checked against the original executable as part of a complete rebuilt-image comparison; the remainder stays original binary context. AI-assisted contributions are welcome; see [CONTRIBUTING.md](CONTRIBUTING.md).

Progress counts verified, nonoverlapping source-built function bytes against all bytes in the original executable sections (`.init` and `.text`). Data and BSS earn no code-progress credit; neither do linker-generated tables or original context. See [how progress is verified and published](docs/Progress.md).

## Verified starting point

- `Moh2RelGC.elf`: 2,866,920 bytes; SHA-1 `eb75ebc0073db35e75eba06742b151591d36c004`.
- 18,811 symbol entries, including 5,005 function entries and 557 source-file entries.
- **Relocations are preserved** (`.rela.text`, `.rela.data`, …), so every cross-reference in the image is known exactly. Rising Sun's target has no such tables.
- Compiled with Metrowerks CodeWarrior (`extab`/`extabindex`, `.mwcats.text`, `__sinit_*_cpp`), not ProDG. No DWARF is present.
- The original-image relink (535 per-file objects) matches the complete 1,641,760-byte DOL derived from the ELF, including its header. Every original allocated ELF file byte, entry point and BSS extent is checked separately.
- Accepted Dolphin SDK units are verified inside the same complete-image comparison.

This comparison is against a derived analysis image. It does **not** mean the original ELF's symbol tables have been reproduced, the on-disc boot DOL has been replaced, or the game has been tested in an emulator. The boot DOL (`boot.dol`, identical to `Moh2BootRel.dol`) is a separate 168,544-byte program that loads the game ELF. See the [initial audit](docs/initial-audit.md) for details.

## Local setup

Requires Python 3.9+ on Linux x86-64 (including WSL2) or macOS ARM64. Tools are downloaded into ignored `build/tools/` and `build/compiler/` with pinned SHA-256 checksums; on Linux, wibo runs the Windows compilers natively.

```sh
python3 tools/import_disc.py "/path/to/Medal of Honor - Frontline (USA).iso"
python3 tools/setup.py
python3 tools/audit.py
python3 tools/baseline.py
python3 tools/reconstruct.py
python3 -m unittest discover -s tests -v
```

The importer reads the standard GameCube filesystem table of an uncompressed image (convert RVZ/NKit with Dolphin first). It extracts only the pinned executables into `orig/GMFE69/`, checks their sizes and SHA-1/SHA-256 digests, and leaves the input image untouched.

Run the scripts from any directory; all outputs remain inside this checkout. `baseline.py` and `reconstruct.py` recreate their own build directories, so keep experiments under `scratch/`.

## Outputs

| Path | Contents |
| --- | --- |
| `config/GMFE69/target.json` | Executable identities and hashes |
| `config/GMFE69/baseline.json` | Entry point, SDA bases and comparison layout |
| `config/GMFE69/project.json` | Accepted source units and the progress denominator |
| `build/audit/summary.json` | Section and symbol statistics |
| `build/audit/symbols.json` | Complete local symbol inventory |
| `build/audit/elf-config/` | dtk's own symbols and splits (research input) |
| `build/audit/file_map.json` | Recovered per-file ranges with evidence |
| `build/baseline/report.json` | Original-image relink result |
| `build/reconstruction/report.json` | Complete source-build result |
| `progress/GMFE69.snapshot.json` | Public, locally verified progress snapshot |

## Contributing units

`tools/port_unit.py` drafts a unit manifest from a source file by matching its relocations to the original's, and `tools/unit_diff.py` shows per-function disassembly differences after a failed verification. After a build, `tools/objdiff_config.py` writes a git-ignored `objdiff.json` for local per-function diffing. See [Dolphin SDK units](docs/Dolphin.md) for the workflow, compiler profile and the CodeWarrior linker behaviors the verifier models.

## Next work

1. **Resolve the remaining file boundaries.** `tools/file_map.py` places most symbols in their source files from symbols, relocations and link order, and the per-file split relinks identically. About 950 symbols near file boundaries remain unresolved, mostly in EA's sound and system libraries (see the [audit](docs/initial-audit.md#per-file-splits)); verified units settle them.
2. **More Dolphin SDK units.** 81 SDK units are verified (79 with CodeWarrior `GC/1.2.5n`, 2 with `GC/1.2.5`); the [open problems](docs/Dolphin.md#open-problems) list what blocks the rest.
3. **Metrowerks runtime libraries.** 64 MSL, runtime, math, MetroTRK and debugger-stub units are verified from the Wind Waker and Prime reconstructions; [Runtime.md](docs/Runtime.md) lists the rest, including MetroTRK.
4. **Game code.** The compiler profile is established; `bstimer.cpp`, a fragment of `propdat.cpp` and five small EA library files are verified; [Game.md](docs/Game.md) has the method.
5. **Mutation tests** for accepted units (see `tests/test_reconstruction.py`).

## License

Project-written tooling, documentation and configuration are dedicated to the public domain under [CC0 1.0](LICENSE). The tooling is adapted from the CC0 [moh-rising-sun](https://github.com/lifewillbeokay/moh-rising-sun) project. See [licensing scope](docs/Licensing.md). Bring your own game copy; this repository contains no game images, executables, assets or compiler binaries.
