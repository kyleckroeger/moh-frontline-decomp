#!/bin/sh
cd /home/kyle/workspace/moh-frontline
for f in SNDPKTPLAY_start PublishEvent__10CPublisherFR8ISubject13ESubjectEvent DrawNode__11CStaticMeshFR7MSHNodeUl _find_aim_blend_values__FffPf DisplayHint__Fv BIFunc_AIStartCoverPointWalkToStored__FPPiPv CheckLineCollision__11CCoverPointCFRC21CAIFilterRealPositionRC21CAIFilterRealPositionP21CAIFilterRealPosition SetFinishedLevelData__13CPlayerObjectFv TestCollision__7CVolBoxCFRC10CVolSphereR10CCollisionb Init__NotTheVirtualOne__19CBulletDecalManagerFv BIFunc_GetObjInfo__FPPiPv MOHFL_VD_create__4RCMPFPQ24RCMP7DECODERPQ24RCMP5FRAME LLCharObjCompositeMorph__FPfP18AnimMotFrameList_t ResetShell__10CShellMenuFv Rotate__7CMatrixF8CVector3f ARAM_alloc_node__FP17ARAM_POOL_MANAGERPCcUi14ARAM_DIRECTION CheckForObstructions__10CExplosionFR10ISceneNode Link__8CPartBinFP7CDmaTagR10CDmaPacket GetExtents__7CVolBoxCFR8CVector3R8CVector3 Draw__6CSceneFR9CViewportR7CScreenR11CRenderListR4CFog random MIX_audioslice CreateDumpObject__FRC8CVector3RC11CQuaternionii; do
  row=$(awk -F'\t' -v f="$f" '$4==f' research/drafts/index.tsv | head -1)
  [ -z "$row" ] && continue
  j=$(echo "$row" | cut -f5); dr=$(echo "$row" | cut -f6)
  suf=cpp; case "$dr" in *.c) suf=c;; esac
  echo "=== $f"
  (ulimit -v 2500000; timeout 1200 python3 scratch/loop3/day2/l1/tsweep.py "$j" "$f" "$dr" $suf 2>&1 | tail -6)
done
echo ALLDONE
