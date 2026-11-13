# Shell screen scripts (UI Studio `.scr`)

The front end (title, Mission Log, options, memory card, briefings, journals, multiplayer
set-up) is driven by EA's UI Studio ("iSiren"): one `.scr` file per screen, inside
`DATA/SHELL/shell.viv` on the disc. This page describes the file and its byte code as far
as needed to find which screen and input send a given shell message. Everything comes from
the interpreter's code in this repository: `src/game/UIStudio.c` (`PatchScrData`,
`UISLoadScreen`), `src/game/UISUtils.c` (`UISFindEventPC`, `UISExecuteFnc`),
`src/game/UISStack.c` (`UISStackProcess`, the interpreter) and `src/game/IStudio.cpp`
(input events, the message callback). `research/tools/uis_disasm.py` lists a script's
event handlers, disassembles them and finds the senders of a message.

## Screens

The game names the screens in `ShellScreenInfo` (`.data` 0x801870d8; `ResLoadScreen`
reads it through `ScreenInfo`): 43 file names, and a screen's id is its index. The ids that
matter for the main menu: 0 `Master_Main.SCR` (the title, "press START"), 10
`opt_Difficulty.SCR`, 38 `MissionLog.SCR` (the main menu: the Mission Log folder), 39
`ConfigPick_NewGame.SCR`. In byte code a screen (or a thread of the current screen) is named
by a 32-bit word `id << 16 | group`, group 0 for the shell.

## File layout

