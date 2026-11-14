# fresh1 (2026-11-12): queue rows untouched this run

Table: scratch/loop3/fresh1/untouched.tsv (mk.py; counts of each function's name in CLAIMS.txt,
loop3/*/notes.md, autopilot.md since 11-11 and all research/log). 118 unlanded non-MP rows; the
zero-mention ones were mostly old register knots with long histories in research/log.

## Landed
- e959464b ShellMenu_sku += CShellMenu::MoveStatsIntoProperAreaForIstudio (236). The parked draft
  had the WRONG LOOP (player index advances only when a record is copied). Found via branch offsets.
- 2cde43ff drafts: CAnimMorph::Draw logic fix (CAnimated::Draw only with a mesh), CheckForObstructions
  IsAbove block order, quick_sort_ 11 -> 6, CheckLineCollision operator| order. research/tools/brchk.py.

- 710b00b8 drafts: death_cb 28 -> 5 (const param copies dropped), SFILTER_unpackxaf 22 -> 7.
- (landing) drafts: DMClusterSkelTraverseHeirarchy 143 -> 31 (param copies dropped; length exact).

## Tools
- research/tools/brchk.py [max]: compile every index draft, compare branch offsets (fast/sbs hide them).
- objchk.sh (scratchpad copy; same idea for one source): see TRICKS.md entry.

## WIPs (not in research/drafts unless noted)
- SFILTER_unpackxaf (now in research draft): xaf/BEST7.cpp, sbs 21 -> 7 with natural code. Left: orig updates state->frame before out; every frame-first
  order sinks the result copy (37) except the declined '+= (result = decoded)' (4 lines).
- death_cb (now in research draft): dcb/d.cpp = old draft WITHOUT the three const param copies (c_header/c_event/c_data):
  32 -> 9 sbs lines (2 IV-init schedule pairs + relocs). Decl-order hill climb: dcb/hill.log.
- CheckLineCollision: in research draft (operator| 'm_bits | bits'). Left mask/flags r4/r5, types r19.
- CheckForObstructions: in research draft. Left: sign r31 / this r30 ranking (soldier cfo18 knot).
- quick_sort_: in research draft (6). Left: tail-loop next r27 vs r29.
- CAnimMorph::Draw: research draft = plain 'left = view' (9); am/c2.cpp = 3 with an Assign() wrapper
  (identity-like helper, not proposed). Orig's operator= return temp sits below the param copies.

## Parked without gain (forms tried in CLAIMS.txt lines)
SFILTER_unpackxalf (8-byte unused slot), SNDtimeremaining (voice/sndgs-base swap), ChooseCoverPointByType
(threat vector FP colour; SubV 312), SFILTER_rsf (0xffff mask hoist), ARAM_alloc_node (callee ranks),
LLCharObjCompositeMorph (not worked).

## More parks
UpdateAnimation (ua/v7.cpp fast 110 -> 108: 'short sx = dx * t; angles[0] = sx + x' fixes the add order; int->float
conversion colouring + 4 other knots), AnimStProcess (owner r31 / slot r30 only), SNDMIXI_volramp (&sndmix in r27),
decodexac, CPartBin::Link, ShellInit (block pilot), PopUpMessageHandler::Loop ('self' removal inert).
