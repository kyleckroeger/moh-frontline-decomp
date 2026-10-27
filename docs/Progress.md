# Progress reports

The project exports the [objdiff report v2 schema](https://github.com/encounter/objdiff/blob/0c48d711c7bd51f791b353d7d85ba948b277e2f2/objdiff-core/protos/report.proto)
using its own byte-verification tooling, adapted from
[moh-rising-sun](https://github.com/lifewillbeokay/moh-rising-sun). CI generates
`build/progress/report.json` from the committed snapshot.

## Verification boundary

The public workflow validates a **locally verified snapshot**. It does not have
the original executables and does not rerun the proprietary compiler toolchain.
`progress/GMFE69.snapshot.json` records hashes of every source file, header,
configuration file and build script, plus verified unit counts and object hashes.
It also includes an executable-symbol map for unfinished code, generated from the
pinned original during capture.
Changes, additions and removals in those directories make CI fail until someone
with the pinned original completes verification and refreshes the snapshot.
Documentation-only changes do not require rebuilding.

To refresh after an accepted source/tooling change, on a supported local machine:

```sh
python3 tools/reconstruct.py
python3 -m unittest discover -s tests -v
python3 tools/progress_report.py --capture
```

Capture rechecks the complete rebuilt image against its pinned identity, original
load layout, source ranges and compiled unit hashes/bytes. It emits only selected
public metadata, never original bytes or local filesystem paths. Commit the
snapshot with the source change. Maintainers must review snapshot updates;
fingerprints detect stale inputs, not fabricated assertions or a malicious verifier.

CI runs the tests, checks the complete input inventory and generates the objdiff
report. Anyone can reproduce that public export without a game image:

```sh
python3 tools/progress_report.py
```

## Extab padding patches

MWCC leaves two alignment bytes uninitialized in some exception-table actions:
the padding after the registers of a `new`/`new[]` cleanup (`DELETEPOINTERCOND`,
action byte `0x8b`) and after the unused byte of a `catch (...)` block
(`CATCHBLOCK_32`, `0x90`), each just before a 4-byte pointer. The original build
holds other leftover values there that no compiler version or source form
reproduces. Approved by the user on 2026-10-27, `tools/project_build.py` patches
those two bytes, and nothing else, after compiling and before linking
(`tools/extab_patch.py`), for the functions listed in
`config/GMFE69/extab_patches.json`. Each entry names the unit, the function, the
offset of the field from the start of the function's extab record (found through
its `extabindex` entry), the action byte, our compiler's bytes and the original bytes. The build
fails unless the offset is the padding of such an action that the record's code
ranges reach and the bytes there are exactly the listed compiler output, so a
source change cannot be hidden by a patch. `compiled.o` and its hash stay the
unpatched compiler output, the report lists each unit's patches, and the patched
bytes are verified like every other byte; there are no masks.

| Unit | Function | Action | Offset | Compiled | Original |
| --- | --- | --- | --- | --- | --- |
| `AISplinePath_finalpoint` | `__ct__20CAISplinePathManagerFUl` | `0x8b` | `0x16` | `066c` | `2500` |
| `AIObject_searchnodes` | `AddChildrenToSearchNodeQueue__9CAIObjectFP11CSearchNodePC9CPathNodeR16CSearchNodeQueueR15CSearchNodeList` | `0x90` | `0x1a` | `ff7f` | `2500` |
| `weaponfactory_params` | `LoadWeaponObjects__14CWeaponFactoryFv` | `0x8b` | `0x16` | `066c` | `2500` |
| `AnimObjFactory` | `Init__18CAnimObjectFactoryFUlPc` | `0x8b` | `0x16` | `066c` | `2500` |

## Counting rules

The overall denominator is every allocated executable byte in the pinned
`Moh2RelGC.elf`: 1,414,572 bytes (`.init` 9,452 + `.text` 1,405,120). Only
nonoverlapping, verified source-built function bytes earn credit. Restored library
code and reconstructed game code are distinct categories. Accepted units may be
fragments of an original translation unit.

Unreconstructed code is represented by named file groups, explicitly unassigned
function groups, shared entry-point groups, and unidentified code/padding ranges,
all with zero matching credit. Their ranges plus accepted source must partition
the complete executable with no gaps or overlaps. File groups come from the
recovered file map (see the [initial audit](initial-audit.md#per-file-splits)).
Groups are labeled `[local symbols only]`, `[symbol evidence]` or
`[inferred file group]`; functions without a recovered file stay under
"Unknown file". Data, BSS, linker tables and original
context earn no code credit. No fuzzy partial-match credit is used. Runtime is untested.
