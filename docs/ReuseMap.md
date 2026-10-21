# Reuse index and next work

How to choose the next reconstruction target for `Moh2RelGC.elf` (GMFE69). This
is a **research lead index**, not accepted source: a symbol or file-name match
earns no credit, and only the complete-image comparison in
`tools/reconstruct.py` accepts a unit. Sizes below come from the current
`progress/GMFE69.snapshot.json` and will move as units are accepted.

Read `docs/Roadmap.md` for the plan and `docs/Licensing.md` for reference terms.

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

## Status (from the verified snapshot, 2026-10-21)

| Category | Bytes |
| --- | ---: |
| Accepted `reconstructed_game` | 684,820 |
| Accepted `restored_library` | 225,408 |
| Unfinished (functions with symbols) | 495,328 |
| Unfinished padding and unsymbolized bytes | 9,016 |
| **Total executable** | **1,414,572** |

## Reference catalog

| Reference | Covers | Built with | Confidence | Notes |
| --- | --- | --- | --- | --- |
| [dolsdk2001](https://github.com/doldecomp/dolsdk2001) | Dolphin SDK | MWCC GC | High | In use; Dec 2001 sits between this and 2004 |
| [dolsdk2004](https://github.com/doldecomp/dolsdk2004) | Dolphin SDK | MWCC GC | High | In use for the units that keep 2004 behaviour |
| Other MWCC GC decomps (e.g. [Prime](https://github.com/PrimeDecomp/prime), [TWW](https://github.com/zeldaret/tww), [mkdd](https://github.com/doldecomp/mkdd), [melee](https://github.com/doldecomp/melee)) | MSL and CW runtime functions | MWCC GC | High | Best lead for `__dl__FPv`, `__nw__FUl`, `__register_global_object`, `__construct_array`, `memset`, `strlen`, `sprintf` |
| [NFS Most Wanted reconstruction](https://github.com/dbalatoni13/nfsmw) | EAGL animation/loading/rendering, EA engine | MWCC GC | Medium-High | Same EA engine family; check layouts against GMFE69 before trusting |
| [Rising Sun](https://github.com/lifewillbeokay/moh-rising-sun) (GR8E69) | EA engine, game C++, Lua/EAGL | ProDG (game) / MWCC (SDK) | Medium | Semantic source; game bytes will not match. Its [ReuseMap](https://github.com/lifewillbeokay/moh-rising-sun/blob/main/docs/ReuseMap.md) shows the method |
| MOH Frontline PS2 (SLUS) | Same game, formats and names | different arch/compiler | Medium | Naming/format leads only; check independently against GMFE69 |
| Lua 4.0.1 / STLport 4.5.3 | Script/containers, *if used* | source | Unknown | Applicability to Frontline unconfirmed |
| [MetroTRK](https://github.com/encounter) | Debugger stubs | MWCC | Low-Medium | Confirm the target really links these units first |

Preserve attribution and license terms for anything adapted; record provenance
in the unit manifest.

## Backlog 1 — remaining libraries

The Dolphin SDK, MSL, MW runtime and EA audio libraries are almost all
accepted. What is left is blocked (see the open problems in `docs/Dolphin.md`:
`PPCArch` / `odenotstub` linked-symbol differences) or depends on game units.

## Backlog 2 — high fan-in helpers

Ranked by unique unfinished callers (`tools/dependencies.py`, 2026-10-21). The
common helpers of the first ranking (`__dl__FPv`, `CMatrix`, `MEM_free`,
`memset`, `sprintf`, …) are all accepted; the remaining fan-in is low.

| Callers | Bytes | Symbol |
| ---: | ---: | --- |
| 11 | 308 | `TLT_LoadFileFromLevelBigFile` |
| 8 | 1,220 | `CInputManager::Update` |
| 7 | 416 | `CFont::DrawTextCentered` |
| 6 | 3,328 | `MohDVDErrorTask` |
| 6 | 360 | `MEM_resize` |
| 5 | 1,176 | `BIG_locateentryz` |
| 4 | 188 | `CVector3::Normalize` |
| 4 | 2,728 | `CStaticObjectFactory::AllocateStaticObject` |
| 4 | 492 | `ForceUpdateProximityTriggerStatus` |
| 4 | 292 | `CParticleSystem::Create` |
| 4 | 464 | `AnimChanStartAnim` |
| 4 | 604 | `CCoverPoint::GetModifiedPosition` |

## Backlog 3 — game code by file

Largest unfinished files (function bytes not yet in a unit, by
`build/audit/file_map.json` ranges). No public source exists for most of
them; the work is Ghidra-led reconstruction, and several need a whole-file
unit because their functions share one `...rodata.0` or `...bss.0` block.

| Bytes | File | Family |
| ---: | --- | --- |
| 33,440 | `bsbifunc.cpp` | Behaviour script built-ins |
| 25,952 | `player.cpp` | Player |
| 19,396 | `cdbgeom.cpp` | Collision/geometry |
| 19,396 | `ShellMenu.cpp` | UI |
| 17,112 | `AIObject.cpp` | AI |
| 16,720 | `box.cpp` | Math/bounds |
| 14,944 | `path.c` | EA audio (music paths) |
| 13,348 | `Moh2.cpp` | Core |
| 11,772 | `particlesystem.cpp` | Particles |
| 11,168 | `UserInterface.cpp` | UI |
| 10,948 | `objcreate.cpp` | Object creation |
| 10,404 | `compartment.cpp` | Rendering |
| 10,392 | `scene.cpp` | Scene graph |
| 8,312 | `buddy.cpp` | AI squad |
| 8,236 | `UISStack.c` | UI script |
| 8,184 | `Soldier_object.cpp` | Actor |
| 8,048 | `static_obj.cpp` | World objects |

`sound.cpp` from the first ranking is complete; `propdat.cpp`,
`AIFilter.cpp` and `snddrv.c` are each below 8 KB unfinished. About 6 KB of unfinished functions fall
outside the file map's ranges and need `tools/file_map.py` evidence before a
source boundary is assigned.

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
