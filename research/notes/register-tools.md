# register-tools

Agent working notes, kept as written; dated claims may be out of date.


Solo /loop 2026-11-01 (the [[parallel-agents]] 10-30 plan): 85.06% -> 85.29%, HEAD 2767dd24, pushed 2026-11-02 (user asked). Stopped by me when the automatic sweeps ran out; the user has not yet said whether to continue.

Compiling with `-sym on` left code identical on all 1095 units that compiled in the check (125 SDK/library units did not compile under that helper) and its DWARF 1 lists each named variable's register. `scratch/solo/pred/varmap.py` and `sbs.py` show which variable is off. The agents never had this.

**Why:** a true register-allocation predictor from asm alone cannot work: first-def order predicts rank about 50%. Source order does: earlier-declared callee-saved locals get the higher register in 85-91% of GPR pairs.

**How to apply:** start any near-miss with varmap, using `scratch/solo/sweep/table2.tsv` (best draft per open function). Levers that landed code: a local assigned from a value-returning inline ignores declaration position, so write it directly (PATHI_choosebeat); a const named copy can flip a scheduling tie (Decode, UISExecuteFnc). Whole-draft copy/const/decl sweeps are exhausted on those drafts, so remaining work is per-function. Details in scratch/AUTOPILOT.md Log 2026-11-01 and scratch/par/TRICKS.md.

2026-11-02 update: plan for the last 15% is in AUTOPILOT "PLAN for the last 15%" + Decisions 2026-11-02 (chained 'x(copy = y)' declined; spline skipped). Steps 1-2 done, 85.41% at 33f5adcc. table3.tsv lists all open functions, but '-' does not mean untried: most have WIPs in the notes. Compile .c drafts with SUFFIX=c. Best landing recipe: Ghidra draft of a never-drafted function, sbs.py, then knobs (CStaticMesh::Init). Paused in step 3: sweeps exhausted, per-function yield low.

2026-11-04: user chose "group B by hand". Landed SNDPLATFORM_outputset (15fc2cd9, local, not pushed): 85.57%. Lever: two 'int const' field copies with later stores routed through them. Rank by real differing lines with scratch/solo/gb/real.py (real.tsv); table3 scores are reloc-inflated. Pair sweep scratch/solo/gb/route2.py (passes outputset control; route.py does not): no landing, but 19 of 40 fns got no usable variants. Most near-misses have earlier attempts in the notes: grep before retrying. Stopped to ask the user.

2026-11-02 later: user put a HOLD on "gap functions" (whole-file unit strips an unmatched function of its file, leaving bytes uncovered); do not build unless lifted. Pushed 33f5adcc at 85.41%, then paused to reassess. ShellMenu.cpp drafts in scratch/solo/sm/ (6 functions match in isolation); blocked by CalculateLevelStats, Add_MPPlayerKilled, ResetShell. See AUTOPILOT decisions.

2026-11-08: plan changed by user (AUTOPILOT Decision 11-08): rank by fewest real lines, stop for review after ~10 landings or 87%; gap hold stays; no parallel agents. Landed ShowDVDError (struct local from inline returning it fixes slot order) and Add_MPPlayerKilled (GetTeam accessor), 85.67%, b7b6fffa, 2 unpushed. New discovery tool scratch/solo/ret/sweep.py (identity RetV routing; never land RetV) + rebase.py. real.tsv is stale: rebase every gain before believing it. Buddy merge and bsbifunc built-ins are NOT cheap (partial-SROA knot; .bss base users); toilet literal order needs a matching Repository.

2026-11-08 later: b7b6fffa pushed, 85.67%. Rank from scratch/solo/ret/rank.tsv with scratch/solo/ret/score.py (the 'stale real.tsv' line above was partly a scorer bug: the sbs ',0' filter undercounts). RetV sweep exhausted. Step 7 review reached: waiting on user's gap-function decision (estimate ~12.5 KB -> ~86.55%, untested, in AUTOPILOT Log). Hold remains until they answer.

2026-11-08 ShellMenu pilot (user lifted gap hold for ShellMenu.cpp only, 'go for it'): stopped before landing. Orphan strings from linker-removed functions decide the block layout, which is the 10-29 declined case. Scratch test (scratch/solo/sm/hold.py) shows 7 functions, 7424 B, look code-exact given true offsets. Waiting on user: allow a strings-only stripped holder? Details in AUTOPILOT Log. Don't land any holder without that answer.

2026-11-08 /loop: user said to use best judgement, so I chose the strings-only ShellMenu holder (ShellMenu only, local commits, no push; must be flagged in the report). Header generator scratch/solo/sm/genh.py, test scratch/solo/sm/tb/h1.py: 7 fns real 0 at compile level, block bytes exact. Nothing landed; the .sdata2+0x4 pool failure is still open. Next steps at the end of the AUTOPILOT Log.

2026-11-08 end of /loop: LANDED 7 ShellMenu functions via src/game/ShellMenu_block.h (my judgement call; cf0be9de..9258a344, NOT pushed), 86.19%. Holder stays ShellMenu-only until the user answers. Lever: file-scope static consts emit in definition order, and 'T t = kStatic;' copies like a local aggregate. Gotcha: score.py ignores SDA constants; 3 drafts scoring 0 had wrong floats, so only one_unit.py proves a draft. ShellInit parked at 124.
