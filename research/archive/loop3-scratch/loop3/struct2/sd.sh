#!/bin/sh
cd /home/kyle/workspace/moh-frontline
(ulimit -v 3000000; timeout 300 python3 scratch/loop3/scripting/tools/sdiff.py scratch/loop3/struct2/t_rs.json scratch/loop3/struct2/$1 RenderSystem__15CParticleSystemFR10CDmaPacket 0x8007d704 4532 -v) 2>/dev/null
