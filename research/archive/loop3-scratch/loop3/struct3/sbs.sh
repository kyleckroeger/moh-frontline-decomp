#!/bin/sh
# sbs.sh tmpl src func start end -> differing lines
cd /home/kyle/workspace/moh-frontline/research/tools
(ulimit -v 3000000; timeout 300 python3 sbs.py ../../$1 ../../$2 $3 $4 $5) | awk '{ if (substr($0,6,30)!=substr($0,37,30)) print }'
