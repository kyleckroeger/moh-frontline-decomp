#!/bin/sh
# sb.sh tmpl src func addr size : side by side
cd /home/kyle/workspace/moh-frontline/research/tools && python3 sbs.py "$1" "$2" "$3" 0x$4 $(printf '0x%x' $((0x$4 + $5)))
