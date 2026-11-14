# sweep1 notes (2026-11-12): recipe sweep over parked Audio/Shell/System/Video/AI/Rendering near-misses

Tools: sc.py <tmpl> <func> <addr> <size> drafts... (real score, -s side by side); region.py (group variants by
instruction window); syms.py (nm of our object). Run from the repo root.

## Landed
- 2e4ac3cd NEW font_centeredf: CFont::DrawTextCenteredF (512, .sdata2 pooled). Mul inline with compound body for the
  width product + function-level compound for the viewport-x product.

## Code-exact, NOT landed (policy question)
- CFont::DrawTextCentered 0x8007cdb0 (416): DTC_BEST0.cpp scores 1 (= only the lis/lfs relocation line; structure and
  registers exact). Needs (a) CScreen::ScaleX(float f) { return Mul((float)GetWidth(), f); } for the width, (b) the
  compound 'center = (float)GetWidth(); center *= viewX;', and (c) the inlined DrawText with 'float scaleY; float
  scaleX; GetScale(scaleX, scaleY);' (scaleY declared first). DTCF (landed) and the landed out-of-line DrawText
  (font_dtor) need scaleX first; with the scaleX-first view DTC is 5 (scale FPRs f28/f29 swapped; 24 decl orders,
  width/center/DrawText grid of 27 inert). QUESTION: may a separate fragment carry its own view of the inline DrawText
  (declaration order of its two float locals differs from the other fragments)? This resembles the declined
  'duplicate inline for UpdateTargetPosition'. Port would be a new unit between font_centeredf and font_dtor, probably
  '--pool .sdata2' like DTCF.

## Improved, parked
- FlagAsOccupied 4 -> 3: FAO_BEST3.cpp (operators '&=' as 'int t = m_bits; t &= x; m_bits = t;'). Left: the 1|2 'or'
  dest register (orig B's reg, ours A's) and the 0xff constant register. 100+ operator/mask/GetTypes forms tried. Note
  the landed GetAvailablePeekPosition uses the plain operators (my form gives it 4), so landing FAO would mean a
  different operator view per fragment (same policy question as DTC).

## No gain (forms tried, all new to these functions)
- RCMP::DECODER ctor (2): 8 body forms (copy-ctor init, const zero, late src, helper, member inline) + compiler 1.3,
  1.2.5, -O4,s: all 2 (vt r6/r5).
- CInputManager::Update (3): 72 shift-pair forms (bool copies, 'x = 1; x <<= s', const/late, masks vs amounts as inline
  args, arg orders, inline levels). The IR is canonical: first-extracted amount always pairs with the first slw.
- PublishEvent (2 -> stays 3 raw here): 62 forms; any callback local in loop 2 fixes the schedule but rotates
  it/event/subject (46): const param copies, (bool), comma for, shared it/callback at function top, access paths inert.
- GetPeekLocation (3): 27 compound-operator + 14 mask-constructor forms, stays 3 (D-load scheduled after the or).
- SNDPKTPLAY_start (2): 30 flat short / byte-offset / IV forms; the IV init always folds the +4 or loses the base add.
- MOHFL_VD_create (17): const/self/late copies in the ctor and at the call (100 forms) inert. '::new (mem) GCHW_VD'
  (placement) gives the orig obj r31 / decoder r29 but stores go via r3 (154).
- EndDraw (11): 100 colour-copy forms. A user copy ctor 'CColor(const CColor& c) { *this = c; }' reproduces the orig's
  ascending register order (data r6, g r7, b r8, a r9, r r0) but copies straight from the struct (no word temp);
  every word-temp form gives descending registers.

## Handoff
Nothing held, nothing running.
