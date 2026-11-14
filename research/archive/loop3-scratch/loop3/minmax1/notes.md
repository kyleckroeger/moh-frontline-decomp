# minmax1 notes (2026-11-12)
Task: scan unlanded drafts' ORIGINAL code for reference min/max (stack-address select) and 'bge; b; fmr' selects.
Tools: scan.py (writes hits.tsv), scan2.py / scan3.py (broader two-arm select census), sc.py (score.real on files).

## Scan result (all 112 index.tsv rows)
- refsel (addi rX,r1,N on both arms, then lfs 0(rX)): only CCDBVolBox::TestCollision(CTriangle) (463, 6 sites in the
  3-arg Separate inline). The draft ALREADY uses reference-returning Min/Max (bodies 'a < b ? a : b' / 'a < b ? b : a');
  the select instructions match the original exactly; only FPR colours differ (from the prologue on, line 41).
  SGI bodies ('b < a ? b : a' / 'b < a ? a : b') 475: worse. SetDifference / CVector3-field setup forms (no dx..gz
  floats) 954: much worse (orig keeps the components live in FPRs). Nothing to land.
- fsel ('bge/ble; b; fmr'): 24 sites in CParticleSystem::UpdateBoundingBox (claimed by real2; draft writes them as
  'a = a < b ? a : b' value ternaries, which is the shape the original's compare direction implies), and one
  non-min/max site in CVolBox::TestCollision(CVolSphere) (real2 too).
- scan3 census of any two-arm select into one register: nothing else (only addi/li pointer-or-null selects in
  CStaticObjectFactory, lbz/li in ChooseCoverPointByType).
- No dwi header added: only one landed unit (AISplinePath_segment) uses dwi::min; include/ has no dwi header.
