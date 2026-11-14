# climb1 (2026-11-12): ohc order climb over drafts <= 15 real lines
Tools here: ohc.py (real3's + REG/AFT region constraint for decl-only moves, EXT for .c), gspec.py (+double, unsigned char),
declspec.py (only 'T x;' lines move, within their brace region), perm.py (exhaustive permutations of a decl group),
fnblock.py (function body -> block file), d.py KEY [draft] [ctx] (real diff lines via score.eq), cd.py (check_draft.compare on a scratch file).
Result: no score change anywhere; bf893650 made three drafts natural at equal score. Table: results.tsv.
Finding: decl/statement order moves FP colouring (real3) but was inert on every GPR-rank, scheduling and stack-slot draft tried.
Scroll: else branch = inlined Reset(); its 6 lines are scheduling of that inlined body (5 only by reordering landed Reset).
