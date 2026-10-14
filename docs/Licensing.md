# Licensing

Original contributions to this project are dedicated to the public domain under
[CC0 1.0 Universal](../LICENSE), to the extent their contributors hold copyright
and related rights in those contributions. This includes project-written tooling,
documentation, configuration and original reconstruction work, subject to the
third-party exclusions below. CC0 includes a fallback license where its waiver
cannot take full effect; the unmodified legal text is in the root `LICENSE`.

The build, verification and progress tooling under `tools/` and `tests/` is
adapted from [moh-rising-sun](https://github.com/lifewillbeokay/moh-rising-sun),
which is also dedicated under CC0 1.0. Credit is retained here and in the README.

## Third-party material

The root dedication does not relicense imported source, headers, library code or
their adaptations. Their existing terms and notices continue to apply. A file's
presence here, or its matching the original executable, does not establish
permission to reuse it under CC0. Where a reference supplies no license, this
project grants no rights in that reference's material.

| Material | Applicable notices and provenance |
| --- | --- |
| Dolphin SDK reconstructions in `src/dolphin/` and headers in `include/dolphin-sdk/` | Copied unchanged from [moh-rising-sun](https://github.com/lifewillbeokay/moh-rising-sun) commit `a701671cad4189cee989327ef33176a7289bac4e`. They keep that project's [SDK notice](../src/dolphin/NOTICE) and network notice; the Prime-derived subset retains [LICENSE.PrimeDecomp](../src/dolphin/LICENSE.PrimeDecomp) and the BFBB-derived subset [LICENSE.bfbb](../src/dolphin/LICENSE.bfbb). The upstream dolsdk2004 reference has no repository-wide license grant |
| `src/dolphin/mtx/mtx.c` and `src/dolphin/os/OSMemory.c` | Imported directly from [dolsdk2004](https://github.com/doldecomp/dolsdk2004) commit `2328b4164b1a98422a2255d83ce9a5a7548990cc`, the same upstream as the rest of `src/dolphin/`; no repository-wide license grant. Local changes are in `docs/Dolphin.md` |
| Dolphin SDK 2001 reconstructions in `src/dolsdk2001/` and headers in `include/dolsdk2001/` | Copied from [dolsdk2001](https://github.com/doldecomp/dolsdk2001) commit `eb1234c45e6df75757c652c835507ca89674f9a8`, which has no license file; see [its notice](../src/dolsdk2001/NOTICE). Local changes are in `docs/Dolphin.md` |
| MSL, runtime, MetroTRK and debugger-stub reconstructions in `src/tww/` and headers in `include/tww/` | Copied from [The Wind Waker decompilation](https://github.com/zeldaret/tww) commit `a1854d47ce5aa9d0f21c7fb5a71aaade63659693`, dedicated under CC0 1.0 ([LICENSE.tww](../src/tww/LICENSE.tww), [notice](../src/tww/NOTICE)). Local changes are in `docs/Runtime.md` |
| MSL and runtime reconstructions in `src/prime/runtime/` and headers in `include/prime/` | Copied from [the Metroid Prime decompilation](https://github.com/PrimeDecomp/prime) commit `32020a07e571ccf6fa42329b4c09c4514daed449`, dedicated under CC0 1.0 ([LICENSE.PrimeDecomp](../src/prime/LICENSE.PrimeDecomp), [notice](../src/prime/NOTICE)). Local changes are in `docs/Runtime.md` |
| MSL reconstructions in `src/msl/` (files no reference matches) | Original reconstruction work, dedicated under this project's CC0. Where a file follows a reference (`uart_console_io.c` follows the Wind Waker decompilation, CC0), its header comment says so. See `docs/Runtime.md` |
| Game code reconstructions in `src/game/` and EA library reconstructions in `src/ea/` | Original reconstruction work, dedicated under this project's CC0. Structure and descriptive names may follow [moh-rising-sun](https://github.com/lifewillbeokay/moh-rising-sun)'s game reconstructions (CC0 1.0); each unit's `adapted_from` names the files. See `docs/Game.md` |
| MPEG-2 decoder files in `src/ea/` (`idct.cpp`, `recon.cpp`, `motion.cpp`, `getvlc.cpp`, `getbits.cpp`, `getblk.cpp`, `getpic.cpp`, and later `gethdr`) | Adapted from the MPEG Software Simulation Group's `mpeg2decode` reference decoder (Copyright (C) 1996, MPEG Software Simulation Group). Each file keeps MSSG's copyright notice and disclaimer of warranty verbatim; the notice grants use without fee on an "as is" basis and warns that MPEG-2 implementations may be subject to patent royalties. The project's CC0 dedication does not apply to this material. Reference copy: [phantomuserland `phantom/apps/mpeg/`](https://github.com/dzavalishin/phantomuserland/tree/0fa666cb50e6ca13dd048d67d397485266c0e814/phantom/apps/mpeg) |

Each accepted unit manifest under `config/GMFE69/` records its upstream source in
`upstream` and, when it follows Rising Sun, its provenance in `adapted_from`:
game units list the Rising Sun source files they follow in `paths`, and library
units copied from Rising Sun name the Rising Sun manifest they were copied from.
A unit drafted from one of this project's own manifests (`port_unit --like`
used only for compiler settings) has no `adapted_from`;
`tests/test_provenance.py` checks these rules. Preserve all notices when reusing
these components. When adding other third-party material, add a row here.

## Original game and tools

This dedication grants no rights in the original game, executable code, assets,
Nintendo SDK material, trademarks or third-party compiler/tool binaries. Those
rights remain with their respective holders. Bring your own game copy; original
images, binaries and assets remain outside the repository. Downloaded build tools
retain their own licenses.

## Contributions

Submit original contributions under the project's CC0 dedication, only for rights
you are entitled to dedicate. For third-party material or adaptations, preserve
the applicable upstream terms and record the source, revision and modifications.
Identify any licensing uncertainty during review. AI-assisted contributions follow
the same requirements; see [CONTRIBUTING.md](../CONTRIBUTING.md).
