# zero1 notes (2026-11-12): shared-zero study
Tools: pc.py (compile a probe with the levelstats flags, disasm), summ.py (li/mr summary per function),
var.py (CLS body variants on cls_T.cpp/cls_T2.cpp), scan.py (all drafts: li/mr pairs orig vs ours), pairs.py <func>.
Probes p/b..f.cpp. Rule in ../TRICKS.md ("Shared zero ... the rule").
- LANDED 26fe226b ShellMenu_levelstats += CalculateLevelStats (432): reuse most/weapon for the location search;
  GetCumulativeStats same form, MostHit inline removed (still 0). Pushed, lock released.
- Scan: no other draft has an orig-only shared zero (CPartBin_Link, TRKSuppAccessFile, DMMorphBlend hits = IV inits/colouring).
