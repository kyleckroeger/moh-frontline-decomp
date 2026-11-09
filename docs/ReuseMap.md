# Reuse index and next work

How to choose the next reconstruction target for `Moh2RelGC.elf` (GMFE69). This
is a **research lead index**, not accepted source: a symbol or file-name match
earns no credit, and only the complete-image comparison in
`tools/reconstruct.py` accepts a unit. Sizes below were measured on 2026-11-09 and
will move as units are accepted.

`docs/Remaining.md` sets the priority order and has the current figures. Read
`docs/Roadmap.md` for the plan and `docs/Licensing.md` for reference terms.

## A compiler caveat that shapes every reference

Frontline is **Metrowerks CodeWarrior**, not SN/ProDG. That splits the useful
references in two:

- **Byte-match candidates** — references built with CodeWarrior/MWCC
  (dolsdk2001/2004, other GC MWCC games). These can sometimes drop in directly.
- **Semantic references only** — the sibling *Rising Sun* game code and its
  libraries are **ProDG/SN**. Their source describes behaviour, but its bytes
  will not match Frontline and its Newlib/libgcc references do not apply here.
  Frontline's C runtime is MSL and its C++ runtime is the MW PPC EABI
  (`__register_global_object`, `__construct_array`), not GCC libgcc.

A name match is a lead; only generated code and data checked byte-for-byte
against GMFE69 accepts source.

## Reference catalog

| Reference | Covers | Built with | Confidence | Notes |
| --- | --- | --- | --- | --- |
| [dolsdk2001](https://github.com/doldecomp/dolsdk2001) | Dolphin SDK | MWCC GC | High | In use; Dec 2001 sits between this and 2004 |
| [dolsdk2004](https://github.com/doldecomp/dolsdk2004) | Dolphin SDK | MWCC GC | High | In use for the units that keep 2004 behaviour |
| Other MWCC GC decomps (e.g. [Prime](https://github.com/PrimeDecomp/prime), [TWW](https://github.com/zeldaret/tww), [mkdd](https://github.com/doldecomp/mkdd), [melee](https://github.com/doldecomp/melee)) | MSL, CW runtime and MetroTRK | MWCC GC | High | In use: Wind Waker and Prime are the base of the accepted runtime units (`docs/Runtime.md`) |
| [NFS Most Wanted reconstruction](https://github.com/dbalatoni13/nfsmw) | EAGL animation/loading/rendering, EA engine | MWCC GC | Medium-High | Same EA engine family; check layouts against GMFE69 before trusting |
| [Rising Sun](https://github.com/lifewillbeokay/moh-rising-sun) (GR8E69) | EA engine, game C++, Lua/EAGL | ProDG (game) / MWCC (SDK) | Medium | Semantic source; game bytes will not match. Its [ReuseMap](https://github.com/lifewillbeokay/moh-rising-sun/blob/main/docs/ReuseMap.md) shows the method |
| MOH Frontline PS2 (SLUS) | Same game, formats and names | different arch/compiler | Medium | Naming/format leads only; check independently against GMFE69 |
| Lua 4.0.1 / STLport 4.5.3 | Script/containers, *if used* | source | Unknown | Applicability to Frontline unconfirmed |

Preserve attribution and license terms for anything adapted; record provenance
in the unit manifest.

## Backlog 1 — remaining libraries

The Dolphin SDK, MSL, MW runtime and MetroTRK are accepted, apart from
`TRKSuppAccessFile`. EA audio is in Backlog 3.

## Backlog 2 — high fan-in helpers

Ranked by unique unfinished callers (`tools/dependencies.py`). Fan-in is now low:
everything not listed has one unfinished caller or none.

| Callers | Bytes | Symbol |
| ---: | ---: | --- |
| 4 | 2,728 | `CStaticObjectFactory::AllocateStaticObject` |
| 2 | 1,220 | `CInputManager::Update` |
| 2 | 912 | `AddBuddy` |
| 2 | 416 | `CFont::DrawTextCentered` |

## Backlog 3 — game code by file

Largest unfinished files, by function bytes in the snapshot's code map. No public
source exists for most of them, and several need a whole-file unit because their
functions share one `...rodata.0` or `...bss.0` block. Every function has a draft to
start from (`research/drafts/`).

| Bytes | File | Family |
| ---: | --- | --- |
| 19,496 | `bsbifunc.cpp` | Behaviour script built-ins |
| 11,740 | `cdbgeom.cpp` | Collision/geometry |
| 10,948 | `objcreate.cpp` | Object creation |
| 8,312 | `buddy.cpp` | AI squad |
| 8,148 | `particlesystem.cpp` | Particles |
| 7,784 | `ShellMenu.cpp` | UI |
| 7,300 | `compartment.cpp` | Rendering |
| 6,352 | `box.cpp` | Math/bounds |
| 6,104 | `staticobjfactory.cpp` | World objects |
| 5,684 | `UserInterface.cpp` | UI |
| 5,608 | `proxtrig.cpp` | Triggers |
| 5,308 | `Soldier_object.cpp` | Actor |
| 5,088 | `path.c` | EA audio (music paths) |

Seven unfinished functions (about 3 KB) have no recovered source file and need
`tools/file_map.py` evidence before a source boundary is assigned.

## Cross-binary matching (Phase C)

Auto-transfer names and structures between binaries with Ghidra Version
Tracking or Diaphora, comparing GMFE69 against:

1. **NFS Most Wanted GC** and other MWCC EA GC titles — closest byte-level
   engine references.
2. **MOH Rising Sun GC** — same engine lineage, different compiler.
3. **MOH Frontline PS2** — same game, different architecture.

A moved name is a hypothesis; confirm the function's behaviour, size and bytes
on GMFE69 before writing source. Keep decompiler output under `scratch/`.

## Maintenance

- Regenerate the numbers with `tools/code_map.py` / `tools/dependencies.py`
  and the snapshot (`tools/progress_report.py --capture`).
- When a reference yields accepted bytes, record it here with the exact scope
  and the evidence, and update the unit manifest's `upstream` provenance.
- References that do not pan out should be marked as tried, not deleted.
