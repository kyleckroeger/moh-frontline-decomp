#!/bin/sh
# sc.sh unit src func : fast score
cd /home/kyle/workspace/moh-frontline && python3 scratch/game/fast.py config/GMFE69/$1.json "$2" $3
