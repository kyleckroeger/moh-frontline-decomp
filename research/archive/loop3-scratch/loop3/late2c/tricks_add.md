
## late2c (2026-11-13; scratch/loop3/late2c, SNDI_precalcaztospkrvol 79 -> 4)
- Volatile-register rotation of loop values (orig sndgs r3, azimuth r4, row r5, count r6, i r7; ours azimuth
  above the row pointer): a value that steps with the loop counter is a strength-reduced IV in the original,
  not an accumulator. 'int azimuth = i * 256;' at the top of the body (not 'azimuth = 0; ... azimuth += 256;')
  creates it after the row pointer and the count, and every outer register matched. Look for this whenever a
  per-iteration '+= K' variable sits in the wrong volatile register relative to the loop's pointer IVs.
- A pointer that the original keeps as its own register while its first element is loaded through the BASE
  ('addi r11,r8,120; lhz r10,120(r8)'; ours 'addi r8,r8,120; lhz r10,0(r8)'): the pointer is not walked
  ('az++; level++') in a later loop. Indexing 'az[k]', 'sndaztospkr[i][k]' in that loop gives the original's
  split, and the level pointer IV takes the original's register.
- Two-term blocks ('before', 'after', two quotients over their sum) colouring wrong under every statement,
  operand and declaration order: pass the two differences to an inline helper as ARGUMENTS (evaluated right to
  left, so the last argument's value is created first). One branch went exact this way; a second branch needed
  the arguments and destination slots swapped and gets to 4 (the sum's register).
- The index row of SNDI_precalcaztospkrvol named ShellMenu_ctor.json; the EA file's own template is
  config/GMFE69/satospkr.json (same score here, but EA flags differ: no exceptions, no stmw, no fp_contract).
