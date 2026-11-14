# merge3 (2026-11-13): whole-file merges, continued from merge2

Tools here (merge2's plus): mrg.sh <new-id> <new-src> <tpl> <orig-file> <index> <fragments...>
[-- port_unit args] replaces every fragment unit (config + source) by one unit, new id/source
allowed; env RMFILES (git rm, e.g. research/data/bss/X.cpp), EDITCMD (shell edit before
port_unit, e.g. a test fixture or research/data/bss/unassigned.cpp), CATEGORY (default
reconstructed_game). land3.sh = lock + mrg.sh + verify + land + push + release, reverting
config/src/research/docs/progress/include/tests on failure. batch*.sh chain landings 60 s
apart. fd.py OBJ FUNC diffs one function against the original. uis/body.py: a fragment's
top-level items without type definitions / listed declarations (used by the UIStudio and
snddrv builders: uis/build.py, snd/build.py); ti/build.py merges fragments that share one
skeleton (targimpl); pd/build.py merges by keyed sequence (propdat). chk.py needs SUFFIX=c
for C sources (fast.py compiles probe.cpp otherwise).

Landed: see the final report (filesys, bsmachin, filedev, smixfx loose end, dvdStatus_base,
UIStudio, UISUtils, snddrv, then batch3: targimpl, propdat, soundschedule, collision).

Tricks learned
- Several fragments' views of one struct: anonymous unions keep each fragment's member names
  (and types/signedness/volatility) at one offset; CW has anonymous unions but no anonymous
  structs (a named struct member in the union instead, e.g. SNDVOICEVIEW.fx).
- One TU inlines what fragments called: '#pragma auto_inline off/reset' around the callee
  (UISUtils' UISExecuteFnc) or the whole file at -inline on (snddrv).
- Return-type mismatch between a definition and its callers' declarations: make the chain
  return int when no function extends the result (UIStudio's load path).
- A plain inline helper defined before its user requests its template instances at its
  definition: make the helper a template so they are requested at the use (propdat's
  PatchUpAnimatedLightArrays; offsetPtr instances are emitted in reverse request order).
- An unprototyped C declaration (`void TRK_flush_cache();`) when one caller passes an integer
  and the prototype changes another function's register allocation (targimpl).
- Constant-pool order that no source order gives: --pool .sdata2 (dvdStatus_base,
  soundschedule).
- tests/test_data_strip.py's file-duplicate fixture moved to box_ivolume (UISUtils_parsehints
  is gone).

Not done, with what I found
- Gecko_ExceptionPPC.cp: needs two compilers. ExPPC_UnwindStack matches only with GC 1.3
  (604-line diff with 1.3.2), __unexpected only with 1.3.2 (with 1.3 one instruction differs:
  `addi r3,r30,0` for the first pooled string becomes `mr`). All other fragments match on
  1.3.2 with exceptions on.
- bsutil.cpp (draft bsu/bsutil_a.cpp): everything matches except DoWeOwnThisMemory: the
  out-of-line copy needs pools' local order (owned, base, offset), the copy inlined into
  BSUtilFreeObjectInstanceMemory needs update's (offset first); variants d-i in bsu/var.py
  don't give both. __arraydtor$623 is only chk's naming (port_unit names it by position).
- Step 2 (vtable owners): try_header CVector3 on sphere/sphere_testtriangle: both lost
  (SetCenter's copy schedules lfd/stfd differently with the shared CVector3, the known
  copy-constructor issue of research/notes/cvector3-header-variants.md). The others have the
  same private views; not attempted.
- world_volume.cpp (draft wv/ge_f.cpp, RTTI on, owns __vt__12CWorldVolume and the RTTI
  records): all 27 functions match (int results throughout, the list template the union of
  the fragments' local views, a private CVector3 as union {double pair[2]; float v[4];} so
  GetExtents copies as two lfd/stfd pairs (the shared header's operator= interleaves them),
  '#pragma auto_inline off' around RemoveSubVolume(list_iter)). Left: the order of the weak
  copies. The original has ~list right after ~CWorldVolume and reserve right after the
  constructor, then the TestCollisionWithSubVolumes instances; the draft emits ~list after the
  constructor (its EH action is the first request; ~CWorldVolume inlines ~list) and reserve
  last. Out-of-class weak ~list (v1/v2) moves it to the end and un-inlines it.
- Step 3: alarm (alarm_builtins has its own ISceneNode view and includes DwiContainers.h while
  the list fragments carry local dwi templates; hdr6: AlarmInfo has no default constructor),
  pumhandler/world_object/world_volume (dwi weak-copy fragments with local templates, the
  hdr6 code-level cases; world_volume also needs the vtable/RTTI owner treatment and has two
  different inline UpdateExtents helpers), anim_obj (own CVector3/CMatrix/ISceneNode views),
  player.cpp (not in complete_files.tsv).
