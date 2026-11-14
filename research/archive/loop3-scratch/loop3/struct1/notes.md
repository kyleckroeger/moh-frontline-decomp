# struct1 (structural matching of far-off drafts outside scripting), 2026-11-12

Tools here: sc.sh (fast score + sdiff structural), sbs.sh (differing lines), hill.py (decl-order hill climb on a
decl block file, fast.score), frame.py (prints a compiled function's frame size). Templates t_*.json = the index manifests.

## Code-exact in isolation, NOT landable (layout) -> drafts updated, commit 39b69cd3
- OPTION_PARSING_PLAYER::Play 0x8010370c (1868): 523 -> 0 code lines (fast 12 = only the .rodata base offsets).
  Source: rcmp_main_play.cpp (unit-ready header) / research/drafts/rcmp_play.cpp. Keys: real VIDEO_DISPLAY virtuals
  (dtor 0x08 so 'delete m_display' gives the double beq), 'if (m_software) GCSW else MOHFL', a stack record
  {int index; float history[16]; float ring[16];} with inline ctor + Add (fixes frame 304 and the r1+8/+12/+76 bases),
  inline Ticks() = mftb*12 read at the TOP of each iteration, 'FrameInfo* info = frame->m_info', hoisted decls in
  hill-climbed order (176 -> 22), '(float)(int)freeMemory / 1024.0f' (22 -> 12).
  BLOCKED: r22 = rcmp_main.cpp ...rodata.0 base; strings at +32..+120 sit behind "Reseting."/"Software Reseting."
  (orphans: stripped functions). Landing needs a gap function (on HOLD) -> maintainer question.
  .sbss: FirstTime$703 0x8034edd0 + init$704 0x8034edd4 are the function's own guarded static (unowned so far).
- UserInterface::Init 0x800bee4c (1160): 398 -> 0 code lines. Keys: CSprite inline class 'operator new(size)
  { return DWI_alloc(0, size, 0x400); }' + 'unsigned char unknown[100]' (double result copy mr r29/mr. r27),
  'new CSprite(path, ...)' in the headshot loop too, CColor(r,g,b,a) TEMPORARIES (slot order 8/12, 16/20),
  ints at +0x7d4/+0x7d8. LOGIC FIX: the floats cleared at the end are 0.0f (old draft 1.0f; check_draft now consistent).
  BLOCKED: r31 = UserInterface.cpp ...rodata.0 base; "" template at +108 (after the dwi prime table), names at +476..
  past orphan strings (Team Score..Accuracy, controller messages; scratch/tmp/block_orphans.py) -> same gap question.

## Looked at, released (no landing)
- CScene::Draw: frame 432 vs 416 comes ONLY from the command loop's 'SModCommand c = *cmd' copy (frame.py: removing
  the loop or calling '(this->*cmd->m_fn)()' gives 416 but drops the 16-byte copy the original makes at r1+16). Tried
  temp/by-value/getter/member/hoisted/assign forms: all 432. 'void (CScene::*fn)() = cmd->m_fn' = 12-byte copy (88).
  camera+192 early not solved (see airender/bigfiles notes).
- CVolBox::GetExtents: pure FP schedule (geometry bge2 already exhaustive). Not worked.
- PROFILE_UPDATE: structural 6 -> 4 with 'move = move * (move * move); g_fGraphCursor += move;' (unfused, 193 -> 190,
  v/pcube.cpp). Rest = which of hist/bins/i live in volatile vs callee regs; permuter territory (2 runs before).
- SoundSchedule::Update (1864): orig MP azimuth = one GetPosition into a by-value temp (200) + dead inline param copy
  (24) + two arg copies (232 FindClosestPlayer, 216 GetAzimuth); else path copies the unconditional 184 position to 248.
  Stack-slot layout knot; not attempted beyond reading.
- DrawBulletStats (1848, struct 92, NOT a rodata.0 user): all differences are int<->float conversion temp slots in the
  three MP blocks. Block 2 orig slots descend in evaluation order, block 1 ascend; ours forwards the single-use ints
  into the DrawText call (sum1 temp right after bx). fxy/ixy/cast/decl-order/inline-helper forms: 258 or worse.

## Batch structural scan (fast / sdiff structural), 2026-11-12
Update_Picture_Buffers 60/6, YUV_WriteRow 132/14, random 143/62, CScene ctor 162/15, UpdateBoundingBox 252/2,
TLT_LoadFromRepository 115/19, ShellInit 119/2, SweepAllExtents 272/15, GetClosestWaypointByType 245/?,
DetermineNewObjectivePosition 235/26, DMMorphBlendDisplayLists 496/56, PlayScriptedSound 133/31, SoundSchedule 226/68,
CProfileInitClass ctor 396/67, PopUpMessageHandler ctor 133/45, TRKSuppAccessFile 151/53, RenderSystem 1691/1069.
Structurally far: RenderSystem (4532; the draft is far from the original), DMMorph, CProfileInitClass, TRK, random.
