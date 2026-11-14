# rank1 notes (2026-11-12): callee-rank study ('later value above this')
Tools: pc.py <src> [func] [json] (game flags; default player_begincrouch.json), extab.py <addr> (orig extab record),
ourext.py <json> <draft>. Probes p/a..q.cpp. CFO variants cfo/ (v.py specs s1-s3; env.sh).
Rule and evidence: ../TRICKS.md "Callee-register RANK". Summary: small functions rank locals (decl order) > params
(reverse order) > ...; inline locals > caller locals; hoisted const bases top. 'this' at r31 in big drafts = degree
promotion (Chaitin threshold), shown by the K/L probe flips.
- CheckForObstructions 25: 'int sign = 1; if (IsAbove) sign = -1;' fixes ALL register ranks (sign r31/this r30) but
  hoists the li 1 before the IsAbove inline: 36 (cfo/A.cpp). li-after forms D/E/F/G (bool local etc.) 25, ranks back.
  Dead pre-init (int sign = 0 or = 1, then the if/else) is removed before allocation: 25 (s4.py).
- CPathNodeList: extab identical to ours (no cleanups) -> the coordinator's EH lead does not apply. Probe Q (NextPrime-style
  table + const table in a loop) puts the .rodata base at r31 as in the orig; our draft's base is demoted by pressure.
- SetFinishedLevelData / DMCluster / GetClosestWaypointByType / CreateDumpObject: not attempted beyond analysis (prior
  agents exhausted decl orders and forms; no new natural lever from the rule).
Nothing claimed, nothing held.
