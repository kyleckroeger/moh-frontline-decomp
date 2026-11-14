# slots1 notes (2026-11-12): land2's stack-slot reading applied to five parks

Tools: slots2.py (land2's slots.py printing op+offset), dis.py (compile + one-line listing).

1. CAnimMorph::Draw (9; am0.cpp, v_*/w_*/x_*.cpp). Orig frame bottom-up: fresh 8, slot 12, dead op= temp 24,
   right arg copy 40, left arg copy 56, TransformPoint arg copy 72, b 88, a 104, right 120, left 136, view 152, pos 168.
   The arg copies are function-level temps (reverse creation); only the op= return temp sits below them, so 'left = view'
   ran inside an inline. Assign(left, view) gives the slots exactly (real 3: the left.zw store schedules early). Inert
   (9): { } scopes, if(1), (void) cast, op= body variants (return CVector3(*this), return other, out-of-class op=);
   CVector3(view) 73; position via 3-float ctor 19. No real helper found (RS has no CAnimMorph). Parked.
2. BIFunc_PlayerScreenFlash (39; psf*.cpp, grid*/). Orig: A (args 1-4, m_from) c@16 copy@20, B c@8 copy@12, A built
   first. Each pair = one argument block (ctor temp + by-value param copy). Set(CColor(..), CColor(..)) by value gives the
   pairs; to put A's block first Set must take (from, to) like the CFaderControl ctor (fader_control.cpp), but MWCC
   evaluates by-value args right to left, so B is built first (q1: 92, +2 len, extra saved reg). Two statements
   (SetFrom/SetTo) give the exact slots but break the load schedule (83). 80+48-variant grid (param order, ref/value,
   op= by value, ctor/BSArgColor/named byte locals RS-style, body order): best 34 = op= taking CColor by value (not
   natural, not committed). Parked.
3. DoTargetCheck (q11): slot layout = one inline taking CVector3 by value (copy @8) returning CVector3(x,y,z) (@24),
   both below the locals = copy_bool's Copy() exactly. But the orig reads the distance from the source (40/44): the
   temp's stores survive only if the temp is read in source (discarded result, unread const ref, unused local: stores
   dropped, -6). So the orig read the result somewhere dead; no natural form found.
4. CScene::Draw: FRAME FIXED. Slot offsets were already identical; the 16 bytes were the CDrawContext view's size:
   292 -> 304 aligned, Moh2.cpp's matched view is 288 (3 CMatrix + ptr + 92). unknownf8[44] -> [40]: frame 416, real
   49 -> 48 (sc1.cpp). Rest = orig hoists camera+192 into r29 before the screen call (one more saved reg, r25);
   reference/getter locals in 10 positions (sc1?/sc2?.cpp): inert or 142-150.
5. UpdateBoundingBox (q10): slots are ALREADY identical in the 101 index draft and the 56 BEST (both keep lo/hi in
   memory), so the slot reading adds nothing. Orig forwards the 0.0 init into the first Turn (constant-propagated) yet
   keeps lo/hi in memory. Promoted to registers (632, -22 len): plain lo.d.v[i], float lo[3] arrays (init list or
   stores), TurnAxis(CVector3&, CVector3&, int i), Turn taking float* with &lo.d.v[i] or lo.d.v + i. Pinned (56):
   operator[] and the index draft's plo/phi aliases (ubb_ptr.cpp = 56: the BEST56 gains don't need operator[], but
   BEST56 also uses one-operator Mul/Mul2 helpers, so not committed).
