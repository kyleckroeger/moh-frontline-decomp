# unlockB2 notes (2026-11-12), took over unlockB's claims

Tools: blk.py LO HI (original .rodata block items with contents), cobj.py (compile a probe),
dd.py <tmpl> <func> <src...> (instruction-text diff count, relocations ignored).

## Landed
- f3585369 rcmp_main_delete += OPTION_PARSING_PLAYER::Play (1,868) with src/game/rcmp_main_block.h.
- 8afc1462 bsbifunc_schedules += BSInitBIFuncs (1,228) with src/game/bsbifunc_block.h (see log).
  The include must sit just before BSInitBIFuncs; at the top the .rodata/.bss base regs swap.

## Parked
- UserInterface::Init (ui/): header ui/UserInterface_block.h recreates +0..+476 exactly
  (prime list, 20-byte zero, quad @875, records @1292/@1319, 9 strings), but Init also reads its
  `char path[20] = ""` template at block +108 (@707, created before @875, i.e. by an earlier
  function or an inline's first expansion). Our compile always creates Init's own template at
  +476 (CW 1.3.2 does not share identical templates: a header static or a holder-local
  `char t[20] = ""` both tried), shifting every name by 20 (fast score 202).
  QUESTION: may Init (real code) copy its path from a header object, or may the holder expand
  an inline with the template? Both go beyond strings-only.
- PopUpMessageHandler ctor (pum/): header pum/pumhandler_block.h places +300..+408 right, but the
  ctor is 76 lines off (this r30 vs orig r31 swapped with the .rodata base, plus Load's loop
  colouring). Header position (20 spots), self copies: inert. __sinit_pumhandler_cpp waits on it.
