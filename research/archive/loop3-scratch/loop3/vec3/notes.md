# vec3 (2026-11-12): UpdateBoundingBox + RenderSystem with the alias-free operator[] pin (from vec1)
Tools: sc.py (python3 sc.py ubb|rs <src> [d] -> (structural, real) [+ row diff]; sc.hunks() for difflib hunks),
hc.py (first-improvement sweep: adjacent swaps, insert moves, per-line forms), hcd.py (decl insert moves),
rw.py / rw_rs.py (random walk accepting sideways moves; rw_rs adds [i] <-> .x spelling toggles and keeps
RandomFloat() lines in order). Usage: python3 rw.py ubb SRC OUT 'start marker' 'end marker' ITERS 1.

## UpdateBoundingBox (101 -> see below), no aliases, operator[] pin only (from vec1/ubb_a.cpp)
- u5.cpp: Turn PARAMETER ORDER 'Turn(float a, float v, limit, lo, hi)' (a before v) fixed block 1's v/a
  register swaps: 93 -> 81. Inline parameter order sets register colouring (all perms with a before v = 81).
- u11.cpp: first-block Min/Max merges written as ternaries (like the p0/p1 merges above them): struct 2 -> 0, 79.
- DECLARATION ORDER of the block-2 floats sets their colours: 'age, vx, vy, vz, ...' first gives
  age/vx/vy/vz consecutive registers like the orig (u15.cpp). Real got worse alone (88); random walk from it:
  u17.cpp (0, ~58). Left: t/t2 in f31/f30 while the orig has sv.z/sa.z (CSE'd loads) in f30/f31 and t/t2 in f8/f10.
- BEST: vec3/UBB_BEST58.cpp (0 structural, 58 real; random walk from u15 plateaued). Its block 2 mixes Mul2/ternary/Min
  per line (random-walk choices, ordinary source but noisy). No aliases; only vec1's operator[] pin.
  Next idea: work out why CW colours the CSE'd z loads (sv.z, sa.z) last in the orig; inlining t/t2/k/a2,
  a named sa.x local, vector-form block 2 (Scale/Add, 4 callee FPRs) and Sec() helpers were all worse/inert.

## RenderSystem (from vec1/rs_ord_best.cpp 644/975)
- 'offset.Scale(pull, emitTime); CVector3 acc; acc.Scale(accel, emit2); force.Scale(...); offset += acc;' (orig fmuls+fadds, no fmadds): 644 -> 632.
- endTime 'if (!m_emitting && m_endTime > m_maxTime) endTime = m_maxTime;' (compare the MEMBER, not the local): extra frsp gone, 632 -> 631 (real 993 -> 858).
- fog: compute z before x (orig loads w[3][2] before w[2][0]): 629/846 (r1.cpp).
- setup-block hill climb (hc.py) 605 (r2.cpp); colour x term 'deathRange[0]' like y/z: hoisted-invariant spill order now matches orig (r3.cpp 606).
- inner-loop random walk with [i]/.x spelling toggles (rw_rs.py, keeps RandomFloat order): 514/835 = vec3/RS_BEST514.cpp.
- Left: orig keeps 1.0f in f22 (spills setup f22/f21/f20 and reloads consts there), ours spills 1.0 to 912; age/seconds
  f24/f25 swapped; frame 1488 vs 1504; inner-loop scheduling. Not landable this session.
- CMatrix33: no real 3x3 matrix type or name in Rising Sun, src/include (only the Dolphin SDK typedef Mtx33 = f32[3][3]) or the ELF
  symbols -> keep CMatrix33, mark it "inferred" when landing (rs drafts lack that comment).
- The fog block's 'const float (*w)[4] = g_WorldToCamera.m;' pointer locals came from struct2; check they are acceptable before landing.
Nothing held, nothing running.
