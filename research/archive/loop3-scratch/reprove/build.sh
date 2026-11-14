#!/bin/sh
# build.sh NAME SCRIPT LEVELPROBE|none: link all drafts but RenderSystem with coverage + route probe
cd /home/kyle/workspace/moh-frontline
N=$1; S=$2; L=$3; R=scratch/reprove/r
[ "$L" = none ] && L=
python3 research/tools/input_script.py $S $R/${N}_probe.cpp || exit 1
[ -n "$HOOKS" ] && { python3 research/tools/probe_hooks.py $R/${N}_probe.cpp $HOOKS || exit 1; }
python3 scratch/reprove/hashes.py > $R/${N}.hashes.before
ulimit -v 3000000
timeout 900 python3 research/tools/link_drafts.py --exclude RenderSystem__15CParticleSystem --coverage --probe $R/${N}_probe.cpp research/probes/clock_probe.cpp $L --output $R/$N.elf 2>&1 | tail -3
python3 scratch/reprove/hashes.py > $R/${N}.hashes.after
cmp -s $R/${N}.hashes.before $R/${N}.hashes.after && echo HASHES_STABLE || echo HASHES_CHANGED
