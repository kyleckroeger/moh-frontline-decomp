# fresh3 (2026-11-12): middle third of fresh1/untouched.tsv (lines 41-80)

## Tools
- fresh3/who.py tmpl func addr size file [a b] [f|r]: generic value-to-register pair table (vec7 who.py generalised).
- fresh3/brchk_sel.py rows.txt: brchk for a listed set of functions. Middle third: branch diffs only in PROFILE_UPDATE (1),
  BIFunc_AddBuddy, DMMorphBlendDisplayLists, SoundSchedule::Update, MemoryCardBootUpPortCheck (all big/known knots).

## Improved drafts (committed)
- 6b950b45: EndDraw 19 -> 11 (below) + CCDBObject::Init chained zeroing -> plain v[2],v[1],v[0] stores (same 41).
- a5ad862e Update_Picture_Buffers: chained frame assignments + plane 'd' block -> plain forms, identical code (27).
- EndDraw: fresh3/ed/BEST16.cpp real 19/-2 -> 11/0 (length exact). The orig's stack word temp = an inline GetColor that copies the
  colour WORD through a cast into a local ('CColor c; *(unsigned int*)&c = *(unsigned int*)&color; return c;', as CFont::GetColor in
  font_render), then a byte-wise operator= (store order a,g,b,r best). Left: byte regs (orig g,b,a,r -> r7,r8,r9,r0, data ptr r6).

## Parked
- CCDBObject::Init 84 fast: FPR rotation of the extents block only (orig sz f30 callee, |r2.x| f13 volatile). Ext()/H() inline forms
  inert or worse. Also: the draft's else branch is a CHAINED assignment (declined form); separate stores need v[2],v[1],v[0] order.
- TRKSuppAccessFile: not worked (exhaustive earlier parks, allocator priority).
- MIX_audioslice 59 fast (57 with a second sndmix pointer for loop bounds, not proposed): m not coalesced into the final loop's
  IV (orig walks m's own reg r25, copies 'out'); 'sndfx.active = 0' zero held in callee r19 before the effect branch (inline param?).
  Writing 'out++' walks out in place instead (50 fast) - the orig does the opposite. fresh3/mix.
- SetFinishedLevelData stays 11: loop in the caller with i declared after stage ranks this/i right but CW then walks 'this' in
  place (drops the orig's 'mr r26,r28' IV copy): real 50/-1. 720 decl orders inert. fresh3/sfl.
- Update_Picture_Buffers: third plane Data() r7 / width r6 swap; see autopilot log line.
- PROFILE_UPDATE / DMMorphBlendDisplayLists: brchk diffs are length shifts only (fused fmadds / 2 instrs), not logic.
- TestCDBBranchNodeCollision<CLine3>: not worked (only __construct_array arg regs; exhaustive s17 probes).
