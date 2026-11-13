# Medal of Honor: Frontline

A matching decompilation of **Medal of Honor: Frontline** for GameCube, targeting the USA release **GMFE69, revision 0**. The game-code target is the disc's `Moh2RelGC.elf`.

[![Verified progress snapshot](https://github.com/kyleckroeger/moh-frontline-decomp/actions/workflows/progress.yml/badge.svg)](https://github.com/kyleckroeger/moh-frontline-decomp/actions/workflows/progress.yml)

The project follows the structure and verification rules of the sibling [Medal of Honor: Rising Sun decompilation](https://github.com/lifewillbeokay/moh-rising-sun), whose CC0 tooling it adapts. Accepted source is checked against the original executable as part of a complete rebuilt-image comparison; the remainder stays original binary context. AI-assisted contributions are welcome; see [CONTRIBUTING.md](CONTRIBUTING.md).

The project reports two figures. **Exact** is the verified progress figure: nonoverlapping source-built function bytes against all bytes in the original executable sections (`.init` and `.text`). Data and BSS earn no code-progress credit; neither do linker-generated tables or original context. **Linked** adds partial source that isn't byte-identical yet but links into a separate, non-matching build. See [how progress is verified and published](docs/Progress.md).

## Status

<!-- sync_docs:status -->
| Measure | Code bytes | Share | What it means |
| --- | ---: | ---: | --- |
| **Exact** | 1,278,192 | **90.36%** | Built from source and byte-identical to the original in the complete-image comparison: 4,905 functions in 1,254 units. |
| **Linked** | 1,414,400 | **99.99%** | The exact code plus 195,128 bytes (13.79%) of partial source in 170 functions. It links into a non-matching build that reaches the main menu in Dolphin (gameplay untested), so linked doesn't mean correct. |

Both are shares of the 1,414,572 code bytes in `.init` and `.text`. Only exact bytes count as verified progress.

Exact bytes by category:

| Category | Code bytes | Functions | Units | Compilers |
| --- | ---: | ---: | ---: | --- |
| `reconstructed_game` (game code and EA engine) | 1,039,132 | 3,910 | 1,070 | `GC/1.3`, `GC/1.3.2` |
| `restored_library` (Dolphin SDK, MSL, MW runtime, EA audio libraries) | 239,060 | 995 | 184 | `GC/1.2.5`, `GC/1.2.5n`, `GC/1.3`, `GC/1.3.2` |

About 136.2 KB isn't exact yet, in 104 functions across 55 source files; [Remaining.md](docs/Remaining.md) breaks it down by area. Another 172 bytes are outside any function symbol, which is why linked stops short of 100%.
<!-- /sync_docs:status -->

- Most game code is accepted as **fragment units**: contiguous runs of functions from one original source file, verified in place while the rest of that file remains original binary context. Fragments grow and merge as neighbouring functions are matched.
- What remains is mostly register-allocation and stack-layout near misses, very large functions, and code whose types are still uncertain. Every remaining function has a draft in [`research/drafts/`](research/README.md), and those drafts are what the linked build is made from.
- Neither build has been runtime-tested.

Most recent work is AI-assisted (Claude Code agents working against the same verification gate as any contributor); see [CONTRIBUTING.md](CONTRIBUTING.md) for the rules every accepted unit follows.

## Target facts

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

`tools/sync_docs.py` regenerates this page's status and the other docs' figures and unit tables from that snapshot. A test fails if they fall behind.

## Contributing units

`tools/port_unit.py` drafts a unit manifest from a source file by matching its relocations to the original's, and `tools/unit_diff.py` shows per-function disassembly differences after a failed verification. After a build, `tools/objdiff_config.py` writes a git-ignored `objdiff.json` for local per-function diffing. See [Dolphin SDK units](docs/Dolphin.md) for the workflow, compiler profile and the CodeWarrior linker behaviors the verifier models, [Game.md](docs/Game.md) for the game-code method, and [Remaining.md](docs/Remaining.md) for priorities and which source forms are accepted.

## Research

[`research/`](research/README.md) holds everything learned so far that isn't accepted source: the full work logs, matching tricks, the best draft of each open function and the scripts behind them. Search it before starting on a function; most have earlier attempts.

## Next work

The matching source is meant to be the base of a native PC port without multiplayer, kept separate from the matching build, so the remaining work is ordered by what that port needs ([details](docs/Remaining.md)):

1. **Collision, scripting, AI and animation.**
2. **Everything else that isn't multiplayer:** rendering, shell and HUD, audio, video and system code.
3. **Multiplayer**, last, for the completeness of the decompilation.

Alongside that:

- **Whole files.** Merge fragments into complete source files as the gaps between them close, and resolve the file boundaries still marked uncertain in the [audit](docs/initial-audit.md#per-file-splits).
- **Runtime testing** of the linked build in Dolphin ([how it's built and its known risks](docs/Remaining.md#two-progress-figures)).

## License

Project-written tooling, documentation and configuration are dedicated to the public domain under [CC0 1.0](LICENSE). The tooling is adapted from the CC0 [moh-rising-sun](https://github.com/lifewillbeokay/moh-rising-sun) project. See [licensing scope](docs/Licensing.md). Bring your own game copy; this repository contains no game images, executables, assets or compiler binaries.
