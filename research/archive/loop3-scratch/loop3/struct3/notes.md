# struct3 (structural matching of far-off drafts), 2026-11-12

Tools: s.sh <src> [-v] (PlayScriptedSound fast + sdiff), var.py base out 'old' 'new'... (substitute + score);
per-function dirs dno/ gcw/ sct/ upb/ each with t.json (template), s.sh, var.py; hill.py (from struct1).

## LANDED
- PlayScriptedSound 0x800e9ba8 (1336) -> sound.cpp, 85412b69. From struct2's PSS_BEST 68/12:
  1. 'extern unsigned long g_pGenericSoundMemory[20]' (the type the bsbifunc units use) and
     'data = (int*)g_pGenericSoundMemory;' AFTER the pitch call: with an int[] CW rematerialised the
     constant address (one fewer saved GPR); the cast copy stays in r22 like the original (98/14 -> 86/7).
  2. 'azimuth = GetAzimuth(position, player);' = the file's own GetAzimuth auto-inlined (its by-value param
     copy is the extra temporary that puts the FindClosestPlayer copy at 120 and the source copy at 104).
  3. 'float scale = parms.m_scale / 256.0f;' (fsubs into f1, product into f31).
  4. Registers: C-style hoisted declarations hill-climbed (type, distance, azimuth, player, soldier, pitch,
     data, handle) + 'distance = 0; azimuth = 0;' -> fast 0.
  Unit merge: ISceneNode view += vf64..vf90 + AsSoldierObject (CBullet's matching vf64..vf94 removed),
  BSObject* GetScriptObject, SSoundParms field view padded to 16 B, PlayScriptedSound decl returns int,
  SoundSchedule::AddSound view. .rodata unchanged (BeginEvent strings unaffected).
  TOOLING ISSUE (coordinator): port_unit on sound.cpp (even unmodified HEAD) drafts extab/extabindex as the full
  linked size, dropping the stripped_objects of the 6 discarded functions' records (kept_span's numbered-record
  rule matches each 8/12-byte record to a same-size original neighbour). I restored the committed form with the
  renumbered labels (@673.. / @674..; checked by relocation targets) and one_unit VERIFIED. Re-porting any unit
  whose discarded statics have extab records will hit this.

## Parked / looked at
- DetermineNewObjectivePosition (1276, ObstacleAvoidance.cpp, would PREPEND to ObstacleAvoidance.json; no
  ...rodata.0, refs are named .bss tables): dno/DNO_BEST.cpp struct 26 -> 21 (fast 235 unchanged):
  'bool IsBlocked() const { if (m_blocked[2]) return true; return false; }' in GetMaskT gives the orig's
  neg/or/srwi. at the 'next' site; 'int t = line - 1; next = t; low = t;' (and +1) gives the mr r0 pair.
  Left: duplicate 'bge' in IsClear (6 && / || / nested / bool-local forms inert); the search loop: orig keeps
  offset IVs (slwi r6,low,5; addi -32) and adds this each iteration ('add r9,r30,r6'), ours strength-reduces to a
  pointer; entry reference / IsLineFree(i) / TakeLine(out,i) member inlines inert or worse. Permuter unusable
  (member-call inlines in the body).
- GetClosestWaypointByType (784): no gain. Orig copies the waypoint position float-wise (lfs/stfs) INTO MEMORY
  (reloaded after MathFunCloseToZero) and does not fold the initial index 0 (li r31,0; cmpw r31,r8 while ctr =
  count). Copy-ctor / member-wise copy -> scalarised (269); by-value inline param -> 342/310; iterator object
  class (index member) -> still folded (274). gcw/GCW_BEST.cpp = draft.
- CScene ctor (900, struct 15, no orphans in scene.cpp): only colouring: orig puts constants 0/1/2 in r27-r29
  and the list sentinel addresses (this+144/+188) in volatile r11/r6; ours the reverse for 1/2 vs addresses.
  list_node(prev, next, data) as in scene_list_reserve, m_owned in ctor bodies: inert.
- Update_Picture_Buffers: structurally exact already (6 = relocations), register-only with permuter history; skipped.
- TLT_LoadFromRepository (572, system.json template; toilet.cpp has no orphans): tlt/f1.cpp ('if (m_count == 0)
  return end();' first in hash_map::find) fast 115 -> 111, structural 19 -> 20. The orig reloads m_size/m_data
  in each find path and rematerialises &g_Repository (lis/addi) for end(); ours hoists both. An iterator CLASS
  for find/end puts the iterator in memory (98/22); BIG_locate branch forms worse. The ''/"%s" lines are
  .sdata2 relocations only.
Session end 2026-11-12: 1 landing (85412b69, 1336 B). Nothing claimed, no permuter running, lock released.
