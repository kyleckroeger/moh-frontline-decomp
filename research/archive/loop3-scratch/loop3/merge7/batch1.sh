#!/bin/sh
cd /home/kyle/workspace/moh-frontline
T=scratch/loop3/merge7
one() { # id src tpl origfile idx frags bytes desc [extra]
  echo "== $1"
  sh $T/land3.sh $1 $2 $T/$3.json $4 $5 "$6" "- 2026-11-13 [loop merge7] merged $4 ($(echo $6 | wc -w) fragments, $7 B): $8" "Merge $4: $(echo $6 | wc -w) fragments into one unit" "$4's whole file in one unit ($6). $8 Complete image identical." "$9"
  sleep 60
}
one rcmp_mpc_codec src/game/rcmp_mpc_codec.cpp tpl_mpc rcmp_mpc_codec.cpp 10240 "rcmp_mpc_create rcmp_mpc_frames rcmp_tdlnode_dtor rcmp_mpc_codec" 3880 "RTTI on and deferred inlining for the whole file (the codec half's flags), written last to first; one MPC_CODEC_INTERNAL view (the frames half's, the current frame number at +0x20 = the chunks read), one TDLIST with the frames half's helpers and the codec half's DeleteAll destructor, one layer_data (the MPEG-2 flag at +0x420); the TDLNODE<MPC_FRAME> destructor explicitly instantiated between the frames and the codec functions. The compiled weak ~TDLIST and CODEC::operator delete are unreferenced (discarded, as the original linker did)."
one ConfigGC src/game/ConfigGC.cpp tpl_cfg ConfigGC.cpp 9826 "ConfigGC_dualconfig ConfigGC_init ConfigGC" 14656 "Init and the destructor/static initialisation appended to the configuration screen; one CDualConfig (the screen's members; the held sprite and fonts in anonymous unions with the destructor's CRenderBin views, the constructor's words under the screen's names), CColor from game/CScreen.h, one CSprite and CFont; --pool .sdata2 --pool .rodata (the original .rodata keeps a removed function's debug string between the screen's and Init's)." "--pool .sdata2 --pool .rodata"
echo BATCH1 DONE