All words are big-endian. Offsets are from the start of the file (the loader adds the
file's address to each, `PatchScrData`).

| Offset | Field |
| --- | --- |
| 0x00 | number of controls |
| 0x04 | offset of the control table (20 bytes per control) |
| 0x08, 0x0c | number and offset of the map table (12 bytes each; word 2 is an offset) |
| 0x10, 0x14 | number and offset of the fix-up list (offsets of words holding a map index, replaced by the map's address) |
| 0x18, 0x1c | number and offset of the object table (8 bytes each; word 1 is an offset) |

A control-table entry is five words: the control's data (its variables, see below), the
number and offset of its sub-control list, and the number and offset of its event list.
An event-list entry is a halfword of flags, a halfword event id and a word with the
handler's offset. Flags `0x8000` mark a control's own handler (`UISFindEventPC`), `0x4000`
a hint handler, neither a sub-control handler (the low bits name the sub-control); an id of
0xffff ends the list. A sub-control (three words: control, event count, event list) has
entries of an id halfword, a state halfword and a handler offset.

Event ids 0 to 19 are the pad: `iSirenEventProcessFnc` sends event `n` while input `121 + n`
of the input manager is set (value 1.0 for 0..15, the analog value for 16..19). With the
default key map, logged on the Mission Log screen (`research/probes/uis_event_probe.cpp`):
START 0, D-pad up 2, down 3, left 4, right 5, B 6, A 7, Y 8, X 9, Z 11, the main stick's
vertical axis 17. Ids from 0xfffb up are the studio's own (activation, initialisation and
the like).

## Byte code

A handler runs on a stack of 32-bit slots (`UISExecuteFnc`): it pushes the result slot,
the event's parameters (for a pad event, its value as a float), the control's address and a
return address of 0, then runs `UISStackProcess` until the code returns to address 0.
Inside a handler, slot -3 is the event value and slot -2 the control: `85 off16 -2` reads
the control's variable at `control + off`. A control's variables lie in the file after its
data, so one control can read another's (in `MissionLog.SCR`, control 217's `+0x344` is
control 221's `+0x70`).

Each instruction is one opcode byte and its immediate operands (big-endian). Operands not
listed are taken from the stack.

| Opcode | Operands | Meaning |
| --- | --- | --- |
| 0x0f, 0x10 | imm32 | push an integer |
| 0x11 | imm32 | push a float |
| 0x0b | | **message**: pop the count `n`, drop one slot; the message number is the next slot down, followed by its arguments; calls `GameMessage(number, screen, n, args, result)` |
| 0x0a | | call a client function from the function table |
| 0x02, 0x71 | count8 | start thread `id` of the screen named by the word below the arguments (thread action 0) |
| 0x45 | count8 | the same, the id read through a pointer |
| 0x08 | | thread action 3 (stop/unload) for the popped id; 0xffff means the current screen |
| 0x03 | | thread action 1 with a value |
| 0x3e / 0x3f | | pop an offset, then a value: jump if nonzero / zero (relative to the next instruction) |
| 0x40 | | pop an offset and jump |
| 0x43 | imm32 | call: push the return address, jump to the file offset |
| 0x44 | | return to the popped address (0 ends the handler) |
| 0x41 / 0x42 / 0x52 | | dup / drop / swap |
| 0x1a / 0x1b | | read / write the slot at a relative index |
| 0x55 / 0x56 / 0x57 | off32, slot16 | load / store / address of `*(slot + off)` |
| 0x6b | off32, slot16 | push `slot + off` |
| 0x59 | off32, slot16, byte | set a control's visibility |
| 0x5b..0x62, 0x6c..0x6e | (some: off32, slot16) | array element read, write or address |
| 0x63 / 0x64 / 0x65 | imm32 | duplicate the top n times / drop n / copy n slots |
| 0x1c..0x2d, 0x5a | | integer and float arithmetic, logic (and, or, not, abs, negate, add, sub, mul, div, mod) |
| 0x32..0x37 | | integer compares: `le ge lt gt eq ne` of the top slot against the one below (`sp[-1] <= sp[-2]` and so on) |
| 0x38..0x3d | | the float compares |
| 0x2e..0x31 | | increment and decrement (int, float) |
| 0x12..0x15, 0x6a | | int and float conversions |
| 0x18 / 0x19 | | load / store through a pointer |
| 0x0d | | push the screen data's address |
| 0x06 / 0x07 / 0x58 | (0x58: kind8) | start, stop or start an advanced rate function (animations) |
| 0x0e, 0x49..0x4c | | string copy, length, get, set, format |
| 0x4e | | skip 4 bytes |
| 0x4f / 0x50 | | clear / set "event not processed" |
| 0x51, 0x70 | count8 | start a modal thread and suspend the handler |
| 0x46, 0x47, 0x09, 0x54 | | activate a control (thread actions 5 and 6) |
| 0x48 | | stop the current screen's thread |
| 0x4d, 0x72, 0x76, 0x0c | | other thread actions (2, 8, 9, 7) |
| 0x6f | | push a map-table entry |
| 0x73 | | is a rate function running |
| 0x74 / 0x75 | | set / test a bit of the studio's flags |

A message call reads `push n-args ... push number push count 0x0b`; for example
`41 10 00000154 10 00000000 0b 42` is "dup, push 340, push 0, message, drop".

## Message 340 (`CShellMenu::ResetShell`)

`uis_disasm.py --find 340` over the 43 scripts finds two senders, both in
`MissionLog.SCR`. The bytes `10 00000154 40` in `Master_Main.SCR` (0x5493) and
`Options_Main.SCR` (0x398a) are "push 340, jump", a branch over 340 bytes, not the message.

- 0x1941c, in the handler at 0x19336 of control 217 (the folder's menu), which runs for
  pad events 0, 7 and 14. The control's variables: `+0x7c` is 1 while the menu is shown
  (set by its initialisation handler), `+0x74` the selected item (0..2, wrapping; events 2
  and 3 move it and play sound 0 through message 530). On the select event with `+0x7c`
  set, item 0 starts thread 17, item 1 thread 16, and item 2 clears `+0x7c` and opens a
  sub-choice. With `+0x7c` clear, the select event sends message 340 when control 221's
  choice (`+0x70`, which control 221's events 4, 5 and 16 change) is 0, then starts thread
  10; otherwise it sets `+0x7c` again (the event-6 handler does the same).
- 0x18d9a, in code after the `ret` of control 221's event-16 handler (0x18c7b). No handler,
  call or jump reaches it: dead code.

So the game resets the shell (cheats, multiplayer records and the rest that `ResetShell`
clears) from the Mission Log menu's third item, on the first of its two choices. On the
routes' first Mission Log screen the D-pad's up and down (events 2, 3) did nothing, so
control 217's menu was not the active one there, and `route_reset_shell` sends the message
from a probe instead (docs/Dolphin.md). The new
game path of the routes (START, A ...) goes through control 2's code at 0x174a0 instead
(messages 430, 435, 437, then screen 10).
