# rs2 (Rising Sun pass, continued from rs1), 2026-11-11

## PATH_event 0x8010b93c (1884): MATCHED in isolation, NOT landed (policy question)
- Function form = 0: scratch/loop3/rs2/pev/q_int_bl.cpp (old draft base). Key (on top of rs1's BEST6): hold the beats byte
  in an int local and build the record index in a separate 'a' local:
    beats = map->beats; a = eventnum; a *= beats; index = beats * (j * map->events); index = a + index; index += pfstate->beat;
  with 'int a; int beats; int index;' at the function top (after map). Path: 6 -> 4 ('a' temp: f_two3, operand order),
  -> 4 ('a = eventnum; a *= beats': permuter find, fixes the mullw/add operand order), -> 0 (int beats local; unsigned int too,
  unsigned char stays 4). Inert: getmastertrack decl orders/types/return/state local, j-loop block decls (128), top sweeps.
- Unit merge (current src/ea/path_stop.c + PATHI_eventrestore proto + the appended helpers/PATH_event):
  scratch/loop3/rs2/pev/PATH_event_unit_autoinline.c scores 0 on all 7 functions (fast real).
- BLOCKER: the original CALLS PATH_pause from PATH_event (but inlines PATH_stop). Under '-inline auto' ours inlines PATH_pause
  (535 lines off). '#pragma dont_inline on/reset' (the accepted form) around PATH_pause's definition stops that but also stops
  selectstate inlining inside PATH_pause (PATH_pause 83 off); the pragma state at the END of the definition decides (placed after
  '{' or before '}' = same; around PATH_event or on a prototype = no effect). '#pragma auto_inline off' ... '#pragma auto_inline reset'
  around PATH_pause gives 0 everywhere, but auto_inline is not on docs/Remaining.md's accepted list -> QUESTION for the user:
  accept '#pragma auto_inline off/reset' (sibling of the accepted dont_inline) for PATH_pause in path_stop.c?
  (The old draft's dont_inline around PATH_pause was the same problem: it breaks PATH_pause.) RS names selectstate PATHI_switchproject.
- research/drafts/PATH_event.8010b93c.cpp is still BEST6 (6); update it to q_int_bl.cpp with the next landing commit.

## Survey of Video/System/Other + all Shell/HUD rows (cmp.py; scratch/loop3/rs2/cmp.out)
No helper-boundary lead. RS counterparts only for: CInputManager::Update (RS input rewritten: event tables, buttons as masks;
diff is the button-pair shift registers), Scroll/Load/PopUp Loop (rs1), Init/ResetTimeStampStuff (already landed; queue stale),
TLT_LoadFromRepository (RS repository rewrite), CalculateLevelStats (RS CGameSettings version calls less, not more).
Tried anyway: RCMP::DECODER ctor (RS shows an out-of-line CODEC_IDATA default ctor = the draft's inline one; 40 default-ctor/ctor
forms, best stays 2: vtable reg r6 vs r5), MOHFL_VD_create (RS GCHW_VD ctor out of line = the draft's inline ctor; width/height
order, mode store position, new-result locals: best 17). No RS counterpart: RCMP video codec (MPC/YUV), ARAM, PROFILE, TRK, random,
CShellMenu (ResetShell/ShellInit/MoveStats/PortCheck), UserInterface Init, PopUpMessageHandler ctor, OPTION_PARSING_PLAYER.

## Handoff
Nothing held, nothing running (permuter finished). Only open item: the PATH_event pragma question.
