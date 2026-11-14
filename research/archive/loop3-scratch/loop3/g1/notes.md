# g1 (2026-11-13, day-2 group 1 re-attack): no landing, no improved draft

Tool: s.sh <func-substr> [draft] = sbs.py side-by-side with include dirs added (prints compile errors).
brchk2 over all 25 rows: no branch-target mismatches. check_draft: all 25 consistent (no logic bugs found).

Worked (all stay at their index score):
- _find_aim_blend_values (5): case 3 alone through a second inline copy TriangleBlendB (accepted "two copies"
  form): statement orders (6), Cross2 parameter orders x 2 statement orders (48), difference/negation locals
  in all orders of 1-3 of {dbx,dby,dcx,dcy,nx,ny} (156): best 5 (base), ny-local 7. The swap is -y (orig f7)
  vs bx-x (orig f8); orig creates -y first, which none of these forms does. fab/res_l.txt.
- CStaticMesh::DrawNode (5): real members DotDotDot + Abs (shared header 29, own view 29-36); 6 statement
  orders x Dot side (n.Dot(v) vs v.Dot(n)) 48 forms: base best. dn/.
- CFontDriver::EndDraw (11): 24 operator= store orders: inert.
- ARAM_alloc_node (25): pool loop inside per-direction inlines 164 (manager r30/size r31: wrong), pool decl
  first / node assigned late 29. Orig ranks pool r31 > size r30 > manager r29 > &sentinel r28 (hoisted
  sentinel address LOWEST, ours highest). aram/.
- _matrix_rot_yzx (109): real Scale members with the shared CMatrix/CVector3 headers use one more callee FPR
  (frame 480 vs 464): 253. mr/h1.cpp.
- CheckLineCollision (10): mask before/inside the flags block (inert), GetTypes() value inline (67 sbs).
- CVolBox::TestCollision(CVolSphere) (13): DotDotDot member x-first body 101, z-first body = base 13.
Inspected only (history already exhausts the forms that fit; no new-rule match found):
  CInputManager::Update (shift pair), AIStartCoverPointWalkToStored (copy/rank dichotomy, ~1700 forms),
  quick_sort_ (tail next r27/r29), _unpack_skel_frame (swap-block schedule; slots already right),
  BIFunc_GetObjInfo (filter r27 / ARG(2) offset r26 swap; ArgObject helper still in the draft),
  CSoldierObject::Draw (weapon IV r24/r28).
Not reached: CreateRootHierObject (placement-new, only declined form left), CStaticObjectFactory::Init,
  TestCollision(CVolCapsule), PATHI_eventstart, DrawBulletStats, PROFILE_UPDATE, DMCluster, MechFollowPath,
  CAnimMorph::Draw, CMatrix::Rotate; buddy.cpp rows (CreateBuddyDataStructures, list_iter range ctor) are
  gated by the __sinit_buddy_cpp temp knot (sinit2/notes.md).
