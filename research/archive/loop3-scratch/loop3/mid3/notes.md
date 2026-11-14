# mid3 notes (2026-11-12)

## Landed
- SNDVOICEI_free 0x8015dbd4 (396) into salloc (prepended). sf/salloc_new.c. Keys: releasevoice(&VOICES[voice]) at each
  call instead of the 'v' local (44 -> 1); 'SNDVOICE* q = &p[i]' in the group loop fixed the compare operand order.

## Drafts improved
- PopUpMessageHandler::Loop 27 -> 0 (pl/PUM_BEST.cpp, code exact, brchk clean). NOT landable: the empty 'case 8' needs
  the identity 'message = (PopUpMessage*)(void*)message;' (declined). All of: case 8: break / ; / {} / (void)0 / if(0){},
  default before/after, case 7+8 merged, cases 8+9 / 8+10 / 6+8+9 lose the 'bge' (-1 instr). Maintainer question.
- ResetShell (18, unchanged): invented ResetGame(1) helper replaced by plain flag stores + the landed SetGameType.
  Left: zero r3 / type 1 r4 swapped (statement orders, bool, type local at top: inert).

## Parked / tried (this run)
- DMClusterSkelTraverseHeirarchy: separate stores instead of the chained 3.4e38f init cost 31 -> 33 in every form
  (24 low orders, reverse loops, named lo/hi/big constant locals, 7 positions, 720 decl orders of b/c/depth/a/scratch):
  the extra two lines are 'depth++' scheduled two slots later. Kept the chained draft (open maintainer question).
  Whole body in an inline (count/bone at caller) moves params to the top ranks (144). Scoped decls inert.
- decodexac (28): table r9 vs orig r6 + in++ schedule. 240 statement orders, index/2D filter forms: inert.
- AnimStProcess (23): owner/slot r30/r31 swap. 8 forms inert; loop in an inline 29.
- SNDMIXI_volramp (20): orig keeps &sndmix in three callee regs (r30 alignedWork, r27 after the early return for
  value0f/scalemix/alignedMix, r31 loop bound). A named SNDMIXVIEW* after the early return: 68-72.

## Tools
- tools/v.py: run(name, [(old,new)]) harness on score.real (env T, F=func:addr:size, BASE).
- tools/br.py tmpl draft func addr size: branch-target compare (same length).
- ChooseCoverPointByType (20): plain 'dy = enemy.y - origin.y' (y,x,z order) = 20, same as the draft's '+ -' form (cleanup
  not committed); ref decl order, ty/oy copies, d decl orders inert/worse.
- ARAM_alloc_node (25): sentinel at function level passed by reference: 36, same rank pairs.
- LLCharObjCompositeMorph (18): 16 decode-line forms; the separate target++ is required; nothing below 18.
- PopUp case 8, more: an empty inline call, a dead store to a register local also prune the node; only identity keeps it.

## Landed commit
- 7dcdeb04 (pushed): SNDVOICEI_free + PopUp/ResetShell drafts. Verified 1,253,796 bytes after it.
