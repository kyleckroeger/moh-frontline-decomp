# knots2 (2026-11-12): register knots in harvest1's updated drafts

## Results
- CCDBObject::Init: CODE EXACT (fast 0, real 0) = scratch/loop3/knots2/cdbobject_init.cpp (shared CVector3 header).
  Keys: (1) all locals declared x, y, z, px, py, pz, qx, qy, qz; px/py/pz ALL read into locals BEFORE the x/y/z
  extents (knots1 had px direct; size read direct as knots1) -> first block exact (cdi/g1.py, 324 forms, 44 -> 10);
  (2) upper bound as 'qx = pos.x; qy..; qz..; qx += x; qy += y; qz += z; m_max = q' (cdi/g3.py: reuse p + '+=' also 0;
  'qx + x' into the store 10-16).
- SNDMIXI_volramp (20, kept): 'SNDMIXVIEW* m = &sndmix;' after 'voice->last = ...', m-> for the pan loop's alignedMix
  (both) and scalemix: fast 40 -> 11, every reg right, but the user pointer is 'addi r4; mr r27,r4' (temp + copy, the
  known CW rule) so the length shifts and real is 68. ref/const/declared-top/all-m forms same. RS has no volramp. vr/g1.py.
- decodexac (28, kept): splitting '*packet->in++' into 'table = ...(*packet->in & 0xF)...; c1 = filter[4]; packet->in++;'
  fixes the lbz/addi/stw order but the xatablef base hoist goes r6 (orig r5) and the table/out loop regs swap (real 34).
  float[2][4] xafilterf / float[16][16] xatablef views, 6x4 spellings, ++ forms/positions, decl orders and scopes: inert.
- SNDVOICEI_free: mid3 landed it (7dcdeb04); not worked.
- SetFinishedLevelData: member inline 'void CPlayerObject::SaveAmmo(int, int)' scores the same 11 -> draft.
- LANDED f748ce87 cdbobject_init (784, .sdata2 pooled, --like cdbobject.json; cdbobject_dtor.json lacks 'include').
- SetFinishedLevelData draft: member-inline form committed 516b5f21 (11).
- PROFILE_UPDATE 88 -> 82 (d63e6754): hoisted locals, hill-climbed order (pu/hc.py) puts i in r29. Left: hist r12 (ours r6),
  bins IV r31 (ours r7), the 300 count in r30 (ours r8), pointer locals in callee regs (orig volatile r7-r11). Loop-count
  forms, bin pointer local, direct history copy: inert. Next idea: a longer hill climb including statement order of the six
  pointer assignments/stores.
