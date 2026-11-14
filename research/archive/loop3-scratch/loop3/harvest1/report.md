# harvest1 (2026-11-12): scratch bests moved into research/drafts

Method: every BEST*/best* file under scratch/loop3 plus every .c/.cpp path named in scratch/loop3/*/*.md,
CLAIMS.txt, research/plans/three-day-loop.md and research/log/*.md (188 files, 884 function/file pairs,
gather.py -> cands.tsv), scored with each draft's index compile settings (check_draft.compare: real lines and
status; cand.py/batch.py also count brchk branch-offset differences) -> res.tsv. A candidate replaced its draft
only if better, consistent, no new branch differences and no declined/pending form. Where the gain was one local
change inside an older or reduced file, the change was ported into the current draft (cls_merge, svf_merge,
dx_merge) instead of swapping the whole file.

Commits: 64c0d5b0 (batch 1, 8 functions), f476a06f (batch 2, 9 functions).

## Replaced

| function | index before | draft rescored | new | source |
| --- | --- | --- | --- | --- |
| TLT_LoadFromRepository | 53 | 53 | 13 | scratch/loop3/struct5/tlt/TLT_BEST.cpp |
| BIFunc_GetInitialBuddyOrders | 27 | 27 | 7 | scratch/loop3/scripting/chk/GI_BEST7.cpp |
| BIFunc_MechFollowPath | 105 | 105 | 83 | scratch/loop3/scripting/mfp3/BEST83.cpp |
| BIFunc_PlayerScreenFlash | 42 | 42 | 39 | scratch/loop3/scripting/psf10/BEST39.cpp |
| BSFileReadClass | 708 | 708 | 414 | scratch/loop3/scripting/s11/bsf/BEST414.cpp |
| CalculateLevelStats | 9 | 9 | 1 | scratch/loop3/harvest1/cls_merge.cpp |
| CreateDestructable | 33 | 33 | 11 | scratch/loop3/scripting/cd/a1.cpp |
| CreateRootHierObject | 44 | 44 | 2 | scratch/loop3/mid2/CRH_BEST2.cpp |
| FlagAsOccupied | 4 | 4 | 3 | scratch/loop3/airender/fao/BEST3.cpp |
| GetExtents | 45 | 45 | 37 | scratch/loop3/geometry/bge/best_split_4_d0m.cpp |
| PATH_event | 6 | 6 | 0 | scratch/loop3/rs2/pev/q_int_bl.cpp |
| PROFILE_UPDATE | 196 | 113 | 88 | scratch/loop3/struct1/v/pcube.cpp |
| SNDMIXI_volramp | 28 | 28 | 20 | scratch/game/smixer_volramp_wip.c |
| SNDVOICEI_free | 47 | 47 | 44 | scratch/loop3/harvest1/svf_merge.c |
| decodexac | 32 | 32 | 28 | scratch/loop3/harvest1/dx_merge.cpp |
| SoundSchedule::Update | 441 | 441 | 50 | scratch/game/soundschedule_full.cpp |
| TestCollision | 56 | 53 | 52 | scratch/loop3/vec7/tcp/hc2_best_1.cpp |

Notes: PATH_event is code exact as a function, but landing still needs State question 5 (`#pragma auto_inline`).
GetInitialBuddyOrders' best uses three single-use inline helpers, one of them a vec_iter overload of
GiveBuddyOrders that only forwards `it.operator->()`; treated as an inline helper, not an identity wrapper.
CreateRootHierObject's best defines the file's own AllocateBoxVolume (attested, same body as
src/game/objcreate_volumes.cpp).

## Skipped (draft kept)

| function | scratch best | score (draft) | reason |
| --- | --- | --- | --- |
| UpdateBoundingBox | scratch/loop3/vec5/UBB_BEST56.cpp | 56 (101) | every better UBB file (56-100) uses the member `float& operator[](int)` pin: State question 10, pending |
| CParticleSystem::RenderSystem | scratch/loop3/vec3/RS_BEST514.cpp | structural 514 (whole-file draft, no real_lines) | operator[] pin plus the inferred CMatrix33 view: question 10, pending |
| DoTargetCheck | scratch/loop3/scripting/s12/dtc/copy_bool.cpp 2, DISC0_ctorcast.cpp 0, s11/dtc/BEST61.cpp 61 | (96) | all need the declined identity Copy() or the ctor-cast view that changes landed AIFilter_queries (question 11). The natural bool-return fix alone on the draft scores 97 (harvest1/dtc_bool.cpp): no gain |
| CAnimMorph::Draw | scratch/loop3/fresh1/am/c2.cpp | 3 (9) | needs an Assign() wrapper (identity, declined). The current draft itself defines `inline CVector3 Copy(const CVector3&)` (line 28): check that use |
| DrawTextCentered | scratch/loop3/sweep1/DTC_BEST0.cpp 1, airender/rs/dtc/g2.cpp 3 | (8) | per-fragment inline view order differs from landed DrawText: question 8 |
| FlagAsOccupied (variant) | scratch/loop3/sweep1/FAO_BEST3.cpp | 3 (4) | changes the CoverPointTypes inline view (question 8); airender/fao/BEST3.cpp (same score, use-site change only) was used instead |
| CreateDumpObject | scratch/solo/cdo/h.cpp | 31 (56) | RetV identity wrapper (declined); nothing else beats 56 |
| CMatrix::Rotate | scratch/loop3/sweep2/rot/k_20.cpp | 13 (14) | adds `MulV(a, b) { return a * b; }`, a wrapper only there to steer (identity-style) |
| CShellMenu::ShellInit | scratch/solo/sm/tb/si2.cpp | 125 (135, status differs) | uses the ShellMenu block's tables inside the function (`SMBQuad quad = s_unusedQuad`), `#pragma exceptions off`, and predates near1's 4ef46f60 switch-order fix (same 125 without the pragma). Needs a policy look at block data used by real code |
| BIFunc_AddBuddy | scratch/buddy/buddy_t4.cpp | 202 (207) | the gain comes from a different BuddyInfo view (336-line context diff) that conflicts with GI_BEST7's view of the same file; gated by GetInitialBuddyOrders anyway |
| random | scratch/lib/random1.cpp | 81 (82) | adds a branch-offset difference |
| CreateDestructable (variants) | scratch/loop3/systems5/cd_i6.cpp, cd_o4.cpp | 415/420 with the draft settings | built for the objcreate_swap template; scripting/cd/a1.cpp used instead (11) |
| TLT_LoadLevelContentsFile | (State: code exact) | (3) | needs the dead-branch string placement (question 3) |
| GetClosestWaypointByType | scratch/loop3/struct6/gcw/GCW_BEST_S0.cpp | 94 (94) | no scratch file beats the draft (struct3-5 GCW_BEST* 165-179) |
| CMatrix::Orthonormalize | scratch/loop3/vec2/orth/BEST7.cpp | 7 (7) | equal, nothing to move |
| DMClusterSkelTraverseHeirarchy | scratch/loop3/fresh1/dm/BEST.cpp | 31 (31) | equal. The draft has `low... = 3.4e38f` constant chains: question 13 |
| PublishEvent | late3 pe0 | 3 (3) | already committed by fresh2 (55afae70) |
| Active claims | EndDraw, Update_Picture_Buffers (knots1) | | not touched |

Files: gather.py, cands.tsv, batch.py, res.tsv (all scores), cand.py (compare a draft with candidates), plan.tsv,
stage.py, stage.tsv, stage/ (the committed files), apply.py.
