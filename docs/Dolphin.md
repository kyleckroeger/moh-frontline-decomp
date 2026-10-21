# Dolphin SDK units

Frontline links a Dolphin SDK build dated **Dec 17 2001** (the only SDK date
string in `Moh2RelGC.elf`). That is between the two public reconstructions this
project uses:

| Source tree | Upstream | SDK | Notes |
| --- | --- | --- | --- |
| `src/dolphin/`, `include/dolphin-sdk/` | Rising Sun's adaptation of [dolsdk2004](https://github.com/doldecomp/dolsdk2004) | 2004 era | Built with `-DSDK_REVISION=1`; `SDK_REVISION=0` did not fix any Frontline difference |
| `src/dolsdk2001/`, `include/dolsdk2001/` | [dolsdk2001](https://github.com/doldecomp/dolsdk2001) | May 22 2001 | Closer to Frontline; unit manifests set `source_root` |

Every accepted SDK unit is built with CodeWarrior **GC 1.2.5n**, `-O4,p
-inline auto`, matching the profile Rising Sun found for its SDK. GC 1.2.5
produces identical sizes but different epilogue scheduling (`mtlr` order), so
size comparisons alone cannot pick the compiler. `AX` and `GXStubs` also
verify with 1.2.5 and are recorded as built. A working profile is not proof of
the original compiler release.

## Local modifications to dolsdk2001

Frontline's Dec 2001 SDK already contains some changes found in later SDKs.
Each change below was required by, and verified against, the original bytes.
Upstream is commit `eb1234c` of dolsdk2001.

| File | Change | Evidence |
| --- | --- | --- |
| `gx/__gx.h` | Struct head `vNumNot`, `bpSentNot`, 16-bit `vNum`/`vLim`; `tevTcEnab` added after `tcsManEnab` (later fields +4) | `dirtyState` at `0x4F4`; `lhz 4`/`lhz 6` loads in `__GXSendFlushPrim` |
| `gx/*.c` (11 files) | `gx->bpSent = x` → `gx->bpSentNot = !x` | Inverted constants stored at `gx+2` |
| `gx/GXGeometry.c`, `gx/GXDisplayList.c` | Flush test on `vNumNot == 0` | Inverted branch in `GXBegin`/`GXCallDisplayList` |
| `gx/GXMisc.c` | `GXSetMisc` maintains `vNumNot`; `GXFlush` writes 8 words | 2004 code, sizes and bytes |
| `gx/GXTransform.c` | Scissor and viewport offset 342 (was 340) | Immediates and `.sdata2` constant `342.0f` |
| `gx/GXPixel.c` | `GXSetFogRangeAdj` centre offset 342 (was 340) | Immediate `center + 342` |
| `dvd/dvdfs.c` | Dec 2001 `OSPanic` line numbers 376, 739, 745 | `OSPanic` line-number immediates |
| `gx/GXFifo.c` | `CPUFifo`/`GPFifo` private and declared first; `__GXFifoInit` clears them | Local symbols and `.sbss` order |
| `ax/AXAlloc.c` | `__AXPushFreeStack` clears `priority` | 2004 code |
| `ax/AXAux.c` | Aux input getters test the callback; the B getter tests `__AXCallbackAuxA` (an SDK bug fixed later) | Relocation to `__AXCallbackAuxA` at `0x8011fd42` |
| `os/OSAlarm.c`, `os/__os.h` | System-time calls throughout; 2004 `OSSetPeriodicAlarm` and `DecrementerExceptionCallback` | Relocations to `__OSGetSystemTime` |
| `card/CARDUnlock.c` | `DoneCallback` returns after each error callback | 2004 code |
| `include/dolphin/card.h` | `CARDControl` gains `cid` and `diskID` (size `0x110`) | `mulli rX, chan, 272` |
| `gx/GXTev.c` | `GXSetTevOrder` updates `tevTcEnab` on the texcoord test (2004 code); replaces the plain ternary | Read-modify-write of `gx+0x4E0` (`lwz 1248`/`andc`/`or`/`stw 1248`) at `0x8012f600`-`0x8012f63c`; function is 476 bytes, not the 416-byte 2001 ternary |
| `os/OSError.c` | `static OSErrorTable[15]` becomes global `__OSErrorTable[16]` (2004 name); Dec 2001 `__OSUnhandledException` adds scheduler disable/enable+reschedule around the handler, an inline `OSGetTime()` TB report, the memory-protection case, the last-interrupt report and `"DSISR = "` spacing | `.bss` object at `0x80316fc0` size 64; only the global `__OSErrorTable` name satisfies BSS ownership. Code at `0x801126e8`: `bl OSDisableScheduler`/`OSEnableScheduler`/`__OSReschedule`, a second `bl OSGetTime` (not a saved local), `cmplwi r0,15` switch bound, `lhz` from `0xCC005030`/`0xCC005020` and `lwz` from `0xCC006014`, `lha -27008(r13)` |
| `os/OSException.h` | `__OS_EXCEPTION_MEMORY_PROTECTION` (15) added | `__OSUnhandledException` switch bound at `cmplwi r0,15` and its memory-protection case |
| `ax/AXCL.c` | Drop `__AXHRTFHistory` and its three writes in the `__AXNextFrame` mode-1 case | No `__AXHRTFHistory` symbol or relocation in the target; the group's `.bss` is only `__AXCommandList` (1536 bytes at `0x8031ac40`) |
| `ax/AXCL.c` | `__AXNextFrame` switch uses the later cases: 0 writes `7` then `+0x546`; 1 and 4 write `0x11` then `+0x5E6` (replaces the 2001 `case 1` HRTF block and the `0/3/4` group) | Dispatcher compares `__AXClMode` with 1 and 4 and branches both to `0x80120078`; stores immediate `7` with `addi 1350` at `0x80120024` and immediate `0x11` with `addi 1510` at `0x80120078` |
| `ax/AXCL.c` | `AXSetMode` drops the `mode == 1` `memset` of the history buffer | `AXSetMode` is 20 bytes (`0x801203a0`) with no `memset` call, matching the 2004 body |
| `ax/AXOut.c` | `__AXOutInit` clears `__AXOutBuffer`/`__AXOutSBuffer` with the inline `BUFFER_MEMSET` loop instead of `memset`, and adds `__AXDebugSteppingMode = 0` | `mtctr 4`/`mtctr 2` eighty-`stw` loops at `0x80120708`/`0x8012086C`; `__AXDebugSteppingMode` `.sbss` object at `0x8034F25C` with `stw` at `0x80120704`. The 2001 `memset` body is 180 bytes, not 832 |
| `ax/AXOut.c` | `__AXOutNewFrame` drops `old = OSEnableInterrupts()`/`OSRestoreInterrupts(old)` and copies `__AXLocalProfile` with an explicit byte loop instead of `memcpy` (the loop form is inferred from the bytes) | `bl __AXServiceCallbackStack` at `0x80120420` with no `OSEnableInterrupts`, and no `OSRestoreInterrupts` before the epilogue at `0x80120524`; `li r0,7`/`mtctr` plus eight `lbz`/`stb` per iteration at `0x801204CC`-`0x80120520`; `memcpy` remains an external call in accepted 2001 units |
| `ax/AXOut.c` | `__AXOutAiCallback` clears `__AXOutDspReady` before calling `__AXOutNewFrame` (2004 order) using `if`/`else` instead of the 2001 early `return` | `stw r0, __AXOutDspReady` at `0x80120570` precedes `bl __AXOutNewFrame` at `0x80120578`; both branches continue to the shared epilogue |
| `os/OS.c` | Dec 2001 revision: `ClearArena` (268 bytes) without the 2004 `__OSSavedRegionStart/End`; `OSInit` keeps the 2004 sequence but drops the seven perf-counter calls and the trailing `DVDInit`/`DVDInquiryAsync` block, adds `OSInitAlarm`, `BI2DebugFlagHolder`, `__OSInIPL`, `__OSStartTime` and inlines `DisableWriteGatherPipe`; `OSDefaultExceptionHandler` is global, not `static` | `ClearArena` at `0x801102e4` (loads `0x812FDFF0`/`0x812FDFEC`); `OSInit` at `0x801103f0`-`0x8011073c` (`bl 0x80110b54` `OSInitAlarm`, `__OSGetSystemTime`, `PPCMfhid2`/`PPCMthid2`, `__OSInitMemoryProtection`); `OSDefaultExceptionHandler` (`0x80110ab4`) is a global symbol, and with `__OSPSInit` (`0x80110b08`) it closes the file; `.data` strings `Revision: 49`, `Dec 17 2001`, `18:46:45` at `0x8018c778` |
| `os/OSRtc.c` | `UnlockSram` clamps `sram->flags` to two bits before the checksum reset (`if (2u < (sram->flags & 3)) sram->flags &= ~3;`), the 2004 fix without the 2004 `Scb.offset <= 0x14` GBS reset | Original `UnlockSram` at `0x80115258` is 776 bytes (the 2001 body builds 752); `0x80115280`: `lbz r3,19(r31)`/`clrlwi r0,r3,30`/`cmplwi r0,2`/`ble 0x80115298`/`clrrwi r0,r3,2`/`stb r0,19(r31)` (flags at `sram+19`) |
| `os/OSRtc.c` | Add public `OSGetWirelessID`/`OSSetWirelessID` from the 2004 bodies (May 2001 keeps them only as `static GetWirelessID`/`SetWirelessID` in `pad/Pad.c`) | Original global functions `OSGetWirelessID` at `0x80115800` (132 bytes) and `OSSetWirelessID` at `0x80115884` (172 bytes), absent from the 2001 tree; the 2004 bodies compile byte-identical with 2001 types |
| `os/OSFont.c` | `GetFontCode` adds the SJIS range bounds (`> 0x889E && <= 0x9872`, `>= 0x8140 && < 0x879E`), rejects adjusted trail indices `>= 188`, and drops the redundant `else return 0` | 2001 body compiles to 264 bytes, the original symbol is 308; `unit_diff` shows `cmplwi r0,39026`/`cmplwi r0,34718`, a second `cmpwi r3,188` after each `j--`, and no second `li r3,0` path at `0x801128e8` |
| `os/OSFont.c` | `OSLoadFont` gains the SJIS image-table fixup: build `u16 imageT[4] = {0x2ABE, 0x003D, 0x003D, 0x003D}`, call `GetFontCode(0x54)`, then per row `y=4..7` store `imageT[y-4]` and `DCStoreRange(src, 2)` | 2001 body compiles to 236 bytes, the original symbol is 576; original `0x80112d48`-`0x80112eb0` loads `imageT` from `.sdata2` (`lwz -24840(r2)`/`-24836(r2)`), and ends the loop with `li r4,2` / `bl 0x801118b4` (`DCStoreRange`) |
| `os/OSFont.c` | `OSGetFontWidth` guards the width store with `if (width)` | 2001 body compiles to 264 bytes, the original symbol is 272; original `0x801135a8` tests `cmplwi r31,0` / `beq` before `bl GetFontCode` |
| `ax/AXVPB.c` | `__AXMixCycles` indices 5-7 and 13-15 take the Dec 2001 values, not the May 2001 ones | `.data` at `0x8018e200`: words 5-7 are `0x116C 0x116C 0x1A22` (2001 `0xE74 0xE74 0x1432`) and 13-15 are `0x1D2E 0x1D2E 0x2BC5` (2001 `0x183D 0x183D 0x21E3`) |
| `ax/AXVPB.c` | `__AXServiceVPB` zeros the 16-word ITD buffer through `pvpb->itdBuffer` (word stores), and feeds the current address back on the non-loop path (the `else` after the `COPYADDR` test) | Original `lwz r8,308(r3)` (`pvpb->itdBuffer`) plus sixteen `stw` in the `COPYITD` case where 2001 stores sixteen `sth` through `dst`; two `lhz`/`sth` at offsets `122`/`124` in the outer `else` |
| `ax/AXVPB.c` | `__AXVPBInit` uses the 2004 inline `BUFFER_MEMSET` word clears (word counts `0xC00`/`0x400`/`0x1F80`, i.e. `sizeof(...)/4`) and sets `pvpb->priority = 1`; the next-pointer needs `sizeof(AXPB)` | `li r0,384`/`mtctr` at `0x801218a8`, `li r0,896` before the 9-`stw` `AXVPB` loop, `stw r0,12(r27)`; original is 512 bytes, the 2001 `memset` body is 384 |
| `gx/GXInit.c` | Dec 2001 splits the single 2001 `GXInit` into `GXInit` (`0x7C8` bytes: FIFO/register setup plus the texture-region and perf-counter setup) and the global `__GXInitGX` (`0x884` bytes: render-state setup). The four `GXInitTexCacheRegion`/`GXInitTlutRegion` loops stay in `GXInit` instead of moving with the state setup | `__GXInitGX` at `0x80129DD8` size `0x884`; `bl __GXSetTmemConfig` (`0x80129DB8`) then `bl __GXInitGX` (`0x80129DBC`) end `GXInit`; the region-loop calls at `0x80129C24`/`0x80129C88`/`0x80129CD4`/`0x80129D20` lie inside `GXInit` |
| `gx/GXInit.c` | `GXInit` clears the added `gx->tevTcEnab`; `__GXInitGX` gains the `for (i = GX_VA_POS; i <= GX_LIGHT_ARRAY; i++) GXSetArray(i, gx, 0)` loop, the `VI_EURGB60` case (`GXEurgb60Hz480IntDf`) and the ending `GXSetGPMetric(GX_PERF0_NONE, GX_PERF1_NONE)`/`GXClearGPMetric()` | `stw r31,0x4E0(gx)` at `0x80129664`; `li r30,9`/`cmplwi r30,0x18`/`bl GXSetArray` at `0x80129F84`-`0x80129FAC`; `cmpwi r3,5`/`GXEurgb60Hz480IntDf` at `0x80129E24`/`0x80129E50`; `GXSetGPMetric(0x23,0x16)` at `0x8012A634` and `bl GXClearGPMetric` at `0x8012A640` |
| `gx/GXInit.c` | `GXInit` adds the perf-counter block and `__GXSetTmemConfig(0)` (the 2004 revision passes `2`); `EnableWriteGatherPipe`/`DisableWriteGatherPipe` are `inline`, so the Mfhid2/Mtwpar/Mthid2 sequence appears in `GXInit`; a retained (unused) `GXRenderModeObj *rmode` local reproduces the original `0x48` stack frame (the body is otherwise instruction-identical without it) | `GX_SET_CP_REG(3,0)` (`sth r12,6(__cpReg)`) at `0x80129D44`; `addi gx+0x4EC`/perfSel writes at `0x80129D50`-`0x80129DB4`; `li r3,0` then `bl __GXSetTmemConfig` at `0x80129D80`/`0x80129DB8`; `bl PPCMfhid2` at `0x801296A8`; original `stwu r1,-0x48`/`stmw r26,0x30(r1)` vs `-0x40`/`0x28` without the local |
| `gx/GXInit.c` | Local `GX_SET_CP_REG` macro plus `__GXInitGX`/`__GXSetTmemConfig` prototypes and a `GXEurgb60Hz480IntDf` declaration, which the 2001 headers omit | Relocations to `__GXSetTmemConfig` at `0x80129DB8` and to `GXEurgb60Hz480IntDf` at `0x80129E50`; `__GXInitGX` symbol at `0x80129DD8` |
| `gx/GXFrameBuf.c` | Add the later `GXGetYScaleFactor` and its `static __GXGetNumXfbLines` from the 2004 tree; `GXSetDispCopyYScale` returns `__GXGetNumXfbLines(ht, iScale)` | `GXGetYScaleFactor` at `0x8012c9c0` (568 bytes); `GXSetDispCopyYScale` at `0x8012cbf8` is 212 bytes with the line-count helper inlined; no `__GXGetNumXfbLines` symbol |
| `gx/GXFrameBuf.c` | Add `GXEurgb60Hz480IntDf` after `GXPal524IntAa`, written like `GXNtsc480IntDf` with `viTVmode` 20. Its place among the stripped render modes, and any other EURGB60 modes the Dec 2001 file defined, are unknown | Global `GXEurgb60Hz480IntDf` at `0x801901d4` (60 bytes) follows `GXPal528IntDf`; its bytes equal `GXNtsc480IntDf`'s except the first word, `0x14`. Only the four `*IntDf` modes are kept (`stripped_objects`) |
| `gx/GXPerf.c` | `GXSetGPMetric` uses the 2004 case order (`GX_PERF0_CLOCKS` first, `GX_PERF1_TC_MISS` before `GX_PERF1_CLOCKS`) and the 2004 `0x2300xxxx` TRIANGLES register values | `GXSetGPMetric` at `0x80130684` is 2,200 bytes; it loads `lis 0x2301` 15 times and never `0x2304` (the 2001 `0x2303xxxx` values); jump tables `@242` (92 bytes) and `@241` (144) at `.data:0x801902b8` |
| `dsp/dsp.c` | `__DSP_first_task`, `__DSP_last_task`, `__DSP_curr_task` and `__DSP_tmp_task` are `extern`: Dec 2001 defines them in `dsp_task.c`, as the 2004 tree does. `BUILD_DATE`/`BUILD_TIME` are `Dec 17 2001`/`18:25:00` | dsp.c's `.sbss` is only `__DSP_init_flag` (`0x8034f2a8`, 4 bytes); the four globals lie at `0x8034f2b8`-`0x8034f2c4` inside the verified `dsp_task` unit, compiled from the unmodified 2004 file. `.data` at `0x8018fc80` holds `Dec 17 2001` and `18:25:00` |
| `gx/GXAttr.c` | `SETVCDATTR`'s `GX_VA_NRM`/`GX_VA_NBT` cases set both normal flags in each branch (2004 form) | `0x8012b1e4`: `cmpwi r4,0`/`beq`, then `stb` of 1 to `hasNrms` (`1052`), 0 to `hasBiNrms` (`1053`) and `stw` of `nrmType`; the else branch stores 0 to `hasNrms`. `GXSetVtxDesc` at `0x8012b08c` is 864 bytes. The dead jump tables of the discarded `*v`/`GXGetVtxDesc` functions are `stripped_objects` |
| `gx/GXTexture.c` | `GXInitTexObj` picks the mipmap min filter by format (`0xA0` for formats 8/9/10, else `0xC0`); `__GXSetSUTexRegs` also tests `gx->tevTcEnab & (1 << i)`. Both are the 2004 code | `GXInitTexObj` at `0x8012da0c` is 628 bytes (2001 builds 596); `__GXSetSUTexRegs` at `0x8012e408` is 380 bytes (2001 builds 364); both compare byte-identical |
| `os/__start.c` | Add `__check_pad3` (from the 2004 file) and its call after `OSInit` when the device-code test passes; `Pad3Button` is a 2001-style absolute variable at `0x800030E4` | File-local `__check_pad3` at `0x80007100` (64 bytes) in the `__start.c` record; `__start` at `0x80007140` is 276 bytes and tests `lhz 0(0x800030E6)` before `bl __check_pad3` (2001 builds 240). The 2004 file's BBA debug code is absent |

## Local modifications to the 2004 tree

Other units are verified from `src/dolphin/` (the 2004 adaptation).
`mtx44.c`, `dsp_task.c` and `mtx.c` need no change; `mtx.c` and `OSMemory.c` were imported
from upstream at the pinned commit, because the local copies lacked them. The files below keep behaviour that the 2004 source
later changed (the `card` units, `pad/Padclamp.c`'s Dec 2001 `PADClampRegion`,
`exi/EXIUart.c`), so they are edited against the original bytes.

| File | Change | Evidence |
| --- | --- | --- |
| `card/CARDCheck.c` | `VerifyID` tests `encode` before the serial checks, using `OSGetFontEncode` | Check order in the original; relocation to `OSGetFontEncode` |
| `card/CARDFormat.c` | Format progress stays at the mount-step offset (`formatStep` = `mountStep`); `OSGetFontEncode` | Relocation to `OSGetFontEncode`; shared progress field |
| `card/CARDWrite.c` | `__CARDAccess` instead of `__CARDIsWritable` | Relocation to `__CARDAccess` |
| `card/CARDRead.c` | `CARDReadAsync` uses `__CARDAccess` and the `CARD_RESULT_NOPERM`/`__CARDIsPublic` test instead of `__CARDIsReadable` | Relocations to `__CARDAccess`/`__CARDIsPublic`; original `CARDReadAsync` is 328 bytes, `__CARDIsReadable` compiles to 324 |
| `card/CARDStat.c` | `CARDGetStatus` keeps the pre-2004 `__CARDAccess` + `__CARDIsPublic` fallback (instead of `__CARDIsReadable`); `CARDSetStatusAsync` uses `__CARDAccess` instead of `__CARDIsWritable` | Relocations to `__CARDAccess` at `0x80128f54` and `0x801290c4`, to `__CARDIsPublic` at `0x80128f68` |
| `pad/Padclamp.c`, `include/dolphin-sdk/dolphin/pad.h` | `PADClampRegion` drops the unused `radStick`/`radSubstick` (8 bytes); `ClampRegion` is non-const (`.sdata`); `ClampTrigger` takes only the trigger and reads `ClampRegion` itself | Region at `.sdata:0x8034e4c0` size 8; original PADClamp loads `-29856(r13)` and `1(r29)`, and reloads `minTrigger` after the store. The `ClampTrigger` signature is inferred from that inlined code; the function itself is dead-stripped |
| `exi/EXIUart.c` | `WriteUARTN` does not disable/restore interrupts (2004's `OSDisableInterrupts`/`OSRestoreInterrupts` calls removed) | Original `WriteUARTN` at `0x80145d68` is 512 bytes with no relocation to either function; the 2004 source builds 540 bytes |
| `card/CARDMount.c` | `CARDProbeEx` and `DoMount` inline the Dec 2001 ID test instead of calling `IsCard`; `DoMount` keeps the size/`sectorSize`/`cBlock` checks after assigning `cid` and `size`. `IsCard` and `__CARDDisable` are unreferenced and dead-stripped | No `IsCard`/`__CARDDisable` symbol in the target. The test `(id == 0x80000004 && __CARDVendorID != 0xFFFF) || !((id & 0xFFFF0000) || (id & 3))` appears inlined at `0x80126848`-`0x80126974` (`CARDProbeEx`) and `0x80126944`-`0x80126a74` (`DoMount`); the `cmpwi` switch 4/8/16/32/64/128 at `0x80126a90`-`0x80126adc`, `sectorSize == 0` at `0x80126b04` and `cBlock < 8` at `0x80126b2c`. `CARDProbe` (56 bytes) and `CARDUnmount` (172 bytes) are neighbours the source already covers |
| `card/CARDMount.c` | `__CARDMountCallback`'s `CARD_RESULT_UNLOCKED` case calls `DoMount` directly; the 2004 `EXILock`/`__CARDUnlockedHandler` block is removed | Original `CARD_RESULT_UNLOCKED` path at `0x80126f18` is 16 bytes with no relocation to `EXILock`/`__CARDUnlockedHandler`; the function is 264 bytes, the 2004 source builds 312 |
| `dvd/dvdlow.c` | `__DVDInterruptHandler` cancels `AlarmForTimeout` once at entry, before the `LastCommandWasRead` block, instead of inside `if (cause)` | `addi r3,r30,104`/`bl OSCancelAlarm` at `0x80116ebc` precede the `LastCommandWasRead` load at `0x80116ec4`; the Dec 2001 `if (cause)` block at `0x80116f5c` only stores `ResetOccurred = FALSE` and `__DIRegs[0]`. Original is 756 bytes, the unmodified 2004 source builds 736 |
| `dvd/dvdlow.c` | `DVDLowClearCallback` drops the 2004 `WaitingCoverClose = FALSE` (2001 body) | Original `DVDLowClearCallback` at `0x80117c4c` is 24 bytes; the 2004 source builds 28 |
| `card/CARDBios.c` | No `__CARDVersion` string/pointer; `CARDInit` does not call `OSRegisterVersion` | Target `.data` at `0x8018fdf8` is 16 bytes with no `__CARDVersion` object and no relocation to `OSRegisterVersion`; with the version string and call the 2004 source builds a 172-byte `CARDInit` and 88 bytes of `.data` |
| `card/CARDBios.c` | `CARDInit` does not assign `__CARDEncode = OSGetFontEncode()` | No relocation to `OSGetFontEncode` in the target; the store has to go for `CARDInit` at `0x801238f0` to reach its 156 bytes |
| `si/SIBios.c` | `SIInit` drops `OSRegisterVersion(__SIVersion)` and the `__SIVersion` definition | Original `SIInit` at `0x8014683c` is 164 bytes; the 2004 body with `OSRegisterVersion` compiles to 180. The unit's `.data` (`0x80190dd0`, size `0xD0`) begins with `Si` and contains no version string, and the unit has no relocation to `OSRegisterVersion` |
| `exi/EXIBios.c` | `EXISync` tests only `__OSGetDIConfig`, `exi->immLen == 4`, `__EXIRegs[chan*5] & 0x70` and `__EXIRegs[chan*5+4] == 0x01010000` (2004's `OSGetConsoleType` term, the `0x05070000`/`0x04220001` compares and the `__OSDeviceCode == 0x8200` test removed) | Original `EXISync` at `0x80144938` is 520 bytes, not 588; its condition at `0x80144ab0`-`0x80144b28` has one `addis r0,r3,-257` (`0x01010000`) compare after the mask test, with no `bl OSGetConsoleType` and no `lhz` of `__OSDeviceCode` |
| `exi/EXIBios.c` | `EXTIntrruptHandler` masks `0x700000U >> (chan * 3)` and clears `__EXIRegs[chan * 5]` (2004 uses `0x500000U` and omits the clear) | Original `EXTIntrruptHandler` at `0x80145590` is 200 bytes: `lis r3,112` (`0x700000`) at `0x801455b8` and `mulli r0,r30,20; lis r4,-13312; stwx` clearing `__EXIRegs` at `0x801455c0` |
| `exi/EXIBios.c` | `EXIInit` drops the 2004 busy-wait and the `__OSInIPL`/`EXIProbeReset`/`__OSEnableBarnacle`/`OSRegisterVersion` sequence; after the eight handler registrations it calls `EXIProbeReset()` under `if (OSGetConsoleType() & 0x10000000)`, and the call is inlined | Original `EXIInit` at `0x80145658` is 276 bytes; it ends with `bl OSGetConsoleType`/`rlwinm. r0,r3,0,3,3` guarding inlined `__gUnknown800030C0`/`idTime` clears and two `bl __EXIProbe`, and has no relocation to `__OSInIPL`, `__OSEnableBarnacle` or `OSRegisterVersion` |
| `exi/EXIBios.c` | `EXIGetID` drops the `IDSerialPort1` early return and the `OSDisableInterrupts`/`OSRestoreInterrupts` pair around `EXILock` (the inlined `__EXIAttach` disables and restores interrupts itself) | Original `EXIGetID` at `0x8014597c` is 892 bytes; its prologue branches straight to the `chan < 2 && dev == 0` test with no `IDSerialPort1` load, and no `bl OSDisableInterrupts` precedes `bl EXILock` at `0x80145abc` |
| `exi/EXIBios.c` | `EXIDetach` masks `0x700000U >> (chan * 3)` (2004 uses `0x500000U`) | Original `EXIDetach` at `0x80144fb8` is 188 bytes with `lis r3,112` (`0x700000`) at `0x80145018` |
| `exi/EXIBios.c` | `CompleteTransfer`, `__EXIAttach` and `EXIProbeReset` are inlined in Frontline's build, and `EXISelectSD`, `EXIGetType` and `EXIGetTypeString` are absent | No original symbols for any of the six (the manifest's `discarded_functions`); the `__EXIAttach` body appears inline in `EXIGetID` at `0x80145a00`-`0x80145a78`, and the `EXIProbeReset` body in `EXIInit` at `0x80145730`-`0x80145754`. The out-of-line `EXIProbeReset` is dead-stripped |
| `vi/vi.c` | `timing[10]` → `timing[8]`; `getTiming` drops `case 24`/`case 26` | `getTiming` at `0x8011b038` is 144 bytes; its jump table dispatches `0..21` and the last case body returns `timing[7]` (`addi r3,r5,266`), with `timing` at `0x8018df38` (stride 38, 8 entries) |
| `vi/vi.c` | `__VIRetraceHandler` uses the short `inter & (4\|8)` branch (`OSSetCurrentContext(context); return;`), no `PositionCallback` call | Handler at `0x8011adcc` is 552 bytes; at `0x8011ae7c` it branches straight to `OSSetCurrentContext` with no `__VIGetCurrentPosition`/callback block |
| `vi/vi.c` | `__VIInit` tests `mode != VI_TVMODE_NTSC_PROG && mode != 3` (no `&& mode != 26`) | `0x8011b28c`: `cmpwi r29,2` / `cmpwi r29,3`, two branches to the progressive path, no `26` compare |
| `vi/vi.c` | `VIInit` drops the `IsInitialized` early return, `OSRegisterVersion`/`__VIVersion`, the `0xCC` `tvInBootrom`/EURGB60 fix-up, `NextBufAddr`/`CurrBufAddr`; sets `encoderType` (not `IsInitialized`) | `VIInit` at `0x8011b2c0` is 1144 bytes; `stw r0,-26704(r13)` with `r0=1` is `encoderType`, read by `VIConfigure` at `0x8011bf64`; no `0xCC` read and no `OSRegisterVersion` relocation |
| `vi/vi.c` | `VIConfigure` drops the `tvInBootrom` switch/`OSPanic`, tests `HorVer.tv == 4 \|\| == 5` (no `== 6`) and `viTVmode == 2 \|\| == 3` (no `== 26`) | `0x8011bc00` is 1948 bytes; `tv` assignment at `0x8011bce0`/`0x8011bce8`, `tv` field test at `0x8011c02c`, clksel test at `0x8011c078` |
| `vi/vi.c` | `VIFlush` drops `NextBufAddr = HorVer.bufAddr;` | `0x8011c39c` is 284 bytes and ends with `flushFlag = 1` then `OSRestoreInterrupts`; no store to `-26656` |
| `vi/vi.c` | `VIGetNextField` calls `getCurrentFieldEvenOdd()` and applies `^ 1` after `OSRestoreInterrupts`; `getCurrentHalfLine` reads `__VIRegs[22]`/`[23]` itself instead of calling `GetCurrentDisplayPosition` | `0x8011c608` is 168 bytes with the half-line loop inlined and `xori r3,r31,1` after the restore; `getCurrentFieldEvenOdd` at `0x8011c5a0` is 104 bytes with the loop inlined |
| `vi/vi.c` | `VIGetTvFormat` omits `case 6` | `0x8011c748` is 124 bytes; the comparison tree handles `0..5` and sends `6` to the default |
| `vi/vi.c` | `setVerticalRegs` tests `equ >= 10` (2001 form), not `regs[54] & 1` | `0x8011ba60` is 416 bytes; `clrlwi r0,r5,24` / `cmplwi r0,10` at `0x8011ba6c` compares the `equ` argument |
| `vi/vi.c` | Drops `VIGetNextFrameBuffer`, `VIGetCurrentFrameBuffer`, `__VIEnableRawPositionInterrupt`, `__VIDisableRawPositionInterrupt` and the `PositionCallback`/`NextBufAddr`/`CurrBufAddr`/`IsInitialized` statics | The original `.sbss` layout ends at `CurrTvMode` (`-26660`), `FBSet` (`-26656`), `message` (`-26652`); there is no `.sbss` object for those statics, and no relocation to the functions |
| `pad/Pad.c` | Drop `__PADVersion` and its `OSRegisterVersion` call | `OSRegisterVersion` exists neither in the target nor in the 2001 tree. The dead `Initialized`, `CmdTypeAndStatus` and `OnReset`'s `recalibrated` stay in the source and are declared `stripped_objects`; `ResetFunctionInfo`'s `.data` is a stripped section |
| `dvd/dvd.c` | Dec 2001 globals: `tmpBuffer`/`currID`/`bootInfo`, no `BB2`/`CurrDiskID`/`IDShouldBe`, and no optional-command or DMA/imm-command tables or their 2004-only accessors | `.bss` `tmpBuffer` 128 at `0x80317b40`, `DummyCommandBlock` 48, `ResetAlarm` 40; `.sbss` `currID` at `0x8034f0a4`; no `BB2`/`CurrDiskID`/`IDShouldBe`/`checkOptionalCommand`/`IsDmaCommand`/`IsImmCommandWithResult` symbols. The unused `CancelAllSyncComplete`/`ResetCount` and the `DVDChangeDiskAsync` company-name message stay and are `stripped_objects` |
| `dvd/dvd.c` | `DVDInit` keeps the 2001 boot-magic reports and adds `OSInitAlarm`/`__DVDInitWA`; no `OSRegisterVersion` | `bl 0x80110b54` `OSInitAlarm` at `0x80118470`, `bl 0x80116e54` `__DVDInitWA` at `0x80118484`; four `.data` reports (21/10/25/9 bytes) and no `__DVDVersion` |
| `dvd/dvd.c` | `stateReadingFST` reads FST length/position from `tmpBuffer[2]`/`tmpBuffer[1]`; `cbForStateReadingFST` drops `__DVDFSInit`; `stateCheckID2` reads into `tmpBuffer` | `lwz r4,8(r5)`/`lwz r5,4(r5)` with `r5 = 0x80317b40`; no `__DVDFSInit` relocation in the callback; `DVDLowRead(&tmpBuffer, 0x20, 0x420, ...)` at `0x80118e64` |
| `dvd/dvd.c` | `cbForStateError` drops `__DVDPrintFatalMessage`; `stateTimeout` inlines its body (156 bytes) | No relocation to `__DVDPrintFatalMessage`; error finalization inlined at `0x801186c0` |
| `dvd/dvd.c` | `stateCheckID` uses `memcmp` (0x1C then 0x20) against `executing->id`/`currID` instead of `DVDCompareDiskID` | `bl memcmp` at `0x80118d84`/`0x80118dec` with `li r5,28`; function is 228 bytes |
| `dvd/dvd.c` | `stateReady` keeps the 2001 `ResumeFromHere` cases 1/2/3/7/4/6/5; `stateBusy` drops the 2004 zero-length read and optional-command default; `cbForStateBusy` tests the DMA/imm commands directly | 8-entry jump table `@293` at `0x8018ddb4`; `stateBusy` is 704 bytes with `cmplwi r0,15` ending at the epilogue; direct `CurrCommand == 1/4/5/14` and `== 9/10/11/12` compares |
| `dvd/dvd.c` | `DVDCancelAsync` and `DVDCheckDisk` drop later-2004 additions | `DVDCancelAsync` has no `executing = &DummyCommandBlock` in the cover/motor case (624 bytes); `DVDCheckDisk` has no `ResumeFromHere` test in the END/PAUSING case (228 bytes) |
| `ar/ar.c` | Drop `__ARVersion` and its `OSRegisterVersion` call | `ARInit` at `0x8011e7d4` is 188 bytes; the 2004 body with the call is 196 |
| `ar/ar.c` | Dec 2001 `__ARChecksize`: the 2004 preamble (wait for the controller, fixed 16 MB internal size, `__AR_ExpansionSize`) without the 2004 ARAM save/restore buffers; five dummy writes, then one test write and read, then the expansion probes, each read followed by `PPCSync` and no invalidate. The mode bits keep the 2004 spellings (`ARAM_mode \|= 0 << 1`, `4 << 1`, ...), which the final register write's code depends on. Reconstructed from the disassembly | `__ARChecksize` at `0x8011e910` is 2,324 bytes with a 312-byte frame (three 63-byte pads; 2004 builds 6,132, 2001 3,788); inlined `__ARWriteDMA`/`__ARReadDMA` with the 2004 `__ARClearInterrupt` sequence (`li -137`/`ori 32`); stores to `__AR_InternalSize` (`-26544(r13)`) and `__AR_ExpansionSize` (`-26540`) |
| `os/OSInterrupt.c` | Restore `OSDisableInterrupts`, `OSEnableInterrupts`, `OSRestoreInterrupts` and the `static` `ExternalInterruptHandler` (`asm`, copied from dolsdk2001), which the local copy had left out | Functions at `0x801135cc` (20 bytes), `0x801135e0` (20), `0x801135f4` (36), and file-local `ExternalInterruptHandler` at `0x80113de8` (76) in the `OSInterrupt.c` record; `__RAS_OSDisableInterrupts_begin`/`_end` labels at `0x801135cc`/`0x801135dc` |
| `os/OSSync.c` | Restore the `static asm` `SystemCallVector` from dolsdk2001, which the local copy had left out | File-local `SystemCallVector` at `0x80115930` (32 bytes) in the `OSSync.c` record, bracketed by `__OSSystemCallVectorStart`/`End` |
| `card/CARDStatEx.c` | Frontline's intermediate revision: `__CARDAccess(card, ent)` (two arguments) with the `CARD_RESULT_NOPERM` → `__CARDIsPublic` fallback instead of `__CARDIsReadable`/`__CARDIsWritable`, and no permission-dependent `memset`s in `__CARDSetStatusExAsync` | `__CARDGetStatusEx` at `0x801291dc` and `__CARDSetStatusExAsync` at `0x80129298` (784 bytes) |
| `card/CARDNet.c` | Replace `CARDSetAttributesAsync` with Frontline's older revision (no permission checks: it only reads the status, sets the permission byte and writes it back); `CARDSetAttributes` inlines it | `CARDSetAttributes` at `0x801294ec` (132 bytes) in the `CARDNet.c` record, the only linked function of the file |
| `card/CARDOpen.c` | Frontline's intermediate revision: `__CARDAccess(card, ent)` reads `card->diskID` directly, `__CARDIsPublic` returns instead of `__CARDIsWritable`/`__CARDIsReadable`, and `CARDFastOpen` uses the `CARD_RESULT_NOPERM` → `__CARDIsPublic` fallback. The unit is compiled with `-char signed`: `__CARDCompareFileName` compares sign-extended characters (`extsb`) in the original, and no source change was invented to get that | All 8 functions from `__CARDCompareFileName` at `0x80127a50` to `__CARDIsOpened` (1,460 bytes) |
| `os/OSCache.c` | Restore the cache `asm` functions from dolsdk2001 (`DCEnable` through `LCDisable`), which the local copy had left out; the ones Frontline does not link are discarded | `DCEnable`, `DCInvalidateRange`, `DCFlushRange`, `DCStoreRange`, `DCFlushRangeNoSync`, `DCStoreRangeNoSync`, `ICInvalidateRange`, `ICFlashInvalidate`, `ICEnable`, `LCDisable` at `0x8011183c`-`0x801119cc` (400 bytes) in the `OSCache.c` record |
| `base/PPCArch.c` (dolsdk2001) | Declare `PPCMtdec` and `PPCHalt` `__declspec(weak)`, as the Pikmin 2 and Mario Kart: Double Dash!! decompilations do | Both are weak symbols in the target (`0x80110188`, `0x80110198`); the other 10 linked functions of the 112-byte `PPCArch.c` record are global |
| `os/OSTime.c` (dolsdk2001) | Add `__OSTimeToSystemTime` after `__OSGetSystemTime` (missing from the 2001 source; the body follows the later SDK, with the 2001 file's literal `0x800030D8` system-time address) | `__OSTimeToSystemTime` at `0x80116ac4` (88 bytes) in the `OSTime.c` record |
| `os/OS.c` (dolsdk2001) | Add `__OSGetDIConfig` at the end (`(u8)__DIRegs[9]`; absent from the 2001 source) | `__OSGetDIConfig` at `0x80110b40` (20 bytes) in the `OS.c` record |
| `gx/GXTexture.c` (dolsdk2001) | Add `__GXSetTmemConfig` at the end: Frontline's revision has two configurations (`case 1` and the default) rather than the later three; register values from the later SDK | `__GXSetTmemConfig` at `0x8012e584` (576 bytes) in the `GXTexture.c` record; the manifest's hand-kept `.data` entry (`stripped_objects`) is unchanged |
| `os/OSMemory.c` | `__OSErrorTable` is `extern`. `__OSInitMemoryProtection` selects `Config24MB`/`Config48MB` right after disabling interrupts, before the MEM register setup, and its 24 MB test sets `__MEMRegs[20]` without `DCInvalidateRange` | `__OSErrorTable` belongs to `OSError.c`; `__OSInitMemoryProtection` at `0x80114210` is 288 bytes (upstream builds 280) with the `RealMode` calls before `sth` to `__MEMRegs[16]`, and no `DCInvalidateRange` relocation |

## Porting workflow

```sh
python3 tools/port_unit.py <id> <source> --like <manifest-or-template> [--original-file X.c] [--compiler 1.2.5n]
# add "<id>.json" to config/GMFE69/project.json, then:
python3 tools/reconstruct.py
python3 tools/unit_diff.py <id>        # if verification reports differing bytes
```

`port_unit.py` compiles the source and drafts a manifest by pairing each
compiled relocation with the original relocation at the same offset. This is
possible because Frontline preserves `.rela.*`. It fixes every section address
and external without guesswork. The draft earns nothing until `reconstruct.py`
verifies the complete image. `--like` copies compiler settings and provenance
from an existing manifest (a Rising Sun manifest, or a template with
`source_root` and `upstream` for dolsdk2001).

## CodeWarrior linker behavior

The original was linked by the MW linker, unlike Rising Sun's ProDG link:

- **Dead stripping of functions.** Functions absent from the original are compiled
  and discarded (`strip_unused`, `discarded_functions`), as in Rising Sun.
- **Dead stripping of data.** MW also removes unreferenced data. A compiler
  section that only discarded code references is declared in
  `stripped_sections`. SN's linker has no `/DISCARD/`, so those sections are
  parked at `0x10000000 + n * 0x100000`, outside main memory, and never enter the
  rebuilt image. `verify_unit` checks they are parked; retained code that used
  them would fail the byte comparison.
- **Trimmed data.** When MW removed unused objects at the start or end of a data
  section, the manifest section names the kept slice (`linked_offset`,
  `linked_size`). The whole compiled section is linked at the shifted base, only
  the kept bytes are compared and credited, the trimmed part must hold whole
  compiler objects, and retained code may not refer into it.
- **Stripped objects.** For unused objects inside a section, the manifest
  section lists them in `stripped_objects` (C names; MW's numbered function
  statics match by identifier, and anonymous literals by their `@N` label).
  `tools/data_strip.py` moves each one, with its trailing padding, after the
  section's other bytes in a copy of the object (`input.o`; `compiled.o` stays
  the hashed compiler output). The trimmed-tail rules then apply. A listed name
  may not exist in the target, and retained code may not refer to it.
  Use it instead of `linked_offset` for leading objects too when the shifted
  base would break the section's alignment; SN then pads the start (`dsp_task`
  `.sbss`, `GXTexture` `.data`).
- **Small data at 4 mod 8.** MWCC gives `.sdata`, `.sbss`, `.sdata2` and
  `.rodata` 8-byte alignment, but MW's linker placed some of these input
  sections at addresses that are 4 mod 8 (`smixfx` `.sbss` at `0x8034f4d4`).
  SN's linker would pad the start or round the size up. A manifest section then
  sets `input_alignment: 4`; `data_strip.py` lowers only that section header's
  alignment from 8 to 4 in `input.o`. `validate_units` accepts the field only on
  a data section whose linked base is 4 mod 8, so it cannot hide a misplaced
  section, and the bytes are compared with the original as usual.
  `port_unit.py` adds the field when a placement needs it.
- **Weak duplicates.** MW's linker keeps the first definition of a weak
  function and drops later copies. When a unit compiles a weak function whose
  original copy came from an earlier file (such as a template's inline
  destructor), the unit lists it in `weak_duplicates` and in `externals` at the
  retained copy's address. `data_strip.py` renames the compiled copy to
  `<name>$duplicate` and points every relocation to it at a new undefined
  symbol of the original name, so the unit calls the retained copy and SN's
  unused-code stripping drops the duplicate. `validate_units` requires
  stripping and exactly one weak original function of that name;
  `validate_object` treats the duplicate like a discarded function. The
  duplicate's exception records, between kept ones, are `stripped_objects`.
  A weak data object, such as the virtual table of a class with no key
  function (all its virtuals inline), is handled the same way: the copy is
  renamed, references go to the original object (`validate_units` requires
  one weak original object or function of that name), and a compiler data
  section that holds only such copies is a stripped section. Its bytes are
  never placed, compared or counted.
  `port_unit.py --weak-duplicate NAME` drafts all of this.
- **File duplicates.** A fragment may compile a function of its own source
  file that another fragment places: a static helper it inlines (and calls out
  of line at the deepest inline level of a recursion), or a global function of
  the file that a recursive inline reaches. The unit lists it in
  `file_duplicates` and in `externals` at the original copy's address; it is
  renamed and redirected exactly like a weak duplicate, whatever its binding.
  `validate_units` requires stripping and exactly one original function of
  that name in the unit's file record (or one global), outside the unit's own
  code. `port_unit.py --file-duplicate NAME` drafts it.
- **Weak locals.** CW emits a member-template instance (such as a `list<T>`
  range constructor) weak, while the original linker kept a file-local copy.
  `port_unit.py` matches a compiled weak function with no global original to
  the file record's local copy of that name and lists it in `weak_locals`;
  the build accepts the weak compiled binding for those names only, where the
  original has no global of that name. Bytes are compared as usual.
- **Pooled constants and strings.** MW collects a file's `@N` floats, doubles
  and string literals into shared `.sdata2`, `.rodata` or `.data` pools. A
  fragment compiles only its own entries, so its pool section cannot sit at one
  original address. The unit lists such sections in `pooled_sections` and each
  compiler object that retained code refers to in `pool_references` (compiled
  section, offset and size, and the original address). `data_strip.py` points
  every retained reference at a new undefined symbol `__pool_<address>`, which
  the link script pins there (an `.sdata2` anchor for small-data references),
  and the compiled section is parked like a stripped one. The build requires
  each listed object to equal the original bytes at its address and to hold no
  relocations; a retained reference to an unlisted pooled object fails. Code is
  still compiled and compared byte for byte, and pool bytes earn no credit.
  `port_unit.py --pool .sdata2` drafts it from the original relocations. It
  does not help when code reaches several entries through one pool base plus
  immediate offsets (`PlayGalleryMovie`'s strings): those offsets need the
  whole original pool layout.
- **Relocations in removed bytes.** Relocations located in stripped sections,
  trimmed slices or stripped objects are pointed at their own section before
  linking. Otherwise SN keeps a discarded function alive through dead data, such
  as `GXPerf`'s jump tables or `Pad`'s `ResetFunctionInfo` (SN rejects
  `R_PPC_NONE`).
- **Weak definitions.** Frontline has weak globals (for example `PPCHalt`,
  `__start`). External resolution treats them like globals.
- **Declaration-only symbols.** `__declspec(section ".init")` declarations in
  `__os.h` make MW emit unreferenced undefined symbols (`__init_data`, …). In
  Frontline some exist only as `static` functions of `__start.c`. A name in
  `undefined_in_discarded_code` may exist in the target only as another source
  file's private symbol, or as a file-local function the unit itself defines;
  relocations to such a name must use the local definition.
- **Code in `.init`.** A unit may cover `.init` as well as, or instead of,
  `.text` (`__start.c`, `__mem.c`, `__ppc_eabi_init.cpp`). Each code section
  must be tiled exactly by original functions, and discarded functions may sit
  anywhere in it.

## Open problems

- **Revision differences.** Remaining SDK files differ from both reconstructions
  in some functions. A `scratch`-style comparison of each function's size against
  the 2001 and 2004 builds shows where to start. `CARDCheck`, `CARDFormat`,
  `CARDWrite` and `Pad` were resolved from the 2004 source that way.
- Compiled code that calls a different function than the original is reported by
  `port_unit.py` (for example `OSGetTime` vs `__OSGetSystemTime`).
- `PPCArch`, `odenotstub`: linked function symbols differ (likely asm-only or
  alias functions).
