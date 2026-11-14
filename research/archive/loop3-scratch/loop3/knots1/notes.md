# knots1 (2026-11-12): fresh3's five near-misses

## Results
- Update_Picture_Buffers: CODE EXACT (fast 0 on all 5 getpic functions; real 6 = relocation lines of the
  fragment manifest only) with an invented inline `third_plane(int height, int width, unsigned char *data)`
  used for each third plane. Parameter order is the lever: (h,w,d) 0, (w,h,d) 48, plain expression 60,
  other orders 72-96 (knots1/upb/v1.py). NOT LANDED: the 11-12 audit rule (no register-steering helpers that
  are not from Rising Sun/original symbols) makes it a maintainer question. Draft = knots1/getpic.cpp
  (whole merged getpic.cpp: Decode_Picture + UPB + slice/decode_macroblock/Add_Block, header rewritten
  for the merge). Landing recipe on a yes: replace src/ea/getpic.cpp with it, delete getpic_head.{cpp,json}
  and its project.json entry, port_unit getpic (--category reconstructed_game, --like getpic.json; .text
  0x80100950.., owns the getpic_head .sbss statics), poolvals/one_unit, land.
- CCDBObject::Init: real 41 -> 22 (fast 84 -> 44): read the size components straight from d->m_size and
  position.x straight from d->m_position (no sx/sy/sz/px locals), keep py/pz/qx/qy/qz locals. That moves
  the callee FPR set to the orig's (0.5 double f31, sz f30); what is left is a volatile rotation in the
  first block (px/py/pz f0/f1/f2 vs ours f2/f0/f1, |r1.y| f6 vs f4, |r0.x| f5 vs f3). 512 local/direct
  masks (cdi/grid2.py), decl/statement hill climbs (cdi/hc3.py), SubV/AddV on min/max: floor 44.
  Draft knots1/cdi/base44.cpp.
- EndDraw (11): data pointer r9 vs orig r6 survives everything: op= orders x {=, SetColor by value/ref,
  field-wise SetColor, by-value op=}, GetColor on CFont (landed body) and GetScale(float&, float&) inline,
  data local, the inline written out in the caller. Scale-first source gives D=r6 but the wrong order.
- SetFinishedLevelData (11): i always ranks below this when the loop is in an inline (member inline, free
  inline with any param order, health/success calls moved in, do-while, info local, int types, self
  copies: all fast 22). 'i = -1; while (++i < 45)' gives fast 9 but a different loop shape.
- MIX_audioslice (59): 35-site m->/sndmix. spelling hill climb inert; the zero in r19 is not an inline
  param (CW constant-propagates it: 73) nor a local 'int active = 0' (59). Compare-operand flips inert.
