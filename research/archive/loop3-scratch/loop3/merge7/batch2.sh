#!/bin/sh
cd /home/kyle/workspace/moh-frontline
T=scratch/loop3/merge7
one() { # id src tpl origfile idx frags bytes desc [extra]
  echo "== $1"
  sh $T/land3.sh $1 $2 $T/$3.json $4 $5 "$6" "- 2026-11-13 [loop merge7] merged $4 ($(echo $6 | wc -w) fragments, $7 B): $8" "Merge $4: $(echo $6 | wc -w) fragments into one unit" "$4's whole file in one unit ($6). $8 Complete image identical." "$9"
  sleep 60
}
one rcmp_mpc_codec src/game/rcmp_mpc_codec.cpp tpl_mpc rcmp_mpc_codec.cpp 10240 "rcmp_mpc_create rcmp_mpc_frames rcmp_tdlnode_dtor rcmp_mpc_codec" 3880 "RTTI on and deferred inlining for the whole file (the codec half's flags), written last to first; one MPC_CODEC_INTERNAL view (the frames half's; the current frame number at +0x20 is the chunks read), one TDLIST with the frames half's helpers and the codec half's DeleteAll destructor, one layer_data (the MPEG-2 flag at +0x420); the weak TDLNODE destructor defined right after the template (so the instance stays weak) and explicitly instantiated between the frames and the codec functions. The compiled weak ~TDLIST and CODEC::operator delete are unreferenced and discarded, as the original linker did."
one animskinned src/game/animskinned.cpp tpl_as animskinned.cpp 3069 "animskinned_as animskinned_skeleton animskinned_dtor animskinned" 1432 "with __vt__12CAnimSkinned and the CAnimated/CAnimSkinned RTTI records (RTTI on): one CAnimated (the constructor's members under the skeleton code's names) and CAnimSkinned (anonymous unions of the frame/count and offsets/storage-pointer views); the shared CVector3 and CMatrix, with a file-private double-pair union for ExtractTranslation's copy; the weak AsAnimSkinned defined first; ~CAnimated a weak duplicate; --pool .sdata2." "--pool .sdata2 --weak-duplicate __dt__9CAnimatedFv"
echo BATCH2 DONE
