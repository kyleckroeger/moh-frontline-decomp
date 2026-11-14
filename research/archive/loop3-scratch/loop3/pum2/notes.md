# pum2 notes (2026-11-12): PopUpMessageHandler ctor

Tools: dd.py (diff count, counts +/- so 16 = 8 lines), sd.py (side by side), var.py BASE < variants
('OLD>>>NEW' pairs split by '----', variants by '===='; prints ctor score and Load__17CUserMessageTableFPcPc score
from the same compile), perm.py/perm2.py/perm3.py/rnd.py (decl / loop-statement / random combo searches).

## Result: 76 -> 16 (8 instruction lines), draft committed (v1.cpp = draft + block header include)
Write Load exactly as the LANDED one (src/game/pumhandler_umt_unload.cpp, 95aae29e): Unload and
CreateUMStructFromFile as separate inline members, Load calling Unload / CreateUMStructFromFile, the loop
locals declared at the helper top (entry, end, i, extra, length, then index/pos/text initialised).
That alone fixes the this r30/r31 vs .rodata-base swap and nearly all of the loop. The same source also
compiles the out-of-line Load to 0 (non-inline Load in the probe = check both at once).

## Left: base/extra volatile swap in the loop
Orig: &umTable loop base r7, extra r6. Ours: base r6, extra r7 (8 lines). Inert (all keep 16 or worse):
120 helper-decl orders, 398 dependency-valid loop statement orders, 400 random 8-decl orders, 500 random
combos (types of extra/length, text expression forms, increment forms), in-loop decls, int/uint types,
struct index-entry view, (m_table+i)->, this->, umTable., self const copy (worse), tbl local for new,
non-inline Load/Unload/Create, header include position (3 spots), static/extern umTable, else-branch forms.
Only lead: with NO extra local (entry[0] read twice) base gets r7 and extra r6 as orig, but entry[0] is
reloaded (different code), i.e. extra must come out "later" than the base vreg while staying one load.
__sinit_pumhandler_cpp not tried: needs the file's .bss layout (orig list at base+92/+80, ours +40/+28)
and 0.0f in f31 across the first call.
