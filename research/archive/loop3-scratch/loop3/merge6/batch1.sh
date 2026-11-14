#!/bin/sh
# batch1.sh: land the small EA/game merges one by one, 60 s apart
cd /home/kyle/workspace/moh-frontline
T=scratch/loop3/merge6
one() { # id src tpl origfile idx frags bytes desc
  sh $T/land3.sh $1 $2 $T/tpl_$3.json $4 $5 "$6" "- 2026-11-13 [loop merge6] merged $4 ($(echo $6 | wc -w) fragments, $7 B): $8" "Merge $4: $(echo $6 | wc -w) fragments into one unit" "$4's whole file in one unit ($6). $8 Complete image identical."
  sleep 60
}
one threads src/ea/threads.cpp threads threads.cpp 12559 "threads threads_current" 360 "THREAD_iscurrent appended to THREAD_init, the alarm handler and THREAD_yield."
one ipad src/ea/ipad.cpp ipad ipad.cpp 12307 "ipad_vbl ipad" 540 "one PADStatus view; vgPadStatus volatile throughout (iPAD_update casts it for MEM_copy)."
one saramman src/ea/saramman.c saramman saramman.c 13293 "saramman saramman_free" 1100 "SNDARAM_free on the allocator's SNDARAMBLOCKVIEW record."
one stagpat src/ea/stagpat.c stagpat stagpat.c 12891 "stagpat stagpat_playpatch" 1776 "the timbre header, voice, play-options and sndgs views joined by offset (detune range, volume/key ranges, patch key, last-of-patch and active bytes, voice count); sndgs as one SNDGSVIEW object."
one eaxadecf src/ea/eaxadecf.cpp eaxadecf eaxadecf.cpp 13255 "eaxadecf eaxadecf_state" 1312 "GetState/SetState added to CEAXABLKDecf (m_state at +0x98 in both views)."
one saemsstr src/ea/saemsstr.c saemsstr_pool saemsstr.c 12630 "saemsstr_pool saemsstr" 3724 "SNDAEMSI_linkstream appended; the sndaems view joins the stream pool (+0x10) and the update hook (+0x24)."
one sbasync src/ea/sbasync.c sbasync_download sbasync.c 12698 "sbasync sbasync_download" 2436 "load/read views joined by offset with unions for the header-copy/download counters and the partial word; the whole file -inline noauto (the read helpers stay out of line; the first three functions match either way)."
one ssysinit src/ea/ssysinit.c ssysinit ssysinit.c 12591 "ssysinit_opts ssysinit" 3032 "the options functions first (reverse source order kept), SNDSYSI_init passing sndgs to them through casts."
one sst src/ea/sst.c sst sst.c 12715 "sst sst_queue" 4172 "the stream record's player, serial and external members as anonymous unions of the two halves' views (volatility and signedness differ); the link node's request word."
echo BATCH1 DONE
